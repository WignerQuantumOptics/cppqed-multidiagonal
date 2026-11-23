#!/usr/bin/env python3
# Part of the cppqed-multidiagonal benchmark suite. BSL-1.0.
"""Reduces the raw results of sessions run with benchmarks/run.sh to Markdown tables on stdout.

usage:
  benchmarks/reduce.py report SESSION
      Everything the session measured, size by size: the median over the processes of every value, each
      implementation's time relative to the library's, and how well the processes agree. Kept beside the
      raw results as SESSION/REPORT.md, so that a figure quoted from a session can be looked up.

  benchmarks/reduce.py ratios [--pair KEY:SLOWER/FASTER ...] NAME=SESSION[/LABEL][:PAGES] ...
      The time of one implementation over that of another, with the sessions side by side: the tables of
      performance.md that compare the library with Eigen. KEY is a benchmark of the main suite or a case
      of kernels and may hold wildcards (slices3_axis*, D_*). Without --pair: the library against Eigen's
      sparse product (apply1, applyJC), and against a CSR matrix of the term for slices (slices3, case D).
      LABEL is needed where a session holds several builds, PAGES (thp or 4k) where it ran on both.

A value is the median over the processes of its configuration (the rROUND files of one build, suite and
page mode). The main suite's values are times per application, those of kernels nanoseconds per nonzero.
A star marks a value whose processes span more than 10% of their median: such values usually depend on
where the arrays lie in memory (performance.md, "Placement of the arrays").

Only the standard library is needed.
"""
import collections, csv, fnmatch, math, os, re, statistics, sys

RESULT_FILE = re.compile(r'^(?P<suite>.+)-(?P<pages>thp|4k)-r(?P<round>\d+)\.csv$')
PAGES = {'thp': 'pages as the system hands them out', '4k': '4 KB pages'}
WIDE = 0.10

DEFAULT_PAIRS = ['apply1:eigen_sparse/multidiagonal',
                 'applyJC_modefast:eigen_sparse/multidiagonal',
                 'applyJC_qbitfast:eigen_sparse/multidiagonal',
                 'slices3_axis*:eigen_sparse_term/multidiagonal',
                 'D_*:termCSR_memoryOrder/library_memoryOrder']


def die(message):
    sys.exit(f'reduce.py: {message}')


class Configuration:
    """The processes of one build, suite and page mode: values[(name, impl, N)] lists one figure per process."""

    def __init__(self, suite, pages):
        self.suite, self.pages, self.kind, self.processes = suite, pages, None, 0
        self.values = collections.defaultdict(list)

    def read(self, path):
        with open(path, newline='') as stream:
            reader = csv.DictReader(stream)
            fields = reader.fieldnames or []
            if 'benchmark' in fields: kind, name, value = 'main', 'benchmark', 'time_per_op_ns'
            elif 'case' in fields: kind, name, value = 'kernels', 'case', 'ns_per_nnz'
            else:
                print(f'reduce.py: {path} holds no results, skipped', file=sys.stderr)
                return
            if self.kind not in (None, kind): die(f'{path}: not the same kind of file as the other rounds')
            self.kind = kind
            for row in reader:
                self.values[(row[name], row['impl'], int(row['N']))].append(float(row[value]))
        self.processes += 1

    def median(self, key):
        return statistics.median(self.values[key]) if key in self.values else None

    def wide(self, key):
        v = self.values[key]
        return (max(v) - min(v)) / statistics.median(v) > WIDE

    def names(self):
        return list(dict.fromkeys(k[0] for k in self.values))

    def impls(self, name):
        return list(dict.fromkeys(k[1] for k in self.values if k[0] == name))

    def sizes(self, name):
        return sorted({k[2] for k in self.values if k[0] == name})


def readLabel(directory):
    """{(suite, pages): Configuration} of one build's results."""
    files = []
    for f in os.listdir(directory):
        m = RESULT_FILE.match(f)
        if m: files.append((m['suite'], m['pages'], int(m['round']), f))
    configurations = {}
    for suite, pages, _, f in sorted(files, key=lambda x: (x[0], x[1], x[2])):
        configurations.setdefault((suite, pages), Configuration(suite, pages)).read(os.path.join(directory, f))
    return {k: c for k, c in configurations.items() if c.processes}


def labelsOf(session):
    return sorted(d for d in os.listdir(session)
                  if os.path.isdir(os.path.join(session, d)) and any(RESULT_FILE.match(f) for f in os.listdir(os.path.join(session, d))))


def suiteOrder(key):
    suite, pages = key
    base = suite.split('.')[0]
    return (['main', 'kernels', 'kernels-cxlr'].index(base) if base in ('main', 'kernels', 'kernels-cxlr') else 9, suite, pages != 'thp')


# ── formatting ──────────────────────────────────────────────────────────────

def digits(x):
    return f'{x:.0f}' if x >= 100 else f'{x:.1f}' if x >= 10 else f'{x:.2f}'

def time(ns):
    for unit, scale in (('ns', 1), ('µs', 1e3), ('ms', 1e6), ('s', 1e9)):
        if ns < 999.5 * scale or unit == 's': return f'{digits(ns / scale)} {unit}'

