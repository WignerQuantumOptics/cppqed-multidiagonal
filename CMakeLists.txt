cmake_minimum_required(VERSION 3.24)

# Boost through its own package config (Boost >= 1.70 ships one): CMake 3.30 removed the FindBoost module, and warns
# while this policy is unset
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

project(cppqed-multidiagonal
  VERSION 0.4.2
  DESCRIPTION "Multidiagonal sparse operator algebra for C++QED"
  HOMEPAGE_URL "https://github.com/WignerQuantumOptics/cppqed-multidiagonal"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Dependencies ────────────────────────────────────────────────────────────
# Minimum versions, also checked by the installed package config (#9)
set(MULTIDIAGONAL_BOOST_MINIMUM 1.80)
set(MULTIDIAGONAL_EIGEN_MINIMUM 3.4)

find_package(Boost ${MULTIDIAGONAL_BOOST_MINIMUM} REQUIRED COMPONENTS json serialization)

# Eigen is found without a version and checked afterwards: its package version file accepts only its own major
# version, so a request for 3.4 would reject Eigen 5
find_package(Eigen3 REQUIRED NO_MODULE)
if(Eigen3_VERSION VERSION_LESS MULTIDIAGONAL_EIGEN_MINIMUM)
  message(FATAL_ERROR "cppqed-multidiagonal needs Eigen >= ${MULTIDIAGONAL_EIGEN_MINIMUM}, found ${Eigen3_VERSION}")
endif()

# Boost.Hana is header-only and part of the Boost distribution;
# it is pulled in transitively via Boost::headers.

# ── Library target (header-only) ────────────────────────────────────────────
add_library(cppqed-multidiagonal INTERFACE)

add_library(cppqed::multidiagonal ALIAS cppqed-multidiagonal)
# the installed package must define the same name as the alias (#21)
set_target_properties(cppqed-multidiagonal PROPERTIES EXPORT_NAME multidiagonal)

# A consumer is compiled as C++23 (CMake raises its standard), and a compiler without C++23 stops the configure step
# instead of failing with obscure errors in the headers (#9)
target_compile_features(cppqed-multidiagonal INTERFACE cxx_std_23)

target_include_directories(cppqed-multidiagonal
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(cppqed-multidiagonal
  INTERFACE
    Boost::headers
    Boost::json
    Boost::serialization
    Eigen3::Eigen)

# ── Tests and benchmarks: only in a top-level build ─────────────────────────
if(PROJECT_IS_TOP_LEVEL)
  include(CTest)                 # defines BUILD_TESTING, default ON
  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()
  option(MULTIDIAGONAL_BUILD_BENCHMARKS "Build the benchmarks" ON)
  if(MULTIDIAGONAL_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
  endif()
endif()

# ── Install ─────────────────────────────────────────────────────────────────
# By default only in a top-level build: a project that fetches the library doesn't install it along with itself
# unless it asks for that (#9)
option(MULTIDIAGONAL_INSTALL "Generate the install rules" ${PROJECT_IS_TOP_LEVEL})

if(MULTIDIAGONAL_INSTALL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)

  install(TARGETS cppqed-multidiagonal
    EXPORT cppqed-multidiagonalTargets)

  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  install(EXPORT cppqed-multidiagonalTargets
    FILE cppqed-multidiagonalTargets.cmake
    NAMESPACE cppqed::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cppqed-multidiagonal)

  # While the major version is 0, a new minor version may break the API, so a request for 0.y accepts only 0.y.z (#9).
  # The package is header-only, so it fits any architecture.
  if(PROJECT_VERSION_MAJOR EQUAL 0)
    set(compatibility SameMinorVersion)
  else()
    set(compatibility SameMajorVersion)
  endif()

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cppqed-multidiagonalConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ${compatibility}
    ARCH_INDEPENDENT)

  configure_package_config_file(
    cmake/cppqed-multidiagonalConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/cppqed-multidiagonalConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cppqed-multidiagonal)

  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cppqed-multidiagonalConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cppqed-multidiagonalConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cppqed-multidiagonal)
endif()
