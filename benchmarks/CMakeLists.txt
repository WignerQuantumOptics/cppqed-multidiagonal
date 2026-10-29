option(MULTIDIAGONAL_BENCHMARK_NATIVE "Compile benchmarks with -march=native" ON)

add_executable(cppqed-multidiagonal-benchmarks benchmarkMultiDiagonal.cc)

# kernels.cc is built twice: with the default complex multiplication (C99 Annex G NaN checks), and without them
add_executable(cppqed-multidiagonal-kernels kernels.cc)
add_executable(cppqed-multidiagonal-kernels-cxlr kernels.cc)
# (Clang 18's driver warns "overriding '' option" for it, spuriously)
target_compile_options(cppqed-multidiagonal-kernels-cxlr PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang>:-fcx-limited-range>
  $<$<CXX_COMPILER_ID:Clang>:-Wno-overriding-option>)

foreach(target cppqed-multidiagonal-benchmarks cppqed-multidiagonal-kernels cppqed-multidiagonal-kernels-cxlr)
  target_link_libraries(${target} PRIVATE cppqed::multidiagonal)
  target_compile_options(${target} PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<AND:$<BOOL:${MULTIDIAGONAL_BENCHMARK_NATIVE}>,$<CXX_COMPILER_ID:GNU,Clang>>:-march=native>)
endforeach()
