#!/usr/bin/env bash
# Part of the cppqed-multidiagonal benchmark suite. BSL-1.0.
# The driver behind benchmarks/performance.md: runs the benchmark executables of one or more builds in
# separate processes, interleaved, pinned to one core, on huge pages and on 4 KB pages, and keeps the raw
# CSV of every process together with a description of the machine and of the builds.
#
# usage: benchmarks/run.sh [options] OUTDIR LABEL=BUILDDIR [LABEL=BUILDDIR ...]
#
#   OUTDIR           where the results go; must not exist yet or be empty
#   LABEL=BUILDDIR   a build of the library (its CMake build directory) and the name its results get,
#                    e.g. now=build-release v0.6.0=../build-v0.6.0. Configure it as a Release build:
#                      cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
#                    The executables needed are (re)built here before anything is measured, so that
#                    they match the source that builds.txt records.
#
#   --core C         pin every process to core C with taskset (pick one that handles no device interrupts)
#   --no-pin         run unpinned; without --core this must be said explicitly
#   --rounds N       processes per configuration (default 5); figures are medians over them
#   --suites LIST    comma-separated, default main,kernels,kernels-cxlr:
#                      main          cppqed-multidiagonal-benchmarks: apply1, compose1, …, picture1, slices3
#                      kernels       cppqed-multidiagonal-kernels: cases A–F, default complex multiplication
#                      kernels-cxlr  cppqed-multidiagonal-kernels-cxlr: the same with -fcx-limited-range
#                    A suite takes a selection after a colon, joined by +, which is passed to the executable:
#                      main:apply1+applyJC   kernels:D   kernels:G (the placement scan, minutes)
#   --pages LIST     comma-separated, default thp,4k: as the system hands out pages, and with --no-thp
#   --smoke          pass --smoke to the executables: a test of this script, the timings are meaningless
#
# Order of the processes: round by round; within a round suite by suite and page mode by page mode; within
# those, one process per build. So the builds alternate, and a drift of the machine during the session
# reaches all of them alike.
#
# It refuses to start while another benchmark or a build runs on the machine.
#
# Output:
#   OUTDIR/machine.txt                       the machine and its state at the start and at the end
#   OUTDIR/builds.txt                        per build: source revision, compiler, flags, Eigen
#   OUTDIR/log                               per process: when it ended, how long it took, and the time the
#                                            hypervisor took from its core meanwhile (steal; 0 on bare metal)
#   OUTDIR/LABEL/build.log                   the output of the build step
#   OUTDIR/LABEL/SUITE-PAGES-rROUND.csv      the standard output of one process
#   OUTDIR/LABEL/SUITE-PAGES-rROUND.err      its standard error (validation, summaries of cases G to I)
# In file names the colon of a suite's selection is written as a dot: kernels.D-4k-r1.csv.
#
# benchmarks/reduce.py turns OUTDIR into tables (medians over the processes, ratios, how well they agree).
set -euo pipefail

declare -A EXE=(
  [main]=cppqed-multidiagonal-benchmarks
  [kernels]=cppqed-multidiagonal-kernels
  [kernels-cxlr]=cppqed-multidiagonal-kernels-cxlr
)

die() { echo "run.sh: $*" >&2; exit 2; }

rounds=5; core=; pin=yes; suites=main,kernels,kernels-cxlr; pages=thp,4k; smoke=
while [[ $# -gt 0 && $1 == --* ]]; do
  case $1 in
    --core)   core=${2:?--core needs a core number}; shift 2;;
    --no-pin) pin=no; shift;;
    --rounds) rounds=${2:?--rounds needs a number}; shift 2;;
    --suites) suites=${2:?--suites needs a list}; shift 2;;
    --pages)  pages=${2:?--pages needs a list}; shift 2;;
    --smoke)  smoke=--smoke; shift;;
    *) die "unknown option $1";;
  esac
