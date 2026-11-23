#!/bin/sh
# cppqed-multidiagonal#26: general-purpose allocators against our 16-position prototype. Modes, all on huge pages except nothp:
# base (glibc, library 14254a1, C++QED 0a0efb12), proto2 (16-position allocator in StorageType), jemalloc 5.4.0 and tcmalloc
# (gperftools 2.19, minimal) preloaded into the base binaries, nothp (glibc on 4 KB pages, the reference).
# First, allocation logs of jc_qjmc at cutoff 2^19 under jemalloc (cache_oblivious on and off) and tcmalloc.
# Workloads: jc_qjmc at cutoffs 2^19 and 523288; kernels B C D F; the main suite's apply1 and applyJC. Five rounds, core 1.
cd ~
while pgrep -f "[r]emeasure.sh|[j]cthp.sh|[p]adthp.sh|[p]rotothp|[s]imd.sh" > /dev/null; do sleep 5; done
set -e
JE=/usr/lib/libjemalloc.so.2; TC=/usr/lib/libtcmalloc_minimal.so.4
out=runs/allocs; mkdir -p $out
args="--T 2 --Dt 0.5 --initFockm 1 --deltaq 0 --deltam 0 --kappam 1 --gamma_mq 1"
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)  thp $(cat /sys/kernel/mm/transparent_hugepage/enabled)" > $out/log
LD_PRELOAD="$HOME/tools/bigalloc.so $JE" taskset -c 1 build-cppqed/examples/jc_qjmc --cutoffm 524288 $args > $out/alloc.jemalloc.out 2> $out/alloc.jemalloc.log
MALLOC_CONF=cache_oblivious:false LD_PRELOAD="$HOME/tools/bigalloc.so $JE" taskset -c 1 build-cppqed/examples/jc_qjmc --cutoffm 524288 $args > $out/alloc.jemalloc-nco.out 2> $out/alloc.jemalloc-nco.log
LD_PRELOAD="$HOME/tools/bigalloc.so $TC" taskset -c 1 build-cppqed/examples/jc_qjmc --cutoffm 524288 $args > $out/alloc.tcmalloc.out 2> $out/alloc.tcmalloc.log
go() {
  mode=$1; shift
  b=build-remeasure; j=build-cppqed; w=""; pre=""
  case $mode in
    proto2)   b=build-proto2; j=build-cppqed-proto2 ;;
    jemalloc) pre=$JE ;;
    tcmalloc) pre=$TC ;;
    nothp)    w=tools/nothp ;;
  esac
  case $1 in
    jc) shift; LD_PRELOAD=$pre taskset -c 1 $w $j/examples/jc_qjmc "$@" ;;
    k)  shift; LD_PRELOAD=$pre taskset -c 1 $w $b/benchmarks/cppqed-multidiagonal-kernels "$@" ;;
    m)  shift; LD_PRELOAD=$pre taskset -c 1 $w $b/benchmarks/cppqed-multidiagonal-benchmarks "$@" ;;
  esac
}
for i in 1 2 3 4 5; do
  for mode in base proto2 jemalloc tcmalloc nothp; do
    for n in 524288 523288; do
      t0=$(date +%s%N); go $mode jc --cutoffm $n $args > $out/jc.$mode.$n.$i.out; t1=$(date +%s%N)
      echo "$mode $n $i $(( (t1-t0)/1000000 ))" >> $out/times
    done
    go $mode k B C D F > $out/kdef.$mode.$i.csv 2> $out/kdef.$mode.$i.err
    go $mode m apply1 applyJC > $out/bench.$mode.$i.csv 2> $out/bench.$mode.$i.err
  done
  echo "$(date +%T) round $i" >> $out/log
done
echo "done $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)" >> $out/log
