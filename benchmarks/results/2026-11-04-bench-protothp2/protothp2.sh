#!/bin/sh
# cppqed-multidiagonal#26: the allocator in StorageType, prototype 2 (large arrays start at one of 16 positions within a page, in turn) against the
# unchanged code. Modes: base (library 14254a1, C++QED 0a0efb12) on huge pages, proto2 on huge pages, base on 4 KB pages
# (tools/nothp). Workloads: jc_qjmc at cutoffs 2^19 and 523288; kernels B C D F; the main suite's apply1 and applyJC.
# Five interleaved rounds, core 1. Plus one allocation log of jc_qjmc built against the prototype.
cd ~
while pgrep -f "[r]efrun|[r]emeasure.sh|[j]cthp.sh|[p]adthp.sh|[p]rotothp.sh" > /dev/null; do sleep 5; done
set -e
D=$HOME/src/cppqed-deps
cmake -S src/cppqed-multidiagonal-proto2 -B build-proto2 -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_COMPILE_WARNING_AS_ERROR=ON -DBUILD_TESTING=OFF > build-proto2.configure.log 2>&1
cmake --build build-proto2 --target cppqed-multidiagonal-kernels cppqed-multidiagonal-benchmarks > build-proto2.build.log 2>&1
cmake -S src/cppqed-proto -B build-cppqed-proto2 -G Ninja -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTING=OFF -DFETCHCONTENT_FULLY_DISCONNECTED=ON \
  -DFETCHCONTENT_SOURCE_DIR_CPPQED-MULTIDIAGONAL=$HOME/src/cppqed-multidiagonal-proto2 -DFETCHCONTENT_SOURCE_DIR_POPL=$D/popl-src \
  -DFETCHCONTENT_SOURCE_DIR_PCG-CPP=$D/pcg-cpp-src -DFETCHCONTENT_SOURCE_DIR_XOSHIRO-CPP=$D/xoshiro-cpp-src \
  -DFETCHCONTENT_SOURCE_DIR_CATCH2=$HOME/src/catch2-src -DEigen3_DIR=$HOME/opt/eigen-3.4.0/share/eigen3/cmake > build-cppqed-proto2.configure.log 2>&1
cmake --build build-cppqed-proto2 --target jc_qjmc > build-cppqed-proto2.build.log 2>&1
out=runs/protothp2; mkdir -p $out
args="--T 2 --Dt 0.5 --initFockm 1 --deltaq 0 --deltam 0 --kappam 1 --gamma_mq 1"
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)  thp $(cat /sys/kernel/mm/transparent_hugepage/enabled)" > $out/log
LD_PRELOAD=$HOME/tools/bigalloc.so taskset -c 1 build-cppqed-proto2/examples/jc_qjmc --cutoffm 524288 $args > $out/alloc.proto2.out 2> $out/alloc.proto2.log
go() {
  mode=$1; shift
  case $mode in
    base)  b=build-remeasure; j=build-cppqed; w="" ;;
    proto2) b=build-proto2; j=build-cppqed-proto2; w="" ;;
    nothp) b=build-remeasure; j=build-cppqed; w=tools/nothp ;;
  esac
  case $1 in
    jc) shift; taskset -c 1 $w $j/examples/jc_qjmc "$@" ;;
    k)  shift; taskset -c 1 $w $b/benchmarks/cppqed-multidiagonal-kernels "$@" ;;
    m)  shift; taskset -c 1 $w $b/benchmarks/cppqed-multidiagonal-benchmarks "$@" ;;
  esac
}
for i in 1 2 3 4 5; do
  for mode in base proto2 nothp; do
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
