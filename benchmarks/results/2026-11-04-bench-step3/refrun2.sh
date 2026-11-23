#!/bin/sh
# Reference measurement for roadmap step 3 on the bench machine: the working tree ("step3") against 7725d79 ("prev"),
# five interleaved rounds, every binary pinned to core 1. Results in ~/runs/step3/, log in ~/runs/step3/log.
set -e
cd ~
src=src/cppqed-multidiagonal-step3; b=build-step3
cmake -S $src -B $b -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_COMPILE_WARNING_AS_ERROR=ON \
  -DFETCHCONTENT_FULLY_DISCONNECTED=ON -DFETCHCONTENT_SOURCE_DIR_CATCH2=$HOME/src/catch2-src > $b.configure.log 2>&1
cmake --build $b > $b.build.log 2>&1
ctest --test-dir $b > $b.ctest.log 2>&1
out=runs/step3; mkdir -p $out
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
