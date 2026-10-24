// Copyright András Vukics 2006–2026. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt)
#pragma once

#include "MultiArrayComplex.h"

#include <algorithm>
#include <map>


/// @brief Comprises modules representing operators of special structure (multidiagonal, sparse) over Hilbert spaces of arbitrary arity.
namespace quantumoperator {

using namespace cppqedutils;


/// @brief A sparse matrix storing an arbitrary set of diagonals, closed under composition, direct product, and Hermitian conjugation.
///
/// `MultiDiagonal<RANK>` supersedes `Tridiagonal<RANK>` from v2, which stored exactly three diagonals
/// per axis — at positions $-K$, $0$, $+K$ for a single shared offset $K$ — and was
/// **not closed under composition**: the product of two tridiagonal operators is in general
/// pentadiagonal, and with different offsets not tridiagonal at all. `MultiDiagonal` lifts all these restrictions:
/// - Any number of diagonals at arbitrary offsets per axis.
/// - `operator|` (composition) produces new diagonals at all pairwise offset sums — the algebra is closed.
/// - `operator*` (direct product) concatenates offset arrays, reflecting tensor product structure exactly.
///
/// ### Representation
/// Each diagonal is keyed by a **signed offset** per axis: positive = upper, negative = lower,
/// zero = main. The offset map is a homomorphism from \f$(\mathbb{Z}^R,+)\f$ into the operator algebra:
/// - composition: \f$\text{offset}_{A|B} = \text{offset}_A + \text{offset}_B\f$ (per axis)
/// - Hermitian conjugation: negation of all offsets
/// - direct product: concatenation of offset arrays
///
/// **Storage convention.** The diagonal at signed offset \f$o\f$ (per axis, dimension \f$D\f$) has
/// extent \f$D-|o|\f$, and entry \f$k\in[0,D-|o|)\f$ holds the matrix element
/// \f[ d[k] = A_{k+o^-,\;k+o^+}, \qquad o^\pm = \max(\pm o,0). \f]
/// This min(row,col)-anchored indexing is symmetric under transposition, so Hermitian conjugation
/// negates the key and conjugates the values **without reindexing**.
/// The invariant \f$\text{extent}_i + |o_i| = D_i\f$ holds for every diagonal and is enforced by
/// `calculateAndCheckDimensions`; entries that are structurally zero (created e.g. by composition)
/// are stored explicitly as zeros to preserve it.
///
/// ### Time dependence
/// `MultiDiagonal` is time-independent. Interaction-picture time dependence — diagonal elements
/// of the form \f$\alpha_{m,n} e^{\delta_{m,n} t}\f$ with complex \f$\delta\f$ — is handled by
/// a derived type in CPPQEDcore that adds a parallel frequency `MultiDiagonal` and `propagate(t)`
/// bookkeeping. This separation keeps `MultiDiagonal` a clean, unconditionally time-independent algebraic type.
///
/// ### Operator conventions
/// - `operator()` applies \f$H/i\f$ (not \f$H\f$) to the state vector, consistent with the framework convention.
/// - \f$H\f$ need not be Hermitian: in QJMC and Master equation contexts it is the full effective
///   non-Hermitian Hamiltonian \f$H_\text{eff} = H_\text{phys} - \frac{i}{2}\sum_k J_k^\dagger J_k\f$.
template <size_t RANK>
struct MultiDiagonal
{
  using Dimensions = Extents<RANK>;
  /// @brief Signed offsets, one per axis: positive = upper, negative = lower, zero = main diagonal.
  /// @note `std::array<ptrdiff_t,RANK>` has lexicographic `operator<` out of the box — no custom comparator needed.
  using Offsets = std::array<ptrdiff_t,RANK>;
  using Diagonal = MultiArray<dcomp,RANK>;

  /// @brief Single-level map: signed offsets → diagonal data.
  using Diagonals = std::map<Offsets,Diagonal>;

  MultiDiagonal(const MultiDiagonal&) = delete; MultiDiagonal& operator=(const MultiDiagonal&) = delete;
  MultiDiagonal(MultiDiagonal&&) = default; MultiDiagonal& operator=(MultiDiagonal&&) = default;

  explicit MultiDiagonal(auto&&... args) : diagonals{FWD(args)...} {}

