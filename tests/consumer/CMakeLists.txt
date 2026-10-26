# Copyright András Vukics 2006–2026. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt)
#
# Minimal downstream project, built by CI as a separate CMake project. It checks that the
# library is consumable the two ways C++QED and others will use it:
#   -DMULTIDIAGONAL_CONSUME=find_package   after `cmake --install` (pass CMAKE_PREFIX_PATH)
#   -DMULTIDIAGONAL_CONSUME=FetchContent   from MULTIDIAGONAL_GIT_REPOSITORY at MULTIDIAGONAL_GIT_TAG
cmake_minimum_required(VERSION 3.24)

project(cppqed-multidiagonal-consumer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MULTIDIAGONAL_CONSUME "find_package" CACHE STRING "find_package or FetchContent")

if(MULTIDIAGONAL_CONSUME STREQUAL "find_package")
  find_package(cppqed-multidiagonal 0.3 REQUIRED)   # first version with <multidiagonal/X.h>
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
