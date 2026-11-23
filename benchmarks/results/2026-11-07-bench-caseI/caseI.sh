#!/bin/sh
# cppqed-multidiagonal#27: kernels.cc case I (crossover of the two shapes for contiguous slices over 15 placements within a
# page, in cache and out of cache, 4 KB pages), from the working tree; three processes, core 1.
cd ~
while pgrep -f "[b]ench5.sh|[p]27.sh|[p]27b.sh|[p]27c.sh|[a]llocs.sh" > /dev/null; do sleep 5; done
set -e
src=src/cppqed-multidiagonal-caseI; b=build-caseI
cmake -S $src -B $b -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_COMPILE_WARNING_AS_ERROR=ON -DBUILD_TESTING=OFF > $b.configure.log 2>&1
cmake --build $b --target cppqed-multidiagonal-kernels > $b.build.log 2>&1
out=runs/caseI; mkdir -p $out
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)" > $out/log
for i in 1 2 3; do
  taskset -c 1 $b/benchmarks/cppqed-multidiagonal-kernels I > $out/I.$i.csv 2> $out/I.$i.err
  echo "$(date +%T) I $i" >> $out/log
done
echo "done $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)" >> $out/log
