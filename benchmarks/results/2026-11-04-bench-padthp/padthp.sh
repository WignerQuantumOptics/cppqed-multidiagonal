#!/bin/sh
# cppqed-multidiagonal#26: does padding large allocations remove the slow distance on huge pages? Three modes per workload:
# thp (as allocated), nothp (tools/nothp: 4 KB pages), pad (huge pages, tools/padalloc.so: +1 KiB on every malloc >= 64 KiB).
# Workloads: jc_qjmc (C++QED 0a0efb12, Eigen 3.4) at cutoffs 2^19 and 523288; kernels B C D F and the main suite (apply1,
# applyJC) of the library at 14254a1 (build-remeasure). Five interleaved rounds, core 1. Plus one allocation log of jc_qjmc
# with padding.
cd ~
while pgrep -f "[r]efrun|[c]aseF.sh|[c]aseG.sh|[c]aseH.sh|[c]aseH2.sh|[r]emeasure.sh|[j]cthp.sh" > /dev/null; do sleep 5; done
set -e
cc -O2 -Wall -Wextra -shared -fPIC -o tools/padalloc.so src/thp-tools/padalloc.c -ldl
out=runs/padthp; mkdir -p $out
j=build-cppqed/examples/jc_qjmc; k=build-remeasure/benchmarks/cppqed-multidiagonal-kernels; m=build-remeasure/benchmarks/cppqed-multidiagonal-benchmarks
args="--T 2 --Dt 0.5 --initFockm 1 --deltaq 0 --deltam 0 --kappam 1 --gamma_mq 1"
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)  thp $(cat /sys/kernel/mm/transparent_hugepage/enabled)" > $out/log
LD_PRELOAD="$HOME/tools/padalloc.so $HOME/tools/bigalloc.so" taskset -c 1 $j --cutoffm 524288 $args > $out/alloc.pad.out 2> $out/alloc.pad.log
go() {
  mode=$1; shift
  case $mode in
    thp)   taskset -c 1 "$@" ;;
    nothp) taskset -c 1 tools/nothp "$@" ;;
    pad)   LD_PRELOAD=$HOME/tools/padalloc.so taskset -c 1 "$@" ;;
  esac
}
for i in 1 2 3 4 5; do
  for mode in thp nothp pad; do
    for n in 524288 523288; do
      t0=$(date +%s%N); go $mode $j --cutoffm $n $args > $out/jc.$mode.$n.$i.out; t1=$(date +%s%N)
      echo "$mode $n $i $(( (t1-t0)/1000000 ))" >> $out/times
    done
    go $mode $k B C D F > $out/kdef.$mode.$i.csv 2> $out/kdef.$mode.$i.err
    go $mode $m apply1 applyJC > $out/bench.$mode.$i.csv 2> $out/bench.$mode.$i.err
  done
  echo "$(date +%T) round $i" >> $out/log
done
echo "done $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)" >> $out/log
