// Part of the cppqed-multidiagonal benchmark suite. BSL-1.0.
// Emits CSV to stdout: benchmark,impl,N,d,time_per_op_ns
// Cross-validates every configuration against Eigen before timing (exit 1 on mismatch).
//
// Benchmarks:
//   apply1            rank-1 application, pentadiagonal, vs Eigen sparse/dense
//   compose1          rank-1 composition (operator*) vs sparse-sparse / dense-dense product
//   construct1        rank-1 construction from scratch vs triplets + makeCompressed
//   applyJC           rank-2 Jaynes–Cummings application, BOTH axis orders:
//                       _qbitfast : qubit = axis 0 → inner loops of length 2
//                       _modefast : mode  = axis 0 → long unit-stride inner loops
//   composeJC         rank-2 composition vs sparse-sparse product
//   constructJC       rank-2 assembly via the operator algebra vs Kronecker triplets (validated against a dense
//                     matrix from the definition up to N = 256)
//   picture1          rank-1 interaction-picture RHS evaluation, harmonic spectrum ω_k = kω. C++QED updates the operator
//                     from (alpha,freqs), then applies it:
//                       multidiagonal_static      the library's operator(), no picture
//                       multidiagonal_scalarfreq  update with one exp per diagonal, then operator()
//                       multidiagonal_tabfreq     update with one exp per element (general spectra), then operator()
//                       handwritten_scalarfreq    the phase fused into a hand-written application loop, one exp per
//                       handwritten_tabfreq         diagonal or per element (the library has no such loop)
//                       eigen_sparse_pernnz       CSR values updated with one exp per nonzero, then Eigen's product
//                       csr_pernnz                the same phases fused into a hand-written loop over Eigen's CSR arrays
//                       eigen_sparse_sandwich     ψ transformed around a static Eigen product; C++QED doesn't do this
//   slices3           a five-diagonal operator of dimension n on axis 0 (contiguous) or axis 2 (stride n²) of an n³
//                     state, applied slice by slice in memory order as C++QED's Broadcaster does; N = n:
//                       multidiagonal             operator() per slice
//                       eigen_sparse_term         Eigen CSR of the term's own n×n space, through strided Maps
//                       multidiagonal_picture_linear     the term updated first, ω_k = kω: one exp per diagonal
//                       multidiagonal_picture_quadratic  ω_k = k²ω: along a diagonal scale and ratio
//                       eigen_sparse_term_picture_pernnz ω_k = k²ω, CSR values updated with one exp per nonzero
//
// picture note: frequency handling deliberately lives OUTSIDE the library (the picture is a property of the trajectory,
// not the operator). The (alpha,freqs) pattern here is built on the public `diagonals` map, i.e. what the physics layer
// does. Its validation doubles as a contract check of that public map.  [contract]
//
// usage: benchmarkMultiDiagonal [--smoke] [--no-thp] [bench ...]
//        bench ∈ {apply1,compose1,construct1,applyJC,composeJC,constructJC,picture1,slices3}; none = all
//        --no-thp keeps the process off transparent huge pages (Linux), where out of cache the distance between input
//        and output arrays can cost up to 3× (kernels.cc case H).
#include <multidiagonal/MultiDiagonal.h>

#include <Eigen/SparseCore>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <random>
#include <set>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

#ifdef __linux__
#include <sys/prctl.h>
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

template <typename F>
double batchNs(F&& f, size_t iters)
{
  const auto t0 = Clock::now();
  for (size_t i=0; i<iters; ++i) f();
  return std::chrono::duration<double,std::nano>(Clock::now()-t0).count();
}

/// doubling calibration to ≥ targetBatchNs, then median of `reps` batches
template <typename F>
double medianPerOpNs(F&& f, double targetBatchNs, size_t reps)
{
  size_t iters=1;
  for (double batch=batchNs(f,iters); batch<targetBatchNs; ) { iters*=2; batch=batchNs(f,iters); }
  std::vector<double> t(reps);
  for (auto& v : t) v = batchNs(f,iters)/double(iters);
  std::nth_element(t.begin(), t.begin()+t.size()/2, t.end());
  return t[t.size()/2];
}

void emit(const char* bench, const char* impl, size_t N, size_t d, double ns)
{
  std::printf("%s,%s,%zu,%zu,%.6g\n", bench, impl, N, d, ns);
  std::fflush(stdout);
}

// ── operator construction (identical coefficients on all sides) ─────────────

dcomp coeff(ptrdiff_t o, size_t k) { return { std::cos(0.1*double(k)+double(o)), std::sin(0.05*double(k)-double(o)) }; }

MultiDiagonal<1> single(ptrdiff_t o, std::vector<dcomp> v)
{
  MultiDiagonal<1> md;
  md.diagonals.emplace(MultiDiagonal<1>::Offsets{o}, multiarray::fromStorage(std::move(v)));
  return md;
}

MultiDiagonal<1> makeMD(size_t N, std::span<const ptrdiff_t> offsets)
{
  MultiDiagonal<1> md;
  for (ptrdiff_t o : offsets) {
    std::vector<dcomp> v(N-size_t(std::abs(o)));
    for (size_t k=0; k<v.size(); ++k) v[k]=coeff(o,k);
    md.diagonals.emplace(MultiDiagonal<1>::Offsets{o}, multiarray::fromStorage(std::move(v)));
  }
  return md;
}