done
[[ $# -ge 2 ]] || die "usage: run.sh [options] OUTDIR LABEL=BUILDDIR [LABEL=BUILDDIR ...]"
out=$1; shift

[[ $rounds =~ ^[1-9][0-9]*$ ]] || die "--rounds: not a positive number: $rounds"
if [[ -n $core ]]; then
  command -v taskset >/dev/null || die "--core needs taskset (util-linux)"
  taskset -c "$core" true 2>/dev/null || die "cannot pin to core $core"
  runner=(taskset -c "$core")
elif [[ $pin == no ]]; then
  runner=()
else
  die "give --core C to pin the runs, or --no-pin to run unpinned"
fi

IFS=, read -ra suiteList <<< "$suites"
IFS=, read -ra pageList <<< "$pages"
declare -A needed
for spec in "${suiteList[@]}"; do
  name=${spec%%:*}
  [[ -v EXE[$name] ]] || die "unknown suite '$name'; known: ${!EXE[*]}"
  [[ $spec != *:* || ${spec#*:} =~ ^[A-Za-z0-9_]+(\+[A-Za-z0-9_]+)*$ ]] || die "suite '$spec': the selection is names joined by +"
  needed[${EXE[$name]}]=1
done
for p in "${pageList[@]}"; do [[ $p == thp || $p == 4k ]] || die "unknown page mode '$p'; known: thp 4k"; done

thpSetting=$(sed -n 's/.*\[\(.*\)\].*/\1/p' /sys/kernel/mm/transparent_hugepage/enabled 2>/dev/null || true)
if [[ " ${pageList[*]} " == *" thp "* && $thpSetting != always ]]; then
  echo "run.sh: note: transparent huge pages are '${thpSetting:-not available}' here, not 'always':" \
       "the thp runs get huge pages only where an executable asks for them (case H)" >&2
fi

labels=(); declare -A dirOf
for arg; do
  [[ $arg == *=* ]] || die "expected LABEL=BUILDDIR, got '$arg'"
  label=${arg%%=*}; dir=${arg#*=}
  [[ $label =~ ^[A-Za-z0-9._-]+$ ]] || die "label '$label': letters, digits, . _ - only"
  [[ -v dirOf[$label] ]] && die "label '$label' given twice"
  [[ -f $dir/CMakeCache.txt ]] || die "$dir is not a CMake build directory"
  labels+=("$label"); dirOf[$label]=$(cd "$dir" && pwd)
done

if pgrep -f 'cppqed-multidiagonal-(benchmarks|kernels)' >/dev/null || pgrep -x 'ninja|make|cc1plus' >/dev/null; then
  die "another benchmark or a build is running on this machine; not started"
fi

if [[ -e $out ]]; then
  [[ -d $out && -z $(ls -A "$out") ]] || die "$out exists and is not empty: results are never overwritten"
fi
mkdir -p "$out"; out=$(cd "$out" && pwd)

# ── the builds ──────────────────────────────────────────────────────────────

for label in "${labels[@]}"; do
  dir=${dirOf[$label]}; mkdir -p "$out/$label"
  echo "building $label: ${!needed[*]}" >&2
  cmake --build "$dir" --target "${!needed[@]}" > "$out/$label/build.log" 2>&1 || die "the build of $label failed, see $out/$label/build.log"
  for exe in "${!needed[@]}"; do [[ -x $dir/benchmarks/$exe ]] || die "$dir/benchmarks/$exe is missing after the build"; done
done

# ── the record of the machine and of the builds ─────────────────────────────

sysfs() { if [[ -r $1 ]]; then cat "$1"; else echo "not available"; fi; }

# ticks the hypervisor took from the pinned core (from all cores when unpinned) since boot
steal() { awk -v c="cpu$core" '$1==c{print $9}' /proc/stat; }

machineState() {
  echo "time (UTC):                $(date -u +%Y-%m-%dT%H:%M:%SZ)"
  echo "load average:              $(cut -d' ' -f1-3 /proc/loadavg)"
  echo "SMT:                       $(sysfs /sys/devices/system/cpu/smt/control)"
  echo "turbo off (intel_pstate):  $(sysfs /sys/devices/system/cpu/intel_pstate/no_turbo)"
  echo "governor of cpu${core:-0}:          $(sysfs /sys/devices/system/cpu/cpu${core:-0}/cpufreq/scaling_governor)"
  echo "transparent huge pages:    $(sysfs /sys/kernel/mm/transparent_hugepage/enabled)"
  echo "steal ticks of cpu$core:      $(steal)"
}

{
  echo "host:     $(uname -n)"
  echo "kernel:   $(uname -sr)"
  echo "pinning:  $([[ -n $core ]] && echo "core $core" || echo none)"
  echo "rounds:   $rounds"
  echo "suites:   ${suiteList[*]}"
  echo "pages:    ${pageList[*]}"
  echo "smoke:    ${smoke:-no}"
  echo
  lscpu | grep -E '^(Architecture|Model name|CPU\(s\)|Thread\(s\) per core|Core\(s\) per socket|Socket\(s\)|CPU max MHz|L1d|L2|L3|Hypervisor vendor|Virtualization type)' || true
  echo
  echo "── state at the start"
  machineState
} > "$out/machine.txt"

cacheValue() { sed -n "s/^$2:[A-Z]*=//p" "$1/CMakeCache.txt" | head -1; }

for label in "${labels[@]}"; do
  dir=${dirOf[$label]}
  src=$(cacheValue "$dir" CMAKE_HOME_DIRECTORY)
  cxx=$(cacheValue "$dir" CMAKE_CXX_COMPILER)
  eigenDir=$(cacheValue "$dir" Eigen3_DIR)
  if revision=$(git -C "$src" rev-parse HEAD 2>/dev/null); then
    [[ -z $(git -C "$src" status --porcelain 2>/dev/null) ]] || revision+=" (with uncommitted changes)"
  else
    revision="not a git checkout"
  fi
  echo "[$label]"
  echo "build directory:  $dir"
  echo "source:           $src"
  echo "revision:         $revision"
  echo "build type:       $(cacheValue "$dir" CMAKE_BUILD_TYPE)"
  echo "compiler:         $cxx: $("$cxx" --version 2>/dev/null | head -1)"
  echo "CMAKE_CXX_FLAGS:  $(cacheValue "$dir" CMAKE_CXX_FLAGS)"
  echo "-march=native:    $(cacheValue "$dir" MULTIDIAGONAL_BENCHMARK_NATIVE)"
  echo "Eigen:            $(sed -n 's/^set(PACKAGE_VERSION "\(.*\)")/\1/p' "$eigenDir/Eigen3ConfigVersion.cmake" 2>/dev/null | head -1) ($eigenDir)"
  echo
done > "$out/builds.txt"

# ── the runs ────────────────────────────────────────────────────────────────

total=$(( rounds * ${#suiteList[@]} * ${#pageList[@]} * ${#labels[@]} )); n=0; failed=0
for (( round=1; round<=rounds; ++round )); do
  for spec in "${suiteList[@]}"; do
    exe=${EXE[${spec%%:*}]}; selection=; [[ $spec == *:* ]] && selection=${spec#*:}
    for p in "${pageList[@]}"; do
      for label in "${labels[@]}"; do
        n=$((n+1)); stem="$out/$label/${spec/:/.}-$p-r$round"
        echo "[$n/$total] $label $spec $p round $round" >&2
        t0=$SECONDS; s0=$(steal); status=ok
        # $smoke and the selection are word lists on purpose
        # shellcheck disable=SC2086
        "${runner[@]}" "${dirOf[$label]}/benchmarks/$exe" $smoke $([[ $p == 4k ]] && echo --no-thp) ${selection//+/ } \
          > "$stem.csv" 2> "$stem.err" || { status="FAILED ($?)"; failed=$((failed+1)); echo "  FAILED, see $stem.err" >&2; }
        printf '%s  %-12s %-28s %-3s r%-2d %5ds  steal %4d  %s\n' \
          "$(date +%T)" "$label" "$spec" "$p" "$round" $((SECONDS-t0)) $(( $(steal) - s0 )) "$status" >> "$out/log"
      done
    done
  done
done

{ echo; echo "── state at the end"; machineState; echo "failed processes:          $failed"; } >> "$out/machine.txt"
echo "results in $out; $failed of $total processes failed" >&2
[[ $failed -eq 0 ]]
