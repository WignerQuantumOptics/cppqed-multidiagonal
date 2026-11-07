# Copyright András Vukics 2006–2026. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt)
#
# Minimal downstream project, built by CI as a separate CMake project. It checks that the
# library is consumable the two ways C++QED and others will use it:
#   -DMULTIDIAGONAL_CONSUME=find_package   after `cmake --install` (pass CMAKE_PREFIX_PATH)
#   -DMULTIDIAGONAL_CONSUME=FetchContent   from MULTIDIAGONAL_GIT_REPOSITORY at MULTIDIAGONAL_GIT_TAG
cmake_minimum_required(VERSION 3.24)

# Boost through its own package config (Boost >= 1.70 ships one): CMake 3.30 removed the FindBoost module, and warns
# while this policy is unset
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

project(cppqed-multidiagonal-consumer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MULTIDIAGONAL_CONSUME "find_package" CACHE STRING "find_package or FetchContent")

if(MULTIDIAGONAL_CONSUME STREQUAL "find_package")
  find_package(cppqed-multidiagonal REQUIRED)
  set(major ${cppqed-multidiagonal_VERSION_MAJOR})
  set(minor ${cppqed-multidiagonal_VERSION_MINOR})
  # The package accepts a request for its own minor version, and while the major version is 0 not one for an
  # earlier minor version (#9)
  find_package(cppqed-multidiagonal ${major}.${minor} REQUIRED)
  if(major EQUAL 0 AND minor GREATER 0)
    math(EXPR previous "${minor} - 1")
    find_package(cppqed-multidiagonal 0.${previous} QUIET)
    if(cppqed-multidiagonal_FOUND)
      message(FATAL_ERROR "the installed 0.${minor} accepted a request for 0.${previous}")
    endif()
  endif()
elseif(MULTIDIAGONAL_CONSUME STREQUAL "FetchContent")
  include(FetchContent)
  FetchContent_Declare(cppqed-multidiagonal
    GIT_REPOSITORY ${MULTIDIAGONAL_GIT_REPOSITORY}
    GIT_TAG        ${MULTIDIAGONAL_GIT_TAG})   # a full commit SHA, as consumers pin it
  FetchContent_MakeAvailable(cppqed-multidiagonal)
  if(TARGET cppqed-multidiagonal-tests OR TARGET cppqed-multidiagonal-benchmarks)
    message(FATAL_ERROR "tests/benchmarks must not be built when the library is a subproject")
  endif()
else()
  message(FATAL_ERROR "MULTIDIAGONAL_CONSUME must be find_package or FetchContent")
endif()

add_executable(consumer consumer.cc)
target_link_libraries(consumer PRIVATE cppqed::multidiagonal)
target_compile_options(consumer PRIVATE $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>)
