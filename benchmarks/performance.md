# Performance of MultiDiagonal: measurements and outlook

This page records what the benchmarks show about applying a `MultiDiagonal` to a state, why the library is currently slower than it could be, and what it can and cannot gain over a general sparse matrix. It is meant as an honest baseline: where the library doesn't have the right loop yet, the figures for "what is possible" come from hand-written loops over the library's data in [`benchmarks/kernels.cc`](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/blob/main/benchmarks/kernels.cc), not from the library itself.

**Short version** (reference machine, default build of both libraries):

- **In cache**, the library is 1.9–3.4× faster than Eigen's sparse product at rank 1 and with the mode on axis 0, and 1.5–2.2× faster with a qubit on axis 0. Contiguous diagonals without column indices vectorize, while a CSR product gathers through column indices. The speed-up depends on the processor: on a 2022 laptop core it was 1.5–2.4× and 1.0–1.6×.
- **Out of cache**, where the operator is as large as the state, the library is between 1.5× faster and 1.45× slower than Eigen, depending on the size and on where its arrays land in memory. Once both sides are limited by memory bandwidth, no storage format gains much: MultiDiagonal stores 16 bytes per nonzero against 20 for a compressed sparse row (CSR) matrix.
- **In C++QED's actual setting**, a small operator applied slice by slice across a large state, the library is 1.4–3× faster than a CSR matrix of the same term. The exception is a power-of-two slice length: there it is at parity on contiguous slices and up to 1.6× slower on strided ones, where a row-oriented loop would reach parity.
- **History.** Up to v0.4.0, the multi-axis loop recomputed every address per element, which made it 3.4–4.5× slower than Eigen at rank 2 and 18× slower at rank 4; fixed on 2026-11-01 ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)). Complex multiplication with the C99 Annex G NaN checks halved the speed of the default build; fixed on 2026-11-02, together with a compile-time unrolled short axis 0 ([#23](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/23)).
- **What remains.** The loop writes the output once per diagonal. That is the wrong shape for operators larger than cache and for slices with a power-of-two stride. It also makes the speed depend on where input and output sit relative to each other in memory, typically by 6% and up to 1.2–2× ([#25](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/25)). A loop that sums the diagonals of a row before writing it addresses all three, without changing the representation.
- **The interaction picture** has not been benchmarked systematically yet (see the section on it below).

## Setup

- **Reference machine** since 2026-11-03: Intel Core i7-7700 desktop (Kaby Lake, 4 cores; L1d 32 KB and L2 256 KB per core, L3 8 MB), Arch Linux, GCC 16.2.1, Eigen 5.0.1, `-O3 -march=native`, single thread.
- **Prepared for measuring:** no graphical session, timers stopped, SMT off, turbo off (fixed 3.6 GHz, `performance` governor). Each run is pinned to a core that handles no device interrupts.
- **Every figure** is the median over five separate processes, with the binaries of different versions interleaved. Processes of the same binary typically agree within 1–2% (90% within 5–8%). Larger differences come from where the arrays land in memory (see "Placement of the arrays" below), not from the machine.
- **Versions**, all built and measured with the same toolchain on 2026-11-03:
  - "v0.4.0": the library of v0.4.0, with the benchmarks of e49319b;
  - "before #23": 84cf7d4;
  - "now": 7725d79.
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
./build-release/benchmarks/cppqed-multidiagonal-kernels        # cases A–E, default complex multiplication
./build-release/benchmarks/cppqed-multidiagonal-kernels-cxlr   # the same with -fcx-limited-range
./build-release/benchmarks/cppqed-multidiagonal-benchmarks     # apply1, compose1, …, picture1
```

`kernels.cc` cases:

| case | what is applied |
|---|---|
| A | rank 1, five diagonals (offsets −2…+2), N = 256 (in L2), 4096 (in L3), 262144 (out of cache) |
| B | rank-2 Jaynes–Cummings Hamiltonian, mode on axis 0, qubit on axis 1; state 512 (in L2), 8192 (in L3), 524288 (out of cache) |
| C | a five-diagonal operator of dimension n = 32 on one axis of an n⁴ state (10⁶ amplitudes), applied to the whole state at once |
| D | the same operator applied slice by slice, as C++QED's `Broadcaster` does; n = 32 and n = 30 |
| E | tridiagonal ⊗ tridiagonal on extents {n₀, 8192/n₀}, for an axis 0 of n₀ = 1 … 32 elements |

## Complex multiplication

By default both GCC and Clang implement `std::complex<double>` multiplication according to C99 Annex G: every product checks for NaN and falls back to a call to `__muldc3`. The check stops the loop from vectorizing. Up to 2026-11-01 it more than halved the library's speed, and `-fcx-limited-range` was needed to remove it.

Since 2026-11-02 ([#23](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/23)) the kernel multiplies with `textbookProduct`, i.e. $(ac-bd)+i(ad+bc)$ written out in real arithmetic. It omits only the recovery of infinite results, which a physical state never needs. The speed no longer depends on the flag:

| case A, in cache (N = 256 / 4096) | default | `-fcx-limited-range` |
|---|---|---|
| library before #23 | 1.50 / 1.58 | 0.64 / 0.78 |
| library now | **0.65 / 0.83** | 0.65 / 0.81 |
| Eigen sparse | 1.80 / 1.95 | 1.62 / 1.77 |

With the flag, before and after agree; in the default build, the library is now 1.9–2.3× faster than before. The flag also speeds up Eigen:

- by 10–17% in cases A to C;
- by 1.1–1.4× for term CSR on slices (case D).

Eigen's sparse product gathers through column indices, so it doesn't vectorize, but the check still costs it. The hand-written loops in `kernels.cc` use `textbookProduct` too, so the flag now changes only Eigen's figures.

The other loops that multiply complex numbers were checked the same way (laptop). Composition, addition and construction (`compose1`, `composeJC`, `construct1`, `constructJC`) are dominated by index arithmetic, allocation and the map. The flag changes them by nothing beyond the noise, so they keep the standard multiplication.

All further figures are from the default build. With the flag, Eigen's would be 10–17% lower.

## The library's multi-axis loop

Up to v0.4.0, `operator()` stepped a multi-index through each diagonal, last axis fastest, and computed the address of every element from all its indices. At rank 1 this cost little, but at higher rank it dominated.

Since 2026-11-01 ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)), `operator()` works as follows:

- For each diagonal, it computes the addresses of the first row and the first column once.
- A loop nest generated at compile time then runs over the axes, with axis 0 innermost. Axis 0 is contiguous in memory, both in the diagonal and in an owning state.
- The outer axes only add strides to pointers.

This is the "hoisted" loop of `kernels.cc`, generalized to strided views:

| | v0.4.0 | before #23 | now | hand-written hoisted loop | Eigen sparse |
|---|---|---|---|---|---|
| B: Jaynes–Cummings, in cache (state 512 / 8192) | 6.95 / 7.24 | 1.27 / 1.63 | **0.59 / 0.96** | 0.58 / 0.96 | 1.96 / 2.18 |
| B: Jaynes–Cummings, out of cache (state 524288) | 7.77 | 4.34 | 3.34 | 3.34 | 3.05 |
| C: rank-4 operator built as $\mathbb{1}\otimes\mathbb{1}\otimes A\otimes\mathbb{1}$, $A$ on axis 0 / axis 2 (out of cache) | 47.7 / 46.6 | 3.37 / 3.25 | 2.64 / 2.31 | — | 2.58 / 2.61 |

- **Case B in cache:** the library is now 8–12× faster than at v0.4.0 and 2.3–3.3× faster than Eigen.
- **Case B out of cache:** it sweeps the state once per diagonal and is 1.1× slower than Eigen.
- **Case C:** the rank-4 operator improves 18–20× and is at parity with Eigen. It stays bandwidth bound, because the materialized operator is five times as large as the state.

The 2026-10 review had flipped the loop order alone and saw no effect beyond noise. That is consistent: memory order only pays once the per-element address computation is gone.

`cppqed-multidiagonal-benchmarks` applies the Jaynes–Cummings Hamiltonian in both axis orders, for mode dimensions N from 16 to 1048576 (state 2N), and a five-diagonal rank-1 operator of dimension N. The table gives the time of Eigen sparse divided by the time of MultiDiagonal; above 1 means MultiDiagonal is faster. Up to N = 1024, operator and state fit in L2, and up to N = 16384 in L3. For `apply1`, N = 65536 (about 7 MB) sits at the edge of L3:

| | v0.4.0, N ≤ 16384 | before #23, N ≤ 16384 | now, N ≤ 16384 | now, N = 65536 / 262144 / 1048576 |
|---|---|---|---|---|
| `applyJC_modefast`: mode on axis 0 | 0.25–0.29 | 1.35–1.58 | **2.14–3.39** | 1.72 / 1.39 / 0.82 |
| `applyJC_qbitfast`: qubit on axis 0 | 0.22–0.28 | 1.04–1.19 | **1.50–2.18** | 1.18 / 0.81 / 0.61 |
| `apply1`: rank 1 | 1.04–1.30 | 1.30–1.75 | **1.92–3.22** | 1.08 / 1.46 / 0.69 |

A spot check with Clang 18 (laptop) gave the same picture, with smaller gains from #23, because Clang's code was faster than GCC's before #23 as well.

### A short axis 0

With the qubit on axis 0, as in C++QED's `jc_qjmc`, the loop over axis 0 has one or two elements; the σ± diagonals have extent 1 along the qubit. Setting up a vectorized loop then costs more than the work in it. With #23's multiplication alone, the mode-first case became about twice as fast, but the qubit-first case up to 1.4× slower (laptop).

Since 2026-11-02, `operator()` therefore dispatches on the length of axis 0 once per diagonal. For up to four elements, the innermost level of the loop nest is unrolled at compile time. The loop over axis 1 then becomes the innermost one, and the compiler vectorizes that instead.

Case E shows where the two shapes cross over (ns per nonzero):

| length of axis 0 | 1 | 2 | 3 | 4 | 5 | 6 | 8 | 16 | 32 |
|---|---|---|---|---|---|---|---|---|---|
| library (unrolled up to 4) | 0.93 | 1.34 | 1.29 | 1.32 | 1.27 | 1.24 | 1.14 | 0.99 | 0.92 |
| hand-written hoisted loop (always loops over axis 0) | 3.72 | 2.98 | 2.16 | 1.80 | 1.38 | 1.31 | 1.18 | 1.03 | 0.94 |
| Eigen sparse | 2.43 | 1.94 | 1.89 | 1.87 | 1.86 | 2.00 | 1.84 | 1.88 | 1.85 |

Up to four elements unrolling wins, by up to 4×; from five on, the loop over axis 0 is as good.

## Placement of the arrays

The loop writes `dpsidt` once per diagonal and element. So its speed depends on where `psi` lies relative to `dpsidt` within a 4 KB page ([#25](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/25)). The processor compares only the low 12 address bits of a load with the stores still in flight. For some placements it then holds loads from `psi` back behind stores to `dpsidt` that they don't depend on ("4K aliasing").

A standalone probe placed `psi` at every 16-byte offset of a page relative to `dpsidt` (rank 1, five diagonals; ns per application):

| loop | N | best | median | worst | offsets more than 5% above the best |
|---|---|---|---|---|---|
| library `operator()`: one diagonal at a time | 1024 | 2976 | 3165 | 3524 | 142 / 256 |
| all diagonals of a row summed in a register | 1024 | 10971 | 10986 | 11004 | 0 / 256 |
| library `operator()` | 4096 | 13507 | 14059 | 16448 | 47 / 256 |
| all diagonals of a row summed in a register | 4096 | 44166 | 44197 | 44320 | 0 / 256 |

- **A typical placement costs about 6%, a bad one 18–22%.** The slow placements are those where `psi` lies a little below `dpsidt`, and the default allocation puts the two arrays exactly there.
- **Occasionally a whole process runs about 2× slower** with an identical layout, presumably because of the state of the processor's memory-disambiguation predictor.
- **Eigen, and the row-oriented loop, store once per row and are not affected.** The row-oriented loop in this table is naive scalar code, 3.5× slower; only its insensitivity counts here.
- **With the qubit on axis 0, placement barely matters (1 offset in 256); with the mode first, nearby placements cost up to 17%.**
- **Out of cache, physical placement matters too.** At N = 1048576 an isolated process takes 2.2 ns per nonzero for `apply1`, but 3.4 ns inside the main suite, with the same in-page offsets. The physical pages there come from a long allocation history. Huge pages make at most a 5% difference.

This is why every figure on this page is a median over separate processes.

## The bandwidth ceiling

When the operator is as large as the state, all variants stream their data from main memory. Case A at N = 262144 (29 MB):

| | ns per nonzero |
|---|---|
| library | 1.56 |
| hand-written hoisted loop | 1.56 |
| row-blocked loop: all diagonals per block of rows | 1.20 |
| Eigen sparse (with `-fcx-limited-range`) | 2.37 (2.19) |

The memory traffic per nonzero:

| | bytes per nonzero |
|---|---|
| stored diagonal value | 16 |
| CSR value plus column index | 20 |
| one-diagonal-at-a-time sweep: value, plus x read, plus y read and written | 64 |

The one-diagonal-at-a-time sweep is the library's current loop. Blocking the rows, so that all diagonals are applied while a block of y is in cache, brings the traffic down to the value plus a share of x and y. **Once both sides are limited by bandwidth, the best possible gain of the format over CSR is therefore about 20%.**

On the reference machine a single thread does not reach that limit with Eigen's product. The row-blocked loop moves about as many bytes per nonzero in half the time. So out of cache, the library is 1.5× faster than Eigen in case A, and the row-blocked loop 2×. In case B (state 524288) the library is 1.1× slower. At N = 1048576 in the main suite it is up to 1.45× slower, which is the placement effect above.

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
| library `operator()` | 2.53 | 0.78 | 5.95 | 1.54 |
| one diagonal at a time, addresses hoisted | 2.54 | 0.80 | 5.94 | 1.58 |
| all diagonals per output row | 2.33 | 2.08 | 3.80 | 2.58 |
| term CSR | 2.81 (2.48) | 2.32 (1.69) | 4.09 (3.69) | 2.64 (2.16) |
| CSR of the whole system | 2.45 (2.26) | 2.46 (2.28) | 2.52 (2.38) | 2.47 (2.32) |

The library's `operator()` runs the same loop as the hand-written "one diagonal at a time" kernel.

Three findings:

1. **Slice order matters as much as the kernel.** `SliceIterator` produces the slices last dummy axis fastest, so consecutive slices lie far apart in memory. In that order every method is 1.1–1.4× slower on axis 0 and 1.3–2.3× slower on axis 2. For example, the library takes 7.72 instead of 5.95 for n = 32 on axis 2, and term CSR 6.16 instead of 2.64 for n = 30. Fixing the order belongs to C++QED, not to the library ([cppqed#90](https://github.com/WignerQuantumOptics/cppqed/issues/90)).
2. **The best loop shape depends on the slice length and the stride.**
   - Without a power of two (n = 30), one diagonal at a time is the fastest: 3.0× faster than term CSR on contiguous slices (2.2× against its flagged build), and 1.7× (1.4×) on strided ones.
   - With a power of two (n = 32) every method slows down, the library most: from 0.78 to 2.53 on contiguous slices, and from 1.54 to 5.95 on strided ones. That leaves it at parity with term CSR on contiguous slices and 1.5–1.6× slower on strided ones.
     - For strided slices, all elements of a slice (stride 1024 amplitudes, 16 KB) fall into the same L1 cache set, and sweeping y once per diagonal thrashes it. Summing all diagonals of an output row in a register reaches parity with term CSR (3.80 against 3.69–4.09).
     - For contiguous slices the cause is not established. The laptop, with its larger and more associative L1, did not show it.

   The kernel can choose between the shapes from the view's strides and extents. That is the library's business, since it only concerns `operator()` on a strided view ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)).
3. **What the design boundary costs.** Applying the operator to many adjacent slices at once (case C, "blocked") gives 1.28 for n = 32 on axis 2, 3× better than any slice-by-slice method at the same n. Getting this inside C++QED's design would mean `Broadcaster` handing a term blocks of adjacent slices. This is noted here, not proposed.

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
| one scalar per diagonal | 0.96 / 1.00 / 1.02 |
| one exponential per element | 8.9 / 7.8 / 6.2 |
| CSR, one exponential per nonzero | 9.3 / 8.1 / 6.5 |

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
| rank 1, in cache | 1.9–3.2× faster than Eigen | same |
| several axes, in cache, long axis 0 (mode first) | 2.1–3.4× faster (v0.4.0: 3.4–4× slower) | same |
| several axes, in cache, short axis 0 (qubit first) | 1.5–2.2× faster (v0.4.0: 3.6–4.5× slower) | same |
| operator as large as the state, out of cache | from 1.5× faster to 1.45× slower | 2× faster at rank 1 (case A, row-blocked); 20% at most where bandwidth bound |
| slice by slice, contiguous slices, n = 30 | 2.2–3.0× faster than term CSR | same |
| slice by slice, strided slices, n = 30 | 1.4–1.7× faster than term CSR | same |
| slice by slice, n = 32 (power of two) | parity (contiguous) to 1.6× slower (strided) | parity (strided, row-oriented); contiguous open |
| interaction picture | — | not yet measured systematically |

- **"Today"** is the library as it is, in the default build, with both sides taking the slices in the same order. Ranges for term CSR span its default and its `-fcx-limited-range` build.
- **"With the remaining fixes"** means blocking for long operands and the row-oriented loop for power-of-two strides. The hand-written loops of `kernels.cc` stand in for them.

The format is not a large speed-up for every application, and should not be presented as one. Its advantages are:

- in cache it is 1.5–3.4× faster than the comparable sparse matrix on the reference machine; out of cache and for power-of-two slice lengths it ranges from 1.5× faster to 1.6× slower until the remaining fixes land;
- the per-axis offsets are the quanta moved per subsystem, and they give per-axis strides, one phase per diagonal for linear spectra, and composition closure without index bookkeeping;
- it is far more readable than a flattened matrix.

Next steps, in order:

1. Done on 2026-11-01: `operator()` rewritten with hoisted addresses and the innermost loop over axis 0 ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)).
2. Done on 2026-11-02: complex multiply-add in real arithmetic inside the kernel, and an axis 0 of up to four elements unrolled at compile time ([#23](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/23)).
3. For long operands, apply all diagonals per block of rows, accumulating several diagonals per store so that the placement of the arrays no longer matters ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1), [#25](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/25)).
4. Choose the loop shape from the view's strides and extents: one diagonal at a time, except where the strides cause L1 set conflicts (power-of-two strides), and there all diagonals per output row. Find out why a power-of-two length also slows down contiguous slices on the reference machine ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)).
5. Benchmarks ([#5](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/5)):
   - make `picture1` call the library;
   - add term CSR through slices to the main suite;
   - add update plus apply for the interaction picture in the slice setting;
   - add the placement scan as a regression check for #25.

In C++QED: produce slices in memory order ([cppqed#90](https://github.com/WignerQuantumOptics/cppqed/issues/90)), and benchmark the interaction picture end to end ([cppqed#91](https://github.com/WignerQuantumOptics/cppqed/issues/91)).
