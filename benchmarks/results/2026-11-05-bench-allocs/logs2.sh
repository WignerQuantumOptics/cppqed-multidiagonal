#!/bin/sh
# Allocation logs of jc_qjmc at cutoff 2^19 under jemalloc (cache_oblivious on and off) and tcmalloc, with bigalloc2.so, which
# also catches C++ operator new (both allocators override it, so bigalloc.so in allocs.sh saw nothing). Runs after allocs.sh.
cd ~
while pgrep -f "[a]llocs.sh" > /dev/null; do sleep 5; done
set -e
cc -O2 -Wall -Wextra -shared -fPIC -o tools/bigalloc2.so src/thp-tools/bigalloc2.c -ldl
JE=/usr/lib/libjemalloc.so.2; TC=/usr/lib/libtcmalloc_minimal.so.4; out=runs/allocs
args="--T 2 --Dt 0.5 --initFockm 1 --deltaq 0 --deltam 0 --kappam 1 --gamma_mq 1"
LD_PRELOAD="$HOME/tools/bigalloc2.so $JE" taskset -c 1 build-cppqed/examples/jc_qjmc --cutoffm 524288 $args > $out/alloc.jemalloc.out 2> $out/alloc.jemalloc.log
MALLOC_CONF=cache_oblivious:false LD_PRELOAD="$HOME/tools/bigalloc2.so $JE" taskset -c 1 build-cppqed/examples/jc_qjmc --cutoffm 524288 $args > $out/alloc.jemalloc-nco.out 2> $out/alloc.jemalloc-nco.log
LD_PRELOAD="$HOME/tools/bigalloc2.so $TC" taskset -c 1 build-cppqed/examples/jc_qjmc --cutoffm 524288 $args > $out/alloc.tcmalloc.out 2> $out/alloc.tcmalloc.log
LD_PRELOAD="$HOME/tools/bigalloc2.so" taskset -c 1 build-cppqed/examples/jc_qjmc --cutoffm 524288 $args > $out/alloc.glibc.out 2> $out/alloc.glibc.log
echo "logs done $(date -Is)" >> $out/log
