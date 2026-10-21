\mainpage cppqed-multidiagonal

A C++20 library implementing sparse multidiagonal operator algebra for quantum systems of arbitrary arity, extracted from the [C++QED](https://github.com/WignerQuantumOptics/cppqed) framework.

## What is a multidiagonal operator?

A multidiagonal operator over a Hilbert space of dimension \f$N\f$ is a sparse matrix storing an arbitrary set of diagonals at arbitrary offsets. In the context of quantum optics and cavity QED, Hamiltonians and jump operators naturally take this form. For example, the harmonic oscillator ladder operators \f$a\f$, \f$a^\dagger\f$ are single off-diagonals; number operators \f$a^\dagger a\f$ are main diagonals; nonlinear terms like \f$(a^\dagger)^2 a^2\f$ are further off-diagonals.

For composite systems of arity \f$M\f$, `MultiDiagonal<RANK>` stores tensor-product-structured operators of the form
\f[
  H/i = \bigotimes_{m=0}^{M-1}
    \sum_{i_m \in \mathbb{K}_m} \sum_{n_m=0}^{N_m-1-i_m}
    \alpha^{i_m}_{m,n} \left|n_m+i_m\right\rangle\!\left\langle n_m\right|
\f]
with \f$\mathbb{K}_m\f$ an **arbitrary** set of offsets per subsystem axis.

## Why not Tridiagonal?

The predecessor class `Tridiagonal<RANK>` in C++QED v2 stored exactly three diagonals per axis — at positions \f$-K\f$, \f$0\f$, \f$+K\f$ for a single shared offset \f$K\f$ — and was **not closed under composition**: the product of two tridiagonal operators is in general pentadiagonal, and with different offsets is not tridiagonal at all. `MultiDiagonal` lifts all these restrictions — any number of diagonals at independent arbitrary offsets, and the operator algebra is closed under composition (`operator|`), direct product (`operator*`), addition, and Hermitian conjugation.

## Features

- **Arbitrary rank**: `MultiDiagonal<RANK>` for any compile-time arity \f$M\f$
- **Closed algebra**: composition, direct product, addition, scalar multiplication, Hermitian conjugation
- **Efficient application**: applies as \f$H/i\f$ on a state vector via strided multi-index iteration, with compile-time loop unrolling via Boost.Hana
- **Move-only semantics**: no accidental copies of large diagonal arrays
- **JSON serialization**: via Boost.JSON `tag_invoke`, useful for testing and debugging
- **Time dependence**: interaction-picture time dependence (\f$\alpha_{m,n} e^{\delta_{m,n} t}\f$) is handled by `InteractionPictureDiagonal` in C++QED proper, keeping this library unconditionally time-independent

## Requirements

- C++20 compiler (GCC ≥ 12, Clang ≥ 15)
- [Boost](https://www.boost.org/) ≥ 1.80 (Hana, JSON, Serialization)
- [Eigen3](https://eigen.tuxfamily.org/) ≥ 3.4
- CMake ≥ 3.24

## Repository structure

- \ref cppqedutils — foundational types: `Extents`, `MultiArray`, `MultiArrayView`, `dcomp`, `directProduct`
- \ref quantumoperator — operator algebra: `MultiDiagonal`, `multidiagonal::identity`, `operator|`
