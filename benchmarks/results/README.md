# Raw benchmark results

One directory per measuring session, named by date and machine, exactly as [`benchmarks/run.sh`](../run.sh) wrote it:

| file | content |
|---|---|
| `machine.txt` | the processor, and the machine's state at the start and at the end of the session |
| `builds.txt` | per build: source revision, compiler, flags, Eigen version |
| `log` | per process: when it ended, how long it took, the steal time of its core, and whether it succeeded |
| `LABEL/SUITE-PAGES-rROUND.csv` | the standard output of one process: suite `main`, `kernels` or `kernels-cxlr`; pages `thp` (as the system hands them out) or `4k` (`--no-thp`) |
| `LABEL/SUITE-PAGES-rROUND.err` | its standard error |
| `LABEL/build.log` | the output of the build step |
| `REPORT.md` | generated from the above by [`benchmarks/reduce.py report`](../reduce.py): every value as the median over the processes, each implementation's time relative to the library's, and how well the processes agree |

The only change made to the files is the `host:` line of `machine.txt`, which carries the name the machine has in [`performance.md`](../performance.md) instead of its host name.

Figures quoted from a session are medians over its processes (the `rROUND` files of one suite and page mode). Look them up in the session's `REPORT.md`; `benchmarks/reduce.py ratios` puts sessions side by side. `benchmarks/plot.py SESSION[:PAGES] OUTDIR` draws the main suite's figures. The tables of `performance.md` are generated from these directories by `benchmarks/tables.py`.

## Sessions

| directory | machine | revision | what |
|---|---|---|---|
| `2026-11-03-bench-ref` | the reference machine | 7725d79, 84cf7d4 and v0.4.0 | the three suites of the first, and what the two older builds had in common with it; huge pages, five rounds |
| `2026-11-04-bench-step3` | the reference machine | the row shape before its commit, and 7725d79 | the three suites against `kernels`, `apply1` and `applyJC` of the older build; huge pages, five rounds |
| `2026-11-04-bench-fb8ca7f` | the reference machine | c346b8a (as fb8ca7f) and 7725d79 | the same; and two placement scans |
| `2026-11-04-bench-caseF` | the reference machine | c346b8a with case F | `kernels` case F, huge pages, five processes |
| `2026-11-04-bench-caseG` | the reference machine | c346b8a with case G | `kernels` case G, two processes |
| `2026-11-04-bench-caseH` | the reference machine | c346b8a with the first case H | `kernels` cases F and H in one process, five processes |
| `2026-11-04-bench-caseH2` | the reference machine | c346b8a with case H | `kernels` case H, five processes |
| `2026-11-04-bench-remeasure` | the reference machine | c346b8a with the benchmarks' `--no-thp` | `kernels` and the main suite's `apply1` and `applyJC`, on huge pages and on 4 KB pages, five rounds |
| `2026-11-04-bench-jcthp` | the reference machine | C++QED 0a0efb12 | `jc_qjmc` on huge pages and on 4 KB pages: times only |
| `2026-11-04-bench-padthp` | the reference machine | 14254a1 | `kernels` B, C, D, F, `apply1`, `applyJC` and `jc_qjmc`: huge pages, 4 KB pages, padded allocations |
| `2026-11-04-bench-protothp` | the reference machine | 14254a1 and a prototype | the same: a padding allocator in `StorageType` |
| `2026-11-04-bench-protothp2` | the reference machine | 14254a1 and a prototype | the same: an allocator with 16 positions in `StorageType` |
| `2026-11-05-bench-allocs` | the reference machine | 14254a1 and the prototype | the same: glibc, jemalloc, tcmalloc and the prototype |
| `2026-11-05-bench-simd` | the reference machine | 14254a1 | `kernels` A, B, E, `apply1`, `applyJC` and `jc_qjmc`: baseline, `x86-64-v3` and native builds, 4 KB pages |
| `2026-11-05-bench-p27`, `-p27b`, `-p27c` | the reference machine | a probe program against 14254a1 | timing scans and `perf stat` for #27; no benchmark files |
| `2026-11-05-bench-b5` | the reference machine | the working tree committed as 4186b8e | the main suite's `picture1`, `constructJC` and `slices3`, on 4 KB pages, five rounds |
| `2026-11-07-bench-caseI` | the reference machine | the working tree for #27 | `kernels` case I, first run with two state sizes, three processes |
| `2026-11-07-bench-caseI2` | the reference machine | the working tree committed as 78785f0 | `kernels` case I, three processes |
| `2026-11-08-bench-caseD4k` | the reference machine | e90ea79 | `kernels` case D, default and `-fcx-limited-range` builds, on 4 KB pages, five rounds |
| `2026-11-23-bench` | the reference machine: Intel Core i7-7700 (Kaby Lake), GCC 16.2.1, Eigen 5.0.1 | 134ddcf | all three suites, on huge pages and on 4 KB pages, five rounds |
| `2026-11-23-zen2-vm` | a virtual machine on an AMD EPYC (Zen 2) host, 16 vCPUs, KVM; GCC 16.2.0, Eigen 5.0.1 | 134ddcf | all three suites, on 4 KB pages, five rounds |

## Sessions from before the driver

The sessions up to 2026-11-08 were run with one-off scripts. Each of them has, instead of `machine.txt` and `builds.txt`:

- the script, as it was on the machine;
- a `README.md` saying what was measured, from which source, where `performance.md` quotes it, and how the files were renamed to the layout above. The contents of the files are unchanged.

Recomputed from these files, the figures that `performance.md` quotes from them come out as on the page.

Some of these sessions also ran C++QED's `jc_qjmc`. Its run times are kept (`times`), its trajectories are not. Helper programs that a script builds, and the patches of the allocator prototypes, lie beside the script that first used them.

Not here: two sessions of 2026-11-03 of which no script was kept (a check of the machine's noise, and Eigen 3.4.0 against 5.0.1), and the measurements on a laptop that the page mentions as history.