template <int Options>
Eigen::SparseMatrix<dcomp,Options> makeSparse(size_t N, std::span<const ptrdiff_t> offsets)
{
  std::vector<Eigen::Triplet<dcomp>> tr;
  for (ptrdiff_t o : offsets) for (size_t k=0; k<N-size_t(std::abs(o)); ++k)
    tr.emplace_back(int(k+size_t(std::max(-o,ptrdiff_t{0}))),
                    int(k+size_t(std::max( o,ptrdiff_t{0}))),
                    coeff(o,k));
  Eigen::SparseMatrix<dcomp,Options> S(N,N);
  S.setFromTriplets(tr.begin(),tr.end());
  S.makeCompressed();
  return S;
}

/// entries coeff(o,k)·e^{−iωo t} — the operator in the picture of H₀ = ω·n at time t
template <int Options>
Eigen::SparseMatrix<dcomp,Options> makeSparsePhased(size_t N, std::span<const ptrdiff_t> offsets, double omega, double t)
{
  std::vector<Eigen::Triplet<dcomp>> tr;
  for (ptrdiff_t o : offsets) for (size_t k=0; k<N-size_t(std::abs(o)); ++k)
    tr.emplace_back(int(k+size_t(std::max(-o,ptrdiff_t{0}))),
                    int(k+size_t(std::max( o,ptrdiff_t{0}))),
                    coeff(o,k)*std::exp(dcomp(0.,-double(o)*omega*t)));
  Eigen::SparseMatrix<dcomp,Options> S(N,N);
  S.setFromTriplets(tr.begin(),tr.end());
  S.makeCompressed();
  return S;
}

MultiArray<dcomp,1> randomState(size_t N, unsigned seed)
{
  std::mt19937 gen{seed};
  std::uniform_real_distribution<double> d{-1.,1.};
  std::vector<dcomp> v(N);
  for (auto& x : v) x={d(gen),d(gen)};
  return multiarray::fromStorage(std::move(v));
}

MultiArray<dcomp,2> randomState(std::array<size_t,2> ext, unsigned seed)
{
  std::mt19937 gen{seed};
  std::uniform_real_distribution<double> dist{-1.,1.};
  return MultiArray<dcomp,2>{ext,[&](size_t e){ std::vector<dcomp> v(e); for (auto& x : v) x={dist(gen),dist(gen)}; return v; }};
}

void validate(std::string_view what, size_t N, const CVector& got, const CVector& ref)
{
  const double rel = (got-ref).norm()/ref.norm();
  if (!(rel < 1e-10)) {
    std::fprintf(stderr, "VALIDATION FAILED: %s N=%zu relative deviation %g\n", std::string(what).c_str(), N, rel);
    std::exit(1);
  }
}

std::vector<size_t> sweep(bool smoke)
{
  return smoke ? std::vector<size_t>{16,256}
               : std::vector<size_t>{16,64,256,1024,4096,16384,65536,262144,1048576};
}

// ── Jaynes–Cummings builders, parametrized by axis order ─────────────────────
// qubitFast: qubit = axis 0 (extent 2, fast), mode = axis 1;  flat = i_q + 2·i_m
// modeFast : mode  = axis 0 (extent N, fast), qubit = axis 1;  flat = i_m + N·i_q

constexpr double jcDelta=0.7, jcG=1.1;
constexpr dcomp  jcEta{0.3,0.2};

MultiDiagonal<1> ladder(size_t N)
{
  std::vector<dcomp> v(N-1);
  for (size_t k=0; k<N-1; ++k) v[k]=std::sqrt(double(k+1));
  return single(+1,std::move(v));
}

MultiDiagonal<1> number(size_t N)
{
  std::vector<dcomp> v(N);
  for (size_t k=0; k<N; ++k) v[k]=double(k);
  return single(0,std::move(v));
}

MultiDiagonal<2> makeJCmd(size_t N, bool qubitFast)
{
  auto mode = jcDelta*number(N) + jcEta*hermitianConjugateOf(ladder(N)) + conj(jcEta)*ladder(N);
  if (qubitFast)
    return jcG*( single(-1,{1.}) % ladder(N) )                          // σ⁺ ⊗ a
         + jcG*( single(+1,{1.}) % hermitianConjugateOf(ladder(N)) )    // σ⁻ ⊗ a†
         + quantumoperator::multidiagonal::identity(2) % mode;
  else
    return jcG*( ladder(N) % single(-1,{1.}) )                          // a ⊗ σ⁺
         + jcG*( hermitianConjugateOf(ladder(N)) % single(+1,{1.}) )    // a† ⊗ σ⁻
         + mode % quantumoperator::multidiagonal::identity(2);
}

template <int Options>
Eigen::SparseMatrix<dcomp,Options> makeJCsparse(size_t N, bool qubitFast)
{
  struct Ent { int r,c; dcomp v; };
  const std::vector<Ent> q_sp{{1,0,1.}}, q_sm{{0,1,1.}}, q_id{{0,0,1.},{1,1,1.}};
  std::vector<Ent> m_a, m_ad, m_n;
  for (size_t k=0; k<N-1; ++k) { m_a.push_back({int(k),int(k+1),std::sqrt(double(k+1))});
                                 m_ad.push_back({int(k+1),int(k),std::sqrt(double(k+1))}); }
  for (size_t k=0; k<N; ++k) m_n.push_back({int(k),int(k),double(k)});

  auto flat = [qubitFast,N](int q, int m) { return qubitFast ? q+2*m : m+int(N)*q; };
  std::vector<Eigen::Triplet<dcomp>> tr;
  auto addKron = [&] (const std::vector<Ent>& q, const std::vector<Ent>& m, dcomp s) {
    for (const auto& qe : q) for (const auto& me : m)
      tr.emplace_back(flat(qe.r,me.r), flat(qe.c,me.c), s*qe.v*me.v);
  };
  addKron(q_sp,m_a ,jcG); addKron(q_sm,m_ad,jcG);
  addKron(q_id,m_n ,jcDelta); addKron(q_id,m_ad,jcEta); addKron(q_id,m_a,conj(jcEta));

  Eigen::SparseMatrix<dcomp,Options> S(Eigen::Index(2*N),Eigen::Index(2*N));
  S.setFromTriplets(tr.begin(),tr.end());
  S.makeCompressed();
  return S;
}

