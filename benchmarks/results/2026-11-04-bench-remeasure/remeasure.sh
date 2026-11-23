#!/bin/sh
# Out-of-cache figures re-measured with transparent huge pages on (as before) and off (--no-thp, where the distance between
# input and output doesn't matter, kernels.cc case H). Five interleaved rounds, core 1. Library as in c346b8a (v0.4.1).
cd ~
while pgrep -f "refrun|caseF.sh|caseG.sh|caseH.sh|caseH2.sh|grid.sh|jcc.sh" > /dev/null; do sleep 5; done
set -e
src=src/cppqed-multidiagonal-remeasure; b=build-remeasure
cmake -S $src -B $b -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_COMPILE_WARNING_AS_ERROR=ON -DBUILD_TESTING=OFF > $b.configure.log 2>&1
cmake --build $b --target cppqed-multidiagonal-kernels cppqed-multidiagonal-benchmarks > $b.build.log 2>&1
out=runs/remeasure; mkdir -p $out; k=$b/benchmarks/cppqed-multidiagonal-kernels; m=$b/benchmarks/cppqed-multidiagonal-benchmarks
run() { f=$1; shift; taskset -c 1 "$@" > $out/$f.csv 2> $out/$f.err; echo "$(date +%T) $f" >> $out/log; }
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)  thp $(cat /sys/kernel/mm/transparent_hugepage/enabled)" > $out/log
for i in 1 2 3 4 5; do
  run kdef.thp.$i   $k
  run kdef.nothp.$i $k --no-thp
  run bench.thp.$i   $m apply1 applyJC
  run bench.nothp.$i $m --no-thp apply1 applyJC
done
echo "done $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)" >> $out/log
