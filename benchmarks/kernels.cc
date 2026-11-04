// Part of the cppqed-multidiagonal benchmark suite. BSL-1.0.
// Kernel experiments behind benchmarks/performance.md: what MultiDiagonal application costs, and what
// alternative loop structures over the same data would cost. Emits CSV to stdout:
//   case,impl,N,ns_per_op,ns_per_nnz
// N is the length of the state vector (for cases F and H, of axis 0; for case G, the offset of psi in bytes). Every variant is cross-validated against Eigen before timing
// (exit 1 on mismatch).
//
// The library is timed as operator() chooses its loop shape ("library"), and with each of its two shapes forced
// ("library_byDiagonals": one diagonal at a time; "library_byRows": all diagonals of a row summed before one store).
//
// Cases:
//   A  rank 1, five diagonals: library vs hoisted loop vs row-blocked loop vs Eigen sparse
//   B  rank-2 Jaynes–Cummings, mode = axis 0: library vs hoisted loop vs Eigen sparse
//   C  a five-diagonal operator of dimension n on axis j of an n^4 state, without materializing it
//      (one diagonal at a time / all diagonals per outer block) vs the library
//      on the materialized rank-4 operator vs Eigen sparse on the flattened Kronecker product
//   D  the same operator applied slice by slice, as C++QED's Broadcaster does: library,
//      hoisted loop (one diagonal at a time), row-oriented loop (all diagonals per output row, scalar), and
//      Eigen sparse of the term's own n×n space on the same strided slices; slices taken either in
//      SliceIterator's order (last dummy axis fastest) or in memory order (first dummy axis fastest).
//      Run at n=32 (power-of-two strides) and n=30.
//   E  a short axis 0: tridiagonal ⊗ tridiagonal on extents {n0, 8192/n0}, n0 = 1…32: library
//      (which unrolls an axis 0 of up to four elements one diagonal at a time) vs hoisted loop (always loops over
//      axis 0) vs Eigen sparse
//   F  where the library's two loop shapes cross over, which sets operator()'s choice: the five-diagonal operator
//      on contiguous slices of length n, of a state in cache (2048 amplitudes) and of one of 2^20; and
//      tridiagonal ⊗ tridiagonal on extents {n0, T/n0}, contiguous, for T = 8192 (in L2), 65536 (in L3), 2^20
//   G  placement (#25): psi at every 16-byte offset of a page relative to dpsidt, for both loop shapes: rank 1 at
//      N = 64, 1024, 4096, and Jaynes–Cummings with the mode first and with the qubit first (state 4096). One row per
//      offset, with N the offset in bytes; best, median, worst and the offsets more than 5% above the best go to
//      stderr. It takes a few minutes, so it runs only when named (and in --smoke).
//   H  why case F's slices of power-of-two length were slow out of cache: its five-diagonal operator on contiguous slices
//      of length n = 16, 24, 32 of a state a little under 2^20 amplitudes, with dpsidt D = 2^24 + delta bytes after psi in
//      one buffer, on huge pages and on 4 KiB pages; both shapes. Two arrays of exactly 16 MiB allocated back to back lie
//      2^24 + 16 bytes apart. The case names the pages and D; stderr gives how much of the buffer the kernel put on huge
//      pages, and case F reports its own psi-dpsidt (mod 4096). H runs only when named (and in --smoke).
//
// Built twice: cppqed-multidiagonal-kernels (default complex multiplication, with the C99 Annex G
// NaN checks) and cppqed-multidiagonal-kernels-cxlr (-fcx-limited-range). The library and the loops
// here multiply with textbookProduct, which has no NaN check, so the flag changes only Eigen's figures.
//
// usage: cppqed-multidiagonal-kernels[-cxlr] [--smoke] [case ...]
//        case ∈ {A,B,C,D,E,F,G,H}; none = A–F. --smoke shrinks the sizes for CI; its timings are meaningless.
#include <multidiagonal/MultiDiagonal.h>

#include <Eigen/SparseCore>

#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <memory>
#include <random>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#ifdef __linux__
#include <sys/mman.h>
#endif

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

