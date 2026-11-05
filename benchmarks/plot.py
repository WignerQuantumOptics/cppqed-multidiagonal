#!/usr/bin/env python3
"""Render the cppqed-multidiagonal benchmark CSV into figures.

Usage: benchmarks/plot.py [bench.csv] [outdir]

Writes:
  benchmarks.{pdf,png}  structural benchmarks — apply / compose / construct, rank 1 and rank 2, and a
                        term applied slice by slice (slices3)
  picture.{pdf,png}     interaction-picture RHS evaluation (absolute times + speedup), and the cost of the
                        picture in the slice-by-slice setting

Panels are emitted only for benchmarks present in the CSV, so partial runs
(./benchmarkMultiDiagonal picture1 applyJC) plot without editing anything.
Repeated (benchmark,impl,N) rows — e.g. concatenated CSVs from several runs —
are reduced by median.
"""
import csv, sys, os, math, collections, statistics
import matplotlib.pyplot as plt

path   = sys.argv[1] if len(sys.argv) > 1 else '/dev/stdin'
outdir = sys.argv[2] if len(sys.argv) > 2 else '.'

# ── ingest ───────────────────────────────────────────────────────────────────

raw = collections.defaultdict(lambda: collections.defaultdict(lambda: collections.defaultdict(list)))
dvals = collections.defaultdict(set)
with open(path) as f:
    for row in csv.DictReader(f):
        raw[row['benchmark']][row['impl']][int(row['N'])].append(float(row['time_per_op_ns']) * 1e-9)
        dvals[row['benchmark']].add(int(row['d']))

data = {b: {i: sorted((N, statistics.median(ts)) for N, ts in per.items())
            for i, per in impls.items()}
        for b, impls in raw.items()}

def series(b, impl):
    return data.get(b, {}).get(impl, [])

def have(*benches):
    return any(b in data for b in benches)

allN = [N for b in data for i in data[b] for N, _ in data[b][i]]
if allN and max(allN) <= 256:
    print('WARNING: largest N is %d — this looks like --smoke output. '
          'Smoke timings are calibration noise, not measurements.' % max(allN), file=sys.stderr)
if 'applyJC' in data:
    print("WARNING: CSV contains legacy 'applyJC' rows; current runs emit "
          "'applyJC_qbitfast' / 'applyJC_modefast'. Legacy rows ignored.", file=sys.stderr)

# ── styling ──────────────────────────────────────────────────────────────────
# (label, colour, marker, linestyle)
STYLE = {
    'multidiagonal':             ('MultiDiagonal',                'C0',  'o', '-'),
    'eigen_sparse':              ('Eigen sparse',                 'C1',  's', '-'),
    'eigen_dense':               ('Eigen dense',                  'C2',  '^', '-'),
    'multidiagonal_scalarfreq':  ('MultiDiagonal, scalar freq (update + apply)',    'C0',  'o', '-'),
    'multidiagonal_tabfreq':     ('MultiDiagonal, tabulated freq (update + apply)', 'C4',  'v', '-'),
    'multidiagonal_static':      ('MultiDiagonal, no picture',                      '0.45', '', '--'),
    'handwritten_scalarfreq':    ('hand-written, scalar freq fused',                'C0',  'o', ':'),
    'handwritten_tabfreq':       ('hand-written, tabulated freq fused',             'C4',  'v', ':'),
    'eigen_sparse_pernnz':       ('Eigen sparse, exp per nonzero (update + apply)', 'C1',  's', '-'),
    'csr_pernnz':                ('hand-written CSR, exp per nonzero fused',        'C1',  's', ':'),
    'eigen_sparse_sandwich':     ('Eigen sparse, state transformed around it',      'C3',  'D', '-'),
    'eigen_sparse_term':         ('Eigen sparse of the term, on the slices',        'C1',  's', '-'),
    'multidiagonal_picture_linear':     ('MultiDiagonal, picture, linear spectrum',    'C0', 'o', '-'),
    'multidiagonal_picture_quadratic':  ('MultiDiagonal, picture, quadratic spectrum', 'C4', 'v', '-'),
    'eigen_sparse_term_picture_pernnz': ('Eigen sparse of the term, exp per nonzero',  'C1', 's', '-'),
}

def curve(ax, b, impl, label=None, **kw):
    pts = series(b, impl)
    if not pts:
        return False
    lab, c, m, ls = STYLE.get(impl, (impl, None, 'o', '-'))
    ax.loglog([p[0] for p in pts], [p[1] for p in pts],
              color=kw.pop('color', c), marker=kw.pop('marker', m),
              ls=kw.pop('ls', ls), lw=kw.pop('lw', 1.6), ms=4,
              label=label or lab, **kw)
    return True

