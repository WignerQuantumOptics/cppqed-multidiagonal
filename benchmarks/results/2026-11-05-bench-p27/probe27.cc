// Probe for cppqed-multidiagonal#27 (not part of the repository): kernels.cc case F's five-diagonal operator on contiguous
// slices of length n, one slice per call, of a state of about 2^20 amplitudes. psi and dpsidt lie in one 2 MiB-aligned buffer,
// dpsidt 2^24 + delta bytes after psi (delta = 2048 by default: neutral in case H), on huge pages or on 4 KB pages.
// psi starts psiOffset bytes into its page (default 0).
// usage: probe27 n rows|diags huge|4k [delta] [applications] [psiOffset]   prints: n S shape pages delta psiOffset hugeKB ns_per_nnz
#include <multidiagonal/MultiDiagonal.h>

#include <sys/mman.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

using namespace cppqedutils;
using quantumoperator::MultiDiagonal;

namespace {

template <typename T> void doNotOptimize(T const& v) { asm volatile("" : : "r,m"(v) : "memory"); }

dcomp coeff(ptrdiff_t o, size_t k) { return { std::cos(0.1*double(k)+double(o)), std::sin(0.05*double(k)-double(o)) }; } // as kernels.cc

MultiDiagonal<1> penta(size_t N) // as kernels.cc
{
  MultiDiagonal<1> md;
  for (ptrdiff_t o=-2; o<=2; ++o) {
    std::vector<dcomp> v(N-size_t(std::abs(o)));
    for (size_t k=0; k<v.size(); ++k) v[k]=coeff(o,k);
    md.diagonals.emplace(MultiDiagonal<1>::Offsets{o}, multiarray::fromStorage(std::move(v)));
  }
  return md;
}

long anonHugePagesKB()
{
  std::ifstream f{"/proc/self/smaps_rollup"}; std::string key; long v=0;
  while (f >> key) { if (key=="AnonHugePages:") { f >> v; break; } f.ignore(std::numeric_limits<std::streamsize>::max(),'\n'); }
  return v;
}

} // anonymous

int main(int argc, char** argv)
{
  if (argc<4) { std::fprintf(stderr,"usage: probe27 n rows|diags huge|4k [delta] [applications] [psiOffset]\n"); return 2; }
  const size_t n=std::strtoul(argv[1],nullptr,10), S=(1uz<<20)/n, N=n*S, P=1uz<<24, huge=1uz<<21;
  const bool rows=std::strcmp(argv[2],"rows")==0, hugePages=std::strcmp(argv[3],"huge")==0;
  const ptrdiff_t delta = argc>4 ? std::strtol(argv[4],nullptr,10) : 2048;
  const size_t applications = argc>5 ? std::strtoul(argv[5],nullptr,10) : 20;
  const size_t psiOffset = argc>6 ? std::strtoul(argv[6],nullptr,10) : 0;
  const size_t bytes=(3*P+huge)/huge*huge;

  const std::unique_ptr<dcomp,decltype(&std::free)> buffer{static_cast<dcomp*>(std::aligned_alloc(huge,bytes)),&std::free};
  madvise(buffer.get(),bytes,hugePages ? MADV_HUGEPAGE : MADV_NOHUGEPAGE);
  const long before=anonHugePagesKB(); std::fill_n(buffer.get(),bytes/sizeof(dcomp),dcomp{});
  const long hugeKB=anonHugePagesKB()-before;

  dcomp* const psi=buffer.get()+psiOffset/sizeof(dcomp); dcomp* const y=psi+(ptrdiff_t(P)+delta)/ptrdiff_t(sizeof(dcomp));
  for (size_t k=0; k<N; ++k) psi[k]=coeff(1,k);
  const auto A=penta(n);
  const std::span<const dcomp> xs{psi,N}; const std::span<dcomp> ys{y,N};
  const auto apply=[&](bool byRows){
    for (size_t b=0; b<S; ++b) {
      const MultiArrayView<const dcomp,1> x{{n},{1},b*n,xs}; const MultiArrayView<dcomp,1> z{{n},{1},b*n,ys};
      if (byRows) A.applyByRows(x,z); else A.applyByDiagonals(x,z);
    }
  };

  // the two shapes must agree
  std::fill_n(y,N,dcomp{}); apply(false); const std::vector<dcomp> ref(y,y+N);
  std::fill_n(y,N,dcomp{}); apply(true);
  double dev=0, norm=0; for (size_t k=0; k<N; ++k) { dev+=std::norm(y[k]-ref[k]); norm+=std::norm(ref[k]); }
  if (!(std::sqrt(dev/norm)<1e-12)) { std::fprintf(stderr,"VALIDATION FAILED: %g\n",std::sqrt(dev/norm)); return 1; }

  apply(rows); // warm up
  const auto t0=std::chrono::steady_clock::now();
  for (size_t a=0; a<applications; ++a) { apply(rows); doNotOptimize(y); }
  const double ns=std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-t0).count();
  const double nnz=double(S)*double(5*n-6)*double(applications);
  std::printf("%zu %zu %s %s %td %zu %ld %.4g\n",n,S,rows ? "rows" : "diags",hugePages ? "huge" : "4k",delta,psiOffset,hugeKB,ns/nnz);
}
