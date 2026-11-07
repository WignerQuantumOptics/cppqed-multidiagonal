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
/// zero = main. The offset counts the quanta a diagonal moves along each axis, and the algebra reduces
/// to arithmetic on these counts:
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
/// are stored explicitly as zeros to preserve it. A diagonal with \f$|o_i|=D_i\f$ on some axis has no
/// entries (e.g. \f$a\f$ for a cutoff of 1); it is a structural zero, and application skips it.
///
/// ### Time dependence
/// `MultiDiagonal` is time-independent and carries no frequencies. Interaction-picture time dependence —
/// diagonal elements of the form \f$\alpha_{m,n} e^{\delta_{m,n} t}\f$ with complex \f$\delta\f$ — belongs
/// to the consumer: in C++QED the trajectory owns the interaction picture, and where phase tabulation is
/// needed, the physics layer keeps a separate `(coefficients, frequencies)` pair of `MultiDiagonal`s.
/// This is a deliberate design boundary.
///
/// ### Operator conventions
/// - `operator()` accumulates \f$A\psi\f$ into `dpsidt` for the stored operator \f$A\f$, with no factor
///   \f$1/i\f$. C++QED's Hamiltonians add \f$H/i\cdot\psi\f$, so they store \f$A=H/i\f$.
/// - The stored operator need not be Hermitian: in QJMC and Master equation contexts \f$H\f$ is the full
///   effective Hamiltonian \f$H_\text{eff} = H_\text{phys} - \frac{i}{2}\sum_k J_k^\dagger J_k\f$.
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

  /// @brief Forwards @p args to the diagonal map. Constrained, so that a non-const lvalue `MultiDiagonal`
  /// reaches the deleted copy constructor rather than this one.
  explicit MultiDiagonal(auto&&... args) requires std::constructible_from<Diagonals,decltype(args)...> : diagonals{FWD(args)...} {}

  /// @brief Named deep copy.
  friend MultiDiagonal copy(const MultiDiagonal& md)
  {
    MultiDiagonal res;
    for (const auto& [offsets,diag] : md.diagonals) res.diagonals.emplace(offsets,copy(diag));
    return res;
  }

  Diagonals diagonals;

  /// @brief Accumulates the operator's action on @p psi into @p dpsidt: `dpsidt(k+o⁻) += diag(k) * psi(k+o⁺)`
  /// for each diagonal and each entry `k`, with no factor \f$1/i\f$ (see the operator conventions above).
  /// Per axis, rows run in \f$[o^-,D-o^+)\f$ and columns in \f$[o^+,D-o^-)\f$.
  ///
  /// The views may be strided and offset, e.g. slices of a larger state, each with its own strides.
  /// **@p psi and @p dpsidt must not overlap.**
  ///
  /// Chooses between two loop shapes, which give the same result up to rounding:
  /// - `applyByDiagonals`, one diagonal at a time, for an axis 0 of up to four elements (e.g. a qubit), and for views
  ///   contiguous along axis 0 that are either small (up to 128 amplitudes, e.g. C++QED's slices of a single mode),
  ///   or fit in a last-level cache (up to 2¹⁸ amplitudes) with an axis 0 shorter than 32, or are slices of up to 512
  ///   amplitudes of a state that does not fit in cache (storage, `dataView`, of at least 2¹⁸ amplitudes);
  /// - `applyByRows`, the diagonals of a row summed before one store, otherwise: for long axes, for operands larger than
  ///   cache, and for views strided along axis 0.
  ///
  /// The row-oriented shape pays a fixed cost per row of axis 0 (the rows at either end); the other sweeps the view once
  /// per diagonal. On slices of a state larger than cache the row shape's speed also depends on where the arrays lie
  /// within their pages, by up to 2.6×, which the other's doesn't (#27). The bounds are where the two cross over on the
  /// reference machine, as medians over placements.
  /// @see @ref md_benchmarks_2performance "Performance of MultiDiagonal" for what the two shapes cost and why.
  void operator () (double, MultiArrayConstView<dcomp,RANK> psi, MultiArrayView<dcomp,RANK> dpsidt) const
  {
    const size_t length0{dpsidt.extents[0]}, size{multiarray::calculateExtent(dpsidt.extents)},
                 storage{std::max(psi.dataView.size(),dpsidt.dataView.size())};
    if (length0<=4 || (psi.strides[0]==1 && dpsidt.strides[0]==1 &&
                       (size<=128 || (length0<32 && size<=(1uz<<18)) || (size<=512 && storage>=(1uz<<18)))))
      applyByDiagonals(psi,dpsidt);
    else applyByRows(psi,dpsidt);
  }

  /// @brief The loop shape of application one diagonal at a time. Same contract as `operator()`.
  ///
  /// Per diagonal, the addresses of the first row and column are computed once. A loop nest generated at compile
  /// time then runs over the axes with axis 0 innermost, i.e. in memory order for both the diagonal and the state.
  /// The outer axes only add strides to pointers. The innermost loop multiplies with `textbookProduct`, so that it
  /// vectorizes also without `-fcx-limited-range`, and has a separate branch for unit stride. An axis 0 of up to four
  /// elements (e.g. a qubit) is unrolled at compile time instead, and the loop over axis 1 becomes the innermost one.
  ///
  /// This shape stores into @p dpsidt once per diagonal and element. For a view larger than cache it therefore sweeps
  /// @p dpsidt once per diagonal, and its speed depends on where @p psi lies relative to @p dpsidt within a page.
  void applyByDiagonals(MultiArrayConstView<dcomp,RANK> psi, MultiArrayView<dcomp,RANK> dpsidt) const
  {
    if (!checkApplication(psi,dpsidt)) return;

    const size_t strideRows0{dpsidt.strides[0]}, strideCols0{psi.strides[0]};

    for (const auto& [offsets,diag] : diagonals) {

      if (diag.dataView.empty()) continue; // |o|=D on some axis: a structural zero

      // first row o⁻ in dpsidt, first column o⁺ in psi
      dcomp* rows{dpsidt.dataView.data()+dpsidt.offset}; const dcomp* cols{psi.dataView.data()+psi.offset};
      for (auto&& [o,sR,sC] : std::views::zip(offsets,dpsidt.strides,psi.strides)) {
        if (o<0) rows+=sR*size_t(-o); else cols+=sC*size_t(o);
      }

      // The loop nest walks the diagonal d in storage order (an owning MultiArray, contiguous, axis 0 fastest), and returns
      // d advanced past the entries it used. len0 is the length of axis 0 if fixed at compile time, otherwise 0.
      // Bounds and strides are copied, and d is passed by value, so that the compiler sees that the stores into dpsidt
      // don't change them, also where it doesn't inline the lambda; otherwise it cannot vectorize.
      const auto loop=[&] (auto n, const auto& self, auto len0, dcomp* r, const dcomp* c, const dcomp* d) -> const dcomp*
      {
        using namespace hana::literals;
        if constexpr (n!=0_c) {
          const size_t extent{diag.extents[n]}, sR{dpsidt.strides[n]}, sC{psi.strides[n]};
          for (size_t k=0; k<extent; ++k) d=self(n-1_c,self,len0,r+k*sR,c+k*sC,d);
          return d;
        }
        else {
          const size_t sR{strideRows0}, sC{strideCols0};
          const auto multiplyAdd=[=] (size_t k) {r[k*sR]+=textbookProduct(d[k],c[k*sC]);};
          if constexpr (decltype(len0)::value) {
            hana::for_each(hana::make_range(hana::size_c<0>,len0),multiplyAdd);
            return d+decltype(len0)::value;
          }
          else {
            const size_t len{diag.extents[0]};
            if (sR==1 && sC==1) for (size_t k=0; k<len; ++k) r[k]+=textbookProduct(d[k],c[k]);
            else for (size_t k=0; k<len; ++k) multiplyAdd(k);
            return d+len;
          }
        }
      };

      const auto run=[&] (auto len0) {loop(hana::llong_c<RANK-1>,loop,len0,rows,cols,diag.dataView.data());};

      // A short axis 0 is unrolled at compile time: a vectorized loop over a few elements costs more than it saves.
      // Up to four is where the two cross over (benchmarks/kernels.cc, case E).
      switch (diag.extents[0]) {
        case 1:  run(hana::size_c<1>); break;
        case 2:  run(hana::size_c<2>); break;
        case 3:  run(hana::size_c<3>); break;
        case 4:  run(hana::size_c<4>); break;
        default: run(hana::size_c<0>);
      }
    }
  }

  /// @brief The loop shape of application by rows: the diagonals contributing to a row are summed before one store.
  /// Same contract as `operator()`.
  ///
  /// A loop nest generated at compile time runs over the rows with axis 0 innermost. Along the outer axes it keeps the
  /// list of diagonals that have entries in the current row, with the addresses where their entries and columns start.
  /// Along axis 0, the rows in which all of them have entries are summed in one loop, with the number of diagonals
  /// fixed at compile time, so that it vectorizes. The few rows at either end, where some diagonals have no entry, are
  /// summed one by one. @p dpsidt is thus stored once per element. An operand larger than cache is therefore swept
  /// once, rather than once per diagonal, and where @p psi lies relative to @p dpsidt within a page matters by at most
  /// about 10%, rather than by up to 20% (and occasionally 2× for a whole process).
  ///
  /// Up to six diagonals are summed per store, since each needs two pointers in registers, and up to sixteen are listed
  /// per sweep. An operator with more diagonals per row is stored into several times, and one with more diagonals in all
  /// is swept several times.
  void applyByRows(MultiArrayConstView<dcomp,RANK> psi, MultiArrayView<dcomp,RANK> dpsidt) const
  {
    if (!checkApplication(psi,dpsidt)) return;

    struct Tap {Offsets o; Extents<RANK> strides;}; // a diagonal: its offsets, and its strides in storage
    std::array<Tap,maxListed> taps;

    using Pointers = std::array<const dcomp*,maxListed>;
    // the diagonals with entries in a row: their indices in taps, and where their entries and their columns start
    struct Listed {size_t size; std::array<size_t,maxListed> tap; Pointers d, c;};

    const auto loop=[&] (auto n, const auto& self, const Listed& listed, dcomp* r) -> void
    {
      using namespace hana::literals;
      const size_t extent{dpsidt.extents[n]}, sR{dpsidt.strides[n]}, sC{psi.strides[n]};
      const auto bounds=[&] (size_t i) {const ptrdiff_t o{taps[listed.tap[i]].o[n]}; return std::pair{size_t(o<0 ? -o : 0),size_t(o>0 ? o : 0)};};

      if constexpr (n!=0_c) for (size_t k=0; k<extent; ++k) { // row k along axis n: entry k-o⁻ of each diagonal with k ∈ [o⁻,D-o⁺)
        Listed next{};
        for (size_t i=0; i<listed.size; ++i)
          if (const auto [om,op]=bounds(i); k>=om && k<extent-op) {
            const size_t t{listed.tap[i]};
            next.tap[next.size]=t; next.d[next.size]=listed.d[i]+(k-om)*taps[t].strides[n]; next.c[next.size]=listed.c[i]+(k-om+op)*sC;
            ++next.size;
          }
        if (next.size) self(n-1_c,self,next,r+k*sR);
      }
      else {
        // rows [lo,hi) have entries in every listed diagonal
        size_t lo=0, hi=extent;
        for (size_t i=0; i<listed.size; ++i) {const auto [om,op]=bounds(i); lo=std::max(lo,om); hi=std::min(hi,extent-op);}

        const auto sumOneByOne=[&] (size_t row) { // entry row-o⁻, column row-o⁻+o⁺
          dcomp sum{};
          for (size_t i=0; i<listed.size; ++i)
            if (const auto [om,op]=bounds(i); row>=om && row<extent-op) sum+=textbookProduct(listed.d[i][row-om],listed.c[i][(row-om+op)*sC]);
          r[row*sR]+=sum;
        };
        for (size_t row=0; row<lo; ++row) sumOneByOne(row);
        for (size_t row=std::max(lo,hi); row<extent; ++row) sumOneByOne(row);

        if (lo<hi) for (size_t i=0; i<listed.size; i+=maxSummed) {
          const size_t count{std::min(maxSummed,listed.size-i)};
          std::array<const dcomp*,maxSummed> d{}, c{};
          for (size_t j=0; j<count; ++j) {const auto [om,op]=bounds(i+j); d[j]=listed.d[i+j]+(lo-om); c[j]=listed.c[i+j]+(lo-om+op)*sC;}
          [&] <size_t... m> (std::index_sequence<m...>) {
            ((count==m+1 ? sumRows(d,c,r+lo*sR,hi-lo,sR,sC,std::make_index_sequence<m+1>{}) : void()), ...);
          } (std::make_index_sequence<maxSummed>{});
        }
      }
    };

    for (auto diag=diagonals.begin(); diag!=diagonals.end(); ) {
      Listed all{};
      for (; diag!=diagonals.end() && all.size<maxListed; ++diag) if (!diag->second.dataView.empty()) {
        taps[all.size]={diag->first,diag->second.strides};
        all.tap[all.size]=all.size; all.d[all.size]=diag->second.dataView.data(); all.c[all.size]=psi.dataView.data()+psi.offset;
        ++all.size;
      }
      if (all.size) loop(hana::llong_c<RANK-1>,loop,all,dpsidt.dataView.data()+dpsidt.offset);
    }
  }