/// three diagonals, offsets −1…+1 (for n = 1 only the main one)
MultiDiagonal<1> tri(size_t n)
{
  MultiDiagonal<1> md;
  for (ptrdiff_t o=-1; o<=1; ++o) if (size_t(std::abs(o))<n) {
    std::vector<dcomp> v(n-size_t(std::abs(o)));
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
      for (size_t m=0; m<len; ++m) ys[m]+=textbookProduct(ds[m],xs[m]);
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
    for (size_t k=kb; k<ke; ++k) ys[k]+=textbookProduct(d[k],xs[k]);
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
      for (size_t b=0; b<nOuter; ++b) { dcomp* __restrict ys=yp+b*n+om; const dcomp* __restrict xs=x+b*n+op; for (size_t k=0; k<len; ++k) ys[k]+=textbookProduct(d[k],xs[k]); }
    else
      for (size_t b=0; b<nOuter; ++b) for (size_t k=0; k<len; ++k) {
        const dcomp c=d[k]; dcomp* __restrict ys=yp+b*blockOuter+(k+om)*sj; const dcomp* __restrict xs=x+b*blockOuter+(k+op)*sj;
        for (size_t m=0; m<sj; ++m) ys[m]+=textbookProduct(c,xs[m]);
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
      for (size_t m=0; m<sj; ++m) ys[m]+=textbookProduct(c,xs[m]);
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
      acc+=textbookProduct(t.d[r-std::max(-t.o,ptrdiff_t{0})],x[size_t(c)*s]);
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
    for (size_t k=0; k<len; ++k) ys[k*s]+=textbookProduct(d[k],xs[k*s]);
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
    zero(y); md.applyByDiagonals(psi,y); check("A library_byDiagonals",toVec(y),ref);
    zero(y); md.applyByRows(psi,y); check("A library_byRows",toVec(y),ref);
    zero(y); hoisted(md,psi,y); check("A hoisted",toVec(y),ref);
    zero(y); rowBlocked(md,psi,y); check("A rowBlocked",toVec(y),ref);
    out("A_rank1","library",N,perOp([&]{md(0.,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out("A_rank1","library_byDiagonals",N,perOp([&]{md.applyByDiagonals(psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out("A_rank1","library_byRows",N,perOp([&]{md.applyByRows(psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out("A_rank1","hoisted",N,perOp([&]{hoisted(md,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out("A_rank1","rowBlocked",N,perOp([&]{rowBlocked(md,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out("A_rank1","eigen_sparse",N,perOp([&]{ye.noalias()+=S*x; doNotOptimize(ye.data());}),nnz);
  }
}

// ── B: rank-2 Jaynes–Cummings, mode = axis 0 ─────────────────────────────────

/// Jaynes–Cummings with a driven mode of dimension N: mode on axis 0 and qubit on axis 1, or the converse
MultiDiagonal<2> jaynesCummings(size_t N, bool qubitFirst)
{
  auto ladder=[&]{ std::vector<dcomp> v(N-1); for (size_t k=0; k<N-1; ++k) v[k]=std::sqrt(double(k+1)); return single(+1,std::move(v)); };
  std::vector<dcomp> nv(N); for (size_t k=0; k<N; ++k) nv[k]=double(k);
  auto mode=0.7*single(0,nv) + dcomp(0.3,0.2)*hermitianConjugateOf(ladder()) + dcomp(0.3,-0.2)*ladder();
  using quantumoperator::multidiagonal::identity;
  if (qubitFirst) return 1.1*(single(-1,{1.})*ladder()) + 1.1*(single(+1,{1.})*hermitianConjugateOf(ladder())) + identity(2)*mode;
  return 1.1*(ladder()*single(-1,{1.})) + 1.1*(hermitianConjugateOf(ladder())*single(+1,{1.})) + mode*identity(2);
}

void caseB(bool smoke)
{
  for (size_t N : sizes(smoke)) {
    const auto jc=jaynesCummings(N,false);
    const Extents<2> ext{N,2}; auto psi=randomState<2>(ext); MultiArray<dcomp,2> y{ext,multiarray::zeroInit<dcomp>};
    const auto S=toSparse(jc,ext); const CVector x=toVec(psi); CVector ye=CVector::Zero(x.size());
    const double nnz=double(S.nonZeros());
    jc(0.,psi,y); const CVector ref=S*x; check("B library",toVec(y),ref);
    zero(y); jc.applyByDiagonals(psi,y); check("B library_byDiagonals",toVec(y),ref);
    zero(y); jc.applyByRows(psi,y); check("B library_byRows",toVec(y),ref);
    zero(y); hoisted(jc,psi,y); check("B hoisted",toVec(y),ref);
    out("B_JC_modefast","library",2*N,perOp([&]{jc(0.,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out("B_JC_modefast","library_byDiagonals",2*N,perOp([&]{jc.applyByDiagonals(psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out("B_JC_modefast","library_byRows",2*N,perOp([&]{jc.applyByRows(psi,y); doNotOptimize(y.dataView.data());}),nnz);
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
    zero(y); full.applyByDiagonals(psi,y); check("C library_byDiagonals",toVec(y),ref);
    zero(y); full.applyByRows(psi,y); check("C library_byRows",toVec(y),ref);
    const char* c = j==0 ? "C_axis0" : "C_axis2";
    out(c,"diagonalwise",N,perOp([&]{broadcast(A,j,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out(c,"blocked",N,perOp([&]{broadcastBlocked(A,j,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out(c,"library_materialized",N,perOp([&]{full(0.,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out(c,"library_materialized_byDiagonals",N,perOp([&]{full.applyByDiagonals(psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out(c,"library_materialized_byRows",N,perOp([&]{full.applyByRows(psi,y); doNotOptimize(y.dataView.data());}),nnz);
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
      const auto libraryByDiagonals=[&](const std::vector<size_t>& offs){ for (size_t o : offs)
        A.applyByDiagonals(MultiArrayView<const dcomp,1>{{n},{s},o,xs},MultiArrayView<dcomp,1>{{n},{s},o,ys}); };
      const auto libraryByRows=[&](const std::vector<size_t>& offs){ for (size_t o : offs)
        A.applyByRows(MultiArrayView<const dcomp,1>{{n},{s},o,xs},MultiArrayView<dcomp,1>{{n},{s},o,ys}); };
      const auto diagonalwise=[&](const std::vector<size_t>& offs){ for (size_t o : offs) diagonalsSlice(A,n,s,xs.data()+o,ys.data()+o); };
      const auto rowwise=[&](const std::vector<size_t>& offs){ for (size_t o : offs) rowsSlice(ts,ptrdiff_t(n),s,xs.data()+o,ys.data()+o); };
      using SMap=Eigen::Map<const CVector,0,Eigen::InnerStride<>>; using DMap=Eigen::Map<CVector,0,Eigen::InnerStride<>>;
      const auto termCSR=[&](const std::vector<size_t>& offs){ for (size_t o : offs) {
        DMap yv(ys.data()+o,Eigen::Index(n),Eigen::InnerStride<>(Eigen::Index(s)));
        yv.noalias()+=As*SMap(xs.data()+o,Eigen::Index(n),Eigen::InnerStride<>(Eigen::Index(s))); } };

      zero(y); library(sliceIteratorOrder); check("D library",toVec(y),ref);
      zero(y); libraryByDiagonals(memoryOrder); check("D library_byDiagonals",toVec(y),ref);
      zero(y); libraryByRows(memoryOrder); check("D library_byRows",toVec(y),ref);
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
      out(c,"library_byDiagonals_memoryOrder",N,perOp([&]{libraryByDiagonals(memoryOrder); doNotOptimize(ys.data());}),nnz);
      out(c,"library_byRows_memoryOrder",N,perOp([&]{libraryByRows(memoryOrder); doNotOptimize(ys.data());}),nnz);
      out(c,"eigen_sparse_kron",N,perOp([&]{ye.noalias()+=S*x; doNotOptimize(ye.data());}),nnz);
    }
  }
}

// ── E: a short axis 0 ────────────────────────────────────────────────────────

void caseE(bool smoke)
{
  for (size_t n0 : smoke ? std::vector<size_t>{1,2,5} : std::vector<size_t>{1,2,3,4,5,6,8,16,32}) {
    const size_t M=(smoke ? 256 : 8192)/n0; const Extents<2> ext{n0,M};
    auto op=tri(n0)*tri(M); auto psi=randomState<2>(ext); MultiArray<dcomp,2> y{ext,multiarray::zeroInit<dcomp>};
    const auto S=toSparse(op,ext); const CVector x=toVec(psi); CVector ye=CVector::Zero(x.size());
    const double nnz=double(S.nonZeros());
    op(0.,psi,y); const CVector ref=S*x; check("E library",toVec(y),ref);
    zero(y); op.applyByDiagonals(psi,y); check("E library_byDiagonals",toVec(y),ref);
    zero(y); op.applyByRows(psi,y); check("E library_byRows",toVec(y),ref);
    zero(y); hoisted(op,psi,y); check("E hoisted",toVec(y),ref);
    char c[32]; std::snprintf(c,sizeof c,"E_axis0_%zu",n0);
    out(c,"library",n0*M,perOp([&]{op(0.,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out(c,"library_byDiagonals",n0*M,perOp([&]{op.applyByDiagonals(psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out(c,"library_byRows",n0*M,perOp([&]{op.applyByRows(psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out(c,"hoisted",n0*M,perOp([&]{hoisted(op,psi,y); doNotOptimize(y.dataView.data());}),nnz);
    out(c,"eigen_sparse",n0*M,perOp([&]{ye.noalias()+=S*x; doNotOptimize(ye.data());}),nnz);
  }
}

// ── F: where the two loop shapes cross over ──────────────────────────────────

void caseF(bool smoke)
{
  // the five-diagonal operator on S contiguous slices of length n of a state {n,S}, one slice per call, as C++QED applies it
  for (size_t total : smoke ? std::vector<size_t>{256} : std::vector<size_t>{2048,1uz<<20})
    for (size_t n : smoke ? std::vector<size_t>{8,30} : std::vector<size_t>{8,16,24,32,48,64,96,128,256}) {
      const size_t S=std::max(total/n,1uz); const Extents<2> ext{n,S};
      auto A=penta(n); auto psi=randomState<2>(ext); MultiArray<dcomp,2> y{ext,multiarray::zeroInit<dcomp>};
      const auto full=copy(A)*quantumoperator::multidiagonal::identity(S);
      const auto Sp=toSparse(full,ext); const CVector ref=Sp*toVec(psi); const double nnz=double(Sp.nonZeros());
      const std::span<const dcomp> xs=psi.dataView; const std::span<dcomp> ys{y.dataStorage()};
      const auto slices=[&](auto apply){ for (size_t b=0; b<S; ++b)
        apply(MultiArrayView<const dcomp,1>{{n},{1},b*n,xs},MultiArrayView<dcomp,1>{{n},{1},b*n,ys}); };
      const auto byDiagonals=[&]{slices([&](auto p, auto q){A.applyByDiagonals(p,q);});};
      const auto byRows=[&]{slices([&](auto p, auto q){A.applyByRows(p,q);});};
      zero(y); byDiagonals(); check("F slices byDiagonals",toVec(y),ref);
      zero(y); byRows(); check("F slices byRows",toVec(y),ref);
      char c[32]; std::snprintf(c,sizeof c,"F_slices_%zu",total);
      std::fprintf(stderr,"%s n=%zu: psi-dpsidt = %zu (mod 4096)\n",c,n,size_t(reinterpret_cast<uintptr_t>(xs.data())-reinterpret_cast<uintptr_t>(ys.data()))%4096);
      out(c,"library_byDiagonals",n,perOp([&]{byDiagonals(); doNotOptimize(ys.data());}),nnz);
      out(c,"library_byRows",n,perOp([&]{byRows(); doNotOptimize(ys.data());}),nnz);
    }
  // nine diagonals with a short contiguous axis 0, the whole operand at once
  for (size_t total : smoke ? std::vector<size_t>{1024} : std::vector<size_t>{8192,65536,1uz<<20})
    for (size_t n0 : smoke ? std::vector<size_t>{5,32} : std::vector<size_t>{5,8,12,16,24,32,48,64,128,256}) {
      const size_t M=total/n0; const Extents<2> ext{n0,M};
      auto op=tri(n0)*tri(M); auto psi=randomState<2>(ext); MultiArray<dcomp,2> y{ext,multiarray::zeroInit<dcomp>};
      const auto Sp=toSparse(op,ext); const CVector ref=Sp*toVec(psi); const double nnz=double(Sp.nonZeros());
      zero(y); op.applyByDiagonals(psi,y); check("F grid byDiagonals",toVec(y),ref);
      zero(y); op.applyByRows(psi,y); check("F grid byRows",toVec(y),ref);
      char c[32]; std::snprintf(c,sizeof c,"F_grid_%zu",total);
      out(c,"library_byDiagonals",n0,perOp([&]{op.applyByDiagonals(psi,y); doNotOptimize(y.dataView.data());}),nnz);
      out(c,"library_byRows",n0,perOp([&]{op.applyByRows(psi,y); doNotOptimize(y.dataView.data());}),nnz);
    }
}

// ── G: placement of psi relative to dpsidt within a page (#25) ────────────────

/// times both loop shapes with dpsidt at the start of a page and psi at every 16-byte offset of its own page
template <size_t R> void scanPlacement(const char* c, const MultiDiagonal<R>& md, Extents<R> ext, bool smoke)
{
  const size_t n=multiarray::calculateExtent(ext), bytes=(n*sizeof(dcomp)+4096+4095)/4096*4096;
  const auto str=multiarray::calculateStrides(ext);
  const auto page=[&]{ return std::unique_ptr<dcomp,decltype(&std::free)>{static_cast<dcomp*>(std::aligned_alloc(4096,bytes)),&std::free}; };
  const auto psiBuffer=page(), yBuffer=page();
  const auto psi0=randomState<R>(ext); const auto S=toSparse(md,ext); const CVector ref=S*toVec(psi0); const double nnz=double(S.nonZeros());
  const MultiArrayView<dcomp,R> y{ext,str,0,std::span<dcomp>{yBuffer.get(),n}};

  for (bool byRows : {false,true}) {
    const char* impl = byRows ? "library_byRows" : "library_byDiagonals";
    std::vector<double> t;
    for (size_t shift=0; shift<4096; shift+=smoke ? 1024 : 16) {
      dcomp* p=psiBuffer.get()+shift/sizeof(dcomp); std::copy_n(psi0.dataView.data(),n,p);
      const MultiArrayView<const dcomp,R> psi{ext,str,0,std::span<const dcomp>{p,n}};
      const auto apply=[&]{ if (byRows) md.applyByRows(psi,y); else md.applyByDiagonals(psi,y); };
      std::fill_n(yBuffer.get(),n,dcomp{}); apply();
      check(c,Eigen::Map<const CVector>(yBuffer.get(),Eigen::Index(n)),ref);
      t.push_back(perOp([&]{apply(); doNotOptimize(yBuffer.get());}));
      out(c,impl,shift,t.back(),nnz);
    }
    std::vector<double> sorted=t; std::sort(sorted.begin(),sorted.end());
    const double best=sorted.front();
    std::fprintf(stderr,"%s %s: best %.5g, median %.5g, worst %.5g ns; %td of %zu offsets more than 5%% above the best\n",c,impl,
                 best,sorted[sorted.size()/2],sorted.back(),std::count_if(t.begin(),t.end(),[=](double v){return v>1.05*best;}),t.size());
  }
}

void caseG(bool smoke)
{
  const double batch=targetBatchNs; targetBatchNs=std::min(targetBatchNs,1e7); // 256 offsets per shape and configuration
  for (size_t N : smoke ? std::vector<size_t>{64} : std::vector<size_t>{64,1024,4096}) {
    char c[32]; std::snprintf(c,sizeof c,"G_rank1_%zu",N);
    scanPlacement(c,penta(N),Extents<1>{N},smoke);
  }
  const size_t N = smoke ? 16 : 2048;
  scanPlacement("G_JC_modefast",jaynesCummings(N,false),Extents<2>{N,2},smoke);
  scanPlacement("G_JC_qbitfast",jaynesCummings(N,true),Extents<2>{2,N},smoke);
  targetBatchNs=batch;
}

// ── H: the distance between psi and dpsidt, out of cache (why case F's slices of power-of-two length were slow) ────

/// anonymous memory of this process on transparent huge pages, in kB (from Linux's /proc; 0 elsewhere)
long anonHugePagesKB()
{
  std::ifstream f{"/proc/self/smaps_rollup"}; std::string key; long v=0;
  while (f >> key) { if (key=="AnonHugePages:") { f >> v; break; } f.ignore(std::numeric_limits<std::streamsize>::max(),'\n'); }
  return v;
}

void caseH(bool smoke)
{
  // case F's five-diagonal operator on contiguous slices of length n, one slice per call, of a state a little under P/16
  // amplitudes. psi starts a 2 MiB-aligned buffer and dpsidt lies D = P + delta bytes after it. Two arrays of exactly 16 MiB
  // allocated back to back give D = 2^24 + 16, as in case F. The buffer is taken once with huge pages requested and once
  // with them refused (madvise, Linux only).
  constexpr size_t huge=1uz<<21;
  const size_t P = smoke ? 1uz<<16 : 1uz<<24, T=(P-8192)/sizeof(dcomp), bytes=(3*P+huge)/huge*huge;
  const std::vector<ptrdiff_t> deltas = smoke ? std::vector<ptrdiff_t>{-16,16,4096+16} :
    std::vector<ptrdiff_t>{-4096,-240,-16,0,16,64,256,1024,2048,4096+16,(1<<16)+16,(1<<20)+16,(1<<21)+16,ptrdiff_t(P)+16};
  for (bool hugePages : {true,false}) {
    const std::unique_ptr<dcomp,decltype(&std::free)> buffer{static_cast<dcomp*>(std::aligned_alloc(huge,bytes)),&std::free};
#ifdef __linux__
    madvise(buffer.get(),bytes,hugePages ? MADV_HUGEPAGE : MADV_NOHUGEPAGE);
#endif
    const long before=anonHugePagesKB(); std::fill_n(buffer.get(),bytes/sizeof(dcomp),dcomp{});
    std::fprintf(stderr,"H %s huge pages: %ld of %zu kB of the buffer on huge pages\n",hugePages ? "with" : "without",anonHugePagesKB()-before,bytes/1024);
    for (size_t n : smoke ? std::vector<size_t>{16} : std::vector<size_t>{16,24,32}) {
      const size_t S=T/n, N=n*S; const Extents<2> ext{n,S};
      auto A=penta(n); const auto psi0=randomState<2>(ext);
      const auto full=copy(A)*quantumoperator::multidiagonal::identity(S);
      const auto Sp=toSparse(full,ext); const CVector ref=Sp*toVec(psi0); const double nnz=double(Sp.nonZeros());
      std::copy_n(psi0.dataView.data(),N,buffer.get()); const std::span<const dcomp> xs{buffer.get(),N};
      for (ptrdiff_t delta : deltas) {
        const std::span<dcomp> ys{buffer.get()+(ptrdiff_t(P)+delta)/ptrdiff_t(sizeof(dcomp)),N};
        const auto slices=[&](auto apply){ for (size_t b=0; b<S; ++b)
          apply(MultiArrayView<const dcomp,1>{{n},{1},b*n,xs},MultiArrayView<dcomp,1>{{n},{1},b*n,ys}); };
        const auto byDiagonals=[&]{slices([&](auto x, auto y){A.applyByDiagonals(x,y);});};
        const auto byRows=[&]{slices([&](auto x, auto y){A.applyByRows(x,y);});};
        const auto validate=[&](const char* what, auto apply){
          std::fill_n(ys.data(),N,dcomp{}); apply(); check(what,Eigen::Map<const CVector>(ys.data(),Eigen::Index(N)),ref); };
        validate("H byDiagonals",byDiagonals); validate("H byRows",byRows);
        char c[48]; std::snprintf(c,sizeof c,"H_%s_2^%d%+td",hugePages ? "huge" : "4k",std::countr_zero(P),delta);
        out(c,"library_byDiagonals",n,perOp([&]{byDiagonals(); doNotOptimize(ys.data());}),nnz);
        out(c,"library_byRows",n,perOp([&]{byRows(); doNotOptimize(ys.data());}),nnz);
      }
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

  static constexpr std::array<std::pair<std::string_view,void(*)(bool)>,8> cases{{{"A",caseA},{"B",caseB},{"C",caseC},{"D",caseD},{"E",caseE},{"F",caseF},{"G",caseG},{"H",caseH}}};

  for (auto sel : only)
    if (std::none_of(cases.begin(),cases.end(),[=](const auto& c){return c.first==sel;})) {
      std::fprintf(stderr,"unknown case '%.*s'; known: A B C D E F G H\n",int(sel.size()),sel.data());
      return 2;
    }

#ifndef NDEBUG
  std::fprintf(stderr,"WARNING: non-Release build — bounds checks active, timings meaningless.\n");
#endif

  if (smoke) targetBatchNs=2e6;

  std::printf("case,impl,N,ns_per_op,ns_per_nnz\n");
  for (const auto& [name,fn] : cases) if (only.empty() ? (name!="G" && name!="H") || smoke : only.contains(name)) fn(smoke);
}
