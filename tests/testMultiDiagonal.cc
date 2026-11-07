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


TEST_CASE("a non-const lvalue doesn't match the variadic constructor — #12", "[multidiagonal][contract]")
{
  STATIC_REQUIRE(!std::is_constructible_v<MultiDiagonal<1>, MultiDiagonal<1>&>);
  STATIC_REQUIRE( std::is_constructible_v<MultiDiagonal<1>, MultiDiagonal<1>::Diagonals>);
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


TEST_CASE("a zero-extent diagonal on a non-leading axis is skipped in application — #12", "[multidiagonal][contract]")
{
  using quantumoperator::multidiagonal::identity;
  auto a1 = destroy(1);                               // cutoff 1: |o| = D, no entries
  REQUIRE(a1.diagonals.begin()->second.extents == Extents<1>{0});
  auto op = identity(3)*identity(1) + identity(3)*a1; // the zero-extent diagonal sits on axis 1
  REQUIRE(op.diagonals.size() == 2);
  CHECK(approxEqual(densify(op), CMatrix::Identity(3,3)));

  MultiArray<dcomp,2> psi{{3,1}, multiarray::zeroInit<dcomp>}, d{{3,1}, multiarray::zeroInit<dcomp>};
  psi(1,0)=2.+1i;
  op.applyByRows(psi, d);
  CHECK(d(1,0) == psi(1,0));
}


#ifndef NDEBUG
TEST_CASE("application checks state dimensions in debug", "[multidiagonal]")
{
  auto a = destroy(4);
  MultiArray<dcomp,1> psi{{5}, multiarray::zeroInit<dcomp>}, d{{5}, multiarray::zeroInit<dcomp>};
  CHECK_THROWS_AS(a(0., psi, d), std::runtime_error);
  MultiArray<dcomp,1> psi4{{4}, multiarray::zeroInit<dcomp>};
  CHECK_THROWS_AS(a(0., psi4, d), std::runtime_error);  // dpsidt of the wrong size
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
  CHECK(approxEqual(densify(twoITimesImagPartOf(a)), da - da.adjoint()));
}


TEST_CASE("twoTimesRealPartOf is Hermitian, twoITimesImagPartOf anti-Hermitian, on a complex operator — #22", "[multidiagonal][contract]")
{
  auto c = complexTridiag(4, 0.3);
  const CMatrix dc = densify(c), re = densify(twoTimesRealPartOf(c)), im = densify(twoITimesImagPartOf(c));

  CHECK(approxEqual(re, dc + dc.adjoint()));
  CHECK(approxEqual(im, dc - dc.adjoint()));
  CHECK(approxEqual(re.adjoint(), re));
  CHECK(approxEqual(im.adjoint(), -im));
  CHECK(im.norm() > 1.);                    // the witness has a genuine anti-Hermitian part
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


namespace {

/// The loop shapes of application: as `operator()` chooses, and each one forced
enum class Shape {chosen, byDiagonals, byRows};
constexpr std::array shapes{Shape::chosen, Shape::byDiagonals, Shape::byRows};

template <size_t RANK>
void apply(Shape shape, const MultiDiagonal<RANK>& md, MultiArrayConstView<dcomp,RANK> psi, MultiArrayView<dcomp,RANK> dpsidt)
{
  switch (shape) {
    case Shape::chosen:      md(0., psi, dpsidt); break;
    case Shape::byDiagonals: md.applyByDiagonals(psi, dpsidt); break;
    case Shape::byRows:      md.applyByRows(psi, dpsidt); break;
  }
}

/// Applies @p md through views into larger buffers, each side with its own strides and offset, as when C++QED
/// applies an operator to slices of a state. Returns the result in flat order (axis 0 fastest), and checks that
/// the dpsidt buffer is untouched outside the view.
template <size_t RANK>
CVector applyThroughViews(const MultiDiagonal<RANK>& md, const CVector& psiFlat,
                          Extents<RANK> stridesPsi, size_t offsetPsi, Extents<RANK> stridesDpsidt, size_t offsetDpsidt,
                          Shape shape = Shape::chosen)
{
  const auto ext = calculateAndCheckDimensions(md);
  const auto flat = multiarray::calculateStrides(ext);
  const auto flatIndex = [&] (Extents<RANK> idx) {size_t res=0; for (size_t i=0; i<RANK; ++i) res+=flat[i]*idx[i]; return Eigen::Index(res);};
  const auto bufferSize = [&] (Extents<RANK> str, size_t off) {size_t res=off+1; for (size_t i=0; i<RANK; ++i) res+=str[i]*(ext[i]-1); return res;};

  const dcomp sentinel{-7.,3.};
  std::vector<dcomp> psiBuffer(bufferSize(stridesPsi,offsetPsi),sentinel), dpsidtBuffer(bufferSize(stridesDpsidt,offsetDpsidt),sentinel);
  MultiArrayView<dcomp,RANK> psi{ext,stridesPsi,offsetPsi,std::span<dcomp>{psiBuffer}}, dpsidt{ext,stridesDpsidt,offsetDpsidt,std::span<dcomp>{dpsidtBuffer}};

  for (Extents<RANK> idx{}; idx[0]!=ext[0]; incrementMultiIndex(idx,ext)) {psi(idx)=psiFlat(flatIndex(idx)); dpsidt(idx)=0.;}
  apply<RANK>(shape, md, psi, dpsidt);

  CVector res(psiFlat.size());
  for (Extents<RANK> idx{}; idx[0]!=ext[0]; incrementMultiIndex(idx,ext)) {res(flatIndex(idx))=dpsidt(idx); dpsidt(idx)=sentinel;}
  CHECK(std::ranges::all_of(dpsidtBuffer, [=] (dcomp v) {return v==sentinel;}));
  return res;
}

} // anonymous


TEST_CASE("application through strided, offset views of larger arrays — #1", "[multidiagonal]")
{
  // the non-product rank-3 operator of the test above, extents {3,2,4}
  auto A0 = complexTridiag(3,0.3), A1 = complexTridiag(2,-1.2), A2 = complexTridiag(4,2.1);
  auto B0 = complexTridiag(3,1.7), B1 = complexTridiag(2,0.4),  B2 = complexTridiag(4,-0.8);
  const CMatrix dense = kron(densify(A2), kron(densify(A1), densify(A0))) + kron(densify(B2), kron(densify(B1), densify(B0)));
  auto X = A0 * A1 * A2 + B0 * B1 * B2;

  CVector psi(24);
  for (Eigen::Index k=0; k<psi.size(); ++k) psi(k)=dcomp(std::cos(0.7*double(k)), std::sin(1.3*double(k)));
  const CVector ref = dense*psi;

  const auto agrees = [&] (const CVector& v) {return (v-ref).norm() < tol*ref.norm();};

  // X|X has 75 diagonals, more than applyByRows lists per sweep, and up to 24 (3·2·4) contributing to one row
  auto XX = X | X;
  const CVector refXX = dense*ref;
  const auto agreesXX = [&] (const CVector& v) {return (v-refXX).norm() < tol*refXX.norm();};

  for (Shape shape : shapes) {
    CAPTURE(shape);
    // sub-block of a {5,3,4} array: unit stride along axis 0, gaps between rows
    CHECK(agrees(applyThroughViews(X, psi, {1,5,15}, 7, {1,5,15}, 7, shape)));
    // strided along axis 0, as a slice along an inner axis of a larger state
    CHECK(agrees(applyThroughViews(X, psi, {2,7,15}, 3, {2,7,15}, 3, shape)));
    // different layouts on the two sides: strided psi, contiguous dpsidt, and the converse
    CHECK(agrees(applyThroughViews(X, psi, {3,10,25}, 1, {1,3,6}, 4, shape)));
    CHECK(agrees(applyThroughViews(X, psi, {1,3,6}, 0, {3,10,25}, 2, shape)));
    // last axis fastest in memory
    CHECK(agrees(applyThroughViews(X, psi, {8,4,1}, 0, {8,4,1}, 5, shape)));

    CHECK(agreesXX(applyThroughViews(XX, psi, {1,3,6}, 0, {1,3,6}, 0, shape)));
    CHECK(agreesXX(applyThroughViews(XX, psi, {2,7,15}, 3, {3,10,25}, 1, shape)));
  }
}


TEST_CASE("application with axis 0 of every length, unrolled up to four elements — #23", "[multidiagonal]")
{
  // a tridiagonal on axis 0 has diagonals of length n0 and n0-1: this covers the unrolled lengths 1–4 and the loop;
  // with n0=70 operator() chooses applyByRows also for the contiguous view
  for (size_t n0 : {1uz,2uz,3uz,4uz,5uz,7uz,70uz}) {
    CAPTURE(n0);
    auto A0 = complexTridiag(n0,0.3), A1 = complexTridiag(3,-1.2);
    const CMatrix dense = kron(densify(A1), densify(A0));
    auto X = A0 * A1;
    CHECK(approxEqual(densify(X), dense));          // contiguous

    CVector psi(Eigen::Index(3*n0));
    for (Eigen::Index k=0; k<psi.size(); ++k) psi(k)=dcomp(std::cos(0.7*double(k)), std::sin(1.3*double(k)));
    const CVector ref = dense*psi;
    const auto agrees = [&] (const CVector& v) {return (v-ref).norm() < tol*ref.norm();};

    for (Shape shape : shapes) {
      CAPTURE(shape);
      CHECK(agrees(applyThroughViews(X, psi, {1,n0}, 0, {1,n0}, 0, shape)));                // contiguous
      CHECK(agrees(applyThroughViews(X, psi, {1,n0+2}, 3, {1,n0+1}, 1, shape)));       // unit stride along axis 0, gaps between rows
      CHECK(agrees(applyThroughViews(X, psi, {2,2*n0+1}, 1, {3,3*n0}, 2, shape)));     // strided along axis 0
    }
  }
}


namespace {

/// A complex operator with all 2m+1 diagonals at offsets −m…m on dimension dim, and its dense matrix, built
/// independently from the storage convention d[k] = A(k+o⁻, k+o⁺)
std::pair<MultiDiagonal<1>,CMatrix> band(size_t dim, ptrdiff_t m)
{
  MultiDiagonal<1> md; CMatrix dense{CMatrix::Zero(Eigen::Index(dim),Eigen::Index(dim))};
  for (ptrdiff_t o=-m; o<=m; ++o) {
    const size_t om=size_t(std::max(-o,ptrdiff_t{0})), op=size_t(std::max(o,ptrdiff_t{0}));
    std::vector<dcomp> v(dim-om-op);
    for (size_t k=0; k<v.size(); ++k) dense(Eigen::Index(k+om),Eigen::Index(k+op)) = v[k] = dcomp(std::cos(0.3*double(k)+double(o)), std::sin(0.2*double(k)-double(o)));
    md.diagonals.emplace(MultiDiagonal<1>::Offsets{o}, multiarray::fromStorage(std::move(v)));
  }
  return {std::move(md),dense};
}

} // anonymous


TEST_CASE("application by rows: rows at the edges, and more diagonals per row than summed per store — #1", "[multidiagonal]")
{
  // m=4 gives nine diagonals, more than applyByRows sums per store; for dim ≤ 2m no row has entries in all of them
  for (size_t dim : {1uz,2uz,3uz,5uz,8uz,9uz,70uz,130uz}) {
    const ptrdiff_t m = std::min(ptrdiff_t{4}, ptrdiff_t(dim)-1);
    CAPTURE(dim, m);
    const auto [md, dense] = band(dim, m);

    CVector psi{Eigen::Index(dim)};
    for (Eigen::Index k=0; k<psi.size(); ++k) psi(k)=dcomp(std::cos(0.7*double(k)), std::sin(1.3*double(k)));
    const CVector ref = dense*psi;
    const auto agrees = [&] (const CVector& v) {return (v-ref).norm() < tol*ref.norm();};

    for (Shape shape : shapes) {
      CAPTURE(shape);
      CHECK(agrees(applyThroughViews(md, psi, {1}, 0, {1}, 0, shape)));
      CHECK(agrees(applyThroughViews(md, psi, {3}, 2, {2}, 1, shape)));
    }

    // on axis 0 and on axis 1 of a rank-2 operator, with a complex tridiagonal on the other axis
    auto B = complexTridiag(3,0.8);
    const CMatrix dB = densify(B);
    for (bool first : {true,false}) {
      CAPTURE(first);
      auto X = first ? copy(md)*B : B*copy(md);
      const CMatrix dX = first ? kron(dB,dense) : kron(dense,dB);
      CVector psi2(dX.cols());
      for (Eigen::Index k=0; k<psi2.size(); ++k) psi2(k)=dcomp(std::cos(0.7*double(k)), std::sin(1.3*double(k)));
      const CVector ref2 = dX*psi2;
      const size_t n0 = first ? dim : 3;
      for (Shape shape : shapes) {
        CAPTURE(shape);
        CHECK((applyThroughViews(X, psi2, {1,n0}, 0, {1,n0}, 0, shape)-ref2).norm() < tol*ref2.norm());
        CHECK((applyThroughViews(X, psi2, {2,2*n0+3}, 1, {1,n0+1}, 2, shape)-ref2).norm() < tol*ref2.norm());
      }
    }
  }
}

TEST_CASE("operator() takes one diagonal at a time for slices of up to 512 amplitudes of a state larger than cache — #27", "[multidiagonal]")
{
  // Application accumulates. Onto a nonzero dpsidt the two shapes round differently, one diagonal at a time adding each
  // diagonal's contribution in turn, by rows adding their sum once, so bitwise equality shows which one operator() took: by rows for a slice of 256 amplitudes of a state of 2^12 amplitudes, one diagonal at
  // a time for the same slice of a state of 2^18; by rows for a slice of 768 in either.
  for (size_t dim : {256uz,768uz}) for (size_t storage : {1uz<<12,1uz<<18}) {
    CAPTURE(dim, storage);
    const auto [A, dense] = band(dim,2);
    std::vector<dcomp> psiStorage(storage), dpsidtStorage(storage);
    for (size_t k=0; k<storage; ++k) psiStorage[k]=dcomp(std::cos(0.7*double(k)), std::sin(1.3*double(k)));
    const size_t offset=storage/2;   // a slice in the middle of the state
    const MultiArrayConstView<dcomp,1> psi{{dim},{1},offset,std::span<const dcomp>{psiStorage}};
    const auto applied = [&] (Shape shape) {
      for (size_t k=0; k<storage; ++k) dpsidtStorage[k]=dcomp(1e3+0.1*double(k), std::sin(0.3*double(k)));
      apply(shape, A, psi, MultiArrayView<dcomp,1>{{dim},{1},offset,std::span<dcomp>{dpsidtStorage}});
      return std::vector<dcomp>(dpsidtStorage.begin()+ptrdiff_t(offset), dpsidtStorage.begin()+ptrdiff_t(offset+dim));
    };
    const auto chosen = applied(Shape::chosen), byDiagonals = applied(Shape::byDiagonals), byRows = applied(Shape::byRows);
    REQUIRE(byDiagonals != byRows);   // otherwise the test couldn't tell the shapes apart
    CHECK(chosen == (dim<=512 && storage>=(1uz<<18) ? byDiagonals : byRows));
  }
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