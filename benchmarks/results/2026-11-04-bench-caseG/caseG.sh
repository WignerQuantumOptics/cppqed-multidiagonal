#!/bin/sh
# Case G of kernels.cc (placement scans, both loop shapes), two processes, core 1. Library as in c346b8a.
cd ~
while pgrep -f "refrun|caseF.sh|grid.sh|jcc.sh" > /dev/null; do sleep 5; done
set -e
src=src/cppqed-multidiagonal-caseG; b=build-caseG
cmake -S $src -B $b -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_COMPILE_WARNING_AS_ERROR=ON -DBUILD_TESTING=OFF > $b.configure.log 2>&1
cmake --build $b --target cppqed-multidiagonal-kernels > $b.build.log 2>&1
out=runs/caseG; mkdir -p $out
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)" > $out/log
for i in 1 2; do taskset -c 1 $b/benchmarks/cppqed-multidiagonal-kernels G > $out/G.$i.csv 2> $out/G.$i.summary; echo "$(date +%T) G $i" >> $out/log; done
echo "done $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)" >> $out/log
