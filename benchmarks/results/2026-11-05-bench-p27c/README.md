# 2026-11-05, reference machine: `perf` counters at a slow and a fast placement

Measured before `benchmarks/run.sh` existed, with the one-off script [`p27c.sh`](p27c.sh), kept here as it was on the machine.

- **What:** `perf stat` for the row shape at slice length 256 on 4 KB pages, at a slow placement (`psi` at offset 0) and a fast one (offset 16), and for one diagonal at a time at the slow placement. Pinned to core 1. No `REPORT.md`, as before.
- **Source:** `probe27.cc` of `2026-11-05-bench-p27`, as built there.
- **Machine:** the reference machine of [`performance.md`](../../performance.md), "Setup". Its state at the start and at the end is in `log`. The script did not record the compiler version; the page gives GCC 16.2.1 for the machine at the time, and Eigen 5.0.1, which the library's benchmarks used.
- **Quoted in `performance.md`:** the statement in "Contiguous slices of a state larger than cache" that in the slow placements the same memory traffic has 5× more loads waiting for DRAM ([#27](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/27)).
- **`perf.SHAPE.OFFSET.GROUP.csv`** is the output of `perf stat -x,`, and the `.txt` beside it the probe's line for that run.
- **`log`:** this script wrote only "done"; it ran directly after the two sessions before.

The files are as the script wrote them.