def perNonzero(ns):
    return f'{ns:.3f}' if ns < 1 else digits(ns)

def ratio(x):
    return digits(x)

def star(text, wide):
    return text + ('*' if wide else '')

def table(header, rows):
    out = ['| ' + ' | '.join(header) + ' |', '|' + '---|' * len(header)]
    out += ['| ' + ' | '.join(row) + ' |' for row in rows]
    return '\n'.join(out) + '\n'

def oriented(header, rows, widest=16):
    """The table of an implementation per row, turned on its side where it has too many columns (a scan)."""
    if len(header) - 1 <= widest: return table(header, rows)
    return table([header[0]] + [row[0] for row in rows], [[header[j]] + [row[j] for row in rows] for j in range(1, len(header))])

def percent(x):
    return f'{100 * x:.1f}%'


# ── report ──────────────────────────────────────────────────────────────────

def steadiness(c):
    medians = {k: statistics.median(v) for k, v in c.values.items()}
    deviations = sorted(abs(x / medians[k] - 1) for k, v in c.values.items() for x in v)
    quantile = lambda p: deviations[min(len(deviations) - 1, int(p * len(deviations)))]
    complete = [k for k, v in c.values.items() if len(v) == c.processes]
    rounds = [math.exp(statistics.fmean(math.log(c.values[k][r] / medians[k]) for k in complete)) for r in range(c.processes)]
    return [str(c.processes), str(len(c.values)), percent(quantile(.5)), percent(quantile(.9)), percent(deviations[-1]),
            str(sum(c.wide(k) for k in c.values)), f'{min(rounds):.3f}–{max(rounds):.3f}']


def valueTable(c, names, columns, cell, first):
    """Rows: the implementations of the names; columns: sizes (one name) or names (one size each)."""
    impls = list(dict.fromkeys(i for n in names for i in c.impls(n)))
    rows = []
    for impl in impls:
        cells = []
        for name, N in columns:
            key = (name, impl, N)
            cells.append(star(cell(c.median(key)), c.wide(key)) if key in c.values else '')
        rows.append([f'`{impl}`'] + cells)
    return oriented(first, rows)


def relativeTable(c, names, columns, baseline, first):
    impls = [i for i in dict.fromkeys(i for n in names for i in c.impls(n)) if i != baseline]
    rows = []
    for impl in impls:
        cells = []
        for name, N in columns:
            a, b = c.median((name, impl, N)), c.median((name, baseline, N))
            cells.append(star(ratio(a / b), c.wide((name, impl, N)) or c.wide((name, baseline, N))) if a and b else '')
        if any(cells): rows.append([f'`{impl}`'] + cells)
    return oriented(first, rows) if rows else ''


def baselineOf(c, names):
    impls = {i for n in names for i in c.impls(n)}
    for candidate in ('multidiagonal', 'library', 'library_memoryOrder'):
        if candidate in impls: return candidate
    return None


def section(c, title, names, columns, header):
    cell = time if c.kind == 'main' else perNonzero
    unit = 'time per application' if c.kind == 'main' else 'ns per nonzero'
    out = f'### {title}\n\n' + valueTable(c, names, columns, cell, [unit] + header)
    baseline = baselineOf(c, names)
    if baseline:
        relative = relativeTable(c, names, columns, baseline, [f'time ÷ that of `{baseline}`'] + header)
        if relative: out += '\n' + relative
    return out + '\n'


def report(session):
    session = session.rstrip('/')
    labels = labelsOf(session)
    if not labels: die(f'{session}: no results of benchmarks/run.sh found')
    results = {label: readLabel(os.path.join(session, label)) for label in labels}

    print(f'# Session {os.path.basename(os.path.abspath(session))}\n')
    print('Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.\n')
    print('- A value is the median over the processes of its configuration.')
    print('- The main suite\'s values are times per application, those of `kernels` nanoseconds per nonzero.')
    print('- The second table of a section gives each implementation\'s time over that of the library: above 1 the library is faster.')
    print(f'- A star marks a value whose processes span more than {WIDE:.0%} of their median.\n')
    for name in ('machine.txt', 'builds.txt'):
        path = os.path.join(session, name)
        if os.path.exists(path):
            with open(path) as stream: print(f'## {name}\n\n```\n{stream.read().rstrip()}\n```\n')

    print('## How well the processes agree\n')
    rows = [[label, c.suite, c.pages] + steadiness(c)
            for label in labels for _, c in sorted(results[label].items(), key=lambda kc: suiteOrder(kc[0]))]
    print(table(['build', 'suite', 'pages', 'processes', 'values', 'deviation from the median: half of the figures below', '90% below',
                 'largest', f'values spanning more than {WIDE:.0%}', 'rounds'], rows))
    print('A figure is one process\'s result for one value, and its deviation is taken from the median over the processes. '
          'The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: '
          'a drift of the machine during the session shows there.\n')

    for label in labels:
        for _, c in sorted(results[label].items(), key=lambda kc: suiteOrder(kc[0])):
            print(f'## {label}: `{c.suite}`, {PAGES[c.pages]} ({c.processes} processes)\n')
            if c.kind == 'main':
                for name in c.names():
                    sizes = c.sizes(name)
                    print(section(c, f'`{name}`', [name], [(name, N) for N in sizes], [str(N) for N in sizes]))
            else:
                families = collections.defaultdict(list)
                for name in c.names(): families[name.split('_')[0]].append(name)
                for family, names in families.items():
                    if all(len(c.sizes(n)) == 1 for n in names) and len(names) > 1:
                        print(section(c, f'cases `{family}_…`', names, [(n, c.sizes(n)[0]) for n in names], [f'`{n}`' for n in names]))
                    else:
                        for name in names:
                            sizes = c.sizes(name)
                            print(section(c, f'case `{name}`', [name], [(name, N) for N in sizes], [str(N) for N in sizes]))


