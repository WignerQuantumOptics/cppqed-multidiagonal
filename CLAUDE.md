# CLAUDE.md — cppqed-multidiagonal

Header-only C++23 library for operators on tensor-product Hilbert spaces that are
**multidiagonal**: every nonzero matrix element moves a fixed number of quanta per axis.
Extracted from C++QED v3 (github.com/WignerQuantumOptics/cppqed), which consumes it through
FetchContent pinned to a commit SHA (or an installed 0.5.x). A change here reaches
C++QED only when its pin is bumped; its jc_qjmc regression test must still pass then.
It is being prepared for standalone publication, so API stability, honest documentation
and test coverage matter more than feature growth.

Released as v0.5.0. No DOI yet. License: BSL-1.0.

## Layout

```
include/multidiagonal/
  Traits.h            FWD, Hana/JSON setup, passByValue_v, ReferenceMF/ConstReferenceMF
  MultiArray.h        rank-R strided array: MultiArrayView (non-owning), MultiArray (owning, move-only)
  MultiArrayComplex.h dcomp specializations, textbookProduct, directProduct, twoTimesRealPartOf…, Eigen interop
  MultiDiagonal.h     the operator algebra, including identity() and composition
tests/                Catch2 v3 suites: testMultiArray, testMultiArrayComplex, testMultiDiagonal
  consumer/           downstream CMake project for the CI consumer job; consumer.cc starts with
                      the README's usage example
benchmarks/           benchmarkMultiDiagonal.cc, plot.py; kernels.cc (loop experiments);
                      performance.md (measurements and outlook, a Doxygen page)
cmake/                package config template
Doxyfile              API docs; README.md is the main page, read through doxygen-math.sed
                      (Doxygen 1.9 does not understand GitHub's $…$ math)
```

