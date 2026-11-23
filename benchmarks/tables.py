#!/usr/bin/env python3
# Part of the cppqed-multidiagonal benchmark suite. BSL-1.0.
"""The tables of benchmarks/performance.md, generated from the sessions in benchmarks/results.

usage: benchmarks/tables.py [NAME ...]   print the tables named (all of them without a name)
       benchmarks/tables.py --write      put the generated tables into performance.md
       benchmarks/tables.py --check      fail if performance.md differs from what --write would give

In the page, a generated table stands between the lines

    <!-- table NAME -->
    <!-- end table -->

with a blank line after the first and before the second.

Each function below is one table: which session it reads (a directory of benchmarks/results), which
values, and how they are set. A value is the median over the processes of its configuration, as in
reduce.py, whose reading of the sessions is used here. Only the standard library is needed.
"""
import collections, contextlib, io, os, re, statistics, sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import reduce

RESULTS = os.path.join(HERE, 'results')
PAGE = os.path.join(HERE, 'performance.md')

# the sessions, by the names they have below
REF = '2026-11-03-bench-ref'              # builds now (7725d79, "after #23"), pre23, v040
ROWSHAPE = '2026-11-04-bench-fb8ca7f'     # build step3: the row shape as committed (c346b8a), "now" in the tables
CASE_F = '2026-11-04-bench-caseF'
CASE_G = '2026-11-04-bench-caseG'
CASE_H = '2026-11-04-bench-caseH2'
REMEASURE = '2026-11-04-bench-remeasure'  # c346b8a on huge pages and on 4 KB pages
B5 = '2026-11-05-bench-b5'
SIMD = '2026-11-05-bench-simd'
CASE_I = '2026-11-07-bench-caseI2'
CASE_D = '2026-11-08-bench-caseD4k'
TODAY = '2026-11-23-bench'                # 134ddcf: the summary and the assessment
SECOND = '2026-11-23-zen2-vm'

_labels = {}

def session(name, label, suite, pages):
    """The Configuration of one build, suite and page mode of a session."""
    key = (name, label)
    if key not in _labels: _labels[key] = reduce.readLabel(os.path.join(RESULTS, name, label))
    c = _labels[key].get((suite, pages))
    if c is None: sys.exit(f'tables.py: {name}/{label} has no {suite} on {pages}')
    return c

def value(c, name, impl, N=None):
    """The median of a value; N may be left out where the case has one size."""
    if N is None:
        sizes = c.sizes(name)
        if len(sizes) != 1: sys.exit(f'tables.py: {name} has the sizes {sizes}: say which')
        N = sizes[0]
    v = c.median((name, impl, N))
    if v is None: sys.exit(f'tables.py: no value for {name}, {impl}, {N} in {c.suite} on {c.pages}')
    return v

def speedup(c, name, slower, faster, N=None):
    return value(c, name, slower, N) / value(c, name, faster, N)


# ── formats ─────────────────────────────────────────────────────────────────

def f2(x): return f'{x:.2f}'
def f1(x): return f'{x:.1f}'
def f0(x): return f'{x:.0f}'

def sig2(x):
    """Two significant digits, as 0.89, 3.1, 5.0, 36."""
    return f'{x:.0f}' if x >= 9.95 else f'{x:.1f}' if x >= 0.995 else f'{x:.2f}'

def sig3(x):
    """Three significant digits for values above 1, as 47.7, 3.37."""
    return f'{x:.1f}' if x >= 9.995 else f'{x:.2f}'

def slash(values, fmt=f2):
    return ' / '.join(fmt(v) for v in values)

def span(values, fmt=f2):
    lo, hi = fmt(min(values)), fmt(max(values))
    return lo if lo == hi else f'{lo}–{hi}'

def bold(text): return f'**{text}**'