def guide(ax, *refs, power=1, label=r'$\propto N$'):
    """O(N**power) guide anchored on the last point of the first available series."""
    pts = next((s for s in (series(*r) for r in refs) if len(s) >= 2), None)
    if pts is None:
        return
    N0, (N1, t1) = pts[0][0], pts[-1]
    ax.loglog([N0, N1], [t1 * (N0 / N1) ** power, t1], 'k--', alpha=.35, lw=1, label=label)

def finish(ax, title, xlabel='dimension $N$', ylabel='time per operation [s]'):
    ax.set_xlabel(xlabel); ax.set_ylabel(ylabel)
    ax.set_title(title, fontsize=9.5)
    ax.grid(True, which='both', alpha=.25)
    ax.legend(fontsize=7.5)

def dtag(b):
    ds = dvals.get(b, set())
    return r', $d=%d$' % ds.pop() if len(ds) == 1 else ''

# ── figure 1: structural benchmarks ──────────────────────────────────────────

def draw_simple(ax, b, title, impls=('eigen_dense', 'eigen_sparse', 'multidiagonal')):
    for impl in impls:
        curve(ax, b, impl)
    guide(ax, (b, 'multidiagonal'))
    finish(ax, title + dtag(b))

def draw_jc_apply(ax):
    for suf, layout, ls, mk in (('modefast', 'mode fast', '-', 'o'),
                                ('qbitfast', 'qubit fast', '--', 'x')):
        b = 'applyJC_' + suf
        for impl in ('multidiagonal', 'eigen_sparse'):
            lab = STYLE[impl][0]
            curve(ax, b, impl, label='%s, %s' % (lab, layout), ls=ls, marker=mk)
    guide(ax, ('applyJC_modefast', 'multidiagonal'), ('applyJC_qbitfast', 'multidiagonal'))
    finish(ax, 'application, Jaynes–Cummings (rank 2)\nboth axis orders',
           xlabel='mode dimension $N$  (total dimension $2N$)')

def draw_slices(ax):
    for axis, ls, mk in (('0', '-', 'o'), ('2', '--', 'x')):
        b = 'slices3_axis' + axis
        for impl in ('multidiagonal', 'eigen_sparse_term'):
            lab = 'MultiDiagonal' if impl == 'multidiagonal' else 'Eigen sparse of the term'
            curve(ax, b, impl, label='%s, axis %s' % (lab, axis), ls=ls, marker=mk)
    finish(ax, 'a term applied slice by slice\n($n^3$ state, slices in memory order)',
           xlabel='term dimension $n$', ylabel='time per application [s]')

PANELS = [
    (lambda: have('apply1'),      lambda ax: draw_simple(ax, 'apply1',
        'application  $y += (H/i)\\,\\psi$\n(rank 1, pentadiagonal)')),
    (lambda: have('compose1'),    lambda ax: draw_simple(ax, 'compose1',
        'composition  $A\\,|\\,B$\n(rank 1, tridiagonal $\\times$ tridiagonal)')),
    (lambda: have('construct1'),  lambda ax: draw_simple(ax, 'construct1',
        'construction from coefficients\n(rank 1, pentadiagonal)')),
    (lambda: have('applyJC_modefast', 'applyJC_qbitfast'), draw_jc_apply),
    (lambda: have('composeJC'),   lambda ax: draw_simple(ax, 'composeJC',
        'composition  $H_\\mathrm{JC}\\,|\\,H_\\mathrm{JC}$\n(rank 2, mode-fast)')),
    (lambda: have('constructJC'), lambda ax: draw_simple(ax, 'constructJC',
        'assembly of $H_\\mathrm{JC}$\n(rank 2, mode-fast)')),
    (lambda: have('slices3_axis0', 'slices3_axis2'), draw_slices),
]

active = [draw for present, draw in PANELS if present()]
if active:
    ncols = min(3, len(active)); nrows = math.ceil(len(active) / ncols)
    fig, axes = plt.subplots(nrows, ncols, figsize=(4.5 * ncols, 3.8 * nrows), squeeze=False)
    axes = axes.ravel()
    for ax, draw in zip(axes, active):
        draw(ax)
    for ax in axes[len(active):]:
        ax.axis('off')
    fig.tight_layout()
    for ext in ('pdf', 'png'):
        fig.savefig(os.path.join(outdir, 'benchmarks.' + ext), dpi=200)
    print('wrote benchmarks.pdf / benchmarks.png  (%d panels)' % len(active))

# ── figure 2: interaction picture ────────────────────────────────────────────

PIC_IMPLS = ('multidiagonal_static', 'multidiagonal_scalarfreq', 'multidiagonal_tabfreq',
             'handwritten_scalarfreq', 'handwritten_tabfreq',
             'eigen_sparse_pernnz', 'csr_pernnz', 'eigen_sparse_sandwich')

def ratio(b, num, den):
    a, c = dict(series(b, num)), dict(series(b, den))
    return [(N, a[N] / c[N]) for N in sorted(set(a) & set(c))]

