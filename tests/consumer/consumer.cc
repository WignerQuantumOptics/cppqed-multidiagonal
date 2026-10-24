// Copyright András Vukics 2006–2026. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt)
// Downstream smoke test: the number operator a†|a applied to Fock states, and JSON output.
#include <multidiagonal/MultiDiagonal.h>

#include <cmath>
#include <cstdio>

using namespace cppqedutils;
using quantumoperator::MultiDiagonal;

int main()
{
  const size_t dim=5;
  std::vector<dcomp> ladder(dim-1);
  for (size_t k=0; k<dim-1; ++k) ladder[k]=std::sqrt(double(k+1));

  MultiDiagonal<1> a;
  a.diagonals.emplace(MultiDiagonal<1>::Offsets{+1}, multiarray::fromStorage(std::move(ladder)));
  auto n = hermitianConjugateOf(a) | a;

  for (size_t k=0; k<dim; ++k) {
    MultiArray<dcomp,1> psi{{dim}, multiarray::zeroInit<dcomp>}, dpsidt{{dim}, multiarray::zeroInit<dcomp>};
    psi(k)=1.;
    n(0., psi, dpsidt);
    if (std::abs(dpsidt(k)-double(k)) > 1e-12) {
      std::fprintf(stderr, "n|%zu> != %zu|%zu>\n", k, k, k);
      return 1;
    }
  }

  std::printf("%s\n", toStringJSON(n).c_str());
}