def table(header, rows):
    lines = ['| ' + ' | '.join(header) + ' |', '|' + '---|' * len(header)] + ['| ' + ' | '.join(row) + ' |' for row in rows]
    return [re.sub(r'\|  (?=\|)', '| ', line) for line in lines]  # an empty cell is written "| |"

def fasterBold(first, second, fmt=f2):
    """Two rows of values, the smaller of each column in bold."""
    a, b = [fmt(x) for x in first], [fmt(x) for x in second]
    return ([bold(x) if p <= q else x for x, p, q in zip(a, first, second)],
            [bold(y) if q <= p else y for y, p, q in zip(b, first, second)])


# ── the tables ──────────────────────────────────────────────────────────────

TABLES = collections.OrderedDict()

def generated(name):
    def register(f):
        TABLES[name] = f
        return f
    return register


@generated('complex-multiplication')
def complexMultiplication():
    """Case A in cache, before and after #23, default build and -fcx-limited-range (session ref)."""
    sizes = (256, 4096)
    def cell(label, suite, impl): return slash(value(session(REF, label, suite, 'thp'), 'A_rank1', impl, N) for N in sizes)
    return table(['case A, in cache (N = 256 / 4096)', 'default', '`-fcx-limited-range`'], [
        ['library before #23', cell('pre23', 'kernels.A+B+C', 'library'), cell('pre23', 'kernels-cxlr.A+B', 'library')],
        ['library after #23', bold(cell('now', 'kernels', 'library')), cell('now', 'kernels-cxlr', 'library')],
        ['Eigen sparse', cell('now', 'kernels', 'eigen_sparse'), cell('now', 'kernels-cxlr', 'eigen_sparse')]])


@generated('instruction-set')
def instructionSet():
    """Speed-up of the x86-64-v3 and native builds over the baseline build (session simd)."""
    def builds(suite): return {b: session(SIMD, b, suite, '4k') for b in ('base', 'v3', 'native')}
    kernels, main = builds('kernels.A+B+E'), builds('main.apply1+applyJC')
    def gains(configs, level, keep):
        return [configs['base'].median(k) / configs[level].median(k) for k in configs['base'].values if keep(k)]
    def medianAndRange(xs): return f'{f2(statistics.median(xs))}× ({f2(min(xs))}–{f2(max(xs))}×)'
    def eigen(level):  # its median over the kernels and its median over the main suite
        isEigen = lambda k: k[1] == 'eigen_sparse'
        return span([statistics.median(gains(kernels, level, isEigen)), statistics.median(gains(main, level, isEigen))]) + '×'
    def jc(level):  # time per derivative call, from the script's `times`: build, cutoff, round, microseconds, calls
        perCall = collections.defaultdict(list)
        with open(os.path.join(RESULTS, SIMD, 'times')) as stream:
            for line in stream:
                build, cutoff, _, microseconds, calls = line.split()
                perCall[(build, cutoff)].append(float(microseconds) / float(calls))
        t = {k: statistics.median(v) for k, v in perCall.items()}
        return slash(t[('base', c)] / t[(level, c)] for c in ('32', '512', '8192', '131072')) + '×'
    notEigen = lambda k: not k[1].startswith('eigen')
    library = lambda k: k[1] == 'multidiagonal'
    return table(['', '`-march=x86-64-v3` (AVX2, FMA)', '`-march=native`'], [
        ['library and hand-written loops, `kernels` A, B, E: median (range)'] + [medianAndRange(gains(kernels, l, notEigen)) for l in ('v3', 'native')],
        ['library, `apply1` and `applyJC`: median (range)'] + [medianAndRange(gains(main, l, library)) for l in ('v3', 'native')],
        ['Eigen sparse, the same: median'] + [eigen(l) for l in ('v3', 'native')],
        ['C++QED `jc_qjmc` per derivative call, state 64 / 1024 / 16384 / 262144'] + [jc(l) for l in ('v3', 'native')]])


