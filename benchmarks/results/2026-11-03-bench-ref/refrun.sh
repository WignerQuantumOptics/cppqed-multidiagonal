#!/bin/sh
# Reference measurement for benchmarks/performance.md on the bench machine: five interleaved rounds,
# every binary pinned to core 1 (no device interrupts there). Results in ~/runs/ref/, log in ~/runs/ref/log.
set -e
cd ~
out=runs/ref; mkdir -p $out
now=build-gcc-Release/benchmarks; pre23=build-pre23/benchmarks; v040=build-v040/benchmarks
run() { f=$1; shift; taskset -c 1 "$@" > $out/$f.csv; echo "$(date +%T) $f" >> $out/log; }
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)" > $out/log
for i in 1 2 3 4 5; do
  run now.kdef.$i     $now/cppqed-multidiagonal-kernels
  run now.kcx.$i      $now/cppqed-multidiagonal-kernels-cxlr
  run pre23.kdef.$i   $pre23/cppqed-multidiagonal-kernels A B C
  run pre23.kcx.$i    $pre23/cppqed-multidiagonal-kernels-cxlr A B
  run v040.kdef.$i    $v040/cppqed-multidiagonal-kernels B C
  run now.bench.$i    $now/cppqed-multidiagonal-benchmarks
  run pre23.bench.$i  $pre23/cppqed-multidiagonal-benchmarks apply1 applyJC
  run v040.bench.$i   $v040/cppqed-multidiagonal-benchmarks apply1 applyJC
done
echo "done $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)" >> $out/log