// ── benchmark: application, rank 1 ──────────────────────────────────────────

void applicationBench(bool smoke)
{
  static constexpr std::array<ptrdiff_t,5> offsets{-2,-1,0,1,2};
  const double target = smoke ? 2e6 : 2e7;  const int reps = smoke ? 2 : 5;
  const size_t denseMax = 4096;

  for (size_t N : sweep(smoke)) {
    auto md  = makeMD(N,offsets);
    auto S   = makeSparse<Eigen::RowMajor>(N,offsets);
    auto psi = randomState(N,12345u);
    MultiArray<dcomp,1> dpsidt{{N},multiarray::zeroInit<dcomp>};
    const CVector x = vectorize(psi);           // copy of the same data for Eigen
    CVector y{CVector::Zero(Eigen::Index(N))};

    md(0.,psi,dpsidt);
    y.noalias() = S*x;
    validate("apply1",N,CVector(vectorize(dpsidt)),y);

    emit("apply1","multidiagonal",N,offsets.size(),
         medianPerOpNs([&]{ md(0.,psi,dpsidt); doNotOptimize(dpsidt.dataView.data()); },target,reps));
    emit("apply1","eigen_sparse",N,offsets.size(),
         medianPerOpNs([&]{ y.noalias() += S*x; doNotOptimize(y.data()); },target,reps));

    if (N<=denseMax) {
      CMatrix D{CMatrix::Zero(Eigen::Index(N),Eigen::Index(N))};
      for (ptrdiff_t o : offsets) for (size_t k=0; k<N-size_t(std::abs(o)); ++k)
        D(Eigen::Index(k+size_t(std::max(-o,ptrdiff_t{0}))),Eigen::Index(k+size_t(std::max(o,ptrdiff_t{0}))))=coeff(o,k);
      emit("apply1","eigen_dense",N,offsets.size(),
           medianPerOpNs([&]{ y.noalias() += D*x; doNotOptimize(y.data()); },target,reps));
    }
  }
}

// ── benchmark: composition, rank 1 ──────────────────────────────────────────

void compositionBench(bool smoke)
{
  static constexpr std::array<ptrdiff_t,3> offsets{-1,0,1};
  const double target = smoke ? 2e6 : 2e7;  const int reps = smoke ? 2 : 5;
  const size_t denseMax = 1024;

  for (size_t N : sweep(smoke)) {
    auto a = makeMD(N,offsets), b = makeMD(N,offsets);
    auto SA = makeSparse<Eigen::ColMajor>(N,offsets); auto SB = SA;
    auto psi = randomState(N,23456u);
    const CVector x = vectorize(psi);

    { // validate (AB)ψ against A·(B·ψ)
      auto c = a*b;
      MultiArray<dcomp,1> dpsidt{{N},multiarray::zeroInit<dcomp>};
      c(0.,psi,dpsidt);
      const CVector ref = SA*(SB*x).eval();
      validate("compose1",N,CVector(vectorize(dpsidt)),ref);
    }

    emit("compose1","multidiagonal",N,offsets.size(),
         medianPerOpNs([&]{ auto c = a*b; doNotOptimize(c.diagonals.size()); },target,reps));
    emit("compose1","eigen_sparse",N,offsets.size(),
         medianPerOpNs([&]{ Eigen::SparseMatrix<dcomp> C = SA*SB; doNotOptimize(C.nonZeros()); },target,reps));

    if (N<=denseMax) {
      CMatrix DA{CMatrix::Zero(Eigen::Index(N),Eigen::Index(N))};
      for (ptrdiff_t o : offsets) for (size_t k=0; k<N-size_t(std::abs(o)); ++k)
        DA(Eigen::Index(k+size_t(std::max(-o,ptrdiff_t{0}))),Eigen::Index(k+size_t(std::max(o,ptrdiff_t{0}))))=coeff(o,k);
      CMatrix C(N,N);   // preallocated — generous to dense
      emit("compose1","eigen_dense",N,offsets.size(),
           medianPerOpNs([&]{ C.noalias() = DA*DA; doNotOptimize(C.data()); },target,reps));
    }
  }
}

// ── benchmark: construction from scratch, rank 1 ────────────────────────────
// Times the full path from coefficients to a ready operator. For sparse this includes
// the triplet fill + setFromTriplets + makeCompressed that a real workflow pays per
// parameter set; MultiDiagonal pays d allocations and map inserts, no symbolic pass.