  /// @brief Named deep copy.
  friend MultiDiagonal copy(const MultiDiagonal& md)
  {
    MultiDiagonal res;
    for (const auto& [offsets,diag] : md.diagonals) res.diagonals.emplace(offsets,copy(diag));
    return res;
  }

  Diagonals diagonals;

  /// @brief Applies the operator as \f$H/i\f$: accumulates `dpsidt(idxLHS) += diag(idxDiag) * psi(idxRHS)`
  /// for each diagonal. Per axis, rows run in \f$[o^-,D-o^+)\f$ and columns in \f$[o^+,D-o^-)\f$.
  void operator () (double, MultiArrayConstView<dcomp,RANK> psi, MultiArrayView<dcomp,RANK> dpsidt) const
  {
    if (diagonals.empty()) return;

#ifndef NDEBUG
    if (psi.extents != calculateAndCheckDimensions(*this)) throw std::runtime_error("Mismatch between StateVector and MultiDiagonal dimensions");
#endif // NDEBUG

    for (const auto& [offsets,diag] : diagonals) {

      Dimensions ubound{psi.extents}, lboundLHS{}, lboundRHS{};
      for (auto&& [o,u,lL,lR] : std::views::zip(offsets,ubound,lboundLHS,lboundRHS)) {
        lL = o<0 ? size_t(-o) : 0uz; lR = o>0 ? size_t(o) : 0uz; u -= lR;
      }

      const auto increment=[&] (auto n, const auto& inc, Dimensions& idxLHS, Dimensions& idxRHS, Dimensions& idxDiag)
      {
        using namespace hana::literals;
        if constexpr (n!=0_c) {
          if (idxLHS[n]==ubound[n]-1) {idxLHS[n]=lboundLHS[n]; idxRHS[n]=lboundRHS[n]; idxDiag[n]=0; inc(n-1_c,inc,idxLHS,idxRHS,idxDiag);}
          else {idxLHS[n]++; idxRHS[n]++; idxDiag[n]++;}
        }
        else {idxLHS[0]++; idxRHS[0]++; idxDiag[0]++;}
      };

      for ( Dimensions idxLHS{lboundLHS}, idxRHS{lboundRHS}, idxDiag{}; idxLHS[0]!=ubound[0] ; increment( hana::llong_c<RANK-1>, increment, idxLHS, idxRHS, idxDiag ) )
        dpsidt(idxLHS) += diag(idxDiag)*psi(idxRHS);
    }
  }


  /// @brief Tensor (direct) product: concatenates the signed offset arrays, reflecting the tensor
  /// product structure exactly. Axes of @p md1 come first.
  template <size_t RANK2>
  friend auto operator* (const MultiDiagonal<RANK>& md1, const MultiDiagonal<RANK2>& md2)
  {
    MultiDiagonal<RANK+RANK2> res;
    for (const auto& [offsets1,diag1] : md1.diagonals) for (const auto& [offsets2,diag2] : md2.diagonals)
      res.diagonals.emplace(concatenate(offsets1,offsets2),directProduct(diag1,diag2));
    return res;
  }


  /// @name Hermitian conjugation
  //@{

  /// @brief In-place Hermitian conjugation: negates all offsets and conjugates all diagonal elements.
  /// No reindexing of the stored data is needed — see the storage convention in the class documentation.
  MultiDiagonal& hermitianConjugate()
  {
    Diagonals newDiagonals;
    for (auto& [offsets,diag] : diagonals) {
      Offsets newOffsets;
      for (size_t i=0; i<RANK; ++i) newOffsets[i]=-offsets[i];
      conj(diag);
      newDiagonals.emplace(newOffsets,std::move(diag));
    }
    diagonals.swap(newDiagonals);
    return *this;
  }

  MultiDiagonal& dagger() {return hermitianConjugate();}

  friend MultiDiagonal hermitianConjugateOf(const MultiDiagonal& md) {MultiDiagonal res(copy(md)); res.hermitianConjugate(); return res;}

  friend MultiDiagonal twoTimesRealPartOf(const MultiDiagonal& md) {return md+hermitianConjugateOf(md);}
  friend MultiDiagonal twoTimesImagPartOf(const MultiDiagonal& md) {return md-hermitianConjugateOf(md);}
  //@}


  /// @name Algebra
  /// @note Boost.Operator cannot be used since `MultiDiagonal` is not copyable.
  //@{
  MultiDiagonal operator-() const {MultiDiagonal res{copy(*this)}; res*=-1.; return res;}

