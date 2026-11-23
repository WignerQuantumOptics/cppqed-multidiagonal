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

Figures quoted from a session are medians over its processes (the `rROUND` files of one suite and page mode). Look them up in the session's `REPORT.md`; `benchmarks/reduce.py ratios` puts sessions side by side.

## Sessions

| directory | machine | revision | what |
|---|---|---|---|
| `2026-11-23-bench` | the reference machine: Intel Core i7-7700 (Kaby Lake), GCC 16.2.1, Eigen 5.0.1 | 134ddcf | all three suites, on huge pages and on 4 KB pages, five rounds |
| `2026-11-23-zen2-vm` | a virtual machine on an AMD EPYC (Zen 2) host, 16 vCPUs, KVM; GCC 16.2.0, Eigen 5.0.1 | 134ddcf | all three suites, on 4 KB pages, five rounds |

The sessions behind the figures that `performance.md` quoted before 2026-11-23 were run with one-off scripts and are not here yet ([#37](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/37)).