void constructionBench(bool smoke)
{
  static constexpr std::array<ptrdiff_t,5> offsets{-2,-1,0,1,2};
  const double target = smoke ? 2e6 : 2e7;  const int reps = smoke ? 2 : 5;

  for (size_t N : sweep(smoke)) {
    { // validate one instance of each before timing
      auto md = makeMD(N,offsets);
      auto S  = makeSparse<Eigen::RowMajor>(N,offsets);
      auto psi = randomState(N,56789u);
      MultiArray<dcomp,1> dpsidt{{N},multiarray::zeroInit<dcomp>};
      const CVector x = vectorize(psi);
      CVector y{CVector::Zero(Eigen::Index(N))};
      md(0.,psi,dpsidt);
      y.noalias() = S*x;
      validate("construct1",N,CVector(vectorize(dpsidt)),y);
    }
    emit("construct1","multidiagonal",N,offsets.size(),
         medianPerOpNs([&]{ auto md = makeMD(N,offsets); doNotOptimize(md.diagonals.size()); },target,reps));
    emit("construct1","eigen_sparse",N,offsets.size(),
         medianPerOpNs([&]{ auto S = makeSparse<Eigen::RowMajor>(N,offsets); doNotOptimize(S.nonZeros()); },target,reps));
  }
}

// ── benchmark: application, rank-2 Jaynes–Cummings, both axis orders ────────

void jaynesCummingsBench(bool smoke)
{
  const double target = smoke ? 2e6 : 2e7;  const int reps = smoke ? 2 : 5;

  for (bool qubitFast : {true,false}) {
    const char* name = qubitFast ? "applyJC_qbitfast" : "applyJC_modefast";
    for (size_t N : sweep(smoke)) {
      const std::array<size_t,2> ext = qubitFast ? std::array<size_t,2>{2,N} : std::array<size_t,2>{N,2};
      auto jc  = makeJCmd(N,qubitFast);
      auto S   = makeJCsparse<Eigen::RowMajor>(N,qubitFast);   // flattened to match the layout
      auto psi = randomState(ext,34567u);
      MultiArray<dcomp,2> dpsidt{ext,multiarray::zeroInit<dcomp>};
      const CVector x = vectorize(psi);
      CVector y{CVector::Zero(Eigen::Index(2*N))};

      jc(0.,psi,dpsidt);
      y.noalias() = S*x;
      validate(name,N,CVector(vectorize(dpsidt)),y);

      emit(name,"multidiagonal",N,jc.diagonals.size(),
           medianPerOpNs([&]{ jc(0.,psi,dpsidt); doNotOptimize(dpsidt.dataView.data()); },target,reps));
      emit(name,"eigen_sparse",N,jc.diagonals.size(),
           medianPerOpNs([&]{ y.noalias() += S*x; doNotOptimize(y.data()); },target,reps));
    }
  }
}

// ── benchmark: composition, rank 2 ──────────────────────────────────────────
// The rank-2 counterpart of compose1: JC·JC has 5×5 pairwise offset sums, i.e. the
// algebraic work grows multiplicatively with d while sparse pays the symbolic pattern
// discovery + sort on every product. Also a contract check of rank-general operator*.

void jaynesCummingsCompositionBench(bool smoke)
{
  const double target = smoke ? 2e6 : 2e7;  const int reps = smoke ? 2 : 5;

  for (size_t N : sweep(smoke)) {
    auto jc = makeJCmd(N,false);                        // mode-fast layout
    auto S  = makeJCsparse<Eigen::ColMajor>(N,false);
    auto psi = randomState({N,2},45678u);
    const CVector x = vectorize(psi);

    { // validate (JC·JC)ψ against S·(S·ψ)
      auto c = jc*jc;
      MultiArray<dcomp,2> dpsidt{{N,2},multiarray::zeroInit<dcomp>};
      c(0.,psi,dpsidt);
      const CVector ref = S*(S*x).eval();
      validate("composeJC",N,CVector(vectorize(dpsidt)),ref);
    }

    emit("composeJC","multidiagonal",N,jc.diagonals.size(),
         medianPerOpNs([&]{ auto c = jc*jc; doNotOptimize(c.diagonals.size()); },target,reps));
    emit("composeJC","eigen_sparse",N,jc.diagonals.size(),
         medianPerOpNs([&]{ Eigen::SparseMatrix<dcomp> C = S*S; doNotOptimize(C.nonZeros()); },target,reps));
  }
}

// ── benchmark: rank-2 operator assembly ─────────────────────────────────────
// Times the full JC Hamiltonian build: MultiDiagonal via the algebra (⊗, +, scalar·)
// vs hand-rolled Kronecker triplets + compression. Both builds are validated against each
// other (applied to a random state) before timing, independently of applyJC, and up to
// N = 256 against a dense matrix built from the definition.

/// The Jaynes–Cummings Hamiltonian as a dense matrix, from dense qubit and mode operators, independently of both builders
CMatrix makeJCdense(size_t N, bool qubitFast)
{
  const Eigen::Index n=Eigen::Index(N);
  CMatrix sp=CMatrix::Zero(2,2), sm=CMatrix::Zero(2,2), id=CMatrix::Identity(2,2), a=CMatrix::Zero(n,n), num=CMatrix::Zero(n,n);
  sp(1,0)=1.; sm(0,1)=1.;                                        // σ⁺ = |1⟩⟨0|, σ⁻ = |0⟩⟨1|
  for (Eigen::Index k=0; k+1<n; ++k) a(k,k+1)=std::sqrt(double(k+1));
  for (Eigen::Index k=0; k<n; ++k) num(k,k)=double(k);
  const CMatrix ad=a.adjoint(), mode=jcDelta*num+jcEta*ad+conj(jcEta)*a;
  CMatrix res=CMatrix::Zero(2*n,2*n);
  const auto add=[&](const CMatrix& q, const CMatrix& m, dcomp s) {   // s·q⊗m in the flat index of the axis order
    for (Eigen::Index qr=0; qr<2; ++qr) for (Eigen::Index qc=0; qc<2; ++qc) for (Eigen::Index mr=0; mr<n; ++mr) for (Eigen::Index mc=0; mc<n; ++mc)
      res(qubitFast ? qr+2*mr : mr+n*qr, qubitFast ? qc+2*mc : mc+n*qc) += s*q(qr,qc)*m(mr,mc);
  };
  add(sp,a,jcG); add(sm,ad,jcG); add(id,mode,1.);
  return res;
}

