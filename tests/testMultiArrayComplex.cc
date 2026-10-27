// Part of the cppqed-multidiagonal test suite. BSL-1.0.
#include <multidiagonal/MultiArrayComplex.h>

#include <catch2/catch_test_macros.hpp>

using namespace cppqedutils;


namespace {
MultiArray<dcomp,1> carray(std::vector<dcomp> v)
{
  const size_t s = v.size();
  return MultiArray<dcomp,1>{{s}, [&] (size_t) {return std::move(v);}};
}
}


TEST_CASE("scalar helpers", "[complex]")
{
  CHECK(sqr(3.) == 9.);
  CHECK(sqr(1.+1i) == 2i);
  CHECK(sqrAbs(3.+4i) == 25.);
  CHECK(isNonZero(0.+2i)); CHECK_FALSE(isNonZero(dcomp{}));
  CHECK(hasRealPart(1.+0i)); CHECK_FALSE(hasImagPart(1.+0i));
  CHECK(relativeDeviation(3.,1.) == 0.5);
}


TEST_CASE("sqrAbs is exactly re²+im², not the square of the rounded abs — #12", "[complex][contract]")
{
  CHECK(sqrAbs(1.+1i) == 2.); // |1+i| = √2 rounds, and its square is 2.0000000000000004
}


TEST_CASE("relativeDeviation(0,0) returns 0 as documented — finding F6", "[complex][contract]")
{
  CHECK(relativeDeviation(0.,0.) == 0.);
}


TEST_CASE("directProduct: values, layout, custom combiner", "[complex]")
{
  auto m1 = carray({1.+0i, 2.+0i});
  auto m2 = carray({1i, 2i, 3i});
  auto p  = directProduct(m1, m2);
  REQUIRE((p.extents == Extents<2>{2,3}));
  for (size_t i=0;i<2;++i) for (size_t j=0;j<3;++j) CHECK(p(i,j) == m1(i)*m2(j));

  auto d = directProduct(m1, m1, [] (dcomp a, dcomp b) {return a*conj(b);});
  CHECK(d(0,1) == m1(0)*conj(m1(1)));
}


TEST_CASE("directProduct applies the custom combiner to complex input — #4", "[complex]")
{
  auto m1 = carray({1.+1i, 2.-1i});
  auto m2 = carray({1i, 3.+2i, -1.-1i});
  auto d  = directProduct(m1, m2, [] (dcomp a, dcomp b) {return a*conj(b);});
  REQUIRE((d.extents == Extents<2>{2,3}));
  for (size_t i=0;i<2;++i) for (size_t j=0;j<3;++j) {
    CHECK(d(i,j) == m1(i)*conj(m2(j)));
    CHECK(d(i,j) != m1(i)*m2(j));                 // the input is genuinely complex
  }
}


TEST_CASE("in-place conj and Frobenius norm", "[complex]")
{
  auto m = carray({1.+2i, -3i});
  conj(m);
  CHECK(m(0) == 1.-2i); CHECK(m(1) == 3i);

  auto n = carray({3.+0i, 4i});
  CHECK(frobeniusNormSqr(n) == 25.);
  CHECK(frobeniusNorm(n) == 5.);
  MultiArray<dcomp,1> empty{{0}};
  CHECK(frobeniusNormSqr(empty) == 0.);
}


TEST_CASE("vectorize and matricize are no-copy Eigen maps", "[complex][eigen]")
{
  MultiArray<dcomp,2> m{{3,3}, multiarray::zeroInit<dcomp>};
  for (size_t i=0;i<3;++i) for (size_t j=0;j<3;++j) m(i,j)=dcomp(double(i),double(j));

  auto vec = vectorize(m);
  CHECK(vec.data() == m.dataStorage().data());
  vec(0) = 42.;
  CHECK(m(0,0) == dcomp(42.,0.));

  auto mat = matricize(m);
  CHECK(mat.data() == m.dataStorage().data());
  for (size_t i=0;i<3;++i) for (size_t j=0;j<3;++j) CHECK(mat(i,j) == m(i,j));

  // rank-4 {2,3,2,3} viewed as a 6×6 matrix: r = i0 + 2 i1, c = j0 + 2 j1
  MultiArray<dcomp,4> m4{{2,3,2,3}, multiarray::zeroInit<dcomp>};
  m4(1,2,0,1) = 7.;
  auto M = matricize(m4);
  CHECK(M(1+2*2, 0+2*1) == dcomp(7.));
}


