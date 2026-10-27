# cppqed-multidiagonal

[![CI](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/actions/workflows/ci.yml/badge.svg)](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/actions/workflows/ci.yml)
[![Documentation](https://img.shields.io/badge/docs-github.io-blue)](https://wignerquantumoptics.github.io/cppqed-multidiagonal/)
[![License](https://img.shields.io/badge/license-BSL--1.0-green)](LICENSE.txt)

A header-only C++ library implementing sparse multidiagonal operator algebra for quantum systems of arbitrary arity, extracted from the [C++QED](https://github.com/WignerQuantumOptics/cppqed) framework.

## What is a multidiagonal operator?

A multidiagonal operator over a Hilbert space of dimension $N$ is a sparse matrix storing an arbitrary set of diagonals at arbitrary offsets. In the context of quantum optics and cavity QED, Hamiltonians and jump operators naturally take this form. For example, the harmonic oscillator ladder operators $a$, $a^\dagger$ are single off-diagonals; number operators $a^\dagger a$ are main diagonals, and so is the Kerr term $(a^\dagger)^2 a^2 = a^\dagger a\,(a^\dagger a - 1)$; two-photon terms like $a^2$ sit two diagonals off the main one.

For a composite system of $R$ subsystems, `MultiDiagonal<RANK>` stores an operator of the form

$$A = \sum_{\mathbf{o} \in \mathbb{K}} \sum_{\mathbf{n}} \alpha^{\mathbf{o}}_{\mathbf{n}} \left|\mathbf{n}\right\rangle\left\langle \mathbf{n}+\mathbf{o}\right|$$

with $\mathbb{K} \subset \mathbb{Z}^R$ an **arbitrary** set of *signed* offset vectors, one entry per subsystem axis, and $\mathbf{n}$ running over the multi-indices for which both $\mathbf{n}$ and $\mathbf{n}+\mathbf{o}$ lie in the truncated Hilbert space. A positive entry thus means an upper diagonal on that axis. The coefficients need not factorize over the subsystems; direct products, built with `operator*`, are the special case where they do.

## The signed-offset representation

Each diagonal is keyed by a signed offset per axis — positive = upper, negative = lower, zero = main diagonal. The offset counts the quanta a diagonal moves along each axis, and the algebra reduces to arithmetic on these counts:

- **composition**: $\text{offset}_{A|B} = \text{offset}_A + \text{offset}_B$ per axis, for arbitrary rank
- **Hermitian conjugation**: negation of all offsets (plus conjugation of the stored values)
- **direct product**: concatenation of the offset arrays, reflecting the tensor product structure exactly

The diagonal at signed offset $o$ (per axis, dimension $D$) has extent $D-|o|$, and entry $k$ holds the matrix element $A_{k+o^-,\,k+o^+}$ with $o^\pm=\max(\pm o,0)$. This min(row,col)-anchored storage is symmetric under transposition, which is what makes conjugation a pure key negation. Application to a state vector costs $O(N \cdot d)$ for $d$ diagonals; composition runs over the $d_A \cdot d_B$ diagonal pairs, each in one pass over the window where the two diagonals overlap.

## Why not `Tridiagonal`?

The predecessor class `Tridiagonal<RANK>` in C++QED v2 stored exactly three diagonals per axis — at positions $-K$, $0$, $+K$ for a single shared offset $K$ — and was **not closed under composition**: the product of two tridiagonal operators is in general pentadiagonal, and with different offsets is not tridiagonal at all. `MultiDiagonal` lifts all these restrictions — any number of diagonals at independent arbitrary offsets, and the operator algebra is closed under composition (`operator|`), direct product (`operator*`), addition, and Hermitian conjugation.

The truncated algebra is represented *exactly*, including its physical deviations from the untruncated one: e.g. $[a,a^\dagger]$ at cutoff $D$ is $\mathrm{diag}(1,\ldots,1,1-D)$, not $\mathbb{1}$ — as it must be for any finite-dimensional representation.

## Features

- **Arbitrary rank**: `MultiDiagonal<RANK>` for any compile-time arity $M$, including composition at arbitrary rank
- **Closed algebra**: composition, direct product, addition, scalar multiplication, Hermitian conjugation — with exact signed-offset arithmetic
- **Application**: accumulates $A\psi$ into a state vector via strided multi-index iteration, with compile-time loop unrolling via Boost.Hana. It applies no factor $1/i$: C++QED's convention is to store $A = H/i$.
- **Move-only semantics**: no accidental copies of large diagonal arrays; deep copies are explicit (`copy`)
- **Deterministic results**: diagonals are stored in an ordered map keyed by signed offsets, so accumulation order in composition is well-defined and reproducible across platforms
- **JSON serialization**: via Boost.JSON `tag_invoke`, useful for testing and debugging
- **No time dependence**: operators carry no frequencies. Interaction-picture time dependence ($\alpha_{m,n} e^{\delta_{m,n} t}$) belongs to the consumer; in C++QED the trajectory owns the interaction picture
- **Property-tested**: the algebraic identities — associativity, $(A|B)^\dagger = B^\dagger|A^\dagger$, distributivity, the mixed-product law $(A{\otimes}B)|(C{\otimes}D) = (A|C){\otimes}(B|D)$ — are verified against dense-matrix oracles in CI

## Performance

Application is on par with Eigen's sparse matrices for operators acting on one axis, but currently 2–2.7× **slower** for operators acting on several axes, such as the Jaynes–Cummings coupling ([#1](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/1)). The benchmarks in `benchmarks/` are being reworked ([#5](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/issues/5)), so other figures are not quoted here.

## Requirements

- C++23 compiler (GCC ≥ 13, Clang ≥ 17)
- [Boost](https://www.boost.org/) ≥ 1.80 (Hana, JSON, Serialization) — header-only use plus Boost.JSON/Serialization
- [Eigen3](https://eigen.tuxfamily.org/) ≥ 3.4
- CMake ≥ 3.24 (for the build/test/install machinery; as a header-only library it can also be consumed by simply adding `include/` to the include path and writing `#include <multidiagonal/MultiDiagonal.h>`)

## Building and testing

The library itself is header-only — nothing to build. Configuring and building compiles the test suite:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

(Debug enables the library's bounds and dimension checks, which the test suite exercises; CI runs the Debug×Release matrix on GCC and Clang.)

## Integration into a CMake project

### Via FetchContent (as C++QED does)

```cmake
include(FetchContent)
FetchContent_Declare(cppqed-multidiagonal
  GIT_REPOSITORY    https://github.com/WignerQuantumOptics/cppqed-multidiagonal.git
  GIT_TAG           <full commit SHA of a release>
  FIND_PACKAGE_ARGS 0.4)   # an installed version is used when found
FetchContent_MakeAvailable(cppqed-multidiagonal)
target_link_libraries(MyTarget PUBLIC cppqed::multidiagonal)
```

A copy of the source tree in your project works too, via `add_subdirectory`.

### Via `find_package` (after installation)

```bash
cmake --install build
```

```cmake
find_package(cppqed-multidiagonal REQUIRED)
target_link_libraries(MyTarget PUBLIC cppqed::multidiagonal)
```

Either way, the headers are included with their directory prefix, e.g. `#include <multidiagonal/MultiDiagonal.h>`.

## Repository structure
```
include/multidiagonal/
  Traits.h — FWD, Boost.Hana/JSON setup, passByValue_v, ReferenceMF
  MultiArray.h — generic rank-N array with strided view
  MultiArrayComplex.h — dcomp specializations, directProduct, Eigen interop
  MultiDiagonal.h — multidiagonal operator algebra (incl. identity and composition)
tests/
  testMultiArray.cc, testMultiArrayComplex.cc, testMultiDiagonal.cc
  consumer/ — downstream project used by CI (find_package and FetchContent)
benchmarks/
  benchmarkMultiDiagonal.cc, plot.py — comparison with Eigen sparse and dense
cmake/
  cppqed-multidiagonalConfig.cmake.in
Doxyfile, doxygen-math.sed — API docs; this README is their main page
```

## License

Distributed under the [Boost Software License, Version 1.0](LICENSE.txt).

## Authors

András Vukics — extracted from C++QED, originally developed 2006–2026.