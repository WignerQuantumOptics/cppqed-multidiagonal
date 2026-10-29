// Part of the cppqed-multidiagonal benchmark suite. BSL-1.0.
// Kernel experiments behind benchmarks/performance.md: what MultiDiagonal application costs, and what
// alternative loop structures over the same data would cost. Emits CSV to stdout:
//   case,impl,N,ns_per_op,ns_per_nnz
// N is the length of the state vector. Every variant is cross-validated against Eigen before timing
// (exit 1 on mismatch).
//
// Cases:
//   A  rank 1, five diagonals: library operator() vs hoisted loop vs row-blocked loop vs Eigen sparse
//   B  rank-2 Jaynes–Cummings, mode = axis 0: library operator() vs hoisted loop vs Eigen sparse
//   C  a five-diagonal operator of dimension n on axis j of an n^4 state, without materializing it
//      (one diagonal at a time / all diagonals per outer block) vs the library
//      on the materialized rank-4 operator vs Eigen sparse on the flattened Kronecker product
//   D  the same operator applied slice by slice, as C++QED's Broadcaster does: library operator(),
//      hoisted loop (one diagonal at a time), row-oriented loop (all diagonals per output row), and
//      Eigen sparse of the term's own n×n space on the same strided slices; slices taken either in
//      SliceIterator's order (last dummy axis fastest) or in memory order (first dummy axis fastest).
//      Run at n=32 (power-of-two strides) and n=30.
//
// Built twice: cppqed-multidiagonal-kernels (default complex multiplication, with the C99 Annex G
// NaN checks) and cppqed-multidiagonal-kernels-cxlr (-fcx-limited-range).
//
// usage: cppqed-multidiagonal-kernels[-cxlr] [--smoke] [case ...]
//        case ∈ {A,B,C,D}; none = all. --smoke shrinks the sizes for CI; its timings are meaningless.
#include <multidiagonal/MultiDiagonal.h>

#include <Eigen/SparseCore>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <set>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

using namespace cppqedutils;
using quantumoperator::MultiDiagonal;

