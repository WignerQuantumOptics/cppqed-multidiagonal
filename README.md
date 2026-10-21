# cppqed-multidiagonal

[![CI](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/actions/workflows/ci.yml/badge.svg)](https://github.com/WignerQuantumOptics/cppqed-multidiagonal/actions/workflows/ci.yml)
[![Documentation](https://img.shields.io/badge/docs-github.io-blue)](https://wignerquantumoptics.github.io/cppqed-multidiagonal/)
[![License](https://img.shields.io/badge/license-BSL--1.0-green)](LICENSE.txt)

A header-only C++ library implementing sparse multidiagonal operator algebra for quantum systems of arbitrary arity, extracted from the [C++QED](https://github.com/WignerQuantumOptics/cppqed) framework.

## What is a multidiagonal operator?

A multidiagonal operator over a Hilbert space of dimension $N$ is a sparse matrix storing an arbitrary set of diagonals at arbitrary offsets. In the context of quantum optics and cavity QED, Hamiltonians and jump operators naturally take this form. For example, the harmonic oscillator ladder operators $a$, $a^\dagger$ are single off-diagonals; number operators $a^\dagger a$ are main diagonals; nonlinear terms like $(a^\dagger)^2 a^2$ are further off-diagonals.

For composite systems of arity $M$, `MultiDiagonal<RANK>` stores tensor-product-structured operators of the form

$$H/i = \bigotimes_{m=0}^{M-1} \sum_{i_m \in \mathbb{K}_m} \sum_{n_m=0}^{N_m-1-i_m} \alpha^{i_m}_{m,n} \left|n_m+i_m\right\rangle\left\langle n_m\right|$$

with $\mathbb{K}_m$ an **arbitrary** set of *signed* offsets per subsystem axis.

## The signed-offset representation

Each diagonal is keyed by a signed offset per axis — positive = upper, negative = lower, zero = main diagonal. The offset counts the quanta a diagonal moves along each axis, and the algebra reduces to arithmetic on these counts:

- **composition**: $\text{offset}_{A|B} = \text{offset}_A + \text{offset}_B$ per axis, for arbitrary rank
- **Hermitian conjugation**: negation of all offsets (plus conjugation of the stored values)
- **direct product**: concatenation of the offset arrays, reflecting the tensor product structure exactly

The diagonal at signed offset $o$ (per axis, dimension $D$) has extent $D-|o|$, and entry $k$ holds the matrix element $A_{k+o^-,\,k+o^+}$ with $o^\pm=\max(\pm o,0)$. This min(row,col)-anchored storage is symmetric under transposition, which is what makes conjugation a pure key negation. Application as $H/i$ on a state vector costs $O(N \cdot d)$ for $d$ diagonals; composition costs $O(d_A \cdot d_B)$ diagonal pairs — far cheaper than a general sparse–sparse multiply.

## Why not `Tridiagonal`?

The predecessor class `Tridiagonal<RANK>` in C++QED v2 stored exactly three diagonals per axis — at positions $-K$, $0$, $+K$ for a single shared offset $K$ — and was **not closed under composition**: the product of two tridiagonal operators is in general pentadiagonal, and with different offsets is not tridiagonal at all. `MultiDiagonal` lifts all these restrictions — any number of diagonals at independent arbitrary offsets, and the operator algebra is closed under composition (`operator|`), direct product (`operator*`), addition, and Hermitian conjugation.

The truncated algebra is represented *exactly*, including its physical deviations from the untruncated one: e.g. $[a,a^\dagger]$ at cutoff $D$ is $\mathrm{diag}(1,\ldots,1,1-D)$, not $\mathbb{1}$ — as it must be for any finite-dimensional representation.

## Features

- **Arbitrary rank**: `MultiDiagonal<RANK>` for any compile-time arity $M$, including composition at arbitrary rank
- **Closed algebra**: composition, direct product, addition, scalar multiplication, Hermitian conjugation — with exact signed-offset arithmetic
- **Efficient application**: applies as $H/i$ on a state vector via strided multi-index iteration, with compile-time loop unrolling via Boost.Hana
- **Move-only semantics**: no accidental copies of large diagonal arrays; deep copies are explicit (`copy`)
- **Deterministic results**: diagonals are stored in an ordered map keyed by signed offsets, so accumulation order in composition is well-defined and reproducible across platforms
- **JSON serialization**: via Boost.JSON `tag_invoke`, useful for testing and debugging
- **Time dependence**: interaction-picture time dependence ($\alpha_{m,n} e^{\delta_{m,n} t}$) is handled by a companion frequency object in C++QED proper, keeping this library unconditionally time-independent
- **Property-tested**: the algebraic identities — associativity, $(A|B)^\dagger = B^\dagger|A^\dagger$, distributivity, the mixed-product law $(A{\otimes}B)|(C{\otimes}D) = (A|C){\otimes}(B|D)$ — are verified against dense-matrix oracles in CI

## Requirements

- C++23 compiler (GCC ≥ 13, Clang ≥ 17)
- [Boost](https://www.boost.org/) ≥ 1.80 (Hana, JSON, Serialization) — header-only use plus Boost.JSON/Serialization
- [Eigen3](https://eigen.tuxfamily.org/) ≥ 3.4
- CMake ≥ 3.24 (for the build/test/install machinery; as a header-only library it can also be consumed by simply adding `include/` to the include path)

## Building and testing

The library itself is header-only — nothing to build. Configuring and building compiles the test suite:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

(Debug enables the library's bounds and dimension checks, which the test suite exercises; CI runs the Debug×Release matrix on GCC and Clang.)

## Integration into a CMake project

### As a submodule (recommended for C++QED development)

```bash
git submodule add https://github.com/WignerQuantumOptics/cppqed-multidiagonal extern/cppqed-multidiagonal
```

In your `CMakeLists.txt`:

```cmake
add_subdirectory(extern/cppqed-multidiagonal)
target_link_libraries(MyTarget PUBLIC cppqed::multidiagonal)
```

### Via `find_package` (after installation)

```bash
cmake --install build
```

```cmake
find_package(cppqed-multidiagonal REQUIRED)
target_link_libraries(MyTarget PUBLIC cppqed::multidiagonal)
```

## Repository structure
```
include/
  Traits.h — FWD, Boost.Hana/JSON setup, passByValue_v, ReferenceMF
  MultiArray.h — generic rank-N array with strided view
  MultiArrayComplex.h — dcomp specializations, directProduct, Eigen interop
  MultiDiagonal.h — multidiagonal operator algebra (incl. identity and composition)
tests/
  testMultiArray.cc, testMultiArrayComplex.cc, testMultiDiagonal.cc
cmake/
  cppqed-multidiagonalConfig.cmake.in
```

## License

Distributed under the [Boost Software License, Version 1.0](LICENSE.txt).

## Authors

András Vukics — extracted from C++QED, originally developed 2006–2026.