@generated('second-machine')
def secondMachine():
    """The reference machine and the second machine side by side: the output of reduce.py ratios."""
    out = io.StringIO()
    with contextlib.redirect_stdout(out):
        reduce.ratios([f'reference={os.path.join(RESULTS, TODAY)}:4k', f'second={os.path.join(RESULTS, SECOND)}'])
    return out.getvalue().rstrip('\n').split('\n')


@generated('loop-shapes')
def loopShapes():
    """Cases B and C through the versions of the library (sessions ref and fb8ca7f), ns per nonzero."""
    old = {label: session(REF, label, suite, 'thp') for label, suite in (('v040', 'kernels.B+C'), ('pre23', 'kernels.A+B+C'), ('now', 'kernels'))}
    new = session(ROWSHAPE, 'step3', 'kernels', 'thp')
    def b(c, impl, sizes): return slash(value(c, 'B_JC_modefast', impl, N) for N in sizes)
    def c4(c, impl): return slash((value(c, 'C_axis0', impl), value(c, 'C_axis2', impl)), sig3)
    rows = []
    for title, sizes in (('B: Jaynes–Cummings, in cache (state 512 / 8192)', (512, 8192)), ('B: Jaynes–Cummings, out of cache (state 524288)', (524288,))):
        rows.append([title] + [b(old[l], 'library', sizes) for l in ('v040', 'pre23', 'now')]
                    + [bold(b(new, 'library', sizes)), b(new, 'library_byDiagonals', sizes), b(new, 'hoisted', sizes), b(new, 'eigen_sparse', sizes)])
    rows.append([r'C: rank-4 operator built as $\mathbb{1}\otimes\mathbb{1}\otimes A\otimes\mathbb{1}$, $A$ on axis 0 / axis 2 (out of cache)']
                + [c4(old[l], 'library_materialized') for l in ('v040', 'pre23', 'now')]
                + [bold(c4(new, 'library_materialized')), c4(new, 'library_materialized_byDiagonals'), '—', c4(new, 'eigen_sparse_kron')])
    return table(['', 'v0.4.0', 'before #23', 'after #23', 'now', 'one diagonal at a time', 'hand-written hoisted loop', 'Eigen sparse'], rows)


@generated('ratios-to-eigen')
def ratiosToEigen():
    """Eigen's time over the library's in the main suite, through the versions (sessions ref, fb8ca7f, remeasure)."""
    small, large = (16, 64, 256, 1024, 4096, 16384), (65536, 262144, 1048576)
    v040 = session(REF, 'v040', 'main.apply1+applyJC', 'thp'); pre23 = session(REF, 'pre23', 'main.apply1+applyJC', 'thp')
    after = session(REF, 'now', 'main', 'thp'); now = session(ROWSHAPE, 'step3', 'main', 'thp'); now4k = session(REMEASURE, 'remeasure', 'main.apply1+applyJC', '4k')
    def r(c, bench, sizes): return [speedup(c, bench, 'eigen_sparse', 'multidiagonal', N) for N in sizes]
    def emphasized(values):  # in the columns of today's library: bold where the library is the faster
        return ' / '.join(bold(f2(v)) if v > 1 else f2(v) for v in values).replace('** / **', ' / ')
    rows = []
    for bench, title in (('applyJC_modefast', '`applyJC_modefast`: mode on axis 0'), ('applyJC_qbitfast', '`applyJC_qbitfast`: qubit on axis 0'), ('apply1', '`apply1`: rank 1')):
        nowSmall = r(now, bench, small)
        rows.append([title, span(r(v040, bench, small)), span(r(pre23, bench, small)), span(r(after, bench, small)),
                     bold(span(nowSmall)) if min(nowSmall) > 1 else span(nowSmall),
                     slash(r(after, bench, large)), emphasized(r(now, bench, large)), emphasized(r(now4k, bench, large))])
    return table(['', 'v0.4.0, N ≤ 16384', 'before #23, N ≤ 16384', 'after #23, N ≤ 16384', 'now, N ≤ 16384',
                  'after #23, N = 65536 / 262144 / 1048576', 'now, N = 65536 / 262144 / 1048576', 'now, 4 KB pages, N = 65536 / 262144 / 1048576'], rows)


