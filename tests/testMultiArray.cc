// Part of the cppqed-multidiagonal test suite. BSL-1.0.
#include <multidiagonal/MultiArray.h>

#include <catch2/catch_test_macros.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <numeric>
#include <sstream>

using namespace cppqedutils;

namespace {

template <size_t RANK>
MultiArray<double,RANK> iota(Extents<RANK> ext)
{
  return MultiArray<double,RANK>{ext, [] (size_t e) {
    std::vector<double> v(e); std::iota(v.begin(),v.end(),0.); return v; }};
}

} // anonymous


TEST_CASE("stride convention: axis 0 is fastest (first-index-fastest storage)", "[multiarray]")
{
  auto m = iota<2>({2,3});
  CHECK((m.strides == Extents<2>{1,2}));
  // flat = i0 + 2*i1
  CHECK(m(0,0)==0.); CHECK(m(1,0)==1.); CHECK(m(0,1)==2.); CHECK(m(1,2)==5.);
}


TEST_CASE("multi-index and variadic subscript agree", "[multiarray]")
{
  auto m = iota<3>({2,3,4});
  for (Extents<3> idx{}; idx[0]!=2; incrementMultiIndex(idx,m.extents))
    CHECK(m(idx) == m(idx[0],idx[1],idx[2]));
}


#ifndef NDEBUG
TEST_CASE("out-of-bounds access throws in debug builds", "[multiarray]")
{
  auto m = iota<2>({2,3});
  CHECK_THROWS_AS(m(2,0), std::range_error);
  CHECK_THROWS_AS(m(0,3), std::range_error);
}
#endif


TEST_CASE("Placeholder row/column slices view the parent data", "[multiarray][slicing]")
{
  auto m = iota<2>({2,3});
  MultiArrayView<double,2> v = m; // Placeholder slicing is reached through a view, not MultiArray directly

  auto row = v(1, multiarray::_);
  REQUIRE((row.extents == Extents<1>{3}));
  for (size_t j=0; j<3; ++j) CHECK(row(j) == m(1,j));

  auto col = v(multiarray::_, 2);
  REQUIRE((col.extents == Extents<1>{2}));
  for (size_t i=0; i<2; ++i) CHECK(col(i) == m(i,2));

  row(0) = 42.;                                   // write-through
  CHECK(m(1,0) == 42.);
}


TEST_CASE("move preserves buffer address and view validity", "[multiarray]")
{
  auto m = iota<2>({2,3});
  const double* buf = m.dataStorage().data();
  MultiArray<double,2> m2{std::move(m)};
  CHECK(m2.dataStorage().data() == buf);
  CHECK(m2.dataView.data() == buf);               // documented invariant: span re-seated/valid after move
  CHECK(m2(1,2) == 5.);
}


TEST_CASE("copy is deep; assignTo copies source into *this; isEqual", "[multiarray]")
{
  auto m = iota<2>({2,3});
  auto c = copy(m);
  CHECK(isEqual(c.constView(), m.constView()));
  c(0,0) = -1.;
  CHECK(m(0,0) == 0.);

  MultiArray<double,2> d{{2,3}};
  d.assignTo(m.constView());
  CHECK(isEqual(d.constView(), m.constView()));
}


TEST_CASE("isEqual detects a single differing element; extent mismatch throws in debug — #4", "[multiarray]")
{
  auto m = iota<2>({2,3});
  auto c = copy(m);
  c(1,2) = -1.;                                    // last element in storage order
  CHECK_FALSE(isEqual(c.constView(), m.constView()));
  c(1,2) = m(1,2); c(0,0) = -1.;                   // first element
  CHECK_FALSE(isEqual(c.constView(), m.constView()));
#ifndef NDEBUG
  auto t = iota<2>({3,2});                         // same size, transposed shape
  CHECK_THROWS_AS(isEqual(t.constView(), m.constView()), std::runtime_error);
#endif
}


TEST_CASE("concatenate std::arrays", "[multiarray]")
{
  constexpr auto r = concatenate(std::array<size_t,2>{1,2}, std::array<size_t,3>{3,4,5});
  STATIC_REQUIRE((r == std::array<size_t,5>{1,2,3,4,5}));
}


TEST_CASE("JSON round trip (double)", "[multiarray][json]")
{
  auto src = iota<2>({2,3});
  auto dst = json::value_to<MultiArray<double,2>>(json::value_from(src));
  CHECK((dst.extents == src.extents));
  CHECK(dst.dataStorage() == src.dataStorage());
}


TEST_CASE("Boost.Serialization round trip restores extents and data", "[multiarray][serialization]")
{
  auto src = iota<2>({2,3});
  std::stringstream ss{std::ios::in|std::ios::out|std::ios::binary};
  { boost::archive::binary_oarchive oa{ss}; oa << src; }
  MultiArray<double,2> dst{{1,1}};
  { boost::archive::binary_iarchive ia{ss}; ia >> dst; }
  CHECK((dst.extents == src.extents));
  CHECK(dst.dataStorage() == src.dataStorage());  // deliberately bypasses dataView (see next test)
}


TEST_CASE("Boost.Serialization round trip restores strides and offset — #4", "[multiarray][serialization]")
{
  auto src = iota<3>({2,3,4});
  std::stringstream ss{std::ios::in|std::ios::out|std::ios::binary};
  { boost::archive::binary_oarchive oa{ss}; oa << src; }
  MultiArray<double,3> dst{{4,1,1}};               // strides {1,4,4} before loading
  dst.offset = 5;                                  // poison: load() must reset it
  { boost::archive::binary_iarchive ia{ss}; ia >> dst; }
  CHECK((dst.strides == src.strides));
  CHECK((dst.strides == Extents<3>{1,2,6}));
  CHECK(dst.offset == 0);
  CHECK(isEqual(dst.constView(), src.constView()));   // element access through strides and offset
}


TEST_CASE("load() re-seats the inherited dataView span — finding F3", "[multiarray][serialization][contract]")
{
  auto src = iota<1>({4});
  std::stringstream ss{std::ios::in|std::ios::out|std::ios::binary};
  { boost::archive::binary_oarchive oa{ss}; oa << src; }
  MultiArray<double,1> dst{{1}};
  { boost::archive::binary_iarchive ia{ss}; ia >> dst; }
  CHECK(dst.dataView.data() == dst.dataStorage().data());
}