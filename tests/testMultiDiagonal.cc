// Part of the cppqed-multidiagonal test suite. BSL-1.0.
#include <multidiagonal/MultiDiagonal.h>

#include <catch2/catch_test_macros.hpp>

#include <cmath>

using namespace cppqedutils;
using quantumoperator::MultiDiagonal;

namespace {

constexpr double tol = 1e-12;

bool approxEqual(const CMatrix& A, const CMatrix& B)
{
  return A.rows()==B.rows() && A.cols()==B.cols() && (A-B).norm() < tol;
}

bool relApproxEqual(const CMatrix& A, const CMatrix& B)   // for operators with large norms
{
  return A.rows()==B.rows() && A.cols()==B.cols() && (A-B).norm() < tol*B.norm();
}

CMatrix kron(const CMatrix& A, const CMatrix& B)   // A = slow factor, B = fast factor
{
  CMatrix res{A.rows()*B.rows(), A.cols()*B.cols()};
  for (Eigen::Index i=0;i<A.rows();++i) for (Eigen::Index j=0;j<A.cols();++j)
    res.block(i*B.rows(), j*B.cols(), B.rows(), B.cols()) = A(i,j)*B;
  return res;
}

/// The oracle: probe with unit vectors; column ordering = the flat storage ordering (axis 0 fastest).
template <size_t RANK>
CMatrix densify(const MultiDiagonal<RANK>& md)
{
  const auto dims = calculateAndCheckDimensions(md);
  const size_t D = multiarray::calculateExtent(dims);
  CMatrix res{CMatrix::Zero(D,D)};
  MultiArray<dcomp,RANK> psi{dims, multiarray::zeroInit<dcomp>}, dpsidt{dims, multiarray::zeroInit<dcomp>};
  for (size_t k=0; k<D; ++k) {
    psi.dataStorage()[k]=1.;
    for (dcomp& v : dpsidt.dataStorage()) v=0.;
    md(0., psi, dpsidt);
    res.col(k) = vectorize(dpsidt);
    psi.dataStorage()[k]=0.;
  }
  return res;
}

/// Single diagonal at a **signed** offset. Storage convention: `d[k] = A(k+o⁻, k+o⁺)`.
MultiDiagonal<1> singleDiag(ptrdiff_t offset, std::vector<dcomp> v)
{
  MultiDiagonal<1> res;
  res.diagonals.emplace(MultiDiagonal<1>::Offsets{offset}, multiarray::fromStorage(std::move(v)));
  return res;
}

std::vector<dcomp> sqrtRamp(size_t n)               // {√1, √2, …, √n}
{
  std::vector<dcomp> v(n);
  for (size_t k=0;k<n;++k) v[k]=std::sqrt(double(k+1));
  return v;
}

MultiDiagonal<1> destroy(size_t dim)                // a: ⟨n|a|n+1⟩ = √(n+1) — upper, offset +1
{
  return singleDiag(+1, sqrtRamp(dim-1));
}

CMatrix denseDestroy(size_t dim)
{
  CMatrix a{CMatrix::Zero(dim,dim)};
  for (size_t n=0;n+1<dim;++n) a(n,n+1)=std::sqrt(double(n+1));
  return a;
}

MultiDiagonal<1> sigmaMinus() {return singleDiag(+1,{1.});}   // ⟨0|σ⁻|1⟩ = 1

/// Complex tridiagonal on dimension `dim`, all three bands, no element real; `s` varies the values.
MultiDiagonal<1> complexTridiag(size_t dim, double s)
{
  auto band = [=] (size_t n, dcomp c) {
    std::vector<dcomp> v(n);
    for (size_t k=0;k<n;++k) v[k]=c*dcomp(1.+double(k),s-double(k));
    return v;
  };
  return singleDiag(-1,band(dim-1,1i)) + singleDiag(0,band(dim,1.)) + singleDiag(+1,band(dim-1,0.5-1i));
}

CMatrix denseSigmaMinus() {CMatrix s{CMatrix::Zero(2,2)}; s(0,1)=1.; return s;}

} // anonymous