- Consumers include the headers as `<multidiagonal/X.h>`, and they install there (#2).
  The prefix keeps `Traits.h` from colliding with C++QED's own. Within the library the
  headers include each other with quotes, which resolve to the sibling file first.
- There is no `SliceIterator.h` here. It exists only in C++QED (`CPPQEDutils/`), and
  where it should live is an open decision (cppqed C-04). `MultiArray.h:55` still refers
  to it.

The library is header-only and exposed as an INTERFACE target `cppqed::multidiagonal`.
Do not add translation units. Everything defined at namespace scope in a header must be
`inline` or a template, otherwise there will be ODR violations in consumers.

## Build and test

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

- A Debug build enables the bounds and dimension checks, and the tests exercise them.
  CI runs Debug×Release × GCC/Clang with `CMAKE_COMPILE_WARNING_AS_ERROR=ON`, so code
  must compile warning-free under `-Wall -Wextra -Wpedantic` on both compilers. CI builds
  the benchmarks with `MULTIDIAGONAL_BENCHMARK_NATIVE=OFF`: the runners' processors vary, and
  Clang 18 with `-march=native` warns about AVX10 on some of them.
- CI also runs an ASan+UBSan job (Clang Debug, tests and benchmark smoke) and a consumer
  job: `tests/consumer` is a separate CMake project that uses the library via
  `find_package` after install and via FetchContent at the pushed SHA. Runners are pinned
  to `ubuntu-24.04` and actions to commit SHAs.
- C++23 is required (`0uz` literals, `std::ranges::fold_left`), which means roughly
  GCC ≥ 13 or Clang ≥ 17.
- Catch2 (tests only) is pinned to a full commit SHA, and an installed Catch2 3 is used when found (#11).
  Otherwise the configure step downloads it through FetchContent; see the working rules at the end of
  this file. Offline: `FETCHCONTENT_FULLY_DISCONNECTED=ON` plus `FETCHCONTENT_SOURCE_DIR_CATCH2`.
- Options: `MULTIDIAGONAL_BUILD_BENCHMARKS` (default ON), `MULTIDIAGONAL_BENCHMARK_NATIVE` (ON),
  `MULTIDIAGONAL_INSTALL` (ON only in a top-level build). Tests and benchmarks are built only at top
  level. The INTERFACE target carries `cxx_std_23`; minimum Boost 1.80 and Eigen 3.4 (Eigen found without
  a version and checked afterwards, so that Eigen 5 is accepted), repeated in the package config (#9).
- Clang 23 warns about Catch2's `__COUNTER__` under `-Wpedantic` (`-Wc2y-extensions`); the test target
  turns that warning off where the compiler has it (#24). CI's Clang 18 doesn't, so CI doesn't cover it.

## Core representation (MultiDiagonal.h)

- `MultiDiagonal<RANK>` stores `std::map<std::array<ptrdiff_t,RANK>, Diagonal>`, where
  `Diagonal = MultiArray<dcomp,RANK>`.
  - The key is the **signed offset per axis**, i.e. the quanta moved. Positive means
    upper, negative means lower, and 0 means main.
  - A diagonal at offset d on an axis of dimension N has length N−|d| along that axis.
- The map is ordered deliberately, because it makes accumulation order deterministic and
  results reproducible. Do not switch it to `unordered_map`.
- Composition `A | B` follows the rule that offsets add.
  - It runs over all pairs of diagonals, with per-axis window arithmetic.
  - Colliding offset pairs must accumulate into the same output diagonal (finding F1b).
  - Composition is rank-general.
- Direct product `A * B` concatenates the offset arrays and takes the `directProduct` of
  the diagonals.
- Hermitian conjugation negates the key **and** conjugates the values. (Not to be confused
  with F4, which concerned the dense `hermitianConjugateSelf`.) The value conjugation is
  covered by complex-valued tests against the dense adjoint (#4); keep at least one
  conjugated operator in the suite genuinely complex.
- Application `y += A ψ` has accumulate semantics. Callers in C++QED apply H/i; the
  library does not care about that convention.
- **No frequency or interaction-picture information lives here.** This is a decided
  design boundary, recorded for the paper. Phase tabulation, i.e. a `(coefficients,
  frequencies)` pair of MultiDiagonals, belongs to C++QED proper. Do not add it to this
  repo.
- Construction (#15, decided 2026-11-07): `multidiagonal::diagonal(offset, values)` gives the rank-1
  operator with one diagonal (dimension `values.size()+|offset|`), and operators are built from these
  with `+`, `|` and `*`. Only a diagonal whose coefficients don't factorize over the axes is emplaced
  into `diagonals` directly. The README's usage example is the first part of
  `tests/consumer/consumer.cc`; keep the two in sync.
- Rank-1 construction from raw storage goes through `multiarray::fromStorage`. The vector
  constructor was removed to fix an ambiguity (finding F9).

## MultiArray facts that are easy to get wrong

- Memory layout:
  - `calculateStrides` gives axis 0 stride 1, i.e. first index fastest (`layout_left`).
    Don't change it: the layout underpins the leading-dimension performance argument and
    a possible mdspan layout policy.
  - `incrementMultiIndex` iterates in the opposite order, last index fastest. Both
    docstrings say so (#13).
- Views (`MultiArrayView`, `MultiArrayConstView`) are passed by value, like `std::span`.
  `MultiArray` owns its data and is move-only. `passByValue_v<T>` encodes this split.
- `hermitianConjugateSelf` and `twoTimesRealPartOfSelf` take views, because C++QED's
  master equation has its ODE derivative only as a view. They address the flat storage,
  so the view must be full and contiguous; `checkContiguous` enforces this in Debug (#8).
- Beware dangling `dataView`/iterators whenever storage is reallocated (finding F3 was
  `load()`). `std::vector` move preserves the buffer, but reallocation does not.
- `Placeholder` slicing exists only for RANK==2.

## Testing conventions

- Catch2 v3. Every behavioral change or new feature gets a test.
- **Dense oracle:** `densify()` probes an operator with unit vectors to build a dense
  matrix. Algebraic identities are checked against it, for example:
  - `(A|B)|C == A|(B|C)`
  - `(A*B)† == A†*B†`
  - distributivity
- **Truncation:** operators are truncated, so `[a, a†] == 1` is *false*. Compare against
  the dense truncated oracle, never against untruncated textbook identities.
- **Tags:**
  - `[contract]` marks acceptance tests for documented findings. Never weaken one to make
    it pass.
  - `[!mayfail]` is temporary scaffolding only. Before declaring a fix done, run once with
    tags ignored and remove the `[!mayfail]` whose fix landed.
- **Findings ledger (closed):** bugs found by tests before issues moved to GitHub carry
  F-numbers. The prefix avoids GitHub auto-linking `#N` to unrelated issues. All are resolved:
  - F1 composition extents; F1b colliding offset pairs must accumulate; F2 JSON
    serialization of MultiDiagonal; F3 dangling `dataView` after `load()`; F4 missing
    conjugation of the diagonal in `hermitianConjugateSelf`; F5 quadrupled diagonal in
    `twoTimesRealPartOfSelf`; F6 NaN from `relativeDeviation(0,0)`; F7 JSON complex read
    path; F8 missing serialization include; F9 rank-1 constructor ambiguity; F10 narrowing
    at the Eigen::Map boundary; F11 sign-compare in `checkBounds`.
  - Do not extend the F-series. A new finding gets a GitHub issue; cite its number in the
    test name and the commit message (`#N` in this repo, `WignerQuantumOptics/cppqed-multidiagonal#N`
    from elsewhere).

## Benchmarks

- `benchmarkMultiDiagonal` compares MultiDiagonal against Eigen sparse and Eigen dense.
  - Benchmarks: `apply1`, `compose1`, `construct1`, `applyJC_{qbitfast,modefast}`,
    `composeJC`, `constructJC` (also checked against a dense matrix up to N = 256), `picture1`
    (library: update, then `operator()`; hand-written fused loops labelled `handwritten_*` and
    `csr_pernnz`; Eigen's product after a per-nonzero update; the "sandwich"), and `slices3`
    (a term on axis 0 or 2 of an n³ state, slice by slice in memory order, against term CSR
    through strided Maps, plus the picture updated and applied there).
  - Every configuration cross-validates against Eigen **before** timing and exits 1 on a
    mismatch.
- `kernels.cc` holds the application-loop experiments (cases A–F: rank 1, Jaynes–Cummings,
  an operator on one axis of a rank-4 state, the same applied slice by slice, a short
  axis 0, and the crossover of the two loop shapes, which sets `operator()`'s choice). It
  times the library as `operator()` chooses and with each shape forced. Case G, the
  placement scan of #25 (psi at every 16-byte offset of a page relative to dpsidt, both
  shapes), takes minutes and runs only when named, or in `--smoke`. So does case H, the
  distance between psi and dpsidt in one buffer, on huge pages and on 4 KB pages (it
  calls `madvise` under `__linux__` and reads `AnonHugePages` from `/proc`), and case I, the
  crossover of the two shapes for contiguous slices over 15 placements, in states of
  2¹²…2²⁰ amplitudes on 4 KB pages (median and worst per configuration to stderr). It is built
  twice: `cppqed-multidiagonal-kernels` and `cppqed-multidiagonal-kernels-cxlr`
  (`-fcx-limited-range`; Clang 18 also needs `-Wno-overriding-option`). It validates
  before timing too.
- Output is CSV on stdout. `plot.py` reads `benchmarkMultiDiagonal`'s CSV
  (`benchmark,impl,N,d,time_per_op_ns`); keep the two in sync.
- Use `doNotOptimize` to prevent elision.
- `--smoke` (both executables) shrinks the sizes. It is a CI correctness check only, and
  its timings are meaningless. CI runs all three binaries with it.
- **`benchmarks/performance.md` is the record of results and diagnosis**, and a page of the
  Doxygen site (linked from the README and from `operator()`). When numbers or the
  diagnosis change, update it rather than this file. In brief (2026-11-04):
  - The reference machine is a quiet i7-7700 desktop (Kaby Lake; GCC 16, Eigen 5.0.1;
    turbo and SMT off, pinned to a core without device interrupts). Every figure is a median
    over five separate processes. Speed-ups over Eigen depend on the processor, so give them
    per machine; the earlier laptop figures are in the page's history.
  - Instruction set (2026-11-05, performance.md "Instruction set"): all figures are for
    `-march=native` (AVX2 + FMA on bench). On the x86-64 baseline (no `-march`) the library is
    typically 1.4–1.7× slower, Eigen 1.25–1.3×, so the in-cache advantage over Eigen drops from
    2.6–3.4× to 1.5–2.0×. `x86-64-v3` equals `native` on bench. C++QED builds for `x86-64-v3` by
    default (cppqed#93); `jc_qjmc` gains 1.12–1.38× per derivative call from it over the baseline.
  - Default `std::complex` multiplication (Annex G NaN checks, `__muldc3`) used to halve the
    library's speed. Since #23 the kernel multiplies with `textbookProduct`, so the default
    build is as fast as `-fcx-limited-range`. That flag makes Eigen 10–17% faster (term CSR
    on slices 1.1–1.4×). Composition and construction don't care.
  - Application has two loop shapes, both compile-time loop nests with axis 0 innermost from
    addresses computed once, and both public so that tests and `kernels.cc` can force each:
    - `applyByDiagonals` (since 2026-11-01, #1): one diagonal at a time. This fixed the
      per-element addressing, which had made the loop 3.4–4.5× slower than Eigen at rank 2
      and 18× slower at rank 4. An axis 0 of up to four elements (qubit first, as in
      `jc_qjmc`) is unrolled at compile time, so the loop over axis 1 is innermost (#23,
      `kernels.cc` case E).
    - `applyByRows` (since 2026-11-04, #1, #25): the diagonals with entries in a row are
      listed along the outer axes and summed along axis 0 before one store, up to six per
      store (eight spill registers). The rows at either end of axis 0 are summed one by
      one, a fixed cost per row of axis 0.
    - `operator()` takes `applyByDiagonals` for an axis 0 of up to four, and for views
      contiguous along axis 0 that are small (≤ 128 amplitudes), or fit in the last-level
      cache (≤ 2¹⁸) with an axis 0 shorter than 32, or (since 2026-11-07, #27) are slices of
      ≤ 512 amplitudes whose storage (`dataView`, the whole state) holds ≥ 2¹⁸ amplitudes;
      `applyByRows` otherwise. The bounds are the measured crossovers (cases F and I, the
      latter as medians over 15 placements within a page); re-measure before changing them.
      Out of cache the row shape on contiguous slices of a few hundred amplitudes depends on
      the placement within a page by up to 2.6× (prefetch coverage); one diagonal at a time
      doesn't. A test (`#27`) checks the choice bitwise: onto a nonzero dpsidt the shapes round
      differently.
  - In cache the library is 2.7–3.6× faster than Eigen at rank 1 and with the mode first
    (1.8–2.2× at dimension 16), 1.4–2.2× with the qubit first. In cache, ratios to Eigen move
    by up to 20% between builds, because Eigen's own speed depends on the code layout; the
    library's speed-up over its earlier versions doesn't.
  - Keep everything the application loops read local or by value (the diagonal pointer is
    passed and returned, bounds and strides are copied). Clang doesn't vectorize through
    captured references once it stops inlining the lambda.
  - `applyByDiagonals` stores once per diagonal and element. That makes its speed depend on
    where `psi` lies relative to `dpsidt` within a page (4K aliasing, #25): typically 6%, up
    to 1.2×, and occasionally 2× for a whole process. `applyByRows` stores once per element
    and varies by at most 1.10×. Compare as medians over processes. Code layout matters too:
    the same per-diagonal loop measured 0.78 and 0.89 ns per nonzero in two binaries.
  - Out of cache, on transparent huge pages (`always` on bench, `madvise` on the laptop), the
    *distance* between `psi` and `dpsidt` matters (`kernels.cc` case H, 2026-11-04): when they
    nearly coincide modulo 1 MiB (`dpsidt` from 16 bytes short to 64 beyond), one diagonal at a
    time is up to 3.4× slower and the row shape up to 1.7×; on 4 KB pages nothing changes.
    Two arrays of a multiple of 1 MiB allocated back to back lie exactly there (2²⁴ + 16 for
    16 MiB). This, not the slice length, made case F's power-of-two slices slow. The mechanism
    (which structure conflicts) is not established. Measure out of cache with `--no-thp`
    (both executables: `prctl(PR_SET_THP_DISABLE)`), where the distance doesn't matter, or
    control the distance. Re-measured that way on 2026-11-04: 76 of 289 configurations had
    moved by more than 8% on huge pages, almost all of them the library's or the hand-written
    loops' (Eigen's in four, by 1.09–1.32×); scattered access
    (the old `SliceIterator` order) is instead 1.1–1.3× faster on huge pages (TLB).
  - Out of cache, on 4 KB pages: 2.0–2.4× faster than Eigen, with the qubit first 0.9–1.1×
    (still one diagonal at a time; 0.89× at N = 2²⁰). On huge pages these were 1.4–2.5× and
    0.6–0.8×. Once both are bandwidth bound, a format gains at most about 20%; single-threaded
    on the reference machine Eigen isn't.
  - In C++QED's slice-by-slice setting, the honest competitor is a CSR matrix of the term's
    own space on the same slices. On 4 KB pages MultiDiagonal is 2.5–2.6× faster on contiguous
    slices (n = 30 and 32) and 1.5–1.6× on strided ones (the row shape avoids the L1 set
    conflicts of power-of-two strides); 1.3–2.0× against term CSR with `-fcx-limited-range`
    (re-measured 2026-11-08). The earlier parity at n = 32 contiguous was the slow
    distance. C++QED's `SliceIterator` used to produce the slices last
    dummy axis fastest, which cost every method 1.1–2.3× more; it takes them in memory order
    since 2026-11-04 (cppqed#90).
  - Composition wins up to 7× at rank 1 (0.9–1.8× for Jaynes–Cummings), but it happens at
    setup time, not in the time-stepping loop.
  - Interaction picture (2026-11-05): C++QED's hot loop updates `(coefficients, frequencies)`
    and applies; it never transforms the state around a static product, so don't present that
    "sandwich" as the reference. Slice by slice (`slices3`) the update costs almost nothing:
    (update + apply) ÷ apply is 0.99–1.02 for the library, 0.98–1.10 for term CSR per nonzero;
    the format's advantage there is the application (term CSR 1.6–2.9× slower, except axis 2 at
    n = 16, 1.09). For an operator as large as the state (`picture1`) the update costs 1.5–3× an
    application with one exp per diagonal; one exp per nonzero is 5–11× slower still. An earlier
    "0.95–1.01×" compared two hand-written loops and is withdrawn.

## Open items (roughly in priority order)

Recommended next (2026-11-07): the performance work is done and released as v0.4.2; v0.5.0 adds
the packaging and API items for the paper (below). What remains of the performance work is the qubit-first loop shape (#1, worth at most about 1.1×). Settled: #27 (the 1 MiB window
of #26, and the placement-dependent row shape, handled by the new rule), the benchmark defects of #5, the slow distance on huge pages (#26: jemalloc, linked by
C++QED when installed; `StorageType` unchanged), the out-of-cache re-measurement (`--no-thp`), the
power-of-two investigation (case H), cppqed#90 and the placement regression check (case G). Outside performance: the
`std::mdspan` question is decided (#14, below); the construction API (#15) with the README usage
example, CMake packaging (#9, #16), the Catch2 pin (#11), the Clang 23 warning (#24) and reading JSON
back (#18) are done (2026-11-07).

1. Application performance (#1; see `benchmarks/performance.md`, next steps): A short
   axis 0 of a state larger than cache (qubit first) is still applied one diagonal at a time, at
   0.9–1.1× of Eigen on 4 KB pages. The slow distance on huge pages is settled (#26): jemalloc
   avoids it, and `StorageType` stays a plain `std::vector`; don't add an allocator unasked. (The hoisted loop,
   the multiplication in real arithmetic, the short axis 0, the row shape for long operands and
   strided views, the placement scan, the power-of-two investigation and the out-of-cache
   re-measurement are done.)
2. `std::mdspan`: decided (András, 2026-11-07, #14; C++QED side cppqed#74). The paper describes the
   current `MultiArray`/`MultiArrayView` architecture, and `std::mdspan` is a prospect for its outlook.
   The prototype (#19) and C++QED's epic (cppqed#20, #75–#78) stay open as prospects, unscheduled.
   **Do not start a migration unasked.**
3. For the paper, outside the code: a DOI (Zenodo's GitHub integration, a repository setting, so
   András's action; the first DOI comes with the next release, cite the concept DOI), the venue, and
   the draft. `CITATION.cff` has András's ORCID (2026-11-07). Cosmetic: #17 (finding
   test names, `[contract]` tags), #20 (the Doxygen theme).
4. Boost.Serialization (#16, done): the library uses only its headers (`MultiArray`'s `save`/`load`)
   and doesn't link the compiled library; whoever writes archives does (C++QED, and this test
   suite). CI's consumer job builds without it. HDF5 is a separate, later C++QED question.

## Style

- camelCase.
  - Acronyms: capitalize the first letter only (`Ode`, `Tdp`). This is cosmetic, so don't
    mass-rename.
  - `FWD(x)` for perfect forwarding; `dcomp` for complex double.
- Doxygen with `///` comments. Document concepts and invariants, not the obvious.
- Public signatures must not depend on the build configuration. For example, silence
  unused parameters with `(... , (void)i);` in the `#else` branch rather than
  `#ifdef`-guarding declarator-ids.
- In docs and comments, use **physics language**: "quanta moved", "the phase belongs to
  the diagonal", "a polynomial spectrum loses one degree". Avoid algebra vocabulary such
  as "homomorphism" or "character".
- Document limitations honestly. Do not oversell performance.
- Facts that must stay correct in any prose:
  - Displacement and squeezing operators are **not** multidiagonal in the Fock basis;
    their generators are.
  - v2's `Tridiagonal` held three bands {−d, 0, +d} at a single scale.
- Every file starts with a BSL-1.0 copyright line, following the existing headers.

## Versioning and releases

- SemVer, with annotated tags that carry detailed messages.
- The package version file is `SameMinorVersion` while the major version is 0 (#9): an installed
  0.y.z satisfies only requests for 0.y. A minor release therefore means bumping consumers' requests,
  such as C++QED's `FIND_PACKAGE_ARGS`, together with their pins. `tests/consumer` checks the rule.
- Zenodo is not connected yet. A DOI per GitHub release (via Zenodo's GitHub integration)
  is a possible later step, e.g. before the paper submission.
- Versioning is Claude's call (András, 2026-10-26): version numbers (0.x: a breaking change
  bumps the minor version), when to release, the version in `CMakeLists.txt`,
  `CITATION.cff` and the `Doxyfile` (`PROJECT_NUMBER`), the annotated tag, the GitHub release notes, and the release milestones.
  Deleting or moving a pushed tag still needs András's OK.

## Working rules (Wigner Research Centre institutional policy)

- **git, gh, glab:** Claude runs them with András's existing logins and remotes: fetch, pull,
  push, issues, labels, milestones, Project items, and CI runs. Ask first before:
  - force-push or rebases of pushed history
  - adding new remotes
  - changing repository or organization settings
  - deleting issues, tags, or other GitHub objects
- **Other network commands** (downloads, configure steps that download) and **package
  installs:** only on explicit request.
- **Out-of-tree files and credentials:** commands that delete or move files outside this
  repo, or that create, change, or reveal keys or tokens (e.g. `gh auth refresh`): show the
  command, explain it, and let András run it.

  Building, testing, and editing inside the repo are fine.
- **Destructive local operations:** ask for confirmation first. This covers `rm -r`,
  `git reset --hard`, and `git clean`.
- **No telemetry. No external APIs or services** unless explicitly requested.
- **Secrets:** never hardcode them.
- **System safety:** never disable antivirus, firewalls, updates, or the VPN, and never
  change network configuration.
