// Copyright András Vukics 2006–2026. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt)
// Downstream smoke test. The part up to the checks is the README's usage example: keep the two in sync.
#include <multidiagonal/MultiDiagonal.h>

#include <cmath>
#include <cstdio>

using namespace cppqedutils;                     // dcomp, MultiArray
namespace md = quantumoperator::multidiagonal;

int main()
{
  const size_t N=5;                              // cutoff of the mode
  std::vector<dcomp> ladder(N-1);
  for (size_t k=0; k<N-1; ++k) ladder[k]=std::sqrt(k+1.);

  auto a {md::diagonal(+1,std::move(ladder))};   // ⟨n|a|n+1⟩ = √(n+1): the diagonal at offset +1
  auto sm{md::diagonal(+1,{1.})};                // σ⁻ = |0⟩⟨1|

  auto n {hermitianConjugateOf(a) * a};                                  // composition: the offsets add up to 0
  auto jc{hermitianConjugateOf(sm) % a + sm % hermitianConjugateOf(a)};  // direct product: qubit on axis 0, mode on axis 1

  MultiArray<dcomp,2> psi{{2,N},multiarray::zeroInit<dcomp>}, dpsi{{2,N},multiarray::zeroInit<dcomp>};
  psi(0,1)=1.;                                   // |0⟩|1⟩
  jc(0.,psi,dpsi);                               // accumulates jc|0⟩|1⟩ = |1⟩|0⟩ into dpsi

  // ── checks ──
  for (size_t q=0; q<2; ++q) for (size_t m=0; m<N; ++m)
    if (std::abs(dpsi(q,m)-(q==1 && m==0 ? 1. : 0.)) > 1e-12) {
      std::fprintf(stderr, "jc|0>|1> has %g%+gi at |%zu>|%zu>\n", dpsi(q,m).real(), dpsi(q,m).imag(), q, m);
      return 1;
    }

  for (size_t k=0; k<N; ++k) {
    MultiArray<dcomp,1> phi{{N}, multiarray::zeroInit<dcomp>}, dphi{{N}, multiarray::zeroInit<dcomp>};
    phi(k)=1.;
    n(0., phi, dphi);
    if (std::abs(dphi(k)-double(k)) > 1e-12) {
      std::fprintf(stderr, "n|%zu> != %zu|%zu>\n", k, k, k);
      return 1;
    }
  }

  std::printf("%s\n", toStringJSON(n).c_str());
}