TEST_CASE("MultiDiagonal is move-only", "[multidiagonal]")
{
  STATIC_REQUIRE(!std::is_copy_constructible_v<MultiDiagonal<1>>);
  STATIC_REQUIRE( std::is_move_constructible_v<MultiDiagonal<1>>);
}


// ── representation and application ───────────────────────────────────────────

TEST_CASE("identity densifies to the identity matrix", "[multidiagonal]")
{
  CHECK(approxEqual(densify(quantumoperator::multidiagonal::identity(5)), CMatrix::Identity(5,5)));
}


TEST_CASE("positive offset is the upper diagonal", "[multidiagonal]")
{
  CHECK(approxEqual(densify(destroy(6)), denseDestroy(6)));
}


TEST_CASE("negative offset is the lower diagonal, min(row,col)-anchored", "[multidiagonal]")
{
  // a† has ⟨n+1|a†|n⟩ = √(n+1): lower diagonal, storage d[k] = A(k+1,k) = √(k+1)
  CHECK(approxEqual(densify(singleDiag(-1,sqrtRamp(3))), denseDestroy(4).adjoint()));
}


TEST_CASE("application accumulates and ignores t", "[multidiagonal]")
{
  auto id = quantumoperator::multidiagonal::identity(3);
  MultiArray<dcomp,1> psi{{3}, multiarray::zeroInit<dcomp>}, d{{3}, multiarray::zeroInit<dcomp>};
  psi(1)=2.+1i;
  id(0., psi, d); id(1., psi, d);
  CHECK(d(1) == 2.*psi(1));
}


TEST_CASE("the empty operator is a no-op", "[multidiagonal]")
{
  MultiDiagonal<1> zero;
  MultiArray<dcomp,1> psi{{3}, multiarray::zeroInit<dcomp>}, d{{3}, multiarray::zeroInit<dcomp>};
  psi(0)=1.;
  CHECK_NOTHROW(zero(0., psi, d));
  CHECK(d(0) == 0.);
}


#ifndef NDEBUG
TEST_CASE("application checks state dimensions in debug", "[multidiagonal]")
{
  auto a = destroy(4);
  MultiArray<dcomp,1> psi{{5}, multiarray::zeroInit<dcomp>}, d{{5}, multiarray::zeroInit<dcomp>};
  CHECK_THROWS_AS(a(0., psi, d), std::runtime_error);
}
#endif


TEST_CASE("calculateAndCheckDimensions", "[multidiagonal]")
{
  CHECK((calculateAndCheckDimensions(MultiDiagonal<1>{}) == Extents<1>{}));
  CHECK((calculateAndCheckDimensions(destroy(7)) == Extents<1>{7}));
  CHECK((calculateAndCheckDimensions(singleDiag(-2,sqrtRamp(3))) == Extents<1>{5}));  // extent + |o| = D

  MultiDiagonal<1> bad;
  bad.diagonals.emplace(MultiDiagonal<1>::Offsets{ 1}, multiarray::fromStorage(std::vector<dcomp>(3,1.))); // implies dim 4
  bad.diagonals.emplace(MultiDiagonal<1>::Offsets{-1}, multiarray::fromStorage(std::vector<dcomp>(4,1.))); // implies dim 5
  CHECK_THROWS_AS(calculateAndCheckDimensions(bad), std::runtime_error);
}


// ── Hermitian conjugation ────────────────────────────────────────────────────

TEST_CASE("Hermitian conjugation matches the dense adjoint; involution", "[multidiagonal]")
{
  auto a = destroy(6);
  CHECK(approxEqual(densify(hermitianConjugateOf(a)), denseDestroy(6).adjoint()));
  CHECK(approxEqual(densify(hermitianConjugateOf(hermitianConjugateOf(a))), denseDestroy(6)));
}


