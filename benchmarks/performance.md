# Performance of MultiDiagonal: measurements and outlook

This page records what the benchmarks show about applying a `MultiDiagonal` to a state, why the library is currently slower than it could be, and what it can and cannot gain over a general sparse matrix. It is meant as an honest baseline: most of the figures for "what is possible" come from hand-written loops over the library's data in [`benchmarks/kernels.cc`](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/blob/main/benchmarks/kernels.cc), not from the library itself.

**Short version.**

- Where the operator is as large as the state, application is limited by memory bandwidth, and no storage format helps much. MultiDiagonal stores 16 bytes per nonzero against 20 for a compressed sparse row (CSR) matrix.
- Where the operator data stays in cache, contiguous diagonals without column indices vectorize, and a well-written loop is 1.4–2× faster than Eigen's sparse product. In C++QED's actual setting, a small operator applied slice by slice across a large state, it is 1.1–1.8× faster than a sparse matrix of the same term.
- The library does not reach this today, for three reasons:
  - Complex multiplication with the C99 Annex G NaN checks halves its speed ([#23](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/23)).
  - Its multi-axis loop recomputes every address per element. This costs 3–5× at rank 2 in cache, and makes rank 4 about 30× slower than Eigen ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)).
  - One diagonal at a time is the wrong loop shape for strided slices whose stride is a power of two.

  All three are fixable without changing the representation.
- The interaction picture has not been benchmarked systematically yet (see the section on it below).

## Setup

- Intel Core i7-1260P laptop (hybrid cores, runs not pinned to a core), WSL2 on Ubuntu 24.04, GCC 13.3, Eigen 3.4.0, `-O3 -march=native`, single thread. Library at v0.4.0; measured 2026-10-29.
- Unit: nanoseconds per stored nonzero, i.e. per `nonZeros()` of the equivalent Eigen matrix. "In cache" means operator and state together fit in L2 (1.25 MB per core); "out of cache" means they exceed L3 (18 MB).
- Repeated runs differ by about ±20%, more out of cache. Treat ratios below 1.3 as parity.
- Every variant is checked against Eigen before it is timed.

To reproduce:

```bash
cmake -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --target cppqed-multidiagonal-benchmarks cppqed-multidiagonal-kernels cppqed-multidiagonal-kernels-cxlr
./build-release/benchmarks/cppqed-multidiagonal-kernels        # cases A–D, default complex multiplication
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

## Complex multiplication

By default both GCC and Clang implement `std::complex<double>` multiplication according to C99 Annex G: every product checks for NaN and falls back to a call to `__muldc3`. The check stops the loop from vectorizing. The existing benchmark binary has 128 such call sites. `-fcx-limited-range` removes them:

| case A, in cache (N = 256 / 4096) | default | `-fcx-limited-range` |
|---|---|---|
| library `operator()` | 1.02 / 1.08 | **0.50 / 0.72** |
| Eigen sparse | 1.10 / 1.12 | 0.99 / 1.03 |

The flag halves MultiDiagonal's time and leaves Eigen's unchanged. Eigen's sparse product gathers through column indices, so it doesn't vectorize either way. The "parity at rank 1" reported earlier was therefore both libraries being held back by the same scalar complex multiply.

Remedy for the library ([#23](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/23)): write the multiply-add inside the kernel in real arithmetic, so that the speed does not depend on a flag the consumer may not set. (Not yet measured.) Executables can also set `-fcx-fortran-rules`, which drops the multiplication check but keeps the careful division. With Clang 18, both flags also need `-Wno-overriding-option` because of a spurious driver warning.

All further figures are with `-fcx-limited-range` unless stated otherwise.

## The library's multi-axis loop

`operator()` steps a multi-index through each diagonal, last axis fastest, and computes the address of every element from all its indices. At rank 1 this costs nothing measurable: in case A the library equals a hand-written loop. At higher rank it dominates. The "hoisted" loop in `kernels.cc` computes the base addresses of each diagonal once and runs the innermost loop over axis 0, which is contiguous in memory:

| | library | hoisted loop | Eigen sparse |
|---|---|---|---|
| B: Jaynes–Cummings, in cache (state 512 / 8192) | 2.54 / 2.73 | **0.51 / 0.84** | 1.05 / 1.05 |
| B: Jaynes–Cummings, out of cache (state 524288) | 2.87 | 1.96 | 1.68 |
| C: rank-4 operator built as $\mathbb{1}\otimes\mathbb{1}\otimes A\otimes\mathbb{1}$ (out of cache) | **41** | — | 1.45 |

In cache, the hoisted loop is 3–5× faster than the library and about 1.3–2× faster than Eigen. The 2026-10 review had flipped the loop order alone and saw no effect beyond noise. That is consistent: memory order only pays once the per-element address computation is gone.

## The bandwidth ceiling

When the operator is as large as the state, all variants stream their data from main memory. Out of cache (case A at N = 262144), the library, the hoisted loop, a row-blocked loop and Eigen all fall in 1.4–2.6 across runs, in no consistent order. In case B out of cache, the hoisted loop (1.96) is slightly slower than Eigen (1.68), because it sweeps the state once per diagonal.

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
| library `operator()` | 0.90 | 0.69 | 4.06 | 1.34 |
| one diagonal at a time, addresses hoisted | **0.78** | **0.66** | 3.77 | **1.02** |
| all diagonals per output row | 1.32 | 1.33 | **1.62** | 1.49 |
| term CSR | 1.21 | 1.20 | 1.80 | 1.46 |
| CSR of the whole system | 1.85 | 1.66 | 1.98 | 1.59 |

Three findings:

1. **Slice order matters more than any kernel.** `SliceIterator` produces the slices last dummy axis fastest, so consecutive slices lie far apart in memory. In that order every method is 1.3–2.2× slower on axis 0 and 2–4.7× slower on axis 2. For example, the library takes 1.30 and 8.25 for n = 32 instead of 0.90 and 4.06, and term CSR 7.45 instead of 1.80. Fixing the order belongs to C++QED, not to the library ([cppqed#90](https://github.com/WignerQuantumOptics/cppqed/issues/90)).
2. **The best loop shape depends on the stride.**
   - Contiguous slices: one diagonal at a time vectorizes, and is about 1.6–1.8× faster than term CSR.
   - Strided slices with a power-of-two stride (n = 32: 1024 amplitudes, 16 KB): all elements of a slice fall into the same L1 cache set. Sweeping y once per diagonal then thrashes the cache (3.77, against 1.02 for n = 30). Summing all diagonals of an output row in a register avoids this and reaches parity with term CSR.

   The kernel can choose between the two from the view's strides. That is the library's business, since it only concerns `operator()` on a strided view ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)).
3. **What the design boundary costs.** Applying the operator to many adjacent slices at once (case C, "blocked") gives 0.83 for n = 32 on axis 2, about 2× better than any slice-by-slice method at the same n. Getting this inside C++QED's design would mean `Broadcaster` handing a term blocks of adjacent slices. This is noted here, not proposed.

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

| | today | with the fixes (measured with hand-written loops) |
|---|---|---|
| rank 1, in cache | ≈ Eigen | 1.4–2× faster |
| several axes, in cache | 2–2.7× slower | 1.25–2× faster |
| operator as large as the state, out of cache | ≈ Eigen | ≈ Eigen (bandwidth bound) |
| slice by slice, contiguous slices | 0.8–1.25× the speed of term CSR | 1.6–1.8× faster |
| slice by slice, strided slices | from 1.8× slower to 1.1× faster than term CSR | 1.1–1.4× faster |
| interaction picture | — | not yet measured systematically |

"Today" is the library as it is, in the default build, with both sides taking the slices in the same order.

The format is not a large speed-up for application, and should not be presented as one. Its advantages are:

- with the fixes, it is never slower than the comparable sparse matrix in the measured cases, and up to about 2× faster where the loops vectorize;
- the per-axis offsets are the quanta moved per subsystem, and they give per-axis strides, one phase per diagonal for linear spectra, and composition closure without index bookkeeping;
- it is far more readable than a flattened matrix.

Next steps, in order:

1. Rewrite `operator()` with hoisted addresses and the innermost loop over axis 0 ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)).
2. Complex multiply-add in real arithmetic inside the kernel ([#23](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/23)).
3. For long operands, apply all diagonals per block of rows ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)).
4. Choose the loop shape from the view's strides: one diagonal at a time when axis 0 is contiguous, all diagonals per output row otherwise ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)).
5. Benchmarks ([#5](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/5)):
   - make `picture1` call the library;
   - add term CSR through slices to the main suite;
   - add update plus apply for the interaction picture in the slice setting.

In C++QED: produce slices in memory order ([cppqed#90](https://github.com/WignerQuantumOptics/cppqed/issues/90)), and benchmark the interaction picture end to end ([cppqed#91](https://github.com/WignerQuantumOptics/cppqed/issues/91)).
