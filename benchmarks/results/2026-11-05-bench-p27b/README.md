# 2026-11-05, reference machine: the row shape against the placement within a page

Measured before `benchmarks/run.sh` existed, with the one-off script [`p27b.sh`](p27b.sh), kept here as it was on the machine.

- **What:** the probe of the session before at slice lengths 255 and 256, with `psi` at three offsets within its page and `dpsidt` at three distances, on huge pages and on 4 KB pages, three rounds. Pinned to core 1. No `REPORT.md`, as before.
- **Source:** `probe27.cc` of `2026-11-05-bench-p27`, rebuilt.
- **Machine:** the reference machine of [`performance.md`](../../performance.md), "Setup". Its state at the start and at the end is in `log`. The script did not record the compiler version; the page gives GCC 16.2.1 for the machine at the time, and Eigen 5.0.1, which the library's benchmarks used.
- **Quoted in `performance.md`:** the first paragraph of "Contiguous slices of a state larger than cache": the dependence on where `psi` and `dpsidt` lie within their pages ([#27](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/27)).
- **`scan.txt`:** as in the session before, with the offset of `psi` within its page added before the huge-page memory.
- **`log`:** this script recorded only the times of its rounds, not the machine's state; it ran directly after the session before.

The files are as the script wrote them.