# ── ratios ──────────────────────────────────────────────────────────────────

def resolve(argument):
    """NAME=SESSION[/LABEL][:PAGES] → (name, {(suite, pages): Configuration} restricted to one page mode, pages)."""
    if '=' not in argument: die(f"expected NAME=SESSION[/LABEL][:PAGES], got '{argument}'")
    name, path = argument.split('=', 1)
    pages = None
    if path.rsplit(':', 1)[-1] in PAGES: path, pages = path.rsplit(':', 1)
    path = path.rstrip('/')
    if not os.path.isdir(path): die(f'{path}: no such directory')
    labels = labelsOf(path)  # empty where the path is a build's directory itself
    if len(labels) > 1: die(f'{path} holds the builds {", ".join(labels)}: name one as {path}/LABEL')
    if labels: path = os.path.join(path, labels[0])
    configurations = readLabel(path)
    if not configurations: die(f'{path}: no results of benchmarks/run.sh found')
    modes = sorted({p for _, p in configurations})
    if pages is None:
        if len(modes) != 1: die(f'{path} ran on {" and ".join(modes)}: choose with :thp or :4k')
        pages = modes[0]
    elif pages not in modes: die(f'{path} has no runs on {pages}')
    return name, {s: c for (s, p), c in configurations.items() if p == pages}, pages


def ratios(arguments):
    pairs, sessions = [], []
    while arguments:
        a = arguments.pop(0)
        if a == '--pair':
            if not arguments: die('--pair needs KEY:SLOWER/FASTER')
            pairs.append(arguments.pop(0))
        else: sessions.append(resolve(a))
    if not sessions: die('no session given')

    print('Generated by `benchmarks/reduce.py ratios`; medians over the processes; a star marks a ratio one of whose two '
          f'values spans more than {WIDE:.0%} of its median over the processes.\n')
    for name, _, pages in sessions: print(f'- {name}: {PAGES[pages]}')
    print()

    for pair in pairs or DEFAULT_PAIRS:
        m = re.fullmatch(r'([^:]+):([^/]+)/([^/]+)', pair)
        if not m: die(f"--pair: expected KEY:SLOWER/FASTER, got '{pair}'")
        pattern, slower, faster = m.groups()
        rows = []  # (key, suite, session name, {N: text})
        for name, suites, _ in sessions:
            for suite, c in sorted(suites.items(), key=lambda sc: suiteOrder((sc[0], 'thp'))):
                for key in c.names():
                    if not fnmatch.fnmatchcase(key, pattern): continue
                    cells = {}
                    for N in c.sizes(key):
                        a, b = c.median((key, slower, N)), c.median((key, faster, N))
                        if a and b: cells[N] = star(ratio(a / b), c.wide((key, slower, N)) or c.wide((key, faster, N)))
                    if cells: rows.append((key, suite, name, cells))
        if not rows:
            print(f'reduce.py: nothing matches {pair}', file=sys.stderr)
            continue
        print(f'**`{slower}` time ÷ `{faster}` time, `{pattern}`**\n')
        severalSuites = len({r[1] for r in rows}) > 1
        if all(len(r[3]) == 1 for r in rows):  # one size per key: the keys become the columns
            keys = list(dict.fromkeys(r[0] for r in rows))
            lines = collections.defaultdict(dict)
            for key, suite, name, cells in rows: lines[(suite, name)][key] = next(iter(cells.values()))
            print(table([''] + [f'`{k}`' for k in keys],
                        [[(f'`{suite}`, ' if severalSuites else '') + name] + [line.get(k, '') for k in keys] for (suite, name), line in lines.items()]))
        else:
            sizes = sorted({N for r in rows for N in r[3]})
            order = list(dict.fromkeys(r[0] for r in rows))
            rows.sort(key=lambda r: (order.index(r[0]), r[1]))  # stable: the sessions keep the order given
            print(table([''] + [str(N) for N in sizes],
                        [[f'`{key}`, ' + (f'`{suite}`, ' if severalSuites else '') + name] + [cells.get(N, '') for N in sizes] for key, suite, name, cells in rows]))


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == 'report': report(sys.argv[2])
    elif len(sys.argv) >= 3 and sys.argv[1] == 'ratios': ratios(sys.argv[2:])
    else: sys.exit(__doc__)