void jaynesCummingsConstructionBench(bool smoke)
{
  const double target = smoke ? 2e6 : 2e7;  const int reps = smoke ? 2 : 5;

  for (size_t N : sweep(smoke)) {
    const size_t d = makeJCmd(N,false).diagonals.size();

    { // validate one instance of each before timing
      auto jc = makeJCmd(N,false);
      auto S  = makeJCsparse<Eigen::RowMajor>(N,false);
      auto psi = randomState({N,2},56789u);
      MultiArray<dcomp,2> dpsidt{{N,2},multiarray::zeroInit<dcomp>};
      jc(0.,psi,dpsidt);
      const CVector ref = S*CVector(vectorize(psi));
      validate("constructJC",N,CVector(vectorize(dpsidt)),ref);
      if (N<=256) validate("constructJC/dense",N,CVector(vectorize(dpsidt)),CVector(makeJCdense(N,false)*CVector(vectorize(psi))));
    }

    emit("constructJC","multidiagonal",N,d,
         medianPerOpNs([&]{ auto jc = makeJCmd(N,false); doNotOptimize(jc.diagonals.size()); },target,reps));
    emit("constructJC","eigen_sparse",N,d,
         medianPerOpNs([&]{ auto S = makeJCsparse<Eigen::RowMajor>(N,false); doNotOptimize(S.nonZeros()); },target,reps));
  }
}

// ── benchmark: interaction-picture RHS evaluation, rank 1 ───────────────────
//
// Free Hamiltonian H₀ = ω·n, harmonic spectrum ω_k = kω. In the picture of H₀ the element (i,j) of a static operator
// acquires e^{i(ω_i−ω_j)t}. Along a diagonal that moves o quanta this is constant, e^{−iωot}: the phase belongs to the
// diagonal, so the operator in the picture costs one exp per diagonal, not per nonzero. The (alpha,freqs) pair below is
// the physics-layer pattern; alpha and freqs share keys by construction, so the ordered maps iterate in lockstep.
//
// C++QED's hot loop updates the operator from (alpha,freqs) and then applies it; it never transforms the state around a
// static product. The library variants do exactly that: rewrite a copy of alpha, then call operator(). The hand-written
// variants fuse the phase into the application loop instead, which the library doesn't offer.

/// y[k+max(−o,0)] += c · v[k] · x[k+max(o,0)]   — one signed-offset-o diagonal
void axpyDiagonal(ptrdiff_t o, const dcomp* v, size_t len, dcomp c, const dcomp* x, dcomp* y)
{
  const dcomp* xs = x + std::max( o,ptrdiff_t{0});
  dcomp*       ys = y + std::max(-o,ptrdiff_t{0});
  for (size_t k=0; k<len; ++k) ys[k] += c*v[k]*xs[k];
}

