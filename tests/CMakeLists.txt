# Catch2 v3, pinned to a full commit SHA. An installed Catch2 3 is used when found, so that the tests also configure
# without network, as on cluster compute nodes (#11). Offline without an installed Catch2: FETCHCONTENT_SOURCE_DIR_CATCH2
# pointing at a copy of the source tree (with FETCHCONTENT_FULLY_DISCONNECTED=ON).
include(FetchContent)
FetchContent_Declare(Catch2
  GIT_REPOSITORY    https://github.com/catchorg/Catch2.git
  GIT_TAG           fa43b77429ba76c462b1898d6cd2f2d7a9416b14 # v3.7.1
  FIND_PACKAGE_ARGS 3)
FetchContent_MakeAvailable(Catch2)

add_executable(cppqed-multidiagonal-tests
  testMultiArray.cc
  testMultiArrayComplex.cc
  testMultiDiagonal.cc
)

# The tests write and read Boost.Serialization archives, so they link the compiled library, which the library itself
# doesn't (#16)
find_package(Boost ${MULTIDIAGONAL_BOOST_MINIMUM} REQUIRED COMPONENTS serialization)

target_link_libraries(cppqed-multidiagonal-tests PRIVATE
  cppqed::multidiagonal          # Boost headers, Boost.JSON and Eigen
  Boost::serialization
  Catch2::Catch2WithMain)

target_compile_options(cppqed-multidiagonal-tests PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>)

# Recent Clang (23, not 18) warns under -Wpedantic that __COUNTER__, which every Catch2 TEST_CASE expands, is a C2y
# extension; with warnings as errors that stops the build (#24). The test sources don't use C2y otherwise. Compilers
# without the warning (GCC, older Clang) reject the flag, and the check leaves it out for them.
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wc2y-extensions MULTIDIAGONAL_HAS_WC2Y_EXTENSIONS)
if(MULTIDIAGONAL_HAS_WC2Y_EXTENSIONS)
  target_compile_options(cppqed-multidiagonal-tests PRIVATE -Wno-c2y-extensions)
endif()

if(catch2_SOURCE_DIR)
  list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
else()                           # installed Catch2: its CMake scripts lie next to its package config
  list(APPEND CMAKE_MODULE_PATH ${Catch2_DIR})
endif()
include(Catch)
catch_discover_tests(cppqed-multidiagonal-tests)
