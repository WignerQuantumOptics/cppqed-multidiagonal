cmake_minimum_required(VERSION 3.24)

project(cppqed-multidiagonal
  VERSION 0.4.0
  DESCRIPTION "Multidiagonal sparse operator algebra for C++QED"
  HOMEPAGE_URL "https://github.com/WignerQuantumOptics/cppqed-multidiagonal"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Dependencies ────────────────────────────────────────────────────────────
find_package(Boost REQUIRED COMPONENTS json serialization)
find_package(Eigen3 REQUIRED NO_MODULE)

# Boost.Hana is header-only and part of the Boost distribution;
# it is pulled in transitively via Boost::headers.

# ── Library target (header-only) ────────────────────────────────────────────
add_library(cppqed-multidiagonal INTERFACE)

add_library(cppqed::multidiagonal ALIAS cppqed-multidiagonal)
# the installed package must define the same name as the alias (#21)
set_target_properties(cppqed-multidiagonal PROPERTIES EXPORT_NAME multidiagonal)

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

# ── Tests ───────────────────────────────────────────────────────────────────
if(PROJECT_IS_TOP_LEVEL)
  include(CTest)                 # defines BUILD_TESTING, default ON
  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()
  add_subdirectory(benchmarks)
endif()

# ── Install ─────────────────────────────────────────────────────────────────
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

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cppqed-multidiagonalConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  cmake/cppqed-multidiagonalConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/cppqed-multidiagonalConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cppqed-multidiagonal)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/cppqed-multidiagonalConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/cppqed-multidiagonalConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cppqed-multidiagonal)