void pictureBench(bool smoke)
{
  static constexpr std::array<ptrdiff_t,5> offsets{-2,-1,0,1,2};
  const double omega=1.3, tVal=0.3, dt=1e-3;
  const double target = smoke ? 2e6 : 2e7;  const int reps = smoke ? 2 : 5;

  for (size_t N : sweep(smoke)) {
    auto alpha = makeMD(N,offsets);
    auto phased = copy(alpha);   // what the library applies, rewritten from alpha before each evaluation

    // scalar frequencies: δ_o = −iωo, phase(t) = e^{δ_o t} — one dcomp per diagonal
    std::map<MultiDiagonal<1>::Offsets,dcomp> scalarFreqs;
    for (const auto& kv : alpha.diagonals)
      scalarFreqs.emplace(kv.first, dcomp(0.,-omega*double(kv.first[0])));

    // tabulated frequencies: same sparsity as alpha — the layout needed for
    // non-harmonic spectra (e.g. particle: ω_rec(k_j²−(k_j+n)²) varies along a diagonal)
    MultiDiagonal<1> tabFreqs;
    for (const auto& kv : alpha.diagonals) {
      std::vector<dcomp> f(N-size_t(std::abs(kv.first[0])), dcomp(0.,-omega*double(kv.first[0])));
      tabFreqs.diagonals.emplace(kv.first, multiarray::fromStorage(std::move(f)));
    }

    // CSR mirror + parallel per-nonzero frequency array (the v2-Tridiagonal-style cost model);
    // Sp's values are rewritten from S's before each evaluation
    auto S = makeSparse<Eigen::RowMajor>(N,offsets);
    auto Sp = S;
    const std::vector<dcomp> values(S.valuePtr(),S.valuePtr()+S.nonZeros());
    std::vector<dcomp> nnzFreqs(size_t(S.nonZeros()));
    for (Eigen::Index r=0; r<S.outerSize(); ++r)
      for (Eigen::Index p=S.outerIndexPtr()[r]; p<S.outerIndexPtr()[r+1]; ++p)
        nnzFreqs[size_t(p)] = dcomp(0., omega*(double(r)-double(S.innerIndexPtr()[p])));

    auto psi = randomState(N,67890u);
    MultiArray<dcomp,1> ym{{N},multiarray::zeroInit<dcomp>};   // output of the library variants
    const CVector x = vectorize(psi);

    const Eigen::Index n = Eigen::Index(N);
    CVector y     = CVector::Zero(n);
    CVector tmp   = CVector::Zero(n);
    CVector z     = CVector::Zero(n);
    CVector phase = CVector::Zero(n);

    const dcomp* xp = x.data();

    // the library: update the operator, then apply it
    auto mdStatic = [&](double) { alpha(0.,psi,ym); };
    auto mdScalarFreq = [&](double t) {
      auto f = scalarFreqs.cbegin(); auto p = phased.diagonals.begin();
      for (const auto& kv : alpha.diagonals) {
        const dcomp c = std::exp((f++)->second*t);              // ONE exp per diagonal
        std::ranges::transform(kv.second.dataView,(p++)->second.dataStorage().begin(),[c](dcomp v) {return c*v;});
      }
      phased(0.,psi,ym);
    };
    auto mdTabFreq = [&](double t) {
      auto f = tabFreqs.diagonals.cbegin(); auto p = phased.diagonals.begin();
      for (const auto& kv : alpha.diagonals)
        std::ranges::transform(kv.second.dataView,(f++)->second.dataView,(p++)->second.dataStorage().begin(),
                               [t](dcomp v, dcomp w) {return v*std::exp(w*t);});   // one exp per element
      phased(0.,psi,ym);
    };
    // hand-written: the phase fused into the application loop
    auto handScalarFreq = [&](double t) {
      auto fIt = scalarFreqs.cbegin();
      for (auto& kv : alpha.diagonals) {
        const dcomp c = std::exp(fIt->second*t); ++fIt;      // ONE exp per diagonal
        axpyDiagonal(kv.first[0], kv.second.dataView.data(),
                     N-size_t(std::abs(kv.first[0])), c, xp, y.data());
      }
    };
    auto handTabFreq = [&](double t) {
      auto fIt = tabFreqs.diagonals.begin();
      for (auto& kv : alpha.diagonals) {
        const dcomp* v = kv.second.dataView.data();
        const dcomp* f = fIt->second.dataView.data(); ++fIt;
        const dcomp* xs = xp + std::max(kv.first[0],ptrdiff_t{0});
        dcomp* ys = y.data() + std::max(-kv.first[0],ptrdiff_t{0});
        for (size_t k=0, len=N-size_t(std::abs(kv.first[0])); k<len; ++k)
          ys[k] += v[k]*std::exp(f[k]*t)*xs[k];              // one exp per element
      }
    };
    // CSR: Eigen's product after its values are rewritten, one exp per nonzero
    auto eigenPerNnz = [&](double t) {
      dcomp* v = Sp.valuePtr();
      for (size_t p=0; p<values.size(); ++p) v[p] = values[p]*std::exp(nnzFreqs[p]*t);
      y.noalias() += Sp*x;
    };
    // the same phases fused into a hand-written loop over Eigen's CSR arrays
    auto csrPerNnz = [&](double t) {
      const auto* outer=S.outerIndexPtr(); const auto* inner=S.innerIndexPtr(); const dcomp* val=S.valuePtr();
      dcomp* yp=y.data();
      for (Eigen::Index r=0; r<S.outerSize(); ++r) {
        dcomp acc{0.,0.};
        for (Eigen::Index p=outer[r]; p<outer[r+1]; ++p)
          acc += val[p]*std::exp(nnzFreqs[size_t(p)]*t)*xp[inner[p]];   // one exp per nonzero
        yp[r] += acc;
      }
    };
    // the state transformed around a static Eigen product. C++QED doesn't do this (its hot loop updates the operator);
    // it is here for codes that do. p_k = e^{iωkt}: multiplicative recurrence, refreshed by a true exp every 1024
    // elements (bounds drift to ~1e-13). It costs two extra full passes over ψ.
    auto sandwich = [&](double t) {
      const dcomp step = std::exp(dcomp(0.,omega*t));
      dcomp cur{1.,0.};
      dcomp* pp = phase.data();
      for (size_t k=0; k<N; ++k) {
        if ((k&1023)==0) cur = std::exp(dcomp(0.,omega*t*double(k)));
        pp[k]=cur; cur*=step;
      }
      tmp.array() = phase.array().conjugate()*x.array();     // ψ → U†ψ
      z.noalias() = S*tmp;                                   // static apply
      y.array() += phase.array()*z.array();                  // U·(HU†ψ)
    };

    { // validate every strategy against an independently phased sparse apply at t = tVal
      const auto checkMD = [&](const char* impl, auto& eval, double t, const CVector& ref) {
        std::ranges::fill(ym.dataStorage(),dcomp{}); eval(t); validate(impl,N,CVector(vectorize(ym)),ref); };
      const auto check = [&](const char* impl, auto& eval, const CVector& ref) { y.setZero(); eval(tVal); validate(impl,N,y,ref); };
      checkMD("picture1/static",mdStatic,0.,CVector(S*x));
      auto St = makeSparsePhased<Eigen::RowMajor>(N,offsets,omega,tVal);
      const CVector ref = St*x;
      checkMD("picture1/scalarfreq",mdScalarFreq,tVal,ref);
      checkMD("picture1/tabfreq",mdTabFreq,tVal,ref);
      check("picture1/handwritten_scalarfreq",handScalarFreq,ref);
      check("picture1/handwritten_tabfreq",handTabFreq,ref);
      check("picture1/eigen_pernnz",eigenPerNnz,ref);
      check("picture1/csr_pernnz",csrPerNnz,ref);
      check("picture1/sandwich",sandwich,ref);
    }

    auto time = [&](const char* impl, auto& eval) {
      double t=tVal;
      emit("picture1",impl,N,offsets.size(),
           medianPerOpNs([&]{ eval(t); t+=dt; doNotOptimize(y.data()); doNotOptimize(ym.dataView.data()); },target,reps));
    };
    time("multidiagonal_static",    mdStatic);
    time("multidiagonal_scalarfreq",mdScalarFreq);
    time("multidiagonal_tabfreq",   mdTabFreq);
    time("handwritten_scalarfreq",  handScalarFreq);
    time("handwritten_tabfreq",     handTabFreq);
    time("eigen_sparse_pernnz",     eigenPerNnz);
    time("csr_pernnz",              csrPerNnz);
    time("eigen_sparse_sandwich",   sandwich);
  }
}

