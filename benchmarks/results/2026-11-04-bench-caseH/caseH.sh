#!/bin/sh
# Cases F and H of kernels.cc in one process (F first, so its allocations are as in the runs of 2026-11-04 09:00),
# five processes, core 1. Library as in c346b8a (v0.4.1). stderr carries case F's psi-dpsidt (mod 4096).
cd ~
while pgrep -f "refrun|caseF.sh|caseG.sh|grid.sh|jcc.sh" > /dev/null; do sleep 5; done
set -e
src=src/cppqed-multidiagonal-caseH; b=build-caseH
cmake -S $src -B $b -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_COMPILE_WARNING_AS_ERROR=ON -DBUILD_TESTING=OFF > $b.configure.log 2>&1
cmake --build $b --target cppqed-multidiagonal-kernels > $b.build.log 2>&1
out=runs/caseH; mkdir -p $out
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)" > $out/log
for i in 1 2 3 4 5; do taskset -c 1 $b/benchmarks/cppqed-multidiagonal-kernels F H > $out/kdef.FH.$i.csv 2> $out/kdef.FH.$i.err; echo "$(date +%T) FH $i" >> $out/log; done
echo "done $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)" >> $out/log
