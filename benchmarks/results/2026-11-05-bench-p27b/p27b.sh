#!/bin/sh
# cppqed-multidiagonal#27, part 2: the row shape at n = 255/256 against the placement within a page. psi starts psiOffset
# bytes into its page (0, 16, 2048); dpsidt lies 2^24 + delta after it: 2048 (neutral), 4112 (case F's in-page relation,
# outside the 1 MiB window), 16 (case F's placement exactly). Huge and 4 KB pages; diags at n = 256 for reference. Core 1.
cd ~
while pgrep -f "[p]27.sh" > /dev/null; do sleep 5; done
set -e
g++ -std=c++23 -O3 -march=native -w -I$HOME/src/cppqed-multidiagonal-remeasure/include -I/usr/include/eigen3 src/p27/probe27.cc -o tools/probe27 -lboost_json -lboost_serialization
out=runs/p27b; mkdir -p $out
echo "start $(date -Is)" > $out/log
for i in 1 2 3; do
  for po in 0 16 2048; do
    for delta in 2048 4112 16; do
      for pages in huge 4k; do
        for cfg in "255 rows" "256 rows" "256 diags"; do taskset -c 1 tools/probe27 $cfg $pages $delta 100 $po >> $out/scan.txt; done
      done
    done
  done
  echo "$(date +%T) round $i" >> $out/log
done
echo "done $(date -Is)" >> $out/log
