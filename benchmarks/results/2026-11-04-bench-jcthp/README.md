# 2026-11-04, reference machine: `jc_qjmc` on huge pages and on 4 KB pages

Measured before `benchmarks/run.sh` existed, with the one-off script [`jcthp.sh`](jcthp.sh), kept here as it was on the machine.

- **What:** C++QED's `jc_qjmc` with a mode cutoff of 2¹⁹ (a state of exactly 16 MiB) and of 523288, on huge pages and on 4 KB pages, five rounds alternating, pinned to core 1. None of the library's benchmarks ran, so there is no `REPORT.md`.
- **Source:** C++QED 0a0efb12 with the library's headers of v0.4.1 and Eigen 3.4.0. Release.
- **Machine:** the reference machine of [`performance.md`](../../performance.md), "Setup". Its state at the start and at the end is in `log`. The script did not record the compiler version; the page gives GCC 16.2.1 for the machine at the time, and Eigen 5.0.1, which the library's benchmarks used.
- **Quoted in `performance.md`:** the item "In C++QED" of "Distance on huge pages": 3184 against 2874 ms, and 2614 against 2799 ms.
- **`times`:** one line per run of `jc_qjmc`: mode, mode cutoff, round, wall time in milliseconds. The trajectories that the runs wrote are not kept here; they are C++QED's output, not this library's.
- **`alloc.N.log`:** the large allocations of one run at cutoff N, as `bigalloc.so` logged them (`+` size address). [`bigalloc.c`](bigalloc.c) and [`nothp.c`](nothp.c), which runs a program off huge pages, are the two helpers the script builds.

The files are as the script wrote them.