  MultiDiagonal operator+() const {return copy(*this);}

  /// @brief In-place addition. New diagonals are inserted; existing ones accumulate element-wise.
  MultiDiagonal& operator+=(const MultiDiagonal& md)
  {
#ifndef NDEBUG
    if (auto dim{calculateAndCheckDimensions(*this)}, mdDim{calculateAndCheckDimensions(md)};
      dim!=Dimensions{} && mdDim!=Dimensions{} && dim!=mdDim)
        throw std::runtime_error("Dimension mismatch in addition of MultiDiagonals");
#endif // NDEBUG
    for (const auto& [offsets,diag] : md.diagonals)
      if (auto it=diagonals.find(offsets); it!=diagonals.end())
        for (auto&& [to,from] : std::views::zip(it->second.dataStorage(),diag.dataStorage())) to+=from;
      else diagonals.emplace(offsets,copy(diag));
    return *this;
  }

  MultiDiagonal& operator-=(const MultiDiagonal& md) {return operator+=(-md);}

  MultiDiagonal& operator*=(scalar auto d)
  {
    for (auto& [offsets,diag] : diagonals) for (dcomp& v : diag.dataStorage()) v*=d;
    return *this;
  }

  MultiDiagonal& operator/=(scalar auto d) {(*this)*=1./d; return *this;}

  friend auto operator+(const MultiDiagonal& md1, const MultiDiagonal& md2) {MultiDiagonal res{copy(md1)}; res+=md2; return res;}
  friend auto operator-(const MultiDiagonal& md1, const MultiDiagonal& md2) {MultiDiagonal res{copy(md1)}; res-=md2; return res;}

  friend auto operator*(scalar auto v, const MultiDiagonal& md) {MultiDiagonal res{copy(md)}; res*=v; return res;}
  friend auto operator/(const MultiDiagonal& md, scalar auto v) {MultiDiagonal res{copy(md)}; res/=v; return res;}
  //@}


  /// @brief Derives and validates the Hilbert space dimensions from the invariant
  /// \f$\text{extent}_i + |o_i| = D_i\f$. Returns `Dimensions{}` for an empty operator.
  /// Throws if any diagonal is inconsistent.
  friend Dimensions calculateAndCheckDimensions(const MultiDiagonal& md)
  {
    if (md.diagonals.empty()) return {};

    auto impliedDimensions = [] (const auto& kv) -> Dimensions {
      auto res{kv.second.extents};
      for (auto&& [v,o] : std::views::zip(res,kv.first)) v += size_t(o<0 ? -o : o);
      return res;
    };

    const auto res{impliedDimensions(*md.diagonals.cbegin())};

    for (const auto& kv : md.diagonals)
      if (impliedDimensions(kv)!=res) throw std::runtime_error("Dimensions mismatch in MultiDiagonal");

    return res;
  }

