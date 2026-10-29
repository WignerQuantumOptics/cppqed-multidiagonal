# CLAUDE.md — cppqed-multidiagonal

Header-only C++23 library for operators on tensor-product Hilbert spaces that are
**multidiagonal**: every nonzero matrix element moves a fixed number of quanta per axis.
Extracted from C++QED v3 (github.com/WignerQuantumOptics/cppqed), which consumes it through
FetchContent pinned to a commit SHA (or an installed version ≥ 0.3). A change here reaches
C++QED only when its pin is bumped; its jc_qjmc regression test must still pass then.
It is being prepared for standalone publication, so API stability, honest documentation
and test coverage matter more than feature growth.

Released as v0.4.0. No DOI yet. License: BSL-1.0.

## Layout

```
include/multidiagonal/
  Traits.h            FWD, Hana/JSON setup, passByValue_v, ReferenceMF/ConstReferenceMF
  MultiArray.h        rank-R strided array: MultiArrayView (non-owning), MultiArray (owning, move-only)
  MultiArrayComplex.h dcomp specializations, directProduct, twoTimesRealPartOf…, Eigen interop
  MultiDiagonal.h     the operator algebra, including identity() and composition
tests/                Catch2 v3 suites: testMultiArray, testMultiArrayComplex, testMultiDiagonal
  consumer/           downstream CMake project for the CI consumer job
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
  must compile warning-free under `-Wall -Wextra -Wpedantic` on both compilers.
- CI also runs an ASan+UBSan job (Clang Debug, tests and benchmark smoke) and a consumer
  job: `tests/consumer` is a separate CMake project that uses the library via
  `find_package` after install and via FetchContent at the pushed SHA. Runners are pinned
  to `ubuntu-24.04` and actions to commit SHAs.
- C++23 is required (`0uz` literals, `std::ranges::fold_left`), which means roughly
  GCC ≥ 13 or Clang ≥ 17.
- The configure step may download dependencies through FetchContent (Catch2, …).
  See the working rules at the end of this file.

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
    `composeJC`, `constructJC`, and `picture1` (five interaction-picture variants).
  - Every configuration cross-validates against Eigen **before** timing and exits 1 on a
    mismatch.
- `kernels.cc` holds the application-loop experiments (cases A–D: rank 1, Jaynes–Cummings,
  an operator on one axis of a rank-4 state, the same applied slice by slice). It is built
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
  diagnosis change, update it rather than this file. In brief (2026-10-29):
  - Default `std::complex` multiplication (Annex G NaN checks, `__muldc3`) halves the
    library's speed; Eigen's sparse product is unaffected (#23).
  - The multi-axis loop recomputes addresses per element: 3–5× at rank 2, about 30× slower
    than Eigen at rank 4. Hoisting addresses with the innermost loop over axis 0 fixes it.
  - Operators as large as the state are bandwidth bound: parity at best.
  - In C++QED's slice-by-slice setting, the honest competitor is a CSR matrix of the term's
    own space on the same slices. With the fixes MultiDiagonal is 1.1–1.8× faster. Strided
    slices with power-of-two strides need a row-oriented loop (L1 set conflicts). C++QED's
    `SliceIterator` order costs 1.3–4.7× on top (cppqed#90).
  - Composition wins 2–9×, but it happens at setup time, not in the time-stepping loop.
  - The interaction picture is not benchmarked systematically. C++QED's hot loop updates
    `(coefficients, frequencies)` and applies; it never transforms the state around a static
    product. Don't present that "sandwich" as the reference.

## Open items (roughly in priority order)

1. Application performance (#1, #23; see `benchmarks/performance.md`, next steps): hoisted
   loop, complex multiply-add in real arithmetic, blocking for long operands, loop shape
   chosen from the strides.
2. Decide on migrating to `std::mdspan`/`std::extents` (Kokkos reference implementation,
   Apache-2.0, FetchContent-pinnable) versus writing the paper on the current
   architecture. **The decision is open; do not start the migration unasked.**
3. README usage snippet. This waits on an API decision: whether test helpers like
   `singleDiag`/`fromStorage` are the blessed construction idiom, or a small factory
   namespace graduates into the library.
4. Deferred: install/export rules review; Boost.Serialization as an optional dependency.

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