@generated('short-axis')
def shortAxis():
    """Case E: a short axis 0 (session fb8ca7f), ns per nonzero."""
    c = session(ROWSHAPE, 'step3', 'kernels', 'thp')
    lengths = (1, 2, 3, 4, 5, 6, 8, 16, 32)
    def row(title, impl): return [title] + [f2(value(c, f'E_axis0_{n}', impl)) for n in lengths]
    return table(['length of axis 0'] + [str(n) for n in lengths], [
        row('library (one diagonal at a time, unrolled up to 4; by rows at 32)', 'library'),
        row('library by rows', 'library_byRows'),
        row('hand-written hoisted loop (always loops over axis 0)', 'hoisted'),
        row('Eigen sparse', 'eigen_sparse')])


def crossover(header, sizes, rows):
    """Pairs of rows, one diagonal at a time and by rows, the faster of each column in bold."""
    out = []
    for title, c, case in rows:
        d, r = fasterBold([value(c, case, 'library_byDiagonals', n) for n in sizes], [value(c, case, 'library_byRows', n) for n in sizes])
        out += [[title[0]] + d, [title[1]] + r]
    return table([header] + [str(n) for n in sizes], out)


@generated('crossover-slices')
def crossoverSlices():
    """Case F: contiguous slices of length n, one per call (sessions caseF and remeasure), ns per nonzero."""
    thp, fourK = session(CASE_F, 'caseF', 'kernels.F', 'thp'), session(REMEASURE, 'remeasure', 'kernels', '4k')
    return crossover('contiguous slices of length n, one per call', (8, 16, 24, 32, 48, 64, 96, 128, 256), [
        (('state in cache: one diagonal at a time', 'state in cache: by rows'), thp, 'F_slices_2048'),
        (('state of 16 MB: one diagonal at a time', 'state of 16 MB: by rows'), thp, 'F_slices_1048576'),
        (('state of 16 MB, 4 KB pages: one diagonal at a time', 'state of 16 MB, 4 KB pages: by rows'), fourK, 'F_slices_1048576')])


@generated('crossover-grid')
def crossoverGrid():
    """Case F: tridiagonal ⊗ tridiagonal with an axis 0 of n₀ (sessions caseF and remeasure), ns per nonzero."""
    thp, fourK = session(CASE_F, 'caseF', 'kernels.F', 'thp'), session(REMEASURE, 'remeasure', 'kernels', '4k')
    return crossover('tridiagonal ⊗ tridiagonal, axis 0 of n₀', (5, 8, 12, 16, 24, 32, 48, 64, 128, 256), [
        (('in L2 (8192): one diagonal at a time', 'in L2: by rows'), thp, 'F_grid_8192'),
        (('in L3 (65536): one diagonal at a time', 'in L3: by rows'), thp, 'F_grid_65536'),
        (('out of cache (2²⁰): one diagonal at a time', 'out of cache: by rows'), thp, 'F_grid_1048576'),
        (('in L3, 4 KB pages: one diagonal at a time', 'in L3, 4 KB pages: by rows'), fourK, 'F_grid_65536'),
        (('out of cache, 4 KB pages: one diagonal at a time', 'out of cache, 4 KB pages: by rows'), fourK, 'F_grid_1048576')])


