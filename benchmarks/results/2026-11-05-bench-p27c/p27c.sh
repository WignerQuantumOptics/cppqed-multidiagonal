#!/bin/sh
# cppqed-multidiagonal#27, part 3: perf counters for the row shape at n = 256 on 4 KB pages, a slow placement (psi at 0,
# dpsidt at 2^24 + 2048) against a fast one (psi at 16, the same delta), and one diagonal at a time at the slow placement.
cd ~
while pgrep -f "[p]27.sh|[p]27b.sh" > /dev/null; do sleep 5; done
set -e
out=runs/p27c; mkdir -p $out
A=cycles:u,instructions:u,l1d.replacement:u,l2_rqsts.miss:u,l2_rqsts.pf_miss:u,l2_lines_in.all:u,offcore_requests.all_data_rd:u,mem_load_retired.l2_miss:u,mem_load_retired.l3_miss:u,mem_load_retired.fb_hit:u
B=cycles:u,ld_blocks_partial.address_alias:u,resource_stalls.sb:u,cycle_activity.stalls_l1d_miss:u,cycle_activity.stalls_l2_miss:u,cycle_activity.stalls_l3_miss:u,l1d_pend_miss.fb_full:u,l2_rqsts.all_rfo:u,l2_rqsts.rfo_miss:u,mem_load_retired.l1_miss:u
for cfg in "rows 0" "rows 16" "diags 0"; do
  set -- $cfg
  for g in A B; do
    eval ev=\$$g
    taskset -c 1 perf stat -x, -e $ev -o $out/perf.$1.$2.$g.csv tools/probe27 256 $1 4k 2048 100 $2 > $out/perf.$1.$2.$g.txt
  done
done
echo done > $out/log
