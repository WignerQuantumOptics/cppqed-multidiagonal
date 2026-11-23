#!/bin/sh
# Reference measurement for roadmap step 3 on the bench machine: fb8ca7f ("step3") against 7725d79 ("prev"),
# five interleaved rounds, every binary pinned to core 1. Results in ~/runs/fb8ca7f/, log in ~/runs/fb8ca7f/log.
set -e
cd ~
src=src/cppqed-multidiagonal-fb8ca7f; b=build-fb8ca7f
cmake -S $src -B $b -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_COMPILE_WARNING_AS_ERROR=ON \
  -DFETCHCONTENT_FULLY_DISCONNECTED=ON -DFETCHCONTENT_SOURCE_DIR_CATCH2=$HOME/src/catch2-src > $b.configure.log 2>&1
cmake --build $b > $b.build.log 2>&1
ctest --test-dir $b > $b.ctest.log 2>&1
out=runs/fb8ca7f; mkdir -p $out
new=$b/benchmarks; prev=build-gcc-Release/benchmarks
run() { f=$1; shift; taskset -c 1 "$@" > $out/$f.csv; echo "$(date +%T) $f" >> $out/log; }
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)" > $out/log
for i in 1 2 3 4 5; do
  run step3.kdef.$i   $new/cppqed-multidiagonal-kernels
  run step3.kcx.$i    $new/cppqed-multidiagonal-kernels-cxlr
  run prev.kdef.$i    $prev/cppqed-multidiagonal-kernels
  run step3.bench.$i  $new/cppqed-multidiagonal-benchmarks
  run prev.bench.$i   $prev/cppqed-multidiagonal-benchmarks apply1 applyJC
done
echo "done $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)" >> $out/log
# placement: repeat the rows scan at N=1024 (one isolated outlier in the first scan), with the code of fb8ca7f
g++ -std=c++23 -O3 -march=native -DNDEBUG -I$src/include -I/usr/include/eigen3 step3/proto/scan.cc -o step3/scan3 -lboost_json -lboost_serialization
for i in 1 2; do taskset -c 1 step3/scan3 apply1 1024 byRows > $out/scan.apply1.1024.byRows.$i.csv; echo "$(date +%T) scan $i" >> $out/log; done
echo "scans done $(date -Is)" >> $out/log
