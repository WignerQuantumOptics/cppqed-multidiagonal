# Session 2026-11-05-bench-b5

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| b5 | main.picture1+constructJC+slices3 | 4k | 5 | 140 | 0.1% | 1.4% | 7.7% | 3 | 1.000–1.004 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## b5: `main.picture1+constructJC+slices3`, 4 KB pages (5 processes)

### `constructJC`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 3.34 µs | 10.0 µs | 36.4 µs | 147 µs | 616 µs | 2.51 ms | 21.9 ms | 90.2 ms | 372 ms |
| `eigen_sparse` | 1.90 µs | 7.15 µs | 38.1 µs | 155 µs | 646 µs | 6.41 ms | 28.0 ms | 114 ms | 517 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 0.57 | 0.71 | 1.05 | 1.05 | 1.05 | 2.56 | 1.27 | 1.26 | 1.39 |


### `picture1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal_static` | 73.1 ns | 174 ns | 714 ns | 3.08 µs | 13.2 µs | 58.0 µs | 268 µs | 1.38 ms | 6.35 ms |
| `multidiagonal_scalarfreq` | 290 ns | 652 ns | 2.42 µs | 9.09 µs | 36.6 µs | 151 µs | 786 µs | 3.76 ms | 16.1 ms |
| `multidiagonal_tabfreq` | 1.61 µs | 6.33 µs | 25.8 µs | 97.8 µs | 366 µs | 1.47 ms | 5.83 ms | 23.5 ms | 94.8 ms |
| `handwritten_scalarfreq` | 266 ns | 685 ns | 2.84 µs | 11.7 µs | 47.5 µs | 191 µs | 756 µs | 3.74 ms | 16.8 ms |
| `handwritten_tabfreq` | 1.55 µs | 6.36 µs | 26.1 µs | 97.3 µs | 347 µs | 1.40 ms | 5.38 ms | 21.9 ms | 89.0 ms |
| `eigen_sparse_pernnz` | 1.58 µs | 6.69 µs | 27.3 µs | 105 µs | 398 µs | 1.60 ms | 6.29 ms | 25.4 ms | 102 ms |
| `csr_pernnz` | 1.55 µs | 6.59 µs | 26.9 µs | 103 µs | 376 µs | 1.50 ms | 5.86 ms | 23.6 ms | 93.7 ms |
| `eigen_sparse_sandwich` | 240 ns | 903 ns | 3.74 µs | 16.1 µs | 65.4 µs | 266 µs | 1.19 ms | 5.30 ms | 21.9 ms |


### `slices3_axis0`

| time per application | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `multidiagonal` | 20.2 µs | 99.1 µs | 124 µs | 939 µs | 3.72 ms |
| `eigen_sparse_term` | 40.1 µs | 276 µs | 335 µs | 2.72 ms | 10.8 ms |
| `multidiagonal_picture_linear` | 20.3 µs | 99.9 µs | 123 µs | 943 µs | 3.71 ms |
| `multidiagonal_picture_quadratic` | 20.5 µs | 100 µs | 124 µs | 959 µs | 3.73 ms |
| `eigen_sparse_term_picture_pernnz` | 44.2 µs | 295 µs | 358 µs | 2.89 ms | 11.3 ms |

| time ÷ that of `multidiagonal` | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `eigen_sparse_term` | 1.98 | 2.78 | 2.69 | 2.89 | 2.90 |
| `multidiagonal_picture_linear` | 1.00 | 1.01 | 0.99 | 1.00 | 1.00 |
| `multidiagonal_picture_quadratic` | 1.01 | 1.01 | 0.99 | 1.02 | 1.00 |
| `eigen_sparse_term_picture_pernnz` | 2.18 | 2.98 | 2.88 | 3.08 | 3.04 |


### `slices3_axis2`

| time per application | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `multidiagonal` | 40.3 µs* | 160 µs | 275 µs | 2.45 ms | 7.63 ms |
| `eigen_sparse_term` | 44.0 µs* | 286 µs | 445 µs | 4.72 ms | 17.3 ms |
| `multidiagonal_picture_linear` | 40.7 µs* | 161 µs | 275 µs | 2.43 ms | 7.53 ms |
| `multidiagonal_picture_quadratic` | 41.0 µs | 162 µs | 275 µs | 2.44 ms | 7.52 ms |
| `eigen_sparse_term_picture_pernnz` | 45.1 µs | 297 µs | 438 µs | 4.70 ms | 16.9 ms |

| time ÷ that of `multidiagonal` | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `eigen_sparse_term` | 1.09* | 1.78 | 1.62 | 1.93 | 2.27 |
| `multidiagonal_picture_linear` | 1.01* | 1.00 | 1.00 | 0.99 | 0.99 |
| `multidiagonal_picture_quadratic` | 1.02* | 1.01 | 1.00 | 1.00 | 0.99 |
| `eigen_sparse_term_picture_pernnz` | 1.12* | 1.85 | 1.59 | 1.92 | 2.22 |


