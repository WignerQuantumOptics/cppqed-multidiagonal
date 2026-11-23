#!/bin/sh
# jc_qjmc (C++QED 0a0efb12, library v0.4.1 headers) with a large mode cutoff, on huge pages and without (tools/nothp):
# cutoff 2^19 (state of exactly 16 MiB) and 523288 (a little less). Allocation logs from tools/bigalloc.so first.
# Eigen 3.4.0 from ~/opt: C++QED requests find_package(Eigen3 3.4), which rejects the system Eigen 5.0.1. Five interleaved rounds, core 1.
cd ~
while pgrep -f "refrun|caseF.sh|caseG.sh|caseH.sh|caseH2.sh|remeasure.sh" > /dev/null; do sleep 5; done
set -e
D=$HOME/src/cppqed-deps; b=build-cppqed
cmake -S src/cppqed-0a0efb12 -B $b -G Ninja -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTING=OFF -DFETCHCONTENT_FULLY_DISCONNECTED=ON \
  -DFETCHCONTENT_SOURCE_DIR_CPPQED-MULTIDIAGONAL=$HOME/src/cppqed-multidiagonal-remeasure -DFETCHCONTENT_SOURCE_DIR_POPL=$D/popl-src \
  -DFETCHCONTENT_SOURCE_DIR_PCG-CPP=$D/pcg-cpp-src -DFETCHCONTENT_SOURCE_DIR_XOSHIRO-CPP=$D/xoshiro-cpp-src \
  -DFETCHCONTENT_SOURCE_DIR_CATCH2=$HOME/src/catch2-src -DEigen3_DIR=$HOME/opt/eigen-3.4.0/share/eigen3/cmake > $b.configure.log 2>&1
cmake --build $b --target jc_qjmc > $b.build.log 2>&1
cc -O2 -Wall -Wextra -o tools/nothp src/thp-tools/nothp.c
cc -O2 -Wall -Wextra -shared -fPIC -o tools/bigalloc.so src/thp-tools/bigalloc.c -ldl
out=runs/jcthp; mkdir -p $out; j=$b/examples/jc_qjmc
args="--T 2 --Dt 0.5 --initFockm 1 --deltaq 0 --deltam 0 --kappam 1 --gamma_mq 1"
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)  thp $(cat /sys/kernel/mm/transparent_hugepage/enabled)" > $out/log
for n in 524288 523288; do LD_PRELOAD=$HOME/tools/bigalloc.so taskset -c 1 $j --cutoffm $n $args > $out/alloc.$n.out 2> $out/alloc.$n.log; done
for i in 1 2 3 4 5; do
  for n in 524288 523288; do
    for m in thp nothp; do
      w=""; [ $m = nothp ] && w=tools/nothp
      t0=$(date +%s%N); taskset -c 1 $w $j --cutoffm $n $args > $out/$m.$n.$i.out; t1=$(date +%s%N)
      echo "$m $n $i $(( (t1-t0)/1000000 ))" >> $out/times
    done
  done
  echo "$(date +%T) round $i" >> $out/log
done
echo "done $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)" >> $out/log