@generated('crossover-placements')
def crossoverPlacements():
    """Case I: the row shape's median over 15 placements divided by that of one diagonal at a time (session caseI2)."""
    c = session(CASE_I, 'caseI', 'kernels.I', 'thp')
    lengths = (128, 160, 192, 256, 320, 384, 512, 768, 1024)
    rows = []
    for T, title in ((4096, '64 KiB (2¹²)'), (65536, '1 MiB (2¹⁶)'), (262144, '4 MiB (2¹⁸)'), (1048576, '16 MiB (2²⁰)')):
        placements = [name for name in c.names() if name.startswith(f'I_{T}_')]
        def overPlacements(impl, n): return statistics.median(value(c, p, impl, n) for p in placements)
        rows.append([title] + [f2(overPlacements('library_byRows', n) / overPlacements('library_byDiagonals', n)) for n in lengths])
    return table(['state, per array', 'n = 128'] + [str(n) for n in lengths[1:]], rows)


@generated('placement-scan')
def placementScan():
    """Case G: psi at every 16-byte offset of a page, first of the two processes (session caseG), ns per application."""
    scans = collections.defaultdict(dict)  # (case, impl) → {offset: ns per application}
    with open(os.path.join(RESULTS, CASE_G, 'caseG', 'kernels.G-thp-r1.csv')) as stream:
        next(stream)
        for line in stream:
            case, impl, offset, perApplication, _ = line.split(',')
            scans[(case, impl)][int(offset)] = float(perApplication)
    summary = {}  # (case, impl) → best, median, worst, count, as the program wrote them to its standard error
    with open(os.path.join(RESULTS, CASE_G, 'caseG', 'kernels.G-thp-r1.err')) as stream:
        for line in stream:
            m = re.match(r'(\S+) (\S+): best (\S+), median (\S+), worst (\S+) ns; (\d+) of (\d+) offsets', line)
            if m: summary[(m[1], m[2])] = (float(m[3]), float(m[4]), float(m[5]), m[6], m[7])
    def row(title, N, case, impl):
        t = scans[(case, impl)]; offsets = sorted(t)
        best, median, worst, above, total = summary[(case, impl)]
        # "worst" leaves out single offsets more than 10% above both their neighbours: disturbances, not placement
        def disturbed(i):
            neighbours = [t[offsets[j]] for j in (i - 1, i + 1) if 0 <= j < len(offsets)]
            return all(t[offsets[i]] > 1.1 * x for x in neighbours)
        if any(disturbed(i) for i in range(len(offsets))): worst = max(t[o] for i, o in enumerate(offsets) if not disturbed(i))
        return [title, N, f0(best), f0(median), f0(worst), f'{above} / {total}']
    return table(['loop', 'N', 'best', 'median', 'worst', 'offsets more than 5% above the best'], [
        row('one diagonal at a time (chosen by `operator()` here)', '64', 'G_rank1_64', 'library_byDiagonals'),
        row('by rows', '64', 'G_rank1_64', 'library_byRows'),
        row('one diagonal at a time', '1024', 'G_rank1_1024', 'library_byDiagonals'),
        row('by rows', '1024', 'G_rank1_1024', 'library_byRows'),
        row('one diagonal at a time', '4096', 'G_rank1_4096', 'library_byDiagonals'),
        row('by rows', '4096', 'G_rank1_4096', 'library_byRows'),
        row('one diagonal at a time, Jaynes–Cummings, mode first, state 4096', '', 'G_JC_modefast', 'library_byDiagonals'),
        row('by rows, the same', '', 'G_JC_modefast', 'library_byRows'),
        row('one diagonal at a time, Jaynes–Cummings, qubit first, state 4096 (chosen by `operator()`)', '', 'G_JC_qbitfast', 'library_byDiagonals')])


