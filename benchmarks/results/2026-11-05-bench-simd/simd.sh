#!/bin/sh
# Instruction-set level: baseline (no -march: SSE2), -march=x86-64-v3 (AVX2+FMA), -march=native (i7-7700: also AVX2+FMA).
# C++QED 0a0efb12 (library v0.4.1 headers, Eigen 3.4): driven jc_qjmc at cutoffs 32, 512, 8192, 131072, compared per derivative
# call. Library 14254a1: kernels A B E and the main suite's apply1 applyJC. All on 4 KB pages (tools/nothp, --no-thp would be
# the same), five interleaved rounds, core 1. Before timing: the regression check of jc_qjmc and a count of AVX instructions.
cd ~
while pgrep -f "[r]efrun|[r]emeasure.sh|[j]cthp.sh|[p]adthp.sh|[p]rotothp|[p]ilot.sh" > /dev/null; do sleep 5; done
set -e
D=$HOME/src/cppqed-deps; L=$HOME/src/cppqed-multidiagonal-remeasure
for lvl in v3 native; do
  case $lvl in v3) f=-march=x86-64-v3 ;; native) f=-march=native ;; esac
  cmake -S src/cppqed-0a0efb12 -B build-cppqed-$lvl -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=$f -DENABLE_TESTING=OFF \
    -DFETCHCONTENT_FULLY_DISCONNECTED=ON -DFETCHCONTENT_SOURCE_DIR_CPPQED-MULTIDIAGONAL=$L -DFETCHCONTENT_SOURCE_DIR_POPL=$D/popl-src \
    -DFETCHCONTENT_SOURCE_DIR_PCG-CPP=$D/pcg-cpp-src -DFETCHCONTENT_SOURCE_DIR_XOSHIRO-CPP=$D/xoshiro-cpp-src \
    -DFETCHCONTENT_SOURCE_DIR_CATCH2=$HOME/src/catch2-src -DEigen3_DIR=$HOME/opt/eigen-3.4.0/share/eigen3/cmake > build-cppqed-$lvl.configure.log 2>&1
  cmake --build build-cppqed-$lvl --target jc_qjmc > build-cppqed-$lvl.build.log 2>&1
done
cmake -S $L -B build-lib-base -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_COMPILE_WARNING_AS_ERROR=ON -DBUILD_TESTING=OFF -DMULTIDIAGONAL_BENCHMARK_NATIVE=OFF > build-lib-base.configure.log 2>&1
cmake --build build-lib-base --target cppqed-multidiagonal-kernels cppqed-multidiagonal-benchmarks > build-lib-base.build.log 2>&1
cmake -S $L -B build-lib-v3 -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_COMPILE_WARNING_AS_ERROR=ON -DBUILD_TESTING=OFF -DMULTIDIAGONAL_BENCHMARK_NATIVE=OFF -DCMAKE_CXX_FLAGS=-march=x86-64-v3 > build-lib-v3.configure.log 2>&1
cmake --build build-lib-v3 --target cppqed-multidiagonal-kernels cppqed-multidiagonal-benchmarks > build-lib-v3.build.log 2>&1
out=runs/simd; mkdir -p $out
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)" > $out/log
for lvl in base v3 native; do
  case $lvl in base) j=build-cppqed ;; *) j=build-cppqed-$lvl ;; esac
  python3 src/cppqed-0a0efb12/tests/regression/compareTrajectory.py $j/examples/jc_qjmc src/cppqed-0a0efb12/tests/regression/jc_qjmc.ref > $out/regression.$lvl.txt 2>&1 && echo "regression $lvl: passed" >> $out/log || echo "regression $lvl: FAILED" >> $out/log
  echo "AVX (ymm) instructions in jc_qjmc $lvl: $(objdump -d --no-show-raw-insn $j/examples/jc_qjmc | grep -c '%ymm')" >> $out/log
done
run() { lvl=$1; shift
  case $lvl in base) j=build-cppqed; b=build-lib-base ;; v3) j=build-cppqed-v3; b=build-lib-v3 ;; native) j=build-cppqed-native; b=build-remeasure ;; esac
  for ct in 32:3000 512:150 8192:2 131072:0.1; do
    cut=${ct%%:*}; T=${ct#*:}
    t0=$(date +%s%N); taskset -c 1 tools/nothp $j/examples/jc_qjmc --cutoffm $cut --T $T --Dt $T --initFockm 1 --deltaq 0 --deltam 0 --kappam 1 --gamma_mq 1 --etam 2 > $out/jc.$lvl.$cut.$i.out; t1=$(date +%s%N)
    echo "$lvl $cut $i $(( (t1-t0)/1000 )) $(grep -o '"nDerivsCalls":[0-9]*' $out/jc.$lvl.$cut.$i.out | cut -d: -f2)" >> $out/times
  done
  taskset -c 1 $b/benchmarks/cppqed-multidiagonal-kernels --no-thp A B E > $out/kdef.$lvl.$i.csv 2> $out/kdef.$lvl.$i.err
  taskset -c 1 $b/benchmarks/cppqed-multidiagonal-benchmarks --no-thp apply1 applyJC > $out/bench.$lvl.$i.csv 2> $out/bench.$lvl.$i.err
}
for i in 1 2 3 4 5; do
  for lvl in base v3 native; do run $lvl; done
  echo "$(date +%T) round $i" >> $out/log
done
echo "done $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)" >> $out/log
