#!/bin/sh
# Case F of kernels.cc (the working tree after fb8ca7f; the library is unchanged): five processes, core 1.
# Waits until the reference run of fb8ca7f has finished, also before building.
cd ~
while pgrep -f refrun3.sh > /dev/null; do sleep 5; done
set -e
src=src/cppqed-multidiagonal-caseF; b=build-caseF
cmake -S $src -B $b -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_COMPILE_WARNING_AS_ERROR=ON -DBUILD_TESTING=OFF > $b.configure.log 2>&1
cmake --build $b --target cppqed-multidiagonal-kernels > $b.build.log 2>&1
out=runs/caseF; mkdir -p $out
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)" > $out/log
for i in 1 2 3 4 5; do taskset -c 1 $b/benchmarks/cppqed-multidiagonal-kernels F > $out/kdef.F.$i.csv; echo "$(date +%T) F $i" >> $out/log; done
echo "done $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)" >> $out/log