@generated('distance')
def distance():
    """Case H: one diagonal at a time on slices of length 16 / 24 / 32 against the distance of the arrays (session caseH2)."""
    c = session(CASE_H, 'caseH', 'kernels.H', 'thp')
    def cell(pages, deltas):
        return ' / '.join(span([value(c, f'H_{pages}_2^24{d:+d}', 'library_byDiagonals', n) for d in deltas]) for n in (16, 24, 32))
    rows = []
    for title, deltas, emphasis in (('−4096', [-4096], False), ('−240', [-240], False), ('−16', [-16], False), ('0', [0], False),
                                    ('+16 (back to back)', [16], True), ('+64', [64], False), ('+256, +1024, +2048', [256, 1024, 2048], False),
                                    ('+4 KiB + 16', [4112], False), ('+64 KiB + 16', [65552], False),
                                    ('+1 MiB + 16, +2 MiB + 16, +16 MiB + 16', [1048592, 2097168, 16777232], False)):
        huge = cell('huge', deltas)
        rows.append([title, bold(huge) if emphasis else huge, cell('4k', deltas)])
    return table(['D − 2²⁴', 'huge pages', '4 KB pages'], rows)


@generated('bandwidth')
def bandwidth():
    """Case A out of cache (session fb8ca7f), ns per nonzero."""
    c, cx = session(ROWSHAPE, 'step3', 'kernels', 'thp'), session(ROWSHAPE, 'step3', 'kernels-cxlr', 'thp')
    N = 262144
    def v(impl): return f2(value(c, 'A_rank1', impl, N))
    return table(['', 'ns per nonzero'], [
        ['library, by rows', bold(v('library'))],
        ['library, one diagonal at a time', v('library_byDiagonals')],
        ['hand-written hoisted loop', v('hoisted')],
        ['hand-written row-blocked loop: one diagonal at a time per block of 1024 rows', v('rowBlocked')],
        ['Eigen sparse (with `-fcx-limited-range`)', f"{v('eigen_sparse')} ({f2(value(cx, 'A_rank1', 'eigen_sparse', N))})"]])


@generated('slices')
def slices():
    """Case D: a term applied slice by slice in memory order (sessions fb8ca7f and caseD4k), ns per nonzero."""
    cases = ('D_n32_axis0', 'D_n30_axis0', 'D_n32_axis2', 'D_n30_axis2')
    thp, thpX = session(ROWSHAPE, 'step3', 'kernels', 'thp'), session(ROWSHAPE, 'step3', 'kernels-cxlr', 'thp')
    k4, k4X = session(CASE_D, 'e90ea79', 'kernels.D', '4k'), session(CASE_D, 'e90ea79', 'kernels-cxlr.D', '4k')
    def plain(c, impl): return [f2(value(c, k, impl)) for k in cases]
    def library(c):  # in bold where at least 5% faster than the term CSR
        return [bold(f2(value(c, k, 'library_memoryOrder'))) if value(c, k, 'library_memoryOrder') < 0.95 * value(c, k, 'termCSR_memoryOrder')
                else f2(value(c, k, 'library_memoryOrder')) for k in cases]
    def withFlag(c, cx, impl): return [f'{f2(value(c, k, impl))} ({f2(value(cx, k, impl))})' for k in cases]
    return table(['', 'n=32, axis 0', 'n=30, axis 0', 'n=32, axis 2', 'n=30, axis 2'], [
        ['library `operator()`'] + library(thp),
        ['library, one diagonal at a time'] + plain(thp, 'library_byDiagonals_memoryOrder'),
        ['library, by rows'] + plain(thp, 'library_byRows_memoryOrder'),
        ['hand-written, one diagonal at a time'] + plain(thp, 'diagonalwise_memoryOrder'),
        ['hand-written, all diagonals per output row (scalar)'] + plain(thp, 'rowwise_memoryOrder'),
        ['term CSR'] + withFlag(thp, thpX, 'termCSR_memoryOrder'),
        ['CSR of the whole system'] + withFlag(thp, thpX, 'eigen_sparse_kron'),
        ['library `operator()`, 4 KB pages'] + library(k4),
        ['term CSR, 4 KB pages'] + withFlag(k4, k4X, 'termCSR_memoryOrder'),
        ['CSR of the whole system, 4 KB pages'] + withFlag(k4, k4X, 'eigen_sparse_kron')])


