# Performance of MultiDiagonal: measurements and outlook

This page records what the benchmarks show about applying a `MultiDiagonal` to a state, where the library is still slower than it could be, and what it can and cannot gain over a general sparse matrix. It is meant as an honest baseline: where the library doesn't have the right loop yet, the figures for "what is possible" come from hand-written loops over the library's data in [`benchmarks/kernels.cc`](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/blob/main/benchmarks/kernels.cc), not from the library itself.

**Short version** (reference machine, default build of both libraries):

- **In cache**, the library is 2.7–3.6× faster than Eigen's sparse product at rank 1 and with the mode on axis 0 (1.8–2.2× for the smallest operators, of dimension 16), and 1.4–2.2× faster with a qubit on axis 0. Contiguous diagonals without column indices vectorize, while a CSR product gathers through column indices. In cache these ratios move by up to 20% between builds, because Eigen's own speed depends on the layout of the code around it (see "Setup"). They also depend on the processor: on a 2022 laptop core they were 1.5–2.4× and 1.0–1.6×, before the last change of the loop.
- **Out of cache**, where the operator is as large as the state, the library is 2.0–2.4× faster than Eigen at rank 1 and with the mode on axis 0, and 0.9–1.1× as fast with a qubit on axis 0. These figures are on 4 KB pages. On transparent huge pages, input and output allocated back to back can make the library up to about 3× slower (see "Distance on huge pages"). Once both sides are limited by memory bandwidth, no storage format gains much: MultiDiagonal stores 16 bytes per nonzero against 20 for a compressed sparse row (CSR) matrix. A single thread on the reference machine doesn't reach that limit with Eigen's product.
- **In C++QED's actual setting**, a small operator applied slice by slice across a large state, the library is 1.7–2.6× faster than a CSR matrix of the same term (1.4–1.9× against its build with `-fcx-limited-range`), also on strided slices of power-of-two length. Contiguous slices of power-of-two length looked like an exception, at parity, but that came from where their arrays lay in memory, not from their length: on 4 KB pages the library is 2.5× faster there as well.
- **The loop has two shapes** since 2026-11-04: one diagonal at a time, and the diagonals contributing to a row summed before one store ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1), [#25](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/25)). `operator()` chooses from the view's strides and extents. The row shape sweeps a large state once instead of once per diagonal, and makes the speed nearly independent of where input and output lie relative to each other within a page. The other is faster where axis 0 is short, and for small views.
- **History.** Up to v0.4.0, the multi-axis loop recomputed every address per element, which made it 3.4–4.5× slower than Eigen at rank 2 and 18× slower at rank 4; fixed on 2026-11-01 ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)). Complex multiplication with the C99 Annex G NaN checks halved the speed of the default build; fixed on 2026-11-02, together with a compile-time unrolled short axis 0 ([#23](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/23)). Up to 2026-11-03 the loop stored the output once per diagonal, which made it up to 1.45× slower than Eigen out of cache and its speed depend on placement by up to 1.2–2×.
- **What remains.** A qubit on axis 0 of a state larger than cache is still applied one diagonal at a time, and at N = 2²⁰ it is 0.89× as fast as Eigen. On transparent huge pages, input and output that lie nearly a multiple of 1 MiB apart, as arrays of a power-of-two size allocated back to back do, make application up to about 3× slower one diagonal at a time and up to 1.7× by rows ([#26](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/26)). Small views are applied one diagonal at a time, and there placement still costs up to 23%, though that shape stays faster than the other at any placement.
- **The interaction picture** has not been benchmarked systematically yet (see the section on it below).

## Setup

- **Reference machine** since 2026-11-03: Intel Core i7-7700 desktop (Kaby Lake, 4 cores; L1d 32 KB and L2 256 KB per core, L3 8 MB), Arch Linux, GCC 16.2.1, Eigen 5.0.1, `-O3 -march=native`, single thread.
- **Prepared for measuring:** no graphical session, timers stopped, SMT off, turbo off (fixed 3.6 GHz, `performance` governor). Each run is pinned to a core that handles no device interrupts. Transparent huge pages are on for all anonymous memory (`always`, Linux 6.18), which matters out of cache (see "Distance on huge pages"). Out-of-cache figures marked "4 KB pages" were measured with `--no-thp`, which keeps the process off huge pages; the others were measured on huge pages, without controlling the distance between the arrays.
- **Every figure** is the median over five separate processes, with the binaries of different versions interleaved. Processes of the same binary typically agree within 1–2% (90% within 5–8%) in cache. Larger differences come from where the arrays land in memory (see "Placement of the arrays" below), not from the machine. Out of cache, separate runs of the same binary differ by up to 25%.
- **Code layout** moves small kernels in cache by 10–20% between builds whose code differs elsewhere. Eigen's sparse product for Jaynes–Cummings, unchanged, ran 3–20% faster in the build of c346b8a than in those of 7725d79 and of a build that differed from c346b8a only in `operator()`'s bounds, consistently over five processes. The library's own times agreed within 5% wherever both applied the same shape. For contiguous slices, the build of 7725d79 ran the loop that applies one diagonal at a time 12% faster than every other build of the same loop. Neither aligning loops nor the assembler's mitigation of the jump erratum changed this. Ratios to Eigen in cache are therefore uncertain by up to 20%, while the library's speed-ups over its earlier versions are not.
- **Versions**, all built and measured with the same toolchain:
  - "v0.4.0": the library of v0.4.0, with the benchmarks of e49319b;
  - "before #23": 84cf7d4;
  - "after #23": 7725d79;
  - "now": c346b8a, the two loop shapes; cases F and G of `kernels.cc` were added after it, over the same library.

  The first three were measured on 2026-11-03. "Now" was measured on 2026-11-04, interleaved with "after #23" again, which reproduced the library's earlier figures within 1–3%.
- **Unit:** nanoseconds per stored nonzero, i.e. per `nonZeros()` of the equivalent Eigen matrix. "In L2" means that operator and state together fit in 256 KB, "in L3" in 8 MB; "out of cache" means beyond that.
- **Every variant is checked against Eigen** before it is timed.
- **Earlier figures**, measured on a laptop (Intel Core i7-1260P under WSL2, GCC 13.3, Eigen 3.4.0), are in the history of this page. They agree qualitatively, with two differences:
  - the run-to-run spread there was 15–45%;
  - the speed-ups over Eigen were smaller.

  Figures marked "laptop" below come from there. The Eigen version matters less than the processor: on the reference machine, Eigen 3.4.0 and 5.0.1 differ by 0.8–1.2× in Eigen's own speed, and leave the library unchanged.

To reproduce (pin the runs to one core, e.g. with `taskset -c 1`):

```bash
cmake -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --target cppqed-multidiagonal-benchmarks cppqed-multidiagonal-kernels cppqed-multidiagonal-kernels-cxlr
./build-release/benchmarks/cppqed-multidiagonal-kernels        # cases A–F, default complex multiplication
./build-release/benchmarks/cppqed-multidiagonal-kernels G      # the placement scan; a summary per shape goes to stderr
./build-release/benchmarks/cppqed-multidiagonal-kernels-cxlr   # the same with -fcx-limited-range
./build-release/benchmarks/cppqed-multidiagonal-benchmarks     # apply1, compose1, …, picture1
# --no-thp (both executables, Linux) keeps the process off transparent huge pages
```

`kernels.cc` times the library as `operator()` chooses its loop shape ("library"), and with each shape forced: `applyByDiagonals` ("one diagonal at a time") and `applyByRows` ("by rows"). Its cases:

| case | what is applied |
|---|---|
| A | rank 1, five diagonals (offsets −2…+2), N = 256 (in L2), 4096 (in L3), 262144 (out of cache) |
| B | rank-2 Jaynes–Cummings Hamiltonian, mode on axis 0, qubit on axis 1; state 512 (in L2), 8192 (in L3), 524288 (out of cache) |
| C | a five-diagonal operator of dimension n = 32 on one axis of an n⁴ state (10⁶ amplitudes), applied to the whole state at once |
| D | the same operator applied slice by slice, as C++QED's `Broadcaster` does; n = 32 and n = 30 |
| E | tridiagonal ⊗ tridiagonal on extents {n₀, 8192/n₀}, for an axis 0 of n₀ = 1 … 32 elements |
| F | where the two loop shapes cross over: case D's operator on contiguous slices of length n; tridiagonal ⊗ tridiagonal on extents {n₀, T/n₀} for T = 8192, 65536, 2²⁰ |
| G | placement: `psi` at every 16-byte offset of a page relative to `dpsidt`, both shapes; rank 1 at N = 64, 1024, 4096, and Jaynes–Cummings in both axis orders (state 4096). It takes a few minutes and runs only when named |
| H | the distance between `psi` and `dpsidt` out of cache: case F's operator on contiguous slices of length 16, 24, 32, with `dpsidt` 2²⁴ + δ bytes after `psi` in one buffer, on huge pages and on 4 KB pages. Runs only when named |

## Complex multiplication

By default both GCC and Clang implement `std::complex<double>` multiplication according to C99 Annex G: every product checks for NaN and falls back to a call to `__muldc3`. The check stops the loop from vectorizing. Up to 2026-11-01 it more than halved the library's speed, and `-fcx-limited-range` was needed to remove it.

Since 2026-11-02 ([#23](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/23)) the kernel multiplies with `textbookProduct`, i.e. $(ac-bd)+i(ad+bc)$ written out in real arithmetic. It omits only the recovery of infinite results, which a physical state never needs. The speed no longer depends on the flag:

| case A, in cache (N = 256 / 4096) | default | `-fcx-limited-range` |
|---|---|---|
| library before #23 | 1.50 / 1.58 | 0.64 / 0.78 |
| library after #23 | **0.65 / 0.83** | 0.65 / 0.81 |
| Eigen sparse | 1.80 / 1.95 | 1.62 / 1.77 |

With the flag, before and after agree; in the default build, the library became 1.9–2.3× faster. Both loop shapes of today multiply the same way, and the flag still leaves them unchanged (case A now: 0.58 / 0.71 with and without). The flag also speeds up Eigen:

- by 10–17% in cases A to C;
- by 1.1–1.4× for term CSR on slices (case D).

Eigen's sparse product gathers through column indices, so it doesn't vectorize, but the check still costs it. The hand-written loops in `kernels.cc` use `textbookProduct` too, so the flag now changes only Eigen's figures.

The other loops that multiply complex numbers were checked the same way (laptop). Composition, addition and construction (`compose1`, `composeJC`, `construct1`, `constructJC`) are dominated by index arithmetic, allocation and the map. The flag changes them by nothing beyond the noise, so they keep the standard multiplication.

All further figures are from the default build. With the flag, Eigen's would be 10–17% lower.

## The library's two loop shapes

Up to v0.4.0, `operator()` stepped a multi-index through each diagonal, last axis fastest, and computed the address of every element from all its indices. At rank 1 this cost little, but at higher rank it dominated.

**One diagonal at a time** (`applyByDiagonals`, since 2026-11-01, [#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)):

- For each diagonal, it computes the addresses of the first row and the first column once.
- A loop nest generated at compile time then runs over the axes, with axis 0 innermost. Axis 0 is contiguous in memory, both in the diagonal and in an owning state.
- The outer axes only add strides to pointers.

This is the "hoisted" loop of `kernels.cc`, generalized to strided views. It stores the output once per diagonal and element, so it sweeps the state once per diagonal.

**By rows** (`applyByRows`, since 2026-11-04, [#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1), [#25](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/25)):

- A loop nest generated at compile time runs over the rows, with axis 0 innermost. Along the outer axes it keeps the list of diagonals that have entries in the current row, and where their entries and columns start.
- Along axis 0, the rows in which all of these diagonals have entries are summed in one loop, with the number of diagonals fixed at compile time, so that it vectorizes. Up to six diagonals are summed per store: each needs two pointers in registers, and with eight a band of nine diagonals became 1.4–1.7× slower.
- The few rows at either end, where some diagonals have no entry, are summed one by one.

This stores the output once per element. Its cost is the rows at either end, a fixed cost per row of axis 0, which a short axis 0 doesn't amortize.

**`operator()` chooses** one diagonal at a time for an axis 0 of up to four elements, and for views contiguous along axis 0 that are either small (up to 128 amplitudes) or fit in a last-level cache (up to 2¹⁸ amplitudes, 4 MB per array) with an axis 0 shorter than 32. Everything else goes by rows: long axes, operands larger than cache, and views strided along axis 0. The bounds are where the two shapes cross over (case F, below and in the section on slices).

| | v0.4.0 | before #23 | after #23 | now | one diagonal at a time | hand-written hoisted loop | Eigen sparse |
|---|---|---|---|---|---|---|---|
| B: Jaynes–Cummings, in cache (state 512 / 8192) | 6.95 / 7.24 | 1.27 / 1.63 | 0.59 / 0.96 | **0.58 / 0.75** | 0.58 / 0.96 | 0.58 / 0.96 | 2.01 / 2.23 |
| B: Jaynes–Cummings, out of cache (state 524288) | 7.77 | 4.34 | 3.34 | **1.99** | 3.33 | 3.33 | 3.22 |
| C: rank-4 operator built as $\mathbb{1}\otimes\mathbb{1}\otimes A\otimes\mathbb{1}$, $A$ on axis 0 / axis 2 (out of cache) | 47.7 / 46.6 | 3.37 / 3.25 | 2.64 / 2.31 | **2.07 / 1.52** | 2.66 / 2.32 | — | 2.71 / 2.73 |

"Now" is `operator()`, which takes the row shape in all these cases except the state of 512; "one diagonal at a time" is the same binary with the other shape forced. The out-of-cache rows were measured on huge pages. On 4 KB pages, case B out of cache takes 1.27 (one diagonal at a time 2.01, Eigen 2.72) and case C 1.95 / 1.45 (Eigen 2.45 / 2.57).

- **Case B in cache:** the library is now 10–12× faster than at v0.4.0 and 3.0–3.5× faster than Eigen. At state 8192 the row shape is 1.3× faster than one diagonal at a time.
- **Case B out of cache:** it sweeps the state once. On 4 KB pages it is 2.1× faster than Eigen, and one diagonal at a time 1.35×. On huge pages, at the distance its arrays happened to have, these were 1.6× and parity. An earlier run of the same code gave 2.47 and 4.71 for the two shapes, with up to 30% between processes: out of cache, physical placement matters (see "Placement of the arrays").
- **Case C:** the rank-4 operator improves 23–31× over v0.4.0 and 1.3–1.5× over 7725d79, and is now 1.3–1.8× faster than Eigen, on 4 KB pages as well. It stays bandwidth bound, because the materialized operator is five times as large as the state.

The 2026-10 review had flipped the loop order alone and saw no effect beyond noise. That is consistent: memory order only pays once the per-element address computation is gone.

`cppqed-multidiagonal-benchmarks` applies the Jaynes–Cummings Hamiltonian in both axis orders, for mode dimensions N from 16 to 1048576 (state 2N), and a five-diagonal rank-1 operator of dimension N. The table gives the time of Eigen sparse divided by the time of MultiDiagonal; above 1 means MultiDiagonal is faster. Up to N = 1024, operator and state fit in L2, and up to N = 16384 in L3. For `apply1`, N = 65536 (about 7 MB) sits at the edge of L3:

| | v0.4.0, N ≤ 16384 | before #23, N ≤ 16384 | after #23, N ≤ 16384 | now, N ≤ 16384 | after #23, N = 65536 / 262144 / 1048576 | now, N = 65536 / 262144 / 1048576 | now, 4 KB pages, N = 65536 / 262144 / 1048576 |
|---|---|---|---|---|---|---|---|
| `applyJC_modefast`: mode on axis 0 | 0.25–0.29 | 1.35–1.58 | 2.14–3.39 | **1.80–3.40** | 1.72 / 1.39 / 0.82 | **2.09 / 1.84 / 1.54** | **2.63 / 2.06 / 2.30** |
| `applyJC_qbitfast`: qubit on axis 0 | 0.22–0.28 | 1.04–1.19 | 1.50–2.18 | 1.34–1.84 | 1.18 / 0.81 / 0.61 | 0.87 / 0.78 / 0.59 | **1.36 / 1.07** / 0.89 |
| `apply1`: rank 1 | 1.04–1.30 | 1.30–1.75 | 1.92–3.22 | **1.84–3.29** | 1.08 / 1.46 / 0.69 | **2.24 / 2.45 / 1.37** | **2.82 / 2.29 / 2.01** |

All columns but the last were measured on huge pages, where the distance between the arrays was not controlled. The last comes from a separate session (2026-11-04, five rounds interleaved with the same binary on huge pages), in which every figure for N ≤ 16384 agreed within 5% between the two.

- **Up to N = 64** (state up to 128) `operator()` applies one diagonal at a time, as before. The library's times agree with those of 7725d79 within 10%; the lower ratios for Jaynes–Cummings come from Eigen, which runs 3–20% faster in this build (see "Setup").
- **From N = 256 to 16384**, the row shape makes the library 1.1–1.4× faster than 7725d79 in the same session (at N = 256 with Jaynes–Cummings, equal within the spread). That makes it 2.7–3.3× faster than Eigen for `apply1`, and 2.9–3.4× for `applyJC_modefast` from N = 1024.
- **Out of cache** (N = 262144 and 1048576) it is 1.4–2× faster than 7725d79, and 1.4–2.5× faster than Eigen on huge pages, 2.0–2.3× on 4 KB pages.
- **A qubit on axis 0** is applied one diagonal at a time, as before, and the library's times are unchanged within 6%. Out of cache it was 0.6–0.9× as fast as Eigen on huge pages; on 4 KB pages it is 1.07× and 0.89× (N = 262144 and 1048576), and 1.36× at the edge of L3. In cache its ratio to Eigen is 1.3–1.8× in this build, and was 1.5–2.2× in the build of 7725d79, with the same library times.

A spot check with Clang 18 (laptop) gave the same picture as GCC up to #23; it has not been repeated for the row shape.

### A short axis 0

With the qubit on axis 0, as in C++QED's `jc_qjmc`, the loop over axis 0 has one or two elements; the σ± diagonals have extent 1 along the qubit. Setting up a vectorized loop then costs more than the work in it. With #23's multiplication alone, the mode-first case became about twice as fast, but the qubit-first case up to 1.4× slower (laptop).

Since 2026-11-02, the shape that applies one diagonal at a time therefore dispatches on the length of axis 0 once per diagonal. For up to four elements, the innermost level of the loop nest is unrolled at compile time. The loop over axis 1 then becomes the innermost one, and the compiler vectorizes that instead.

Case E shows where the two loops cross over, and what a short axis 0 costs the row shape (ns per nonzero):

| length of axis 0 | 1 | 2 | 3 | 4 | 5 | 6 | 8 | 16 | 32 |
|---|---|---|---|---|---|---|---|---|---|
| library (one diagonal at a time, unrolled up to 4; by rows at 32) | 0.94 | 1.32 | 1.27 | 1.38 | 1.31 | 1.28 | 1.18 | 1.01 | 0.91 |
| library by rows | 9.65 | 6.21 | 5.32 | 3.88 | 3.21 | 2.61 | 2.02 | 1.26 | 0.91 |
| hand-written hoisted loop (always loops over axis 0) | 3.74 | 2.98 | 2.13 | 1.82 | 1.42 | 1.35 | 1.21 | 1.04 | 0.94 |
| Eigen sparse | 2.38 | 2.05 | 1.95 | 1.92 | 1.90 | 1.89 | 1.88 | 1.91 | 1.88 |

Up to four elements unrolling wins, by up to 4×; from five on, the loop over axis 0 is as good. The row shape, with nine diagonals here, pays its fixed cost per row of axis 0: 10× at one element, 2.5× at five, and parity at 32.

Case F maps the crossover between the two shapes for contiguous views, which sets `operator()`'s bounds (ns per nonzero):

| contiguous slices of length n, one per call | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| state in cache: one diagonal at a time | **1.66** | **0.97** | **0.79** | **0.70** | **0.61** | **0.56** | **0.54** | **0.56** | 0.67 |
| state in cache: by rows | 2.93 | 1.58 | 1.18 | 0.99 | 0.80 | 0.70 | 0.62 | 0.60 | **0.56** |
| state of 16 MB: one diagonal at a time | **1.80** | 2.76 | **0.95** | 2.60 | **0.81** | 2.48 | **0.75** | 2.44 | 2.47 |
| state of 16 MB: by rows | 2.99 | **2.25** | 1.40 | **1.91** | 1.48 | **1.76** | 1.65 | **1.58** | **1.45** |
| state of 16 MB, 4 KB pages: one diagonal at a time | **1.81** | **1.14** | **0.96** | **0.89** | **0.82** | **0.81** | **0.76** | **0.75** | 0.81 |
| state of 16 MB, 4 KB pages: by rows | 2.98 | 1.67 | 1.37 | 1.28 | 1.16 | 1.18 | 1.49 | 1.70 | **0.77** |

| tridiagonal ⊗ tridiagonal, axis 0 of n₀ | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| in L2 (8192): one diagonal at a time | **1.26** | **1.15** | **1.04** | **1.00** | **0.93** | **0.91** | 0.89 | 0.88 | 0.86 | 0.89 |
| in L2: by rows | 3.29 | 2.06 | 1.54 | 1.30 | 1.05 | 0.94 | **0.84** | **0.79** | **0.72** | **0.65** |
| in L3 (65536): one diagonal at a time | **2.16** | **2.10** | **1.31** | **1.27** | **1.22** | 1.21 | **1.18** | **1.18** | **1.18** | 1.21 |
| in L3: by rows | 3.47 | 2.48 | 1.67 | 1.44 | 1.23 | **1.12** | 1.22 | 1.28 | 1.22 | **1.03** |
| out of cache (2²⁰): one diagonal at a time | **2.92** | 2.84 | 2.37 | 2.63 | 2.16 | 2.55 | 2.14 | 2.53 | 2.46 | 2.53 |
| out of cache: by rows | 3.61 | **2.61** | **1.98** | **1.97** | **1.77** | **1.72** | **1.87** | **2.16** | **2.03** | **1.84** |
| in L3, 4 KB pages: one diagonal at a time | **1.53** | **1.42** | **1.33** | **1.30** | **1.25** | 1.23 | **1.20** | **1.20** | **1.21** | 1.24 |
| in L3, 4 KB pages: by rows | 3.26 | 2.15 | 1.64 | 1.43 | 1.28 | **1.17** | 1.32 | 1.25 | 1.38 | **0.93** |
| out of cache, 4 KB pages: one diagonal at a time | **2.56** | 2.40 | 2.30 | 2.27 | 2.19 | 2.18 | 2.17 | 2.16 | 2.12 | 2.18 |
| out of cache, 4 KB pages: by rows | 3.42 | **2.30** | **1.95** | **1.69** | **1.76** | **1.57** | **1.92** | **1.80** | **1.94** | **1.40** |

- **One slice per call:** one diagonal at a time is faster up to n = 128 in cache, hence the bound of 128 amplitudes. Out of cache the powers of two from 16 on are slow because of where their arrays lie, not because of their length: case F allocates `psi` and `dpsidt` back to back, and for these lengths they lie 2²⁴ + 16 bytes apart (see "Distance on huge pages"). On 4 KB pages one diagonal at a time is faster up to n = 128 out of cache too.
- **Short axis 0, whole operand:** one diagonal at a time is faster below 32 in L2 and L3 (at 24 in L3 they are equal), the row shape from 8 on out of cache, hence the bounds of 32 and 2¹⁸. At the margins `operator()` gives away a little: 3% at 32 in L2, and up to 8% between 48 and 128 in L3. On 4 KB pages, where the distance between the arrays doesn't matter, the crossovers are the same, and between 48 and 128 in L3 the row shape gives away 4–14%.
- For an axis 0 of 5 and 8 in L3, one diagonal at a time took 2.1–2.2 here and 1.4–1.5 in an earlier run of the same code, and takes 1.42–1.53 on 4 KB pages: the slower figures came from the distance between the arrays.

## Placement of the arrays

The shape that applies one diagonal at a time writes `dpsidt` once per diagonal and element. So its speed depends on where `psi` lies relative to `dpsidt` within a 4 KB page ([#25](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/25)). The processor compares only the low 12 address bits of a load with the stores still in flight. For some placements it then holds loads from `psi` back behind stores to `dpsidt` that they don't depend on ("4K aliasing"). The row shape writes `dpsidt` once per element.

Case G of `kernels.cc` places `psi` at every 16-byte offset of a page relative to `dpsidt`, for both shapes (ns per application; the first of two runs):

| loop | N | best | median | worst | offsets more than 5% above the best |
|---|---|---|---|---|---|
| one diagonal at a time (chosen by `operator()` here) | 64 | 161 | 163 | 198 | 21 / 256 |
| by rows | 64 | 215 | 218 | 240 | 6 / 256 |
| one diagonal at a time | 1024 | 2968 | 3136 | 3577 | 142 / 256 |
| by rows | 1024 | 2764 | 2792 | 2899 | 1 / 256 |
| one diagonal at a time | 4096 | 13657 | 14164 | 15500 | 83 / 256 |
| by rows | 4096 | 12197 | 12467 | 12835 | 2 / 256 |
| one diagonal at a time, Jaynes–Cummings, mode first, state 4096 | | 11142 | 11307 | 12330 | 27 / 256 |
| by rows, the same | | 8918 | 9365 | 9813 | 128 / 256 |
| one diagonal at a time, Jaynes–Cummings, qubit first, state 4096 (chosen by `operator()`) | | 17199 | 17545 | 19000 | 14 / 256 |

"Worst" leaves out single offsets far above both their neighbours: nine of the 2560 measurements in this run, 1.2–1.9× the best, in both shapes. The second run had them at other offsets. They are disturbances during that measurement, not placement. Otherwise the second run agrees within a few percent, except that the first two offsets of one of its scans came out 1.4× slow.

- **One diagonal at a time: a typical placement costs 1–6%, a bad one 10–23%.** The slow placements are those where `psi` lies a little below `dpsidt`, and the default allocation puts the two arrays exactly there.
- **Occasionally a whole process runs about 2× slower** with an identical layout, presumably because of the state of the processor's memory-disambiguation predictor.
- **By rows, at rank 1, every offset is within 5% of the best from N = 1024 on**, and the rise for `psi` just below `dpsidt` is 4–6%. At N = 64 a few offsets cost up to 12%. The row shape is also 1.1–1.2× faster at the median there. With Jaynes–Cummings it alternates between 1–3% and 6–8% above the best with a period of 32 bytes, which is the alignment of 32-byte vector loads against cache lines, not aliasing; the worst offset is 1.10× the best.
- **Eigen stores once per row and is not affected.**
- **With the qubit on axis 0, placement barely matters**: one diagonal at a time, 14 offsets in 256 are more than 5% slower, and the worst is 1.10×.
- **Small views are still applied one diagonal at a time** (N = 64 above), and there the placements just below cost up to 23%. Even at its worst placement, though, this shape is faster than the row shape at its best (198 against 215 ns), so `operator()` has nothing to gain by switching.
- **Out of cache, the distance between the arrays matters as well** (next section). At N = 1048576 an isolated process took 2.2 ns per nonzero for `apply1` one diagonal at a time, but 3.4 ns inside the main suite, with the same in-page offsets, and case B out of cache varies by up to 30% between processes, for both shapes. On 4 KB pages, where the distance doesn't matter, out-of-cache figures vary by at most 7% between processes.

This is why every figure on this page is a median over separate processes.

### Distance on huge pages

Case F's contiguous slices of power-of-two length were 2.4–2.8× slower out of cache, one diagonal at a time, than those of other lengths. The cause is where the arrays lie. Case F allocates `psi` and `dpsidt` back to back; for a power of two both hold exactly 2²⁰ amplitudes (16 MiB), so `dpsidt` starts 2²⁴ + 16 bytes after `psi`. Every slow point has this distance. The fast ones don't: n = 8 got separate mappings, and n = 24, 48 and 96 have the arrays 240 or 1008 bytes short of 2²⁴ apart. A first version of case H (not kept, it set only the offset within a page) placed the arrays explicitly and showed that neither the slice length nor the array size matters: at a given placement the powers of two fall between their neighbours, and one slice fewer changes nothing.

Case H puts both arrays in one buffer, with `dpsidt` D bytes after `psi`, and takes the buffer once on transparent huge pages and once on 4 KB pages (`madvise`; the kernel reported all of it, and none of it, on huge pages). One diagonal at a time, ns per nonzero for n = 16 / 24 / 32, medians of five processes:

| D − 2²⁴ | huge pages | 4 KB pages |
|---|---|---|
| −4096 | 1.08 / 0.92 / 0.83 | 1.11 / 0.93 / 0.84 |
| −240 | 1.17 / 0.95 / 0.84 | 1.10 / 0.93 / 0.85 |
| −16 | 1.69 / 1.53 / 1.48 | 1.11 / 0.93 / 0.85 |
| 0 | 2.33 / 2.19 / 2.14 | 1.10 / 0.93 / 0.84 |
| +16 (back to back) | **3.03 / 2.91 / 2.85** | 1.14 / 0.93 / 0.89 |
| +64 | 2.93 / 2.87 / 2.87 | 1.14 / 0.96 / 0.90 |
| +256, +1024, +2048 | 1.12–1.13 / 0.95–1.00 / 0.84–0.95 | 1.13–1.15 / 0.93–0.96 / 0.86–0.89 |
| +4 KiB + 16 | 1.12 / 0.93 / 0.87 | 1.14 / 0.94 / 0.89 |
| +64 KiB + 16 | 1.13 / 0.92 / 0.87 | 1.14 / 0.94 / 0.88 |
| +1 MiB + 16, +2 MiB + 16, +16 MiB + 16 | 3.03 / 2.90–2.91 / 2.85–2.86 | 1.14 / 0.94 / 0.89 |

- **On 4 KB pages the distance doesn't matter**: every distance is within 5%.
- **On huge pages, `psi` and `dpsidt` must not nearly coincide modulo 1 MiB.** With `dpsidt` 16 or 64 bytes beyond the coincidence, one diagonal at a time is 2.7–3.4× slower than at a neutral distance; at the coincidence itself 2.1–2.5×, and 16 bytes short of it 1.5–1.75×. 240 bytes short or 256 beyond, it isn't slower. Moving by 4 KiB or 64 KiB removes the effect; moving by 1 MiB, 2 MiB or 16 MiB doesn't. So the address bits from the cache line up to about bit 19 have to agree, and in physical memory they do so only when huge pages make the physical address follow the virtual one. Which part of the memory system conflicts is not established: conflicts in the sets of L2 alone would also make +64 KiB + 16 slow.
- **By rows** the same distances cost 1.25–1.7×: 2.12–2.31 against 1.68–1.72 for n = 16, 1.66–1.90 against 1.11–1.27 for n = 32.
- **At a neutral distance huge pages gain nothing here** (1.08–1.13 against 1.10–1.15 for n = 16).
- **It reaches users** whenever two arrays whose size is a multiple of 1 MiB (2¹⁶ amplitudes) are allocated back to back on a system with transparent huge pages set to `always`, as the arrays of an ODE stepper may be. The laptop has them on `madvise`, so ordinary allocations there stay on 4 KB pages, which is why it never showed the effect.
- **The earlier out-of-cache figures on this page were measured on huge pages, with arrays of a multiple of 1 MiB.** They were re-measured on 2026-11-04, with five rounds of `kernels` (cases A–F) and of the main suite's `apply1` and `applyJC`, each run once on huge pages and once with `--no-thp`, interleaved:
  - 76 of 289 configurations differ by more than 8% between the two, all of them out of cache or at the edge of L3. In cache nothing changed.
  - On huge pages the library and the hand-written loops were 1.1–3.1× slower: case B out of cache, C, D at n = 32, case F's states of 16 MB and its grids, and the main suite from N = 65536 on. Case A out of cache was not affected, because its arrays happened not to lie at the slow distance.
  - Eigen's own figures moved by more than 8% in four configurations, by 1.09–1.32×. The distance matters mostly for the library.
  - In the other direction, slices taken in C++QED's old `SliceIterator` order (case D) were 1.1–1.3× faster on huge pages, which spare them TLB misses, except at n = 32 on axis 0, where the slow distance hit them too; slices in memory order were not affected otherwise. 4 KB pages are a fair reference for memory-order access, not for scattered access.
  - Huge pages also cause most of the spread between processes out of cache: up to 1.7× between processes, against at most 1.07× on 4 KB pages.
  - The ratios to Eigen in this page's summaries are now the 4 KB-page figures; the tables keep the earlier figures and add the 4 KB-page ones.

## The bandwidth ceiling

When the operator is as large as the state, all variants stream their data from main memory. Case A at N = 262144 (29 MB):

| | ns per nonzero |
|---|---|
| library, by rows | **1.06** |
| library, one diagonal at a time | 1.56 |
| hand-written hoisted loop | 1.57 |
| hand-written row-blocked loop: one diagonal at a time per block of 1024 rows | 1.21 |
| Eigen sparse (with `-fcx-limited-range`) | 2.47 (2.20) |

The memory traffic per nonzero:

| | bytes per nonzero |
|---|---|
| stored diagonal value | 16 |
| CSR value plus column index | 20 |
| one-diagonal-at-a-time sweep: value, plus x read, plus y read and written | 64 |

Summing the diagonals of a row before the store brings the traffic down to the value plus a share of x and y, as does the hand-written row-blocked loop, which applies all diagonals while a block of y is in cache. **Once both sides are limited by bandwidth, the best possible gain of the format over CSR is therefore about 20%.**

On the reference machine a single thread does not reach that limit with Eigen's product. The library's row shape moves about as many bytes per nonzero in 0.43 of the time, so out of cache it is 2.4× faster than Eigen in case A and 2.1× in case B, on 4 KB pages. In the main suite at N = 1048576 it is 2.0–2.3× faster on 4 KB pages and 1.4–1.6× on huge pages, where one diagonal at a time was 1.2–1.45× slower.

With several threads, or a machine with less bandwidth per core, the 20% bound applies. Doing better would require not storing the values at all, e.g. generating √n on the fly, and that is outside the library's scope.

## Operators applied slice by slice: C++QED's setting

In C++QED, a term of the Hamiltonian is a MultiDiagonal over its own few axes, e.g. one mode with dimension 32. C++QED's `Broadcaster` applies it to every slice of the full state along the remaining ("dummy") axes. The MultiDiagonal never sees the larger array; that is a design decision of C++QED. In this setting:

- the operator data is tiny and stays in L1;
- the state is large and is streamed once per term;
- the slices of an inner axis are strided.

The honest competitor for the *format* is therefore a CSR matrix of the term's own space, applied to the same slices. In the tables this is "term CSR": Eigen with a strided map. A CSR matrix of the whole system, flattened from the Kronecker product as QuTiP does, answers a different question: it compares architectures, not formats.

Case D, n⁴ state, operator on axis 0 (contiguous slices) or axis 2 (stride n²). All values are with the slices taken in memory order, first dummy axis fastest. Eigen's figures with `-fcx-limited-range` are in brackets:

| | n=32, axis 0 | n=30, axis 0 | n=32, axis 2 | n=30, axis 2 |
|---|---|---|---|---|
| library `operator()` | 2.64 | **0.90** | **2.36** | **1.57** |
| library, one diagonal at a time | 2.63 | 0.89 | 6.00 | 1.70 |
| library, by rows | 1.84 | 1.24 | 2.32 | 1.54 |
| hand-written, one diagonal at a time | 2.55 | 0.80 | 5.94 | 1.53 |
| hand-written, all diagonals per output row (scalar) | 2.60 | 2.63 | 3.96 | 2.91 |
| term CSR | 2.64 (2.49) | 2.38 (1.73) | 4.07 (3.71) | 2.67 (2.16) |
| CSR of the whole system | 2.57 (2.29) | 2.58 (2.30) | 2.61 (2.39) | 2.57 (2.33) |
| library `operator()`, 4 KB pages | **0.89** | **0.91** | **2.45** | **1.64** |
| term CSR, 4 KB pages | 2.26 | 2.29 | 4.24 | 2.69 |

The rows above the last two were measured on huge pages; there the library's 2.64 for n = 32 on axis 0 is the slow distance between the arrays.

`operator()` applies the contiguous slices one diagonal at a time, since they are small, and the strided ones by rows.

Three findings:

1. **Slice order matters as much as the kernel.** C++QED's `SliceIterator` used to produce the slices last dummy axis fastest, so consecutive slices lay far apart in memory. In that order every method is 1.1–1.35× slower on axis 0 and 1.3–2.3× slower on axis 2. For example, the library takes 4.84 instead of 2.36 for n = 32 on axis 2, and term CSR 6.17 instead of 2.67 for n = 30. The order is C++QED's, not the library's, and since 2026-11-04 it is memory order there ([cppqed#90](https://github.com/WignerQuantumOptics/cppqed/issues/90)).
2. **The best loop shape depends on the slice length and the stride**, and `operator()` now takes it from the view.
   - On strided slices the row shape wins: 1.7× faster than term CSR for n = 30 (1.4× against its flagged build) and 1.7× (1.6×) for n = 32. A sweep per diagonal put all elements of a strided slice of power-of-two length (stride 1024 amplitudes, 16 KB) into the same L1 cache set and thrashed it: 6.00, against 2.36 now, and 1.5× slower than term CSR before.
   - On contiguous slices of length 30, one diagonal at a time is the fastest: 2.6× faster than term CSR (1.9× against its flagged build). The row shape would take 1.24, because of its fixed cost per row of axis 0.
   - On contiguous slices of length 32 the library came out at parity with term CSR, and case F showed both shapes slow out of cache at every power of two from 16 to 256 (one diagonal at a time 2.4–2.8 ns against 0.75–0.95 in between). That came from where the arrays lay, not from the length: cases D and F allocate `psi` and `dpsidt` back to back, 2²⁴ + 16 bytes apart for these states (see "Distance on huge pages"). On 4 KB pages the library takes 0.89 for n = 32 on axis 0, against 2.26 for term CSR: 2.5× faster, as for n = 30.
   - For contiguous slices of length 30, the build of 7725d79 measured 0.78 here, against 0.89–0.90 now for the same loop. A separate probe gives 0.88 for both, and neither loop alignment nor the assembler's mitigation of the jump erratum changes it. That figure was a property of that binary, not of the loop (see "Setup").
3. **What the design boundary costs.** Applying the operator to many adjacent slices at once (case C, "blocked") gives 1.28 for n = 32 on axis 2, 1.8× better than the best slice-by-slice method at the same n. Getting this inside C++QED's design would mean `Broadcaster` handing a term blocks of adjacent slices. This is noted here, not proposed.

Not measured, but relevant: a slice of rank ≥ 2 whose axes are not adjacent in memory, e.g. a coupling on axes 0 and 2 of a rank-3 system, is not uniformly strided. A flattened CSR matrix then has to translate each column index into a memory address, from the multi-index or from a precomputed table, or it has to copy the slice into a contiguous buffer and back. A MultiDiagonal applies per-axis strides directly.

## The interaction picture

In C++QED's hot loop, the interaction picture is applied by updating the operator with its frequencies and then applying it. No transformation of the state takes place around the application. The physics layer keeps a pair (coefficients $\alpha$, frequencies $\delta$) with the same diagonals, and before each right-hand-side evaluation writes $\alpha\,e^{\delta\tau}$ into the operator to be applied. The library itself carries no frequencies.

Structural facts:

- Let the free evolution be diagonal, $U=\mathrm{diag}(e^{\lambda_k\tau})$ with complex $\lambda_k$, which covers damping. The element in row $m$ and column $m+o$, which moves $o$ quanta, then acquires the factor $e^{(\lambda_{m+o}-\lambda_m)\tau}$. This depends only on the axes the operator moves quanta along; the dummy axes cancel. **The update therefore costs O(operator size), not O(state size)**, which in C++QED's setting is a few hundred exponentials per evaluation, against a state of 10⁶.
- What one diagonal needs depends on the spectrum:

  | spectrum | factor along one diagonal | data needed |
  |---|---|---|
  | linear in the quanta (Mode, Qbit, Spin, and sums of these) | constant | one scalar per diagonal |
  | quadratic (Kerr, particle kinetic energy) | geometric sequence | scale and ratio (a recurrence, refreshed periodically) |
  | higher polynomial, or arbitrary levels | — | one frequency per element |

  A polynomial spectrum loses one degree along a diagonal.
- A flattened sparse matrix can do the same update elementwise, with a frequency array parallel to its values. No multi-indices are needed for that. It cannot use one scalar per diagonal, however, because the flat offset does not determine the per-axis offsets. For extents (n₀, n₁), the offsets (−1, +1) and (n₀−1, 0) both have flat offset n₀−1, so one flat diagonal mixes elements with different phases even for a linear spectrum.

What `picture1` measures (rank 1, operator as large as the state; time relative to applying with no picture at all, N = 256 / 4096 / 262144):

| variant | relative time |
|---|---|
| one scalar per diagonal | 0.95 / 0.99 / 1.01 |
| one exponential per element | 8.9 / 7.8 / 6.3 |
| CSR, one exponential per nonzero | 9.3 / 8.1 / 6.6 |

`picture1` also times a transformation of the state around a static product, which C++QED does not use.

Because the operator is as large as the state here, the per-element update looks far more expensive than it would in C++QED's setting. What is missing:

- update plus apply in the slice-by-slice setting, for linear and quadratic spectra, against term CSR with a per-nonzero frequency array ([#5](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/5));
- the end-to-end question: how many fewer and longer ODE steps the interaction picture buys, by integrating the fast free evolution exactly. That gain is independent of the storage format and is probably the larger one. It can only be measured in C++QED once its exact propagators work ([cppqed#91](https://github.com/WignerQuantumOptics/cppqed/issues/91), blocked by [cppqed#57](https://github.com/WignerQuantumOptics/cppqed/issues/57)).

## Composition and construction

From `cppqed-multidiagonal-benchmarks`, time of Eigen sparse divided by time of MultiDiagonal (above 1 means MultiDiagonal is faster):

| | N = 16 | 1024 | 262144 |
|---|---|---|---|
| `compose1`: rank-1 product of two tridiagonals | 3.1 | 6.7 | 5.2 |
| `composeJC`: Jaynes–Cummings squared | 0.89 | 1.5 | 1.8 |
| `construct1`: rank-1, five diagonals | 1.6 | 1.7 | 2.4 |
| `constructJC`: Jaynes–Cummings via ⊗, + and scalars | 0.61 | 1.1 | 1.1 |

Composition never meets the symbolic pass and sorting of a sparse product: the offsets of the result are known from the offsets of the factors. In C++QED, however, composition and construction happen when the system is set up, e.g. assembling $H_\text{eff}=H-\tfrac{i}{2}\sum_k J_k^\dagger J_k$, not in the time-stepping loop. They matter little for run time.

## Assessment and next steps

| | today | with the remaining fixes |
|---|---|---|
| rank 1, in cache | 2.7–3.3× faster than Eigen (1.8–1.9× at N = 16) | same |
| several axes, in cache, long axis 0 (mode first) | 2.9–3.6× faster (v0.4.0: 3.4–4× slower) | same |
| several axes, in cache, short axis 0 (qubit first) | 1.4–2.2× faster (v0.4.0: 3.6–4.5× slower) | same |
| operator as large as the state, out of cache, rank 1 or mode first | 2.0–2.4× faster (4 KB pages) | same |
| operator as large as the state, out of cache, qubit first | 0.9–1.1× (4 KB pages; 0.6–0.8× on huge pages) | not yet known |
| slice by slice, n = 30, contiguous / strided | 2.5× / 1.6× faster than term CSR (4 KB pages) | same |
| slice by slice, n = 32, contiguous / strided | 2.5× / 1.7× faster (4 KB pages; contiguous at parity on huge pages) | same |
| interaction picture | — | not yet measured systematically |

- **"Today"** is the library as it is, in the default build, with both sides taking the slices in the same order. Ranges in cache span two builds of today's library (c346b8a, and one that differed only in `operator()`'s bounds), between which Eigen's speed moved by up to 20%.
- **Out of cache** the figures are for 4 KB pages. On huge pages, input and output allocated back to back can cost the library up to about 3× (see "Distance on huge pages", [#26](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/26)).

The format is not a large speed-up for every application, and should not be presented as one. Its advantages are:

- in cache it is 1.4–3.6× faster than the comparable sparse matrix on the reference machine, and out of cache 2.0–2.4× faster on 4 KB pages, except with a qubit on axis 0 of a large state (0.9–1.1×);
- the per-axis offsets are the quanta moved per subsystem, and they give per-axis strides, one phase per diagonal for linear spectra, and composition closure without index bookkeeping;
- it is far more readable than a flattened matrix.

Next steps:

1. Done on 2026-11-01: `operator()` rewritten with hoisted addresses and the innermost loop over axis 0 ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)).
2. Done on 2026-11-02: complex multiply-add in real arithmetic inside the kernel, and an axis 0 of up to four elements unrolled at compile time ([#23](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/23)).
3. Done on 2026-11-04: the diagonals of a row summed before one store, for long operands and strided views, with `operator()` choosing the shape from the view's strides and extents ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1), [#25](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/25)). This also settled the strided half of the next step.
4. The loop shapes that remain ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)):
   - done on 2026-11-04: contiguous slices of power-of-two length were slow out of cache because of the distance between the arrays on huge pages, not because of their length (case H);
   - a short axis 0 of a state larger than cache (qubit first), still applied one diagonal at a time: block the outer axes, or sum the diagonals over the few rows of axis 0. On 4 KB pages it is 1.36×, 1.07× and 0.89× as fast as Eigen at N = 65536, 262144 and 1048576, so there is little left to gain.
5. Benchmarks ([#5](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/5)):
   - done on 2026-11-04: the out-of-cache figures re-measured on 4 KB pages (`--no-thp`) against huge pages, interleaved;
   - done on 2026-11-04: the placement scan as a regression check for #25, for both shapes (case G);
   - make `picture1` call the library;
   - add term CSR through slices to the main suite;
   - add update plus apply for the interaction picture in the slice setting.

In C++QED: done on 2026-11-04, the slices in memory order ([cppqed#90](https://github.com/WignerQuantumOptics/cppqed/issues/90)); still to do, benchmark the interaction picture end to end ([cppqed#91](https://github.com/WignerQuantumOptics/cppqed/issues/91)).

**What to do next:** decide how to keep arrays out of the slow distance ([#26](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/26)). On huge pages it costs the library up to about 3× out of cache, while the remaining loop shape, for a qubit on axis 0 of a large state, is worth at most about 1.1×. C++QED's ODE stepper integrates the raw storage, `multiarray::StorageType<dcomp>` (a `std::vector`), and allocates its own scratch arrays of that type, so `MultiArray` alone can't prevent the slow distance; an allocator in `StorageType` that pads every allocation by a few hundred bytes could. The re-measurement, the power-of-two investigation, cppqed#90 and the placement regression check are done.
