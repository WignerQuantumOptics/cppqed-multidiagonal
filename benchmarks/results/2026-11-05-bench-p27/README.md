# 2026-11-05, reference machine: the row shape on contiguous slices, timing scan and `perf`

Measured before `benchmarks/run.sh` existed, with the one-off script [`p27.sh`](p27.sh), kept here as it was on the machine.

- **What:** a probe program on contiguous slices of a state of about 2²⁰ amplitudes: the time per nonzero over the slice length, for both loop shapes, on huge pages and on 4 KB pages (three processes each), and `perf stat` counters for the lengths around 256. Pinned to core 1. None of the library's benchmarks ran, so there is no `REPORT.md`.
- **Source:** [`probe27.cc`](probe27.cc), which is not part of the repository, compiled against the library's headers at 14254a1 with `-O3 -march=native`.
- **Machine:** the reference machine of [`performance.md`](../../performance.md), "Setup". Its state at the start and at the end is in `log`. The script did not record the compiler version; the page gives GCC 16.2.1 for the machine at the time, and Eigen 5.0.1, which the library's benchmarks used.
- **Quoted in `performance.md`:** the first paragraph of "Contiguous slices of a state larger than cache" ([#27](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/27)).
- **`scan.txt`:** the probe's output, one line per run: slice length, number of slices, loop shape, pages, δ (the distance of `dpsidt` from `psi` beyond 2²⁴ bytes), huge-page memory in KB, nanoseconds per nonzero.
- **`perf.LENGTH_SHAPE.PAGES.GROUP.csv`** is the output of `perf stat -x,` for one of the two groups of counters, and the `.txt` beside it the probe's line for that run.

The files are as the script wrote them.
