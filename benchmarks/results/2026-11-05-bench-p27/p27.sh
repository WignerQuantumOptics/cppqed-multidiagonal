#!/bin/sh
# cppqed-multidiagonal#27: the row shape on contiguous slices of 4 KiB, huge pages against 4 KB pages (probe27, library 14254a1
# headers, -O3 -march=native). Phase 1: timing scan over the slice length, both shapes and page sizes, three processes each.
# Phase 2: perf stat (user space) for the lengths around 256, two groups of counters. Core 1.
cd ~
while pgrep -f "[a]llocs.sh|[l]ogs2.sh|[s]imd.sh|[j]emcheck.sh" > /dev/null; do sleep 5; done
set -e
g++ -std=c++23 -O3 -march=native -w -I$HOME/src/cppqed-multidiagonal-remeasure/include -I/usr/include/eigen3 src/p27/probe27.cc -o tools/probe27 -lboost_json -lboost_serialization
out=runs/p27; mkdir -p $out
c=/sys/devices/system/cpu
echo "start $(date -Is)  smt $(cat $c/smt/control)  no_turbo $(cat $c/intel_pstate/no_turbo)  gov $(cat $c/cpu1/cpufreq/scaling_governor)  thp $(cat /sys/kernel/mm/transparent_hugepage/enabled)" > $out/log
for i in 1 2 3; do
  for n in 128 192 240 248 252 254 255 256 257 258 260 264 320 384 448 511 512 513 1024; do
    for shape in rows diags; do
      for pages in huge 4k; do taskset -c 1 tools/probe27 $n $shape $pages 2048 100 >> $out/scan.txt; done
    done
  done
  echo "$(date +%T) scan $i" >> $out/log
done
A=cycles:u,instructions:u,l1d.replacement:u,l2_rqsts.miss:u,l2_rqsts.rfo_miss:u,l2_lines_in.all:u,l2_trans.l2_wb:u,offcore_requests.all_data_rd:u,dtlb_load_misses.walk_completed:u,dtlb_store_misses.walk_completed:u
B=cycles:u,ld_blocks_partial.address_alias:u,resource_stalls.sb:u,exe_activity.bound_on_stores:u,cycle_activity.stalls_l1d_miss:u,cycle_activity.stalls_l2_miss:u,cycle_activity.stalls_l3_miss:u,l1d_pend_miss.fb_full:u,machine_clears.memory_ordering:u,mem_load_retired.fb_hit:u
for cfg in "255 rows" "256 rows" "257 rows" "512 rows" "256 diags"; do
  for pages in huge 4k; do
    for g in A B; do
      eval ev=\$$g
      f=$out/perf.$(echo $cfg | tr ' ' '_').$pages.$g
      taskset -c 1 perf stat -x, -e $ev -o $f.csv tools/probe27 $cfg $pages 2048 100 > $f.txt
    done
  done
done
echo "done $(date -Is)" >> $out/log