@generated('picture')
def picture():
    """picture1: time relative to the library's application without a picture, N = 256 / 4096 / 262144 (session b5)."""
    c = session(B5, 'b5', 'main.picture1+constructJC+slices3', '4k')
    def row(title, impl): return [title, slash((speedup(c, 'picture1', impl, 'multidiagonal_static', N) for N in (256, 4096, 262144)), sig2)]
    return table(['variant', 'relative time'], [
        row('library: the operator updated with one exponential per diagonal, then `operator()`', 'multidiagonal_scalarfreq'),
        row('library: updated with one exponential per element, then `operator()`', 'multidiagonal_tabfreq'),
        row('hand-written: one exponential per diagonal, fused into the application loop', 'handwritten_scalarfreq'),
        row('hand-written: one exponential per element, fused', 'handwritten_tabfreq'),
        row("Eigen: the CSR values updated with one exponential per nonzero, then Eigen's product", 'eigen_sparse_pernnz'),
        row('hand-written CSR loop with one exponential per nonzero fused', 'csr_pernnz'),
        row("the state transformed around a static Eigen product (C++QED doesn't do this)", 'eigen_sparse_sandwich')])


@generated('slices3')
def slices3():
    """slices3: the term CSR's time over the library's (session b5)."""
    c = session(B5, 'b5', 'main.picture1+constructJC+slices3', '4k')
    sizes = (16, 30, 32, 64, 100)
    return table(['term CSR ÷ library, application', 'n = 16'] + [str(n) for n in sizes[1:]], [
        [f'axis {axis}'] + [f2(speedup(c, f'slices3_axis{axis}', 'eigen_sparse_term', 'multidiagonal', n)) for n in sizes] for axis in (0, 2)])


@generated('composition')
def composition():
    """Composition and construction: Eigen's time over the library's (session ref, build now)."""
    c = session(REF, 'now', 'main', 'thp')
    def row(title, bench): return [title] + [sig2(speedup(c, bench, 'eigen_sparse', 'multidiagonal', N)) for N in (16, 1024, 262144)]
    return table(['', 'N = 16', '1024', '262144'], [
        row('`compose1`: rank-1 product of two tridiagonals', 'compose1'),
        row('`composeJC`: Jaynes–Cummings squared', 'composeJC'),
        row('`construct1`: rank-1, five diagonals', 'construct1'),
        row('`constructJC`: Jaynes–Cummings via ⊗, + and scalars', 'constructJC')])