private:
  static constexpr size_t maxSummed=6, maxListed=16; // the most diagonals applyByRows sums per store, and lists per sweep

  /// Debug checks of application. Returns false if there is nothing to apply.
  bool checkApplication(MultiArrayConstView<dcomp,RANK> psi, MultiArrayView<dcomp,RANK> dpsidt) const
  {
    if (diagonals.empty()) return false;
#ifndef NDEBUG
    if (psi.extents != calculateAndCheckDimensions(*this)) throw std::runtime_error("Mismatch between StateVector and MultiDiagonal dimensions");
    checkExtents(psi,MultiArrayConstView<dcomp,RANK>{dpsidt},"MultiDiagonal application");
#else
    (void)psi; (void)dpsidt;
#endif // NDEBUG
    return true;
  }

  /// The rows [0,len) of a line, with the diagonals g summed before one store. Their entries start at d[g], and their
  /// columns at c[g]. The pointers are copied, so that the compiler sees that the stores into r don't change them.
  template <size_t... g>
  static void sumRows(const std::array<const dcomp*,maxSummed>& dAll, const std::array<const dcomp*,maxSummed>& cAll,
                      dcomp* r, size_t len, size_t sR, size_t sC, std::index_sequence<g...>)
  {
    const std::array d{dAll[g]...}, c{cAll[g]...};
    if (sR==1 && sC==1) for (size_t k=0; k<len; ++k) r[k]+=(... + textbookProduct(d[g][k],c[g][k]));
    else for (size_t k=0; k<len; ++k) r[k*sR]+=(... + textbookProduct(d[g][k],c[g][k*sC]));
  }