TEST_CASE("Hermitian conjugation conjugates complex diagonal values — #4", "[multidiagonal][contract]")
{
  auto a = complexTridiag(4,0.3);
  const CMatrix da = densify(a);
  REQUIRE(da.imag().norm() > 1.);                   // a real operator cannot detect a missing conj
  CHECK(approxEqual(densify(hermitianConjugateOf(a)), da.adjoint()));

  auto x = complexTridiag(3,0.3) * complexTridiag(2,-1.2) * complexTridiag(4,2.1);
  CHECK(approxEqual(densify(hermitianConjugateOf(x)), densify(x).adjoint()));
}


TEST_CASE("conjugation negates the offset key", "[multidiagonal]")
{
  auto ad = hermitianConjugateOf(destroy(4));
  REQUIRE(ad.diagonals.size() == 1);
  CHECK((ad.diagonals.begin()->first == MultiDiagonal<1>::Offsets{-1}));
}


TEST_CASE("addition, subtraction, scalar algebra, real/imag parts", "[multidiagonal]")
{
  auto a = destroy(5);
  const CMatrix da = denseDestroy(5);

  CHECK(approxEqual(densify(a + hermitianConjugateOf(a)), da + da.adjoint()));   // inserts a new diagonal
  CHECK(approxEqual(densify(a + a), 2.*da));                                     // accumulates on an existing one
  CHECK(densify(a - a).norm() < tol);
  CHECK(approxEqual(densify(2.*a/2.), da));
  CHECK(approxEqual(densify(twoTimesRealPartOf(a)), da + da.adjoint()));
  CHECK(approxEqual(densify(twoTimesImagPartOf(a)), da - da.adjoint()));
}


TEST_CASE("copy is deep", "[multidiagonal]")
{
  auto a = destroy(4);
  auto b = copy(a);
  b *= 3.;
  CHECK(approxEqual(densify(a), denseDestroy(4)));
}


// ── composition ──────────────────────────────────────────────────────────────

TEST_CASE("composition: like directions (upper·upper, lower·lower)", "[multidiagonal][composition]")
{
  auto a = destroy(6); auto ad = hermitianConjugateOf(a);
  const CMatrix da = denseDestroy(6);
  CHECK(approxEqual(densify(a | a),   (da*da).eval()));
  CHECK(approxEqual(densify(ad | ad), (da.adjoint()*da.adjoint()).eval()));
}


TEST_CASE("composition: number operator a†|a", "[multidiagonal][composition]")
{
  auto a = destroy(6);
  CHECK(approxEqual(densify(hermitianConjugateOf(a) | a), (denseDestroy(6).adjoint()*denseDestroy(6)).eval()));
}


TEST_CASE("composition: a|a† keeps the full Hilbert dimension — finding F1", "[multidiagonal][composition]")
{
  auto a = destroy(4);
  auto aad = a | hermitianConjugateOf(a);
  CHECK((calculateAndCheckDimensions(aad) == Extents<1>{4}));   // trailing structural zero stored explicitly
  CHECK(approxEqual(densify(aad), (denseDestroy(4)*denseDestroy(4).adjoint()).eval()));
}


TEST_CASE("truncated commutator [a,a†]: identity except the top level", "[multidiagonal][composition]")
{
  const size_t dim=4;
  auto a = destroy(dim); auto ad = hermitianConjugateOf(a);
  const CMatrix da = denseDestroy(dim);
  // = diag(1,…,1,1−dim): truncation necessarily breaks the CCR at the top level,
  // and the library represents the truncated algebra exactly, deviations included.
  CHECK(approxEqual(densify((a|ad)-(ad|a)), (da*da.adjoint()-da.adjoint()*da).eval()));
}


TEST_CASE("composition accumulates contributions across diagonal pairs — finding F1b", "[multidiagonal][composition]")
{
  auto sx = singleDiag(+1,{1.}) + singleDiag(-1,{1.});
  CHECK(approxEqual(densify(sx|sx), CMatrix::Identity(2,2)));   // two pairs merge on s=0

  auto x = destroy(5) + hermitianConjugateOf(destroy(5));
  const CMatrix dx = denseDestroy(5) + denseDestroy(5).adjoint();
  CHECK(approxEqual(densify(x|x), (dx*dx).eval()));             // 4 pairs, 2 merging on s=0
}