// ── benchmark: a term applied slice by slice, and the interaction picture there ──
//
// A five-diagonal operator of dimension n on axis 0 (contiguous slices) or axis 2 (stride n²) of an n³ state, applied
// slice by slice in memory order, first dummy axis fastest, as C++QED's Broadcaster does (cppqed#90). The term is small
// and the state large, so the honest competitor for the format is a CSR matrix of the term's own n×n space applied to the
// same slices through strided Maps (kernels.cc case D). The picture variants update the term before each application:
// O(n) work against O(n³) for the application.
//   linear spectrum ω_k = kω: one exp per diagonal;
//   quadratic spectrum ω_k = k²ω (Kerr-like): the element in row r and column r+o acquires e^{−iωo(2r+o)t}, so along a
//   diagonal a geometric sequence, scale e^{−iωo(2r₀+o)t} and ratio e^{−2iωot};
//   CSR, quadratic: one exp per nonzero from a parallel frequency array ω(r²−c²).

void slicesBench(bool smoke)
{
  static constexpr std::array<ptrdiff_t,5> offsets{-2,-1,0,1,2};
  const double omega=1.3, tVal=0.3, dt=1e-3;
  const double target = smoke ? 2e6 : 2e7;  const int reps = smoke ? 2 : 5;
  using SMap=Eigen::Map<const CVector,0,Eigen::InnerStride<>>; using DMap=Eigen::Map<CVector,0,Eigen::InnerStride<>>;
  using RowSparse=Eigen::SparseMatrix<dcomp,Eigen::RowMajor>;

  for (size_t n : smoke ? std::vector<size_t>{8,16} : std::vector<size_t>{16,30,32,64,100}) {
    const size_t N=n*n*n;
    auto alpha = makeMD(n,offsets);
    auto phased = copy(alpha);
    const auto S = makeSparse<Eigen::RowMajor>(n,offsets);
    auto Sp = S;
    const std::vector<dcomp> values(S.valuePtr(),S.valuePtr()+S.nonZeros());
    const auto frequency = [&](bool quadratic, Eigen::Index r, Eigen::Index c) {   // ω_r − ω_c
      const double dr=double(r), dc=double(c); return quadratic ? omega*(dr*dr-dc*dc) : omega*(dr-dc); };
    std::vector<double> nnzFreqs(values.size());
    for (Eigen::Index r=0; r<S.outerSize(); ++r)
      for (Eigen::Index p=S.outerIndexPtr()[r]; p<S.outerIndexPtr()[r+1]; ++p) nnzFreqs[size_t(p)]=frequency(true,r,S.innerIndexPtr()[p]);

    MultiArray<dcomp,3> psi{{n,n,n},[](size_t e) {
      std::mt19937 gen{24680u}; std::uniform_real_distribution<double> d{-1.,1.};
      std::vector<dcomp> v(e); for (auto& x : v) x={d(gen),d(gen)}; return v; }};
    MultiArray<dcomp,3> dpsidt{{n,n,n},multiarray::zeroInit<dcomp>};
    const std::span<const dcomp> xs=psi.dataView; const std::span<dcomp> ys{dpsidt.dataStorage()};

    for (size_t axis : {0uz,2uz}) {
      const size_t s = axis==0 ? 1 : n*n;
      const auto offset = [&](size_t b) {return axis==0 ? b*n : b;};   // slice b in memory order: first dummy axis fastest

      const auto library = [&](const MultiDiagonal<1>& A) { for (size_t b=0; b<n*n; ++b)
        A(0.,MultiArrayView<const dcomp,1>{{n},{s},offset(b),xs},MultiArrayView<dcomp,1>{{n},{s},offset(b),ys}); };
      const auto termCSR = [&](const RowSparse& T) { for (size_t b=0; b<n*n; ++b) {
        DMap yv(ys.data()+offset(b),Eigen::Index(n),Eigen::InnerStride<>(Eigen::Index(s)));
        yv.noalias() += T*SMap(xs.data()+offset(b),Eigen::Index(n),Eigen::InnerStride<>(Eigen::Index(s))); } };

      const auto linear = [&](double t) {
        auto p = phased.diagonals.begin();
        for (const auto& kv : alpha.diagonals) {
          const dcomp c = std::exp(dcomp(0.,-omega*double(kv.first[0])*t));
          std::ranges::transform(kv.second.dataView,(p++)->second.dataStorage().begin(),[c](dcomp v) {return c*v;});
        }
        library(phased);
      };
      const auto quadratic = [&](double t) {
        auto p = phased.diagonals.begin();
        for (const auto& kv : alpha.diagonals) {
          const double o=double(kv.first[0]), r0=double(std::max(-kv.first[0],ptrdiff_t{0}));
          dcomp f = std::exp(dcomp(0.,-omega*o*(2*r0+o)*t)); const dcomp ratio = std::exp(dcomp(0.,-2*omega*o*t));
          auto& dst = (p++)->second.dataStorage();
          for (size_t k=0; k<dst.size(); ++k) { dst[k]=kv.second.dataView[k]*f; f*=ratio; }
        }
        library(phased);
      };
      const auto csrPerNnz = [&](double t) {
        dcomp* v = Sp.valuePtr();
        for (size_t p=0; p<values.size(); ++p) v[p] = values[p]*std::exp(dcomp(0.,nnzFreqs[p]*t));
        termCSR(Sp);
      };

      { // validate against each slice gathered into a contiguous vector and multiplied by a CSR matrix phased element by element
        const auto reference = [&](const RowSparse& T) {
          CVector ref = CVector::Zero(Eigen::Index(N)), xv = CVector::Zero(Eigen::Index(n));
          for (size_t b=0; b<n*n; ++b) {
            for (size_t k=0; k<n; ++k) xv[Eigen::Index(k)] = xs[offset(b)+k*s];
            const CVector yv = T*xv;
            for (size_t k=0; k<n; ++k) ref[Eigen::Index(offset(b)+k*s)] += yv[Eigen::Index(k)];
          }
          return ref;
        };
        const auto phasedCSR = [&](bool quad) {
          RowSparse T = S;
          for (Eigen::Index r=0; r<T.outerSize(); ++r) for (Eigen::Index p=T.outerIndexPtr()[r]; p<T.outerIndexPtr()[r+1]; ++p)
            T.valuePtr()[p] *= std::exp(dcomp(0.,frequency(quad,r,T.innerIndexPtr()[p])*tVal));
          return T;
        };
        const auto check = [&](const char* what, auto apply, const CVector& ref) {
          std::ranges::fill(ys,dcomp{}); apply(); validate(what,n,CVector(Eigen::Map<const CVector>(ys.data(),Eigen::Index(N))),ref); };
        const CVector ref0 = reference(S), refLinear = reference(phasedCSR(false)), refQuadratic = reference(phasedCSR(true));
        check("slices3/multidiagonal",[&]{library(alpha);},ref0);
        check("slices3/eigen_sparse_term",[&]{termCSR(S);},ref0);
        check("slices3/picture_linear",[&]{linear(tVal);},refLinear);
        check("slices3/picture_quadratic",[&]{quadratic(tVal);},refQuadratic);
        check("slices3/picture_pernnz",[&]{csrPerNnz(tVal);},refQuadratic);
      }

      char bench[32]; std::snprintf(bench,sizeof bench,"slices3_axis%zu",axis);
      double t = tVal;
      const auto time = [&](const char* impl, auto f) {
        emit(bench,impl,n,offsets.size(),medianPerOpNs([&]{ f(); doNotOptimize(ys.data()); },target,reps)); };
      time("multidiagonal",                   [&]{library(alpha);});
      time("eigen_sparse_term",               [&]{termCSR(S);});
      time("multidiagonal_picture_linear",    [&]{linear(t); t+=dt;});
      time("multidiagonal_picture_quadratic", [&]{quadratic(t); t+=dt;});
      time("eigen_sparse_term_picture_pernnz",[&]{csrPerNnz(t); t+=dt;});
    }
  }
}