public:


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

  /// @brief \f$A+A^\dagger = 2\,\mathrm{Re}A\f$, which is Hermitian.
  friend MultiDiagonal twoTimesRealPartOf(const MultiDiagonal& md) {return md+hermitianConjugateOf(md);}

  /// @brief \f$A-A^\dagger = 2i\,\mathrm{Im}A\f$, which is anti-Hermitian.
  ///
  /// This is \f$H/i\f$ for \f$H=i(A-A^\dagger)\f$, e.g. a coherent drive \f$H=i(\eta a^\dagger-\eta^*a)\f$ with \f$A=\eta a^\dagger\f$.
  friend MultiDiagonal twoITimesImagPartOf(const MultiDiagonal& md) {return md-hermitianConjugateOf(md);}
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

  /// @brief Boost.JSON deserialization: reconstructs from the array that `value_from` emits.
  /// Throws if two entries have the same offsets, or if the diagonals imply different dimensions
  /// (see `calculateAndCheckDimensions`).
  friend MultiDiagonal tag_invoke( const json::value_to_tag< MultiDiagonal >&, const json::value& jv )
  {
    MultiDiagonal res;
    for (const json::value& entry : jv.as_array()) {
      const json::object& o{entry.as_object()};
      if (!res.diagonals.emplace(json::value_to<Offsets>(o.at("offsets")),json::value_to<Diagonal>(o.at("diagonal"))).second)
        throw std::runtime_error("Duplicate offsets in the JSON of a MultiDiagonal: "+json::serialize(o.at("offsets")));
    }
    calculateAndCheckDimensions(res);
    return res;
  }

};