TEST_CASE("const vectorize and matricize compile and map read-only — #3", "[complex][eigen][contract]")
{
  MultiArray<dcomp,2> m{{2,2}, multiarray::zeroInit<dcomp>};
  m(0,0)=1.; m(1,0)=2i; m(0,1)=3.; m(1,1)=4i;
  const auto& cm = m;

  auto vec = vectorize(cm);
  STATIC_REQUIRE(std::is_same_v<decltype(vec), Eigen::Map<const CVector>>);
  CHECK(vec.data() == m.dataStorage().data());
  CHECK(vec.sum() == 4.+6i);

  auto mat = matricize(cm);
  STATIC_REQUIRE(std::is_same_v<decltype(mat), Eigen::Map<const CMatrix>>);
  CHECK(mat.data() == m.dataStorage().data());
  CHECK(mat.trace() == 1.+4i);
}


TEST_CASE("halveExtents", "[complex]")
{
  CHECK((halveExtents(Extents<4>{2,3,2,3}) == Extents<2>{2,3}));
#ifndef NDEBUG
  CHECK_THROWS_AS(halveExtents(Extents<2>{2,3}), std::invalid_argument);
#endif
}


TEST_CASE("hermitianConjugateSelf: off-diagonal part", "[complex]")
{
  MultiArray<dcomp,2> m{{3,3}, multiarray::zeroInit<dcomp>};
  for (size_t i=0;i<3;++i) for (size_t j=0;j<3;++j) m(i,j)=dcomp(1.+i, double(j));
  auto h = copy(m);
  hermitianConjugateSelf(h);
  for (size_t i=0;i<3;++i) for (size_t j=0;j<3;++j) if (i!=j) CHECK(h(i,j) == conj(m(j,i)));
}


TEST_CASE("hermitianConjugateSelf conjugates the diagonal — finding F4", "[complex][contract]")
{
  MultiArray<dcomp,2> m{{2,2}, multiarray::zeroInit<dcomp>};
  m(0,0)=1.+2i; m(1,1)=-3i; m(0,1)=1.; m(1,0)=2i;
  hermitianConjugateSelf(m);
  CHECK(m(0,0) == 1.-2i);
  CHECK(m(1,1) == 3i);
}


TEST_CASE("twoTimesRealPartOfSelf: off-diagonal part is M+M† and Hermitian", "[complex]")
{
  MultiArray<dcomp,2> m{{3,3}, multiarray::zeroInit<dcomp>};
  for (size_t i=0;i<3;++i) for (size_t j=0;j<3;++j) m(i,j)=dcomp(1.+2.*i+j, 1.-double(j)+i);
  auto r = copy(m);
  twoTimesRealPartOfSelf(r.mutableView());
  for (size_t i=0;i<3;++i) for (size_t j=i+1;j<3;++j) {
    CHECK(r(i,j) == m(i,j)+conj(m(j,i)));
    CHECK(r(j,i) == conj(r(i,j)));
  }
}


TEST_CASE("twoTimesRealPartOfSelf: diagonal equals 2·Re — finding F5", "[complex][contract]")
{
  MultiArray<dcomp,2> m{{2,2}, multiarray::zeroInit<dcomp>};
  m(0,0)=3.+1i; m(1,1)=-2.+4i;
  auto expected0 = 2.*real(m(0,0)), expected1 = 2.*real(m(1,1));
  twoTimesRealPartOfSelf(m.mutableView());
  CHECK(m(0,0) == dcomp(expected0));   // currently 4·Re as written
  CHECK(m(1,1) == dcomp(expected1));
}


TEST_CASE("JSON round trip for dcomp and complex arrays — finding F7 if this file fails to COMPILE", "[complex][json]")
{
  dcomp c{1.5,-2.5};
  CHECK(json::value_to<dcomp>(json::value_from(c)) == c);
  auto m = carray({1.+2i, 3.-4i});
  auto back = json::value_to<MultiArray<dcomp,1>>(json::value_from(m));
  CHECK(back.dataStorage() == m.dataStorage());
}