TEST_CASE("composition: identity is two-sided neutral on all branches", "[multidiagonal][composition]")
{
  auto a = destroy(5); auto ad = hermitianConjugateOf(a);
  auto id = quantumoperator::multidiagonal::identity(5);
  const CMatrix da = denseDestroy(5);
  CHECK(approxEqual(densify(id | a),  da));
  CHECK(approxEqual(densify(a | id),  da));
  CHECK(approxEqual(densify(id | ad), da.adjoint()));
  CHECK(approxEqual(densify(ad | id), da.adjoint()));
}


TEST_CASE("composition associativity", "[multidiagonal][composition]")
{
  auto a = destroy(6); auto ad = hermitianConjugateOf(a);
  CHECK(approxEqual(densify((ad|a)|a), densify(ad|(a|a))));
  CHECK(approxEqual(densify((a|ad)|a), densify(a|(ad|a))));     // exercises the mixed-direction branches
}


TEST_CASE("composition distributes over addition", "[multidiagonal][composition]")
{
  auto a = destroy(4);
  auto ad = hermitianConjugateOf(destroy(4));
  auto id = quantumoperator::multidiagonal::identity(4);
  CHECK(approxEqual(densify(a|(ad+id)), densify((a|ad)+(a|id))));
  CHECK(approxEqual(densify((ad+id)|a), densify((ad|a)+(id|a))));
}


TEST_CASE("(A|B)† == B†|A† on a non-Hermitian witness", "[multidiagonal][composition]")
{
  auto a = destroy(5);
  auto K = destroy(5) + (1.+2i)*quantumoperator::multidiagonal::identity(5);
  CHECK(approxEqual(densify(hermitianConjugateOf(a|K)),
                    densify(hermitianConjugateOf(K)|hermitianConjugateOf(a))));
}


TEST_CASE("composition beyond the truncation is the zero operator", "[multidiagonal][composition]")
{
  auto a = destroy(3);
  CHECK((a|a).diagonals.size() == 1);        // a²: offset 2 < 3
  CHECK(((a|a)|a).diagonals.empty());        // a³: offset 3 ≥ 3 — structurally zero
  CHECK((MultiDiagonal<1>{} | a).diagonals.empty());
}


// ── direct product ───────────────────────────────────────────────────────────

TEST_CASE("direct product densifies to a Kronecker product (axis 0 fastest)", "[multidiagonal][directproduct]")
{
  auto a  = destroy(3);
  auto id = quantumoperator::multidiagonal::identity(2);
  CHECK(approxEqual(densify(a * id), kron(CMatrix::Identity(2,2), denseDestroy(3))));  // a on axis 0 (fast)
  CHECK(approxEqual(densify(id * a), kron(denseDestroy(3), CMatrix::Identity(2,2))));  // a on axis 1 (slow)
}


TEST_CASE("direct product concatenates the offset arrays", "[multidiagonal][directproduct]")
{
  auto p = destroy(3) * hermitianConjugateOf(destroy(4));
  REQUIRE(p.diagonals.size() == 1);
  CHECK((p.diagonals.begin()->first == MultiDiagonal<2>::Offsets{+1,-1}));
}


TEST_CASE("Jaynes–Cummings coupling σ⁺⊗a + σ⁻⊗a† is Hermitian and correct", "[multidiagonal][directproduct]")
{
  const size_t dim=4;
  auto sminus = sigmaMinus();
  auto a      = destroy(dim);
  auto jc = hermitianConjugateOf(sigmaMinus())*a + sminus*hermitianConjugateOf(destroy(dim));

  const CMatrix ds = denseSigmaMinus(), da = denseDestroy(dim);
  const CMatrix ref = kron(da, ds.adjoint()) + kron(da.adjoint(), ds);  // qubit fast (axis 0), mode slow (axis 1)

  auto dense = densify(jc);
  CHECK(approxEqual(dense, dense.adjoint().eval()));
  CHECK(approxEqual(dense, ref));
}