/// @brief Composition (matrix product) of two `MultiDiagonal` operators of arbitrary rank:
/// \f$(A|B)_{rc} = \sum_m A_{rm} B_{mc}\f$.
///
/// For each pair of diagonals — \f$A\f$ at signed offsets \f$p\f$, \f$B\f$ at \f$q\f$ (per axis,
/// dimension \f$D\f$) — the contribution lands on the result diagonal at
/// \f[ s = p + q \qquad\text{(the quanta moved add up).} \f]
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

/// @brief The rank-1 operator with the single diagonal @p values at the signed offset @p offset: the building block of operators.
///
/// Operators with several diagonals, or on several axes, are built from these with `operator+`, `operator|`
/// (composition) and `operator*` (direct product, the left operand's axes first). Only a diagonal whose coefficients
/// don't factorize over the axes needs to be put into `MultiDiagonal::diagonals` directly.
///
/// Following the storage convention of `MultiDiagonal`, `values[k]` is the matrix element \f$A_{k+o^-,\,k+o^+}\f$,
/// i.e. \f$\langle k|A|k+o\rangle\f$ for an upper diagonal (\f$o>0\f$), and the dimension is `values.size()+|offset|`.
/// Empty @p values give a structural zero of dimension `|offset|`, e.g. \f$a\f$ for a cutoff of 1.
///
/// ```
/// auto a {multidiagonal::diagonal(+1,{1.,std::sqrt(2.),std::sqrt(3.)})};   // a for a cutoff of 4
/// auto sm{multidiagonal::diagonal(+1,{1.})};                              // σ⁻ = |0⟩⟨1|
/// auto jc{hermitianConjugateOf(sm) * a + sm * hermitianConjugateOf(a)};   // qubit on axis 0, mode on axis 1
/// ```
MultiDiagonal<1> diagonal(ptrdiff_t offset, std::vector<dcomp> values);

/// @brief Constructs the rank-1 identity operator of dimension @p dim.
MultiDiagonal<1> identity(size_t dim);

} // multidiagonal


} // quantumoperator


inline quantumoperator::MultiDiagonal<1> quantumoperator::multidiagonal::diagonal(ptrdiff_t offset, std::vector<dcomp> values)
{
  MultiDiagonal<1> res;
  res.diagonals.emplace(MultiDiagonal<1>::Offsets{offset}, multiarray::fromStorage(std::move(values)));
  return res;
}


/// @brief Rank-1 identity operator: a single main diagonal (signed offset 0) filled with 1.
inline quantumoperator::MultiDiagonal<1> quantumoperator::multidiagonal::identity(size_t dim)
{
  return diagonal(0, std::vector<dcomp>(dim,1.));
}