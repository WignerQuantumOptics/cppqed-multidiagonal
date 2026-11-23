#!/bin/sh
# cppqed-multidiagonal#5: the reworked picture1, constructJC (dense check) and the new slices3, from the working tree.
# Release, -march=native, 4 KB pages (--no-thp), five processes, core 1.
cd ~
while pgrep -f "[p]27.sh|[p]27b.sh|[p]27c.sh" > /dev/null; do sleep 5; done
set -e
src=src/cppqed-multidiagonal-b5; b=build-b5
cmake -S $src -B $b -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_COMPILE_WARNING_AS_ERROR=ON -DBUILD_TESTING=OFF > $b.configure.log 2>&1
cmake --build $b --target cppqed-multidiagonal-benchmarks > $b.build.log 2>&1
out=runs/b5; mkdir -p $out
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)" > $out/log
for i in 1 2 3 4 5; do
  taskset -c 1 $b/benchmarks/cppqed-multidiagonal-benchmarks --no-thp picture1 constructJC slices3 > $out/bench.$i.csv 2> $out/bench.$i.err
  echo "$(date +%T) run $i" >> $out/log
done
echo "done $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)" >> $out/log