namespace {

// ── harness ──────────────────────────────────────────────────────────────────

#if defined(__GNUC__) || defined(__clang__)
template <typename T> void doNotOptimize(T const& v) { asm volatile("" : : "r,m"(v) : "memory"); }
#else
volatile const void* dnoSink_;
template <typename T> void doNotOptimize(T const& v) { dnoSink_ = &v; }
#endif

using Clock = std::chrono::steady_clock;

double targetBatchNs = 3e7;

/// doubling calibration to ≥ targetBatchNs, then median of five batches
template <typename F> double perOp(F&& f)
{
  size_t it=1;
  auto run=[&]{ const auto t0=Clock::now(); for (size_t i=0; i<it; ++i) f(); return std::chrono::duration<double,std::nano>(Clock::now()-t0).count(); };
  while (run()<targetBatchNs) it*=2;
  std::vector<double> t(5); for (auto& v : t) v=run()/double(it);
  std::nth_element(t.begin(),t.begin()+2,t.end()); return t[2];
}

void out(const char* c, const char* impl, size_t N, double ns, double nnz)
{
  std::printf("%s,%s,%zu,%.4g,%.3g\n",c,impl,N,ns,ns/nnz);
  std::fflush(stdout);
}

void check(const char* what, const CVector& got, const CVector& ref)
{
  const double rel=(got-ref).norm()/ref.norm();
  if (!(rel<1e-10)) { std::fprintf(stderr,"VALIDATION FAILED: %s relative deviation %g\n",what,rel); std::exit(1); }
}

// ── operators and states ────────────────────────────────────────────────────

dcomp coeff(ptrdiff_t o, size_t k) { return { std::cos(0.1*double(k)+double(o)), std::sin(0.05*double(k)-double(o)) }; }

MultiDiagonal<1> single(ptrdiff_t o, std::vector<dcomp> v)
{
  MultiDiagonal<1> md;
  md.diagonals.emplace(MultiDiagonal<1>::Offsets{o}, multiarray::fromStorage(std::move(v)));
  return md;
}

/// five diagonals, offsets −2…+2
MultiDiagonal<1> penta(size_t N)
{
  MultiDiagonal<1> md;
  for (ptrdiff_t o=-2; o<=2; ++o) {
    std::vector<dcomp> v(N-size_t(std::abs(o)));
    for (size_t k=0; k<v.size(); ++k) v[k]=coeff(o,k);
    md.diagonals.emplace(MultiDiagonal<1>::Offsets{o}, multiarray::fromStorage(std::move(v)));
  }
  return md;
}

template <size_t R> MultiArray<dcomp,R> randomState(Extents<R> ext)
{
  std::mt19937 g{1}; std::uniform_real_distribution<double> d{-1.,1.};
  return MultiArray<dcomp,R>{ext,[&](size_t e){ std::vector<dcomp> v(e); for (auto& x : v) x={d(g),d(g)}; return v; }};
}

template <size_t R> void zero(MultiArray<dcomp,R>& a) { for (auto& v : a.dataStorage()) v=0; }

template <size_t R> CVector toVec(const MultiArray<dcomp,R>& a)
{
  const auto s=a.dataView; return Eigen::Map<const CVector>(s.data(),Eigen::Index(s.size()));
}

/// Eigen sparse matrix of an arbitrary MultiDiagonal over the flattened (first-index-fastest) space
template <size_t R> Eigen::SparseMatrix<dcomp,Eigen::RowMajor> toSparse(const MultiDiagonal<R>& md, Extents<R> ext)
{
  std::vector<Eigen::Triplet<dcomp>> tr;
  Extents<R> str; str[0]=1; for (size_t i=1; i<R; ++i) str[i]=str[i-1]*ext[i-1];
  const Eigen::Index dim=Eigen::Index(str[R-1]*ext[R-1]);
  for (const auto& [off,diag] : md.diagonals) {
    if (diag.dataView.empty()) continue;
    for (Extents<R> k{}; k[0]!=diag.extents[0]; incrementMultiIndex(k,diag.extents)) {
      size_t r=0, c=0;
      for (size_t i=0; i<R; ++i) {
        r+=str[i]*(k[i]+size_t(std::max(-off[i],ptrdiff_t{0})));
        c+=str[i]*(k[i]+size_t(std::max( off[i],ptrdiff_t{0})));
      }
      tr.emplace_back(int(r),int(c),diag(k));
    }
  }
  Eigen::SparseMatrix<dcomp,Eigen::RowMajor> S(dim,dim);
  S.setFromTriplets(tr.begin(),tr.end());
  S.makeCompressed();
  return S;
}

// ── alternative application kernels ─────────────────────────────────────────

/// per diagonal: base addresses computed once; the innermost loop runs over axis 0 with raw pointers
template <size_t R> void hoisted(const MultiDiagonal<R>& md, const MultiArray<dcomp,R>& psi, MultiArray<dcomp,R>& y)
{
  const auto& str=psi.strides;
  for (const auto& [off,diag] : md.diagonals) {
    if (diag.dataView.empty()) continue;
    const dcomp* d=diag.dataView.data(); const dcomp* x=psi.dataView.data(); dcomp* yp=y.dataStorage().data();
    ptrdiff_t baseY=0, baseX=0;
    for (size_t i=0; i<R; ++i) { baseY+=ptrdiff_t(str[i])*std::max(-off[i],ptrdiff_t{0}); baseX+=ptrdiff_t(str[i])*std::max(off[i],ptrdiff_t{0}); }
    const size_t len=diag.extents[0];
    Extents<R> outerExt=diag.extents; outerExt[0]=1;
    size_t dOff=0;
    for (Extents<R> k{}; k[0]!=1; incrementMultiIndex(k,outerExt)) {
      ptrdiff_t oy=baseY, ox=baseX; for (size_t i=1; i<R; ++i) { oy+=ptrdiff_t(str[i]*k[i]); ox+=ptrdiff_t(str[i]*k[i]); }
      dcomp* __restrict ys=yp+oy; const dcomp* __restrict xs=x+ox; const dcomp* __restrict ds=d+dOff;
      for (size_t m=0; m<len; ++m) ys[m]+=ds[m]*xs[m];
      dOff+=len;
    }
  }
}

/// rank 1, rows blocked so that the y block stays in L1 across all diagonals
void rowBlocked(const MultiDiagonal<1>& md, const MultiArray<dcomp,1>& psi, MultiArray<dcomp,1>& y)
{
  const size_t N=psi.extents[0], B=1024; const dcomp* x=psi.dataView.data(); dcomp* yp=y.dataStorage().data();
  for (size_t r0=0; r0<N; r0+=B) for (const auto& [off,diag] : md.diagonals) {
    const ptrdiff_t o=off[0]; const size_t om=size_t(std::max(-o,ptrdiff_t{0})), op=size_t(std::max(o,ptrdiff_t{0})), len=N-size_t(std::abs(o));
    // diagonal entry k writes row k+om; rows [r0,r0+B) ↔ k ∈ [r0-om, r0+B-om) ∩ [0,len)
    const size_t kb = r0>om ? r0-om : 0, ke = std::min(len, r0+B>om ? r0+B-om : 0);
    const dcomp* __restrict d=diag.dataView.data(); dcomp* __restrict ys=yp+om; const dcomp* __restrict xs=x+op;
    for (size_t k=kb; k<ke; ++k) ys[k]+=d[k]*xs[k];
  }
}

/// rank-1 operator A on axis j of a rank-R state, not materialized; one diagonal at a time over the whole state
template <size_t R> void broadcast(const MultiDiagonal<1>& A, size_t j, const MultiArray<dcomp,R>& psi, MultiArray<dcomp,R>& y)
{
  const size_t n=psi.extents[j], sj=psi.strides[j], blockOuter=sj*n, nOuter=psi.dataView.size()/blockOuter;
  const dcomp* x=psi.dataView.data(); dcomp* yp=y.dataStorage().data();
  for (const auto& [off,diag] : A.diagonals) {
    const ptrdiff_t o=off[0]; const size_t om=size_t(std::max(-o,ptrdiff_t{0})), op=size_t(std::max(o,ptrdiff_t{0})), len=n-size_t(std::abs(o));
    const dcomp* d=diag.dataView.data();
    if (sj==1)
      for (size_t b=0; b<nOuter; ++b) { dcomp* __restrict ys=yp+b*n+om; const dcomp* __restrict xs=x+b*n+op; for (size_t k=0; k<len; ++k) ys[k]+=d[k]*xs[k]; }
    else
      for (size_t b=0; b<nOuter; ++b) for (size_t k=0; k<len; ++k) {
        const dcomp c=d[k]; dcomp* __restrict ys=yp+b*blockOuter+(k+om)*sj; const dcomp* __restrict xs=x+b*blockOuter+(k+op)*sj;
        for (size_t m=0; m<sj; ++m) ys[m]+=c*xs[m];
      }
  }
}

/// the same, all diagonals per outer block (one slab along the axes > j), so that the slab stays in cache
template <size_t R> void broadcastBlocked(const MultiDiagonal<1>& A, size_t j, const MultiArray<dcomp,R>& psi, MultiArray<dcomp,R>& y)
{
  const size_t n=psi.extents[j], sj=psi.strides[j], blockOuter=sj*n, nOuter=psi.dataView.size()/blockOuter;
  const dcomp* x=psi.dataView.data(); dcomp* yp=y.dataStorage().data();
  for (size_t b=0; b<nOuter; ++b) for (const auto& [off,diag] : A.diagonals) {
    const ptrdiff_t o=off[0]; const size_t om=size_t(std::max(-o,ptrdiff_t{0})), op=size_t(std::max(o,ptrdiff_t{0})), len=n-size_t(std::abs(o));
    const dcomp* d=diag.dataView.data();
    for (size_t k=0; k<len; ++k) {
      const dcomp c=d[k]; dcomp* __restrict ys=yp+b*blockOuter+(k+om)*sj; const dcomp* __restrict xs=x+b*blockOuter+(k+op)*sj;
      for (size_t m=0; m<sj; ++m) ys[m]+=c*xs[m];
    }
  }
}

/// one diagonal of a rank-1 operator, as offset and data pointer
struct Tap { ptrdiff_t o; const dcomp* d; };

std::vector<Tap> taps(const MultiDiagonal<1>& A)
{
  std::vector<Tap> res; for (const auto& [off,diag] : A.diagonals) res.push_back({off[0],diag.dataView.data()});
  return res;
}

/// rank-1 operator on a slice of length n and stride s, row-oriented: all diagonals of one output row summed in a register
void rowsSlice(const std::vector<Tap>& ts, ptrdiff_t n, size_t s, const dcomp* x, dcomp* y)
{
  for (ptrdiff_t r=0; r<n; ++r) {
    dcomp acc{};
    for (const auto& t : ts) { // row r, column r+o; entry index k = r − o⁻
      const ptrdiff_t c=r+t.o; if (c<0 || c>=n) continue;
      acc+=t.d[r-std::max(-t.o,ptrdiff_t{0})]*x[size_t(c)*s];
    }
    y[size_t(r)*s]+=acc;
  }
}

/// rank-1 operator on a slice of length n and stride s, one diagonal at a time, addresses hoisted
void diagonalsSlice(const MultiDiagonal<1>& A, size_t n, size_t s, const dcomp* x, dcomp* y)
{
  for (const auto& [off,diag] : A.diagonals) {
    const ptrdiff_t o=off[0]; const size_t om=size_t(std::max(-o,ptrdiff_t{0})), op=size_t(std::max(o,ptrdiff_t{0})), len=n-size_t(std::abs(o));
    const dcomp* d=diag.dataView.data(); dcomp* ys=y+om*s; const dcomp* xs=x+op*s;
    for (size_t k=0; k<len; ++k) ys[k*s]+=d[k]*xs[k*s];
  }
}

std::vector<size_t> sizes(bool smoke) { return smoke ? std::vector<size_t>{256} : std::vector<size_t>{256,4096,262144}; }

// ── A: rank 1 ────────────────────────────────────────────────────────────────

void caseA(bool smoke)
{
  for (size_t N : sizes(smoke)) {
    auto md=penta(N); auto psi=randomState<1>({N}); MultiArray<dcomp,1> y{{N},multiarray::zeroInit<dcomp>};
    const auto S=toSparse(md,{N}); const CVector x=toVec(psi); CVector ye=CVector::Zero(Eigen::Index(N));
    const double nnz=double(S.nonZeros());
    md(0.,psi,y); const CVector ref=S*x; check("A library",toVec(y),ref);
    zero(y); hoisted(md,psi,y); check("A hoisted",toVec(y),ref);
    zero(y); rowBlocked(md,psi,y); check("A rowBlocked",toVec(y),ref);
    out("A_rank1","library",N,perOp([&]{md(0.,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out("A_rank1","hoisted",N,perOp([&]{hoisted(md,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out("A_rank1","rowBlocked",N,perOp([&]{rowBlocked(md,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out("A_rank1","eigen_sparse",N,perOp([&]{ye.noalias()+=S*x; doNotOptimize(ye.data());}),nnz);
  }
}

// ── B: rank-2 Jaynes–Cummings, mode = axis 0 ─────────────────────────────────

void caseB(bool smoke)
{
  for (size_t N : sizes(smoke)) {
    auto ladder=[&]{ std::vector<dcomp> v(N-1); for (size_t k=0; k<N-1; ++k) v[k]=std::sqrt(double(k+1)); return single(+1,std::move(v)); };
    std::vector<dcomp> nv(N); for (size_t k=0; k<N; ++k) nv[k]=double(k);
    auto mode=0.7*single(0,nv) + dcomp(0.3,0.2)*hermitianConjugateOf(ladder()) + dcomp(0.3,-0.2)*ladder();
    auto jc = 1.1*(ladder()*single(-1,{1.})) + 1.1*(hermitianConjugateOf(ladder())*single(+1,{1.})) + mode*quantumoperator::multidiagonal::identity(2);
    const Extents<2> ext{N,2}; auto psi=randomState<2>(ext); MultiArray<dcomp,2> y{ext,multiarray::zeroInit<dcomp>};
    const auto S=toSparse(jc,ext); const CVector x=toVec(psi); CVector ye=CVector::Zero(x.size());
    const double nnz=double(S.nonZeros());
    jc(0.,psi,y); const CVector ref=S*x; check("B library",toVec(y),ref);
    zero(y); hoisted(jc,psi,y); check("B hoisted",toVec(y),ref);
    out("B_JC_modefast","library",2*N,perOp([&]{jc(0.,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out("B_JC_modefast","hoisted",2*N,perOp([&]{hoisted(jc,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out("B_JC_modefast","eigen_sparse",2*N,perOp([&]{ye.noalias()+=S*x; doNotOptimize(ye.data());}),nnz);
  }
}

// ── C: an operator on one axis of a rank-4 state, not materialized ───────────

void caseC(bool smoke)
{
  const size_t n = smoke ? 8 : 32; const Extents<4> ext{n,n,n,n};
  auto A=penta(n); auto psi=randomState<4>(ext); MultiArray<dcomp,4> y{ext,multiarray::zeroInit<dcomp>};
  const CVector x=toVec(psi); CVector ye=CVector::Zero(x.size()); const size_t N=size_t(x.size());
  for (size_t j : {0uz,2uz}) {
    auto id=[&]{return quantumoperator::multidiagonal::identity(n);};
    MultiDiagonal<4> full = j==0 ? A*id()*id()*id() : id()*id()*A*id();
    const auto S=toSparse(full,ext); const double nnz=double(S.nonZeros());
    const CVector ref=S*x;
    zero(y); broadcast(A,j,psi,y); check("C broadcast",toVec(y),ref);
    zero(y); broadcastBlocked(A,j,psi,y); check("C broadcastBlocked",toVec(y),ref);
    zero(y); full(0.,psi,y); check("C library",toVec(y),ref);
    const char* c = j==0 ? "C_axis0" : "C_axis2";
    out(c,"diagonalwise",N,perOp([&]{broadcast(A,j,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out(c,"blocked",N,perOp([&]{broadcastBlocked(A,j,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out(c,"library_materialized",N,perOp([&]{full(0.,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out(c,"eigen_sparse_kron",N,perOp([&]{ye.noalias()+=S*x; doNotOptimize(ye.data());}),nnz);
  }
}

// ── D: the same operator, applied slice by slice ─────────────────────────────

void caseD(bool smoke)
{
  for (size_t n : smoke ? std::vector<size_t>{8,7} : std::vector<size_t>{32,30}) {
    const Extents<4> ext{n,n,n,n};
    auto A=penta(n); auto psi=randomState<4>(ext); MultiArray<dcomp,4> y{ext,multiarray::zeroInit<dcomp>};
    const CVector x=toVec(psi); CVector ye=CVector::Zero(x.size()); const size_t N=size_t(x.size());
    const auto As=toSparse(A,Extents<1>{n});   // the term's own n×n space, flattened
    const auto ts=taps(A);
    for (size_t j : {0uz,2uz}) {
      auto id=[&]{return quantumoperator::multidiagonal::identity(n);};
      MultiDiagonal<4> full = j==0 ? A*id()*id()*id() : id()*id()*A*id();
      const auto S=toSparse(full,ext); const double nnz=double(S.nonZeros()); const CVector ref=S*x;
      const size_t s=psi.strides[j];

      // slice offsets over the three dummy axes, in two orders
      std::array<size_t,3> dAx; for (size_t i=0, m=0; i<4; ++i) if (i!=j) dAx[m++]=i;
      std::vector<size_t> sliceIteratorOrder, memoryOrder;
      for (size_t a=0; a<n; ++a) for (size_t b=0; b<n; ++b) for (size_t c=0; c<n; ++c) {
        sliceIteratorOrder.push_back(a*psi.strides[dAx[0]]+b*psi.strides[dAx[1]]+c*psi.strides[dAx[2]]); // last dummy fastest
        memoryOrder       .push_back(c*psi.strides[dAx[0]]+b*psi.strides[dAx[1]]+a*psi.strides[dAx[2]]); // first dummy fastest
      }

      const std::span<const dcomp> xs=psi.dataView; const std::span<dcomp> ys{y.dataStorage()};
      const auto library=[&](const std::vector<size_t>& offs){ for (size_t o : offs)
        A(0.,MultiArrayView<const dcomp,1>{{n},{s},o,xs},MultiArrayView<dcomp,1>{{n},{s},o,ys}); };
      const auto diagonalwise=[&](const std::vector<size_t>& offs){ for (size_t o : offs) diagonalsSlice(A,n,s,xs.data()+o,ys.data()+o); };
      const auto rowwise=[&](const std::vector<size_t>& offs){ for (size_t o : offs) rowsSlice(ts,ptrdiff_t(n),s,xs.data()+o,ys.data()+o); };
      using SMap=Eigen::Map<const CVector,0,Eigen::InnerStride<>>; using DMap=Eigen::Map<CVector,0,Eigen::InnerStride<>>;
      const auto termCSR=[&](const std::vector<size_t>& offs){ for (size_t o : offs) {
        DMap yv(ys.data()+o,Eigen::Index(n),Eigen::InnerStride<>(Eigen::Index(s)));
        yv.noalias()+=As*SMap(xs.data()+o,Eigen::Index(n),Eigen::InnerStride<>(Eigen::Index(s))); } };

      zero(y); library(sliceIteratorOrder); check("D library",toVec(y),ref);
      zero(y); diagonalwise(memoryOrder);   check("D diagonalwise",toVec(y),ref);
      zero(y); rowwise(memoryOrder);        check("D rowwise",toVec(y),ref);
      zero(y); termCSR(memoryOrder);        check("D termCSR",toVec(y),ref);

      char c[32]; std::snprintf(c,sizeof c,"D_n%zu_axis%zu",n,j);
      for (const auto& [order,offs] : {std::pair{"sliceIteratorOrder",&sliceIteratorOrder},std::pair{"memoryOrder",&memoryOrder}}) {
        char impl[64];
        std::snprintf(impl,sizeof impl,"library_%s",order);      out(c,impl,N,perOp([&]{library(*offs);      doNotOptimize(ys.data());}),nnz);
        std::snprintf(impl,sizeof impl,"diagonalwise_%s",order); out(c,impl,N,perOp([&]{diagonalwise(*offs); doNotOptimize(ys.data());}),nnz);
        std::snprintf(impl,sizeof impl,"rowwise_%s",order);      out(c,impl,N,perOp([&]{rowwise(*offs);      doNotOptimize(ys.data());}),nnz);
        std::snprintf(impl,sizeof impl,"termCSR_%s",order);      out(c,impl,N,perOp([&]{termCSR(*offs);      doNotOptimize(ys.data());}),nnz);
      }
      out(c,"eigen_sparse_kron",N,perOp([&]{ye.noalias()+=S*x; doNotOptimize(ye.data());}),nnz);
    }
  }
}

} // anonymous


int main(int argc, char** argv)
{
  bool smoke=false;
  std::set<std::string_view> only;
  for (int i=1; i<argc; ++i) {
    std::string_view a{argv[i]};
    if (a=="--smoke") smoke=true; else only.insert(a);
  }

  static constexpr std::array<std::pair<std::string_view,void(*)(bool)>,4> cases{{{"A",caseA},{"B",caseB},{"C",caseC},{"D",caseD}}};

  for (auto sel : only)
    if (std::none_of(cases.begin(),cases.end(),[=](const auto& c){return c.first==sel;})) {
      std::fprintf(stderr,"unknown case '%.*s'; known: A B C D\n",int(sel.size()),sel.data());
      return 2;
    }

#ifndef NDEBUG
  std::fprintf(stderr,"WARNING: non-Release build — bounds checks active, timings meaningless.\n");
#endif

  if (smoke) targetBatchNs=2e6;

  std::printf("case,impl,N,ns_per_op,ns_per_nnz\n");
  for (const auto& [name,fn] : cases) if (only.empty() || only.contains(name)) fn(smoke);
}