  /// @brief Boost.JSON serialization: emits an array of `{"offsets": [...], "diagonal": {...}}` objects.
  friend void tag_invoke( const json::value_from_tag&, json::value& jv, const MultiDiagonal& md )
  {
    json::array& arr = jv.emplace_array();
    for (const auto& [offsets,diag] : md.diagonals)
      arr.push_back( json::object{ {"offsets",json::value_from(offsets)}, {"diagonal",json::value_from(diag)} } );
  }

};


/// @brief Composition (matrix product) of two `MultiDiagonal` operators of arbitrary rank:
/// \f$(A|B)_{rc} = \sum_m A_{rm} B_{mc}\f$.
///
/// For each pair of diagonals — \f$A\f$ at signed offsets \f$p\f$, \f$B\f$ at \f$q\f$ (per axis,
/// dimension \f$D\f$) — the contribution lands on the result diagonal at
/// \f[ s = p + q \qquad\text{(the offset homomorphism).} \f]
/// With the storage convention \f$d[k]=M_{k+o^-,k+o^+}\f$, entry \f$k\f$ of the result receives
/// \f[ r[k] \mathrel{+}= d_A[k+i_A]\, d_B[k+i_B], \qquad i_A = s^--p^-, \quad i_B = s^-+p-q^-, \f]
/// valid on the window
/// \f[ k \in \bigl[\, \max(0,-i_A,-i_B),\ \min(D-|s|,\,D-|p|-i_A,\,D-|q|-i_B) \,\bigr). \f]
/// Entries of the result diagonal outside the window are structural zeros, stored explicitly so that
/// the invariant \f$\text{extent}+|s|=D\f$ holds. Pairs with \f$|s_i|\ge D_i\f$ or an empty window
/// on any axis are structurally zero and skipped. **Contributions from different \f$(p,q)\f$ pairs
/// to the same \f$s\f$ accumulate** — this is essential for correctness, not an optimization
/// (e.g. \f$\sigma_x|\sigma_x\f$ assembles its main diagonal from two complementary pairs).
///
/// Complexity: \f$O(d_A\, d_B)\f$ diagonal pairs, each linear in the window volume.
///
/// @note `operator*` has higher precedence than `operator|` in C++; use parentheses in mixed expressions.
template <size_t RANK>
MultiDiagonal<RANK> operator|(const MultiDiagonal<RANK>& a, const MultiDiagonal<RANK>& b)
{
  using MD = MultiDiagonal<RANK>;

  if (a.diagonals.empty() || b.diagonals.empty()) return MD{}; // composition with the zero operator

  const auto dim = calculateAndCheckDimensions(a);
  if (dim != calculateAndCheckDimensions(b)) throw std::runtime_error("Mismatch in MultiDiagonal composition dimensions");

  MD res;

  for (const auto& [p,diagA] : a.diagonals) for (const auto& [q,diagB] : b.diagonals) {

    typename MD::Offsets s;                     // s = p + q
    Extents<RANK> resExtents, start, box;       // result-diagonal extents; valid window [start, start+box)
    std::array<ptrdiff_t,RANK> iA, iB;          // index shifts into the operand diagonals

    bool nonzero=true;

    for (size_t i=0; i<RANK; ++i) {
      const ptrdiff_t D=ptrdiff_t(dim[i]), pi=p[i], qi=q[i], si=pi+qi,
        sm = si<0 ? -si : 0, pm = pi<0 ? -pi : 0, qm = qi<0 ? -qi : 0,      // negative parts o⁻
        sa = si<0 ? -si : si, pa = pi<0 ? -pi : pi, qa = qi<0 ? -qi : qi;   // magnitudes |o|

      if (sa >= D) {nonzero=false; break;}

      const ptrdiff_t ia = sm - pm, ib = sm + pi - qm,
        st = std::max({ptrdiff_t{0}, -ia, -ib}),
        en = std::min({D - sa, D - pa - ia, D - qa - ib});

      if (en <= st) {nonzero=false; break;}

      s[i]=si; resExtents[i]=size_t(D-sa); start[i]=size_t(st); box[i]=size_t(en-st); iA[i]=ia; iB[i]=ib;
    }
    if (!nonzero) continue;

    // find-or-create the result diagonal (zero-initialized); accumulate the windowed products
    auto& resDiag = res.diagonals.try_emplace(s, resExtents, multiarray::zeroInit<dcomp>).first->second;

    for (Extents<RANK> t{}; t[0]!=box[0]; incrementMultiIndex(t,box)) {
      Extents<RANK> kRes, kA, kB;
      for (size_t i=0; i<RANK; ++i) {
        kRes[i]=start[i]+t[i];
        kA[i]=size_t(ptrdiff_t(kRes[i])+iA[i]);
        kB[i]=size_t(ptrdiff_t(kRes[i])+iB[i]);
      }
      resDiag(kRes) += diagA(kA)*diagB(kB);
    }
  }

  return res;
}


namespace multidiagonal {

/// @brief Constructs the rank-1 identity operator of dimension @p dim.
MultiDiagonal<1> identity(size_t dim);

} // multidiagonal


} // quantumoperator


/// @brief Rank-1 identity operator: a single main diagonal (signed offset 0) filled with 1.
/** (This could be compiled separately, but let’s not drop header-onliness for a single function) */
inline quantumoperator::MultiDiagonal<1> quantumoperator::multidiagonal::identity(size_t dim)
{
  MultiDiagonal<1> res;
  res.diagonals.emplace(
    MultiDiagonal<1>::Offsets{0},
    MultiDiagonal<1>::Diagonal{ {dim}, [] (size_t e) {return multiarray::StorageType<dcomp>(e,1.);} } );
  return res;
}