/// --no-thp: keep this process off transparent huge pages, on which the distance between input and output matters out of
/// cache (kernels.cc case H); on 4 KB pages it doesn't. Linux only.
void disableHugePages()
{
#ifdef __linux__
  if (prctl(PR_SET_THP_DISABLE,1,0,0,0)!=0 || prctl(PR_GET_THP_DISABLE,0,0,0,0)!=1) { std::perror("--no-thp: prctl(PR_SET_THP_DISABLE)"); std::exit(2); }
  std::fprintf(stderr,"transparent huge pages disabled for this process\n");
#else
  std::fprintf(stderr,"--no-thp: not supported on this system, ignored\n");
#endif
}

} // anonymous


int main(int argc, char** argv)
{
  bool smoke=false;
  std::set<std::string_view> only;
  for (int i=1; i<argc; ++i) {
    std::string_view a{argv[i]};
    if (a=="--smoke") smoke=true; else if (a=="--no-thp") disableHugePages(); else only.insert(a);
  }

  static constexpr std::array<std::pair<std::string_view,void(*)(bool)>,8> benches{{
    {"apply1",applicationBench}, {"compose1",compositionBench}, {"construct1",constructionBench},
    {"applyJC",jaynesCummingsBench}, {"composeJC",jaynesCummingsCompositionBench},
    {"constructJC",jaynesCummingsConstructionBench}, {"picture1",pictureBench}, {"slices3",slicesBench}}};

  for (auto sel : only)
    if (std::none_of(benches.begin(),benches.end(),[=](const auto& b){return b.first==sel;})) {
      std::fprintf(stderr,"unknown benchmark '%.*s'; known:",int(sel.size()),sel.data());
      for (const auto& b : benches) std::fprintf(stderr," %.*s",int(b.first.size()),b.first.data());
      std::fprintf(stderr,"\n");
      return 2;
    }

#ifndef NDEBUG
  std::fprintf(stderr,"WARNING: non-Release build — bounds checks active, timings meaningless.\n");
#endif

  std::printf("benchmark,impl,N,d,time_per_op_ns\n");
  for (const auto& [name,fn] : benches) if (only.empty() || only.contains(name)) fn(smoke);
}