TEST_CASE("mixed-product law (A*B)|(C*D) == (A|C)*(B|D)", "[multidiagonal][composition][directproduct]")
{
  const size_t dim=4;
  auto A = destroy(dim);                                  // mode, axis 0 (fast)
  auto C = hermitianConjugateOf(destroy(dim));
  auto Sm = sigmaMinus();                                 // qubit, axis 1 (slow)
  auto Sp = hermitianConjugateOf(sigmaMinus());

  auto lhs = (A*Sm)|(C*Sp);
  CHECK(approxEqual(densify(lhs), densify((destroy(dim)|hermitianConjugateOf(destroy(dim)))
                                          * (sigmaMinus()|hermitianConjugateOf(sigmaMinus())))));

  const CMatrix ds = denseSigmaMinus(), da = denseDestroy(dim);
  CHECK(approxEqual(densify(lhs), kron((ds*ds.adjoint()).eval(), (da*da.adjoint()).eval())));
}


TEST_CASE("rank-2 composition is genuinely rank-general", "[multidiagonal][composition]")
{
  // (a ⊗ σ⁻)|(a ⊗ σ⁺) on a 3×2 space: offsets add per axis to (+2, 0)
  auto lhs = (destroy(3)*sigmaMinus()) | (destroy(3)*hermitianConjugateOf(sigmaMinus()));
  const CMatrix da = denseDestroy(3), ds = denseSigmaMinus();
  CHECK(approxEqual(densify(lhs), kron((ds*ds.adjoint()).eval(), (da*da).eval())));
  REQUIRE(lhs.diagonals.size() == 1);
  CHECK((lhs.diagonals.begin()->first == MultiDiagonal<2>::Offsets{+2,0}));
}


TEST_CASE("rank-3 application and composition with colliding offsets — #4", "[multidiagonal][composition][directproduct]")
{
  // extents {3,2,4}: axis 0 fastest, so the dense matrix is kron(axis 2, kron(axis 1, axis 0))
  auto A0 = complexTridiag(3,0.3), A1 = complexTridiag(2,-1.2), A2 = complexTridiag(4,2.1);
  auto B0 = complexTridiag(3,1.7), B1 = complexTridiag(2,0.4),  B2 = complexTridiag(4,-0.8);

  const CMatrix dA = kron(densify(A2), kron(densify(A1), densify(A0)));
  const CMatrix dB = kron(densify(B2), kron(densify(B1), densify(B0)));

  auto A = A0 * A1 * A2;
  auto X = A + B0 * B1 * B2;                        // not a product operator
  REQUIRE(A.diagonals.size() == 27);
  CHECK(approxEqual(densify(A), dA));
  CHECK(approxEqual(densify(X), (dA+dB).eval()));

  auto XX = X | X;                                  // 27×27 pairs onto 5·3·5 offsets (|±2| exceeds axis 1)
  CHECK(XX.diagonals.size() == 75);
  CHECK(relApproxEqual(densify(XX), ((dA+dB)*(dA+dB)).eval()));
  CHECK(relApproxEqual(densify(X | hermitianConjugateOf(X)), ((dA+dB)*(dA+dB).adjoint()).eval()));
}


// ── serialization ────────────────────────────────────────────────────────────

TEST_CASE("JSON serialization of MultiDiagonal — finding F2", "[multidiagonal][json]")
{
  auto md = destroy(3) + hermitianConjugateOf(destroy(3));
  json::value jv;
  REQUIRE_NOTHROW(jv = json::value_from(md));
  REQUIRE(jv.is_array());
  CHECK(jv.as_array().size() == md.diagonals.size());
  // map order is lexicographic in the signed offsets: −1 before +1
  CHECK(json::value_to<std::array<ptrdiff_t,1>>(jv.as_array().at(0).as_object().at("offsets"))[0] == -1);
}