@generated('assessment')
def assessment():
    """Where the library stands (session of 2026-11-23 on the reference machine)."""
    main, mainThp = session(TODAY, '134ddcf', 'main', '4k'), session(TODAY, '134ddcf', 'main', 'thp')
    k, kThp = session(TODAY, '134ddcf', 'kernels', '4k'), session(TODAY, '134ddcf', 'kernels', 'thp')
    inCache, outOfCache = (64, 256, 1024, 4096, 16384, 65536), (262144, 1048576)
    def r(c, bench, sizes): return [speedup(c, bench, 'eigen_sparse', 'multidiagonal', N) for N in sizes]
    def x(values, fmt=f1): return span(values, fmt) + '×'
    def d(c, case): return speedup(c, case, 'termCSR_memoryOrder', 'library_memoryOrder')
    def pictureCost(num, den): return span([speedup(main, f'slices3_axis{a}', num, den, n) for a in (0, 2) for n in (16, 30, 32, 64, 100)])
    out = r(main, 'apply1', outOfCache) + r(main, 'applyJC_modefast', outOfCache)
    parity = 'contiguous at parity on huge pages' if 0.9 < d(kThp, 'D_n32_axis0') < 1.1 else f"contiguous {f1(d(kThp, 'D_n32_axis0'))}× on huge pages"
    return table(['', 'today', 'with the remaining fixes'], [
        ['rank 1, in cache', f"{x(r(main, 'apply1', inCache))} faster than Eigen ({f1(r(main, 'apply1', (16,))[0])}× at N = 16)", 'same'],
        ['several axes, in cache, long axis 0 (mode first)', f"{x(r(main, 'applyJC_modefast', inCache))} faster ({f1(r(main, 'applyJC_modefast', (16,))[0])}× at N = 16; v0.4.0: 3.4–4× slower)", 'same'],
        ['several axes, in cache, short axis 0 (qubit first)', f"{x(r(main, 'applyJC_qbitfast', inCache))} faster (v0.4.0: 3.6–4.5× slower)", 'same'],
        ['operator as large as the state, out of cache, rank 1 or mode first', f'{x(out)} faster (4 KB pages)', 'same'],
        ['operator as large as the state, out of cache, qubit first',
         f"{x(r(main, 'applyJC_qbitfast', outOfCache), f2)} (4 KB pages; {x(r(mainThp, 'applyJC_qbitfast', outOfCache))} on huge pages)", 'not yet known'],
        ['slice by slice, n = 30, contiguous / strided', f"{f1(d(k, 'D_n30_axis0'))}× / {f1(d(k, 'D_n30_axis2'))}× faster than term CSR (4 KB pages)", 'same'],
        ['slice by slice, n = 32, contiguous / strided', f"{f1(d(k, 'D_n32_axis0'))}× / {f1(d(k, 'D_n32_axis2'))}× faster (4 KB pages; {parity})", 'same'],
        ['interaction picture, slice by slice: (update + application) ÷ application',
         f"{span([speedup(main, f'slices3_axis{a}', p, 'multidiagonal', n) for a in (0, 2) for n in (16, 30, 32, 64, 100) for p in ('multidiagonal_picture_linear', 'multidiagonal_picture_quadratic')])}"
         f" (term CSR, one exponential per nonzero: {pictureCost('eigen_sparse_term_picture_pernnz', 'eigen_sparse_term')})", 'same']])


# ── the page ────────────────────────────────────────────────────────────────

BLOCK = re.compile(r'(?ms)^<!-- table (?P<name>[a-z0-9-]+) -->\n(?P<body>.*?)^<!-- end table -->\n')

def rewritten(text):
    seen = []
    def replace(match):
        name = match['name']
        if name not in TABLES: sys.exit(f'tables.py: performance.md has a table "{name}" that is not defined here')
        seen.append(name)
        return f'<!-- table {name} -->\n\n' + '\n'.join(TABLES[name]()) + '\n\n<!-- end table -->\n'
    new = BLOCK.sub(replace, text)
    missing = [name for name in TABLES if name not in seen]
    if missing: sys.exit(f'tables.py: performance.md has no place for: {", ".join(missing)}')
    return new


if __name__ == '__main__':
    arguments = sys.argv[1:]
    if arguments in (['--write'], ['--check']):
        with open(PAGE, encoding='utf-8') as stream: text = stream.read()
        new = rewritten(text)
        if arguments == ['--write']:
            with open(PAGE, 'w', encoding='utf-8') as stream: stream.write(new)
            print(f'{len(TABLES)} tables written to {os.path.relpath(PAGE)}' + ('' if new != text else ' (no change)'))
        elif new != text:
            old, now = text.split('\n'), new.split('\n')
            for i, (a, b) in enumerate(zip(old, now), 1):
                if a != b:
                    print(f'performance.md, line {i}, differs from the raw results:\n  page:      {a}\n  generated: {b}')
                    break
            sys.exit('tables.py: performance.md is not what benchmarks/tables.py --write gives')
        else: print(f'{len(TABLES)} tables of performance.md agree with the raw results')
    elif arguments and arguments[0].startswith('-'): sys.exit(__doc__)
    else:
        for name in arguments or TABLES:
            if name not in TABLES: sys.exit(f'tables.py: no table "{name}"; known: {", ".join(TABLES)}')
            print(f'<!-- table {name} -->\n\n' + '\n'.join(TABLES[name]()) + '\n\n<!-- end table -->\n')