slicesPicture = have('slices3_axis0', 'slices3_axis2')
if 'picture1' in data or slicesPicture:
    fig, axes = plt.subplots(1, 3 if slicesPicture else 2, figsize=(14.4 if slicesPicture else 9.6, 3.9))
    axA, axB = axes[0], axes[1]

    for impl in PIC_IMPLS:
        curve(axA, 'picture1', impl,
              lw=2.4 if impl == 'multidiagonal_scalarfreq' else 1.6)
    guide(axA, ('picture1', 'multidiagonal_scalarfreq'))
    finish(axA, 'interaction-picture RHS evaluation\n(rank 1, pentadiagonal, harmonic $\\omega_k = k\\omega$)',
           ylabel='time per RHS evaluation [s]')

    for num, den, lab, col, mk in (('eigen_sparse_pernnz', 'multidiagonal_scalarfreq', 'vs. Eigen, exp per nonzero', 'C1', 's'),
                                   ('eigen_sparse_sandwich', 'multidiagonal_scalarfreq', 'vs. state transformed around Eigen', 'C3', 'D'),
                                   ('multidiagonal_scalarfreq', 'multidiagonal_static', 'cost of the picture\n(scalar freq / static)',
                                                                                        '0.45', 'o')):
        pts = ratio('picture1', num, den)
        if pts:
            axB.semilogx([p[0] for p in pts], [p[1] for p in pts],
                         color=col, marker=mk, ms=4, lw=1.6, label=lab)
    axB.axhline(1., color='k', ls=':', lw=1, alpha=.6)
    axB.set_yscale('log')
    finish(axB, 'one exp per diagonal against the sparse baselines,\nand the cost of the picture',
           ylabel='time ratio')

    if slicesPicture:
        axC = axes[2]
        for axis, ls in (('0', '-'), ('2', '--')):
            b = 'slices3_axis' + axis
            for num, den in (('multidiagonal_picture_linear', 'multidiagonal'),
                             ('multidiagonal_picture_quadratic', 'multidiagonal'),
                             ('eigen_sparse_term_picture_pernnz', 'eigen_sparse_term')):
                pts = ratio(b, num, den)
                if pts:
                    lab, col, mk, _ = STYLE[num]
                    axC.semilogx([p[0] for p in pts], [p[1] for p in pts], color=col, marker=mk, ms=4, lw=1.6, ls=ls,
                                 label='%s, axis %s' % (lab, axis))
        axC.axhline(1., color='k', ls=':', lw=1, alpha=.6)
        finish(axC, 'cost of the picture, slice by slice\n(update + apply over static apply)',
               xlabel='term dimension $n$', ylabel='time ratio')

    fig.tight_layout()
    for ext in ('pdf', 'png'):
        fig.savefig(os.path.join(outdir, 'picture.' + ext), dpi=200)
    print('wrote picture.pdf / picture.png')

# ── headline numbers for the text ────────────────────────────────────────────

def at_max_N(b, num, den):
    pts = ratio(b, num, den)
    return pts[-1] if pts else None

print('\n-- ratios at the largest common N --')
for b, num, den in (('apply1',           'eigen_sparse',           'multidiagonal'),
                    ('compose1',         'eigen_sparse',           'multidiagonal'),
                    ('construct1',       'eigen_sparse',           'multidiagonal'),
                    ('applyJC_modefast', 'eigen_sparse',           'multidiagonal'),
                    ('applyJC_qbitfast', 'eigen_sparse',           'multidiagonal'),
                    ('composeJC',        'eigen_sparse',           'multidiagonal'),
                    ('constructJC',      'eigen_sparse',           'multidiagonal'),
                    ('picture1',         'eigen_sparse_pernnz',    'multidiagonal_scalarfreq'),
                    ('picture1',         'eigen_sparse_sandwich',  'multidiagonal_scalarfreq'),
                    ('picture1',         'multidiagonal_scalarfreq', 'multidiagonal_static'),
                    ('picture1',         'multidiagonal_scalarfreq', 'handwritten_scalarfreq'),
                    ('slices3_axis0',    'eigen_sparse_term',      'multidiagonal'),
                    ('slices3_axis2',    'eigen_sparse_term',      'multidiagonal'),
                    ('slices3_axis2',    'multidiagonal_picture_quadratic', 'multidiagonal'),
                    ('slices3_axis2',    'eigen_sparse_term_picture_pernnz', 'eigen_sparse_term')):
    r = at_max_N(b, num, den)
    if r:
        print('  %-18s %-26s / %-26s  N=%-8d %6.2f x' % (b, num, den, r[0], r[1]))

# layout sensitivity — the axis-order claim, isolated
mf, qf = dict(series('applyJC_modefast', 'multidiagonal')), dict(series('applyJC_qbitfast', 'multidiagonal'))
common = sorted(set(mf) & set(qf))
if common:
    N = common[-1]
    print('  %-18s MultiDiagonal qubit-fast / mode-fast          N=%-8d %6.2f x'
          % ('applyJC', N, qf[N] / mf[N]))