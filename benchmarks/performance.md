# Performance of MultiDiagonal: measurements and outlook

This page records what the benchmarks show about applying a `MultiDiagonal` to a state, why the library is currently slower than it could be, and what it can and cannot gain over a general sparse matrix. It is meant as an honest baseline: where the library doesn't have the right loop yet, the figures for "what is possible" come from hand-written loops over the library's data in [`benchmarks/kernels.cc`](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/blob/main/benchmarks/kernels.cc), not from the library itself.

**Short version.**

- Where the operator is as large as the state, application is limited by memory bandwidth, and no storage format helps much. MultiDiagonal stores 16 bytes per nonzero against 20 for a compressed sparse row (CSR) matrix.
- Where the operator data stays in cache, contiguous diagonals without column indices vectorize, and the library is 1.5–2.4× faster than Eigen's sparse product. In C++QED's actual setting, a small operator applied slice by slice across a large state, a loop of the right shape is 1.1–1.9× faster than a sparse matrix of the same term.
- Up to v0.4.0, the library's multi-axis loop recomputed every address per element, which made it 3–5× slower at rank 2 in cache and about 30× slower than Eigen at rank 4. Since 2026-11-01 `operator()` computes the addresses once per diagonal and runs its innermost loop over axis 0 ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)).
- Since 2026-11-02 the kernel multiplies in real arithmetic, without the C99 Annex G NaN checks that halved its speed, so the default build is as fast as one with `-fcx-limited-range`. An axis 0 of up to four elements, e.g. a qubit, is unrolled at compile time. In the default build, the library is now 1.5–2.4× faster than Eigen in cache with the mode on axis 0, and 1.0–1.6× faster with the qubit on axis 0, where it was up to 1.4× slower ([#23](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/23)).
- The library does not yet have the best loop everywhere: one diagonal at a time is the wrong shape for operators larger than cache, and for strided slices whose stride is a power of two. Both are fixable without changing the representation.
- The interaction picture has not been benchmarked systematically yet (see the section on it below).

## Setup

- Intel Core i7-1260P laptop (hybrid cores, runs not pinned to a core), WSL2 on Ubuntu 24.04, GCC 13.3, Eigen 3.4.0, `-O3 -march=native`, single thread.
- Dates and library versions:
  - `kernels.cc` (cases A–D) and the application benchmarks `apply1` and `applyJC`: measured 2026-11-01, with `operator()` rewritten after v0.4.0 ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)). Kernel figures are medians of three runs.
  - Figures marked "v0.4.0" are from the kernel measurement of 2026-10-29. The `applyJC` ones are from a rerun of the v0.4.0 binaries on 2026-11-01, which also reproduced the 2026-10-29 kernel figures within the noise.
  - Composition, construction and `picture1`: measured 2026-10-29 at v0.4.0. The code they time has not changed since.
  - Figures for [#23](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/23) (complex multiplication and the short axis 0): measured 2026-11-02, before and after the change, interleaved. The host was noisier than usual that day, so these are medians over all runs of identical binaries: 5–9 for cases A and B and the application benchmarks, three for case E. A spot check with Clang 18 is reported where it differs.
- Unit: nanoseconds per stored nonzero, i.e. per `nonZeros()` of the equivalent Eigen matrix. "In cache" means operator and state together fit in L2 (1.25 MB per core); "out of cache" means they exceed L3 (18 MB).
- Repeated runs differ by about ±20%, more out of cache. Treat ratios below 1.3 as parity.
- Every variant is checked against Eigen before it is timed.

To reproduce:

```bash
cmake -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --target cppqed-multidiagonal-benchmarks cppqed-multidiagonal-kernels cppqed-multidiagonal-kernels-cxlr
./build-release/benchmarks/cppqed-multidiagonal-kernels        # cases A–E, default complex multiplication
./build-release/benchmarks/cppqed-multidiagonal-kernels-cxlr   # the same with -fcx-limited-range
./build-release/benchmarks/cppqed-multidiagonal-benchmarks     # apply1, compose1, …, picture1
```

`kernels.cc` cases:

| case | what is applied |
|---|---|
| A | rank 1, five diagonals (offsets −2…+2), N = 256, 4096, 262144 |
| B | rank-2 Jaynes–Cummings Hamiltonian, mode on axis 0, qubit on axis 1 |
| C | a five-diagonal operator of dimension n = 32 on one axis of an n⁴ state (10⁶ amplitudes), applied to the whole state at once |
| D | the same operator applied slice by slice, as C++QED's `Broadcaster` does; n = 32 and n = 30 |
| E | tridiagonal ⊗ tridiagonal on extents {n₀, 8192/n₀}, for an axis 0 of n₀ = 1 … 32 elements |

## Complex multiplication

By default both GCC and Clang implement `std::complex<double>` multiplication according to C99 Annex G: every product checks for NaN and falls back to a call to `__muldc3`. The check stops the loop from vectorizing. Up to 2026-11-01 it halved the library's speed, and `-fcx-limited-range` was needed to remove it.

Since 2026-11-02 ([#23](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/23)) the kernel multiplies with `textbookProduct`, i.e. $(ac-bd)+i(ad+bc)$ written out in real arithmetic. It omits only the recovery of infinite results, which a physical state never needs. The speed no longer depends on the flag:

| case A, in cache (N = 256 / 4096) | default | `-fcx-limited-range` |
|---|---|---|
| library before #23 | 1.02 / 1.07 | 0.53 / 0.76 |
| library now | **0.57 / 0.84** | 0.47 / 0.95 |
| Eigen sparse | 1.21 / 1.27 | 1.09 / 1.27 |

With the flag, before and after agree within the noise; in the default build, the library is now 1.3–1.8× faster than before. The flag also speeds up Eigen, by 0–20% in cases A and B and by 1.2–1.5× for term CSR on strided slices (case D): its sparse product gathers through column indices, so it doesn't vectorize, but the check still costs it. The hand-written loops in `kernels.cc` use `textbookProduct` too, so the flag now changes only Eigen's figures.

The other loops that multiply complex numbers were checked the same way. Composition, addition and construction (`compose1`, `composeJC`, `construct1`, `constructJC`) are dominated by index arithmetic, allocation and the map, and the flag changes them by nothing beyond the noise; they keep the standard multiplication.

The tables of cases B, C and D below were measured on 2026-11-01 with `-fcx-limited-range`, before #23. For the library and the hand-written loops they now hold in the default build: a re-measurement on 2026-11-02 agreed within the noise. For Eigen they are the more favourable figures.

## The library's multi-axis loop

Up to v0.4.0, `operator()` stepped a multi-index through each diagonal, last axis fastest, and computed the address of every element from all its indices. At rank 1 this cost nothing measurable, but at higher rank it dominated.

Since 2026-11-01 ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)), `operator()` works as follows:

- For each diagonal, it computes the addresses of the first row and the first column once.
- A loop nest generated at compile time then runs over the axes, with axis 0 innermost. Axis 0 is contiguous in memory, both in the diagonal and in an owning state.
- The outer axes only add strides to pointers.

This is the "hoisted" loop of `kernels.cc`, generalized to strided views:

| | v0.4.0 | library now | hand-written hoisted loop | Eigen sparse |
|---|---|---|---|---|
| B: Jaynes–Cummings, in cache (state 512 / 8192) | 2.54 / 2.73 | **0.53 / 0.71** | 0.52 / 0.71 | 1.11 / 1.17 |
| B: Jaynes–Cummings, out of cache (state 524288) | 2.87 | 2.28 | 2.23 | 1.44 |
| C: rank-4 operator built as $\mathbb{1}\otimes\mathbb{1}\otimes A\otimes\mathbb{1}$, $A$ on axis 0 / axis 2 (out of cache) | 41 | 2.19 / 2.20 | — | 1.66 / 1.69 |

In cache, the library is now about 4–5× faster than at v0.4.0 and 1.6–2.1× faster than Eigen. Out of cache, it sweeps the state once per diagonal and is about 1.3–1.6× slower than Eigen (see the next section). The rank-4 case improves 19×. It stays bandwidth bound, because the materialized operator is five times as large as the state.

The 2026-10 review had flipped the loop order alone and saw no effect beyond noise. That is consistent: memory order only pays once the per-element address computation is gone.

In the default build, `cppqed-multidiagonal-benchmarks` applies the Jaynes–Cummings Hamiltonian in both axis orders, for mode dimensions N from 16 to 1048576 (state 2N), and a five-diagonal rank-1 operator of dimension N. The table gives the time of Eigen sparse divided by the time of MultiDiagonal; above 1 means MultiDiagonal is faster. Up to N = 4096, operator and state fit in L2; up to N = 65536, in L3:

| | v0.4.0, N ≤ 65536 | before #23, N ≤ 16384 | now, N ≤ 16384 | now, N = 65536 / 262144 / 1048576 |
|---|---|---|---|---|
| `applyJC_modefast`: mode on axis 0 | 0.39–0.44 | 1.09–1.21 | **1.45–2.38** | 1.27 / 1.09 / 0.86 |
| `applyJC_qbitfast`: qubit on axis 0 | 0.39–0.47 | 0.70–1.03 | **1.04–1.58** | 1.12 / 0.82 / 0.68 |
| `apply1`: rank 1 | — | 1.06–1.19 | **1.52–2.22** | 1.17 / 1.43 / 0.89 |

Eigen here is in the default build too; with `-fcx-limited-range` it would be up to 20% faster. A spot check with Clang 18 gives the same picture, with smaller gains, because Clang's code was faster than GCC's before #23 as well: with the qubit first 1.5–1.9× faster than before up to N = 4096, with the mode first and at rank 1 between parity and 1.5× faster.

### A short axis 0

With the qubit on axis 0, as in C++QED's `jc_qjmc`, the loop over axis 0 has one or two elements; the σ± diagonals have extent 1 along the qubit. Setting up a vectorized loop then costs more than the work in it: with #23's multiplication alone, the mode-first case became about twice as fast, but the qubit-first case up to 1.4× slower.

Since 2026-11-02, `operator()` therefore dispatches on the length of axis 0 once per diagonal. For up to four elements, the innermost level of the loop nest is unrolled at compile time, so the loop over axis 1 becomes the innermost one, and the compiler vectorizes that instead. This settles the case of a short axis 0, which the next steps had listed under the loop shape.

Case E shows where the two shapes cross over (default build, ns per nonzero):

| length of axis 0 | 1 | 2 | 3 | 4 | 5 | 6 | 8 | 16 | 32 |
|---|---|---|---|---|---|---|---|---|---|
| library (unrolled up to 4) | 0.71 | 0.87 | 1.00 | 1.00 | 0.91 | 0.91 | 0.85 | 0.88 | 0.74 |
| hand-written hoisted loop (always loops over axis 0) | 2.32 | 1.73 | 1.11 | 1.19 | 0.81 | 0.84 | 0.87 | 0.68 | 0.80 |
| Eigen sparse | 1.65 | 1.21 | 1.43 | 1.29 | 1.22 | 1.37 | 1.36 | 1.17 | 1.33 |

Up to four elements unrolling wins, by up to 3.3×; from five on, the loop over axis 0 is as good, within the noise.

## The bandwidth ceiling

When the operator is as large as the state, all variants stream their data from main memory. Out of cache (case A at N = 262144), the library, the hoisted loop, a row-blocked loop and Eigen all fall in 1.1–1.7 across runs. The medians are 1.14 for the row-blocked loop and 1.33–1.40 for the others; on 2026-10-29 the four spread over 1.4–2.6 in no consistent order. In case B out of cache, the library (2.28) is slower than Eigen (1.44), because it sweeps the state once per diagonal.

The memory traffic explains why:

| | bytes per nonzero |
|---|---|
| stored diagonal value | 16 |
| CSR value plus column index | 20 |
| one-diagonal-at-a-time sweep: value, plus x read, plus y read and written | 64 |

The one-diagonal-at-a-time sweep is the library's current loop. Blocking the rows, so that all diagonals are applied while a block of y is in cache, brings the traffic down to the value plus a share of x and y. Even then the best possible gain over CSR is about 20%.

**For operators as large as the state, the format can at best match a general sparse matrix.** Doing better would require not storing the values at all, e.g. generating √n on the fly, and that is outside the library's scope.

## Operators applied slice by slice: C++QED's setting

In C++QED, a term of the Hamiltonian is a MultiDiagonal over its own few axes, e.g. one mode with dimension 32. C++QED's `Broadcaster` applies it to every slice of the full state along the remaining ("dummy") axes. The MultiDiagonal never sees the larger array; that is a design decision of C++QED. In this setting:

- the operator data is tiny and stays in L1;
- the state is large and is streamed once per term;
- the slices of an inner axis are strided.

The honest competitor for the *format* is therefore a CSR matrix of the term's own space, applied to the same slices. In the tables this is "term CSR": Eigen with a strided map. A CSR matrix of the whole system, flattened from the Kronecker product as QuTiP does, answers a different question: it compares architectures, not formats.

Case D, n⁴ state, operator on axis 0 (contiguous slices) or axis 2 (stride n²). All values are with the slices taken in memory order, first dummy axis fastest:

| | n=32, axis 0 | n=30, axis 0 | n=32, axis 2 | n=30, axis 2 |
|---|---|---|---|---|
| library `operator()` | 0.64 | 0.62 | 3.65 | 0.80 |
| one diagonal at a time, addresses hoisted | 0.63 | 0.64 | 3.44 | 0.96 |
| all diagonals per output row | 1.34 | 1.32 | 1.39 | 1.34 |
| term CSR | 1.20 | 1.21 | 1.53 | 1.35 |
| CSR of the whole system | 1.54 | 1.47 | 1.38 | 1.49 |

The library's `operator()` now runs the same loop as the hand-written "one diagonal at a time" kernel (at v0.4.0: 0.90, 0.69, 4.06 and 1.34).

Three findings:

1. **Slice order matters more than any kernel.** `SliceIterator` produces the slices last dummy axis fastest, so consecutive slices lie far apart in memory. In that order every method is 1.2–2.3× slower on axis 0 and 1.9–4.7× slower on axis 2. For example, the library takes 1.40 and 6.82 for n = 32 instead of 0.64 and 3.65, and term CSR 5.99 instead of 1.53. Fixing the order belongs to C++QED, not to the library ([cppqed#90](https://github.com/WignerQuantumOptics/cppqed/issues/90)).
2. **The best loop shape depends on the stride.**
   - Contiguous slices: one diagonal at a time vectorizes, and is about 1.9× faster than term CSR.
   - Strided slices with a power-of-two stride (n = 32: 1024 amplitudes, 16 KB): all elements of a slice fall into the same L1 cache set. Sweeping y once per diagonal then thrashes the cache (3.65, against 0.80 for n = 30). Summing all diagonals of an output row in a register avoids this and reaches parity with term CSR (1.39 against 1.53).
   - Strided slices without such set conflicts (n = 30): one diagonal at a time stays the fastest, at 0.80, against 1.34 for summing per row and 1.35 for term CSR.

   The kernel can choose between the shapes from the view's strides. That is the library's business, since it only concerns `operator()` on a strided view ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)).
3. **What the design boundary costs.** Applying the operator to many adjacent slices at once (case C, "blocked") gives 0.77 for n = 32 on axis 2, about 1.8× better than any slice-by-slice method at the same n. Getting this inside C++QED's design would mean `Broadcaster` handing a term blocks of adjacent slices. This is noted here, not proposed.

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

What `picture1` measures (default build; rank 1, operator as large as the state; time relative to applying with no picture at all, N = 256 / 4096 / 262144):

| variant | relative time |
|---|---|
| one scalar per diagonal | 1.02 / 1.06 / 1.04 |
| one exponential per element | 12 / 8.8 / 6.9 |
| CSR, one exponential per nonzero | 12 / 9.3 / 7.3 |

`picture1` also times a transformation of the state around a static product, which C++QED does not use.

Because the operator is as large as the state here, the per-element update looks far more expensive than it would in C++QED's setting. What is missing:

- update plus apply in the slice-by-slice setting, for linear and quadratic spectra, against term CSR with a per-nonzero frequency array ([#5](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/5));
- the end-to-end question: how many fewer and longer ODE steps the interaction picture buys, by integrating the fast free evolution exactly. That gain is independent of the storage format and is probably the larger one. It can only be measured in C++QED once its exact propagators work ([cppqed#91](https://github.com/WignerQuantumOptics/cppqed/issues/91), blocked by [cppqed#57](https://github.com/WignerQuantumOptics/cppqed/issues/57)).

## Composition and construction

From `cppqed-multidiagonal-benchmarks`, time of Eigen sparse divided by time of MultiDiagonal (above 1 means MultiDiagonal is faster):

| | N = 16 | 1024 | 262144 |
|---|---|---|---|
| `compose1`: rank-1 product of two tridiagonals | 3.7 | 6.8 | 9.2 |
| `composeJC`: Jaynes–Cummings squared | 1.7 | 2.6 | 4.5 |
| `construct1`: rank-1, five diagonals | 1.8 | 1.6 | 4.3 |
| `constructJC`: Jaynes–Cummings via ⊗, + and scalars | 0.5 | 0.65 | 1.3 |

Composition never meets the symbolic pass and sorting of a sparse product: the offsets of the result are known from the offsets of the factors. In C++QED, however, composition and construction happen when the system is set up, e.g. assembling $H_\text{eff}=H-\tfrac{i}{2}\sum_k J_k^\dagger J_k$, not in the time-stepping loop. They matter little for run time.

## Assessment and next steps

| | today | with the remaining fixes |
|---|---|---|
| rank 1, in cache | 1.5–2.2× faster than Eigen | same |
| several axes, in cache, long axis 0 (mode first) | 1.5–2.4× faster (v0.4.0: 2.1–2.6× slower) | same |
| several axes, in cache, short axis 0 (qubit first) | 1.0–1.6× faster (v0.4.0: 2.1–2.6× slower) | same |
| operator as large as the state, out of cache | ≈ Eigen to 1.5× slower | ≈ Eigen (bandwidth bound) |
| slice by slice, contiguous slices | 1.9× faster than term CSR (2.5× against its default build) | same |
| slice by slice, strided slices | from 2.4× slower (power-of-two stride) to 1.7× faster than term CSR | 1.1–1.7× faster |
| interaction picture | — | not yet measured systematically |

- "Today" is the library as it is, after [#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1) and [#23](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/23), in the default build, with both sides taking the slices in the same order. Eigen is in the default build too, except in the slice rows, which compare against term CSR with `-fcx-limited-range` (the tables above); that flag makes Eigen up to 20% faster, and term CSR 1.2–1.5×.
- "With the remaining fixes" means blocking for long operands and the row-oriented loop for power-of-two strides. The hand-written loops of `kernels.cc` stand in for them.

The format is not a large speed-up for application, and should not be presented as one. Its advantages are:

- with the fixes, it is never slower than the comparable sparse matrix in the measured cases, and about 1.5–2.4× faster where the loops vectorize;
- the per-axis offsets are the quanta moved per subsystem, and they give per-axis strides, one phase per diagonal for linear spectra, and composition closure without index bookkeeping;
- it is far more readable than a flattened matrix.

Next steps, in order:

1. Done on 2026-11-01: `operator()` rewritten with hoisted addresses and the innermost loop over axis 0 ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)).
2. Done on 2026-11-02: complex multiply-add in real arithmetic inside the kernel, and an axis 0 of up to four elements unrolled at compile time ([#23](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/23)).
3. For long operands, apply all diagonals per block of rows ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)).
4. Choose the loop shape from the view's strides: one diagonal at a time, except where the strides cause L1 set conflicts (power-of-two strides), and there all diagonals per output row ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)).
5. Benchmarks ([#5](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/5)):
   - make `picture1` call the library;
   - add term CSR through slices to the main suite;
   - add update plus apply for the interaction picture in the slice setting.

In C++QED: produce slices in memory order ([cppqed#90](https://github.com/WignerQuantumOptics/cppqed/issues/90)), and benchmark the interaction picture end to end ([cppqed#91](https://github.com/WignerQuantumOptics/cppqed/issues/91)).
