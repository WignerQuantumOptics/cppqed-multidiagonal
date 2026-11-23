#!/bin/sh
# Case H of kernels.cc (distance between psi and dpsidt; huge pages on and off), five processes, core 1. Library as in c346b8a.
cd ~
while pgrep -f "refrun|caseF.sh|caseG.sh|caseH.sh|grid.sh|jcc.sh" > /dev/null; do sleep 5; done
set -e
src=src/cppqed-multidiagonal-caseH; b=build-caseH
cmake --build $b --target cppqed-multidiagonal-kernels > $b.build2.log 2>&1
out=runs/caseH2; mkdir -p $out
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)  thp $(cat /sys/kernel/mm/transparent_hugepage/enabled)" > $out/log
for i in 1 2 3 4 5; do taskset -c 1 $b/benchmarks/cppqed-multidiagonal-kernels H > $out/kdef.H.$i.csv 2> $out/kdef.H.$i.err; echo "$(date +%T) H $i" >> $out/log; done
echo "done $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)" >> $out/log
