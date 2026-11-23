#!/bin/sh
# kernels.cc case D on 4 KB pages (--no-thp), default build and -fcx-limited-range build interleaved,
# five rounds, core 1. Source: e90ea79 (v0.5.0 + CITATION.cff). For performance.md's summary of the slice setting.
cd ~
if pgrep -f "[c]ppqed-multidiagonal-kernels|[c]ppqed-multidiagonal-benchmarks|[n]inja" > /dev/null; then echo "busy, not started" > caseD4k.busy; exit 1; fi
set -e
src=src/cppqed-multidiagonal-e90ea79; b=build-e90ea79
cmake -S $src -B $b -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_COMPILE_WARNING_AS_ERROR=ON -DBUILD_TESTING=OFF > $b.configure.log 2>&1
cmake --build $b --target cppqed-multidiagonal-kernels cppqed-multidiagonal-kernels-cxlr > $b.build.log 2>&1
out=runs/caseD4k; mkdir -p $out
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)" > $out/log
for i in 1 2 3 4 5; do
  taskset -c 1 $b/benchmarks/cppqed-multidiagonal-kernels --no-thp D > $out/default.$i.csv 2> $out/default.$i.err
  echo "$(date +%T) default $i" >> $out/log
  taskset -c 1 $b/benchmarks/cppqed-multidiagonal-kernels-cxlr --no-thp D > $out/cxlr.$i.csv 2> $out/cxlr.$i.err
  echo "$(date +%T) cxlr $i" >> $out/log
done
echo "done $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)" >> $out/log
