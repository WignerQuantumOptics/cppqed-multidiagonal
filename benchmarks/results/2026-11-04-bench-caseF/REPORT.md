# Session 2026-11-04-bench-caseF

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| caseF | kernels.F | thp | 5 | 96 | 0.1% | 1.4% | 14.6% | 1 | 1.000–1.004 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## caseF: `kernels.F`, pages as the system hands them out (5 processes)

### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.66 | 0.975 | 0.793 | 0.705 | 0.611 | 0.556 | 0.535 | 0.558 | 0.671 |
| `library_byRows` | 2.93 | 1.58 | 1.18 | 0.990 | 0.800 | 0.703 | 0.621 | 0.596 | 0.559 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.80 | 2.76 | 0.947 | 2.60 | 0.806 | 2.48 | 0.753 | 2.44 | 2.47 |
| `library_byRows` | 2.99 | 2.25 | 1.40 | 1.91 | 1.48 | 1.76 | 1.65 | 1.58* | 1.45 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.26 | 1.15 | 1.04 | 0.997 | 0.934 | 0.913 | 0.893 | 0.884 | 0.865 | 0.891 |
| `library_byRows` | 3.29 | 2.06 | 1.54 | 1.30 | 1.05 | 0.935 | 0.837 | 0.789 | 0.717 | 0.648 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.16 | 2.10 | 1.31 | 1.27 | 1.22 | 1.21 | 1.18 | 1.18 | 1.18 | 1.21 |
| `library_byRows` | 3.47 | 2.48 | 1.67 | 1.44 | 1.23 | 1.12 | 1.22 | 1.28 | 1.22 | 1.03 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.92 | 2.84 | 2.37 | 2.63 | 2.16 | 2.55 | 2.14 | 2.53 | 2.46 | 2.53 |
| `library_byRows` | 3.61 | 2.61 | 1.98 | 1.97 | 1.77 | 1.72 | 1.87 | 2.16 | 2.03 | 1.84 |


