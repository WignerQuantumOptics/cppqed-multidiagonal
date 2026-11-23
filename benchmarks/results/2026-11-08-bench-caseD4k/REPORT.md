# Session 2026-11-08-bench-caseD4k

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| e90ea79 | kernels.D | 4k | 5 | 44 | 0.0% | 0.6% | 2.4% | 0 | 1.000–1.001 |
| e90ea79 | kernels-cxlr.D | 4k | 5 | 44 | 0.0% | 0.5% | 2.4% | 0 | 1.000–1.001 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## e90ea79: `kernels.D`, 4 KB pages (5 processes)

### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.49 | 5.53 | 1.16 | 4.30 |
| `diagonalwise_sliceIteratorOrder` | 1.39 | 8.08 | 1.05 | 3.58 |
| `rowwise_sliceIteratorOrder` | 3.33 | 9.11 | 3.18 | 6.89 |
| `termCSR_sliceIteratorOrder` | 3.10 | 9.43 | 2.95 | 7.04 |
| `library_memoryOrder` | 0.917 | 2.42 | 0.893 | 1.66 |
| `diagonalwise_memoryOrder` | 0.829 | 5.43 | 0.799 | 1.44 |
| `rowwise_memoryOrder` | 2.61 | 3.44 | 2.64 | 2.99 |
| `termCSR_memoryOrder` | 2.30 | 3.59 | 2.33 | 2.73 |
| `library_byDiagonals_memoryOrder` | 0.911 | 5.46 | 0.890 | 1.68 |
| `library_byRows_memoryOrder` | 1.13 | 2.39 | 1.24 | 1.63 |
| `eigen_sparse_kron` | 2.55 | 2.54 | 2.57 | 2.58 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.62 | 2.29 | 1.30 | 2.59 |
| `diagonalwise_sliceIteratorOrder` | 1.52 | 3.34 | 1.18 | 2.16 |
| `rowwise_sliceIteratorOrder` | 3.63 | 3.76 | 3.56 | 4.15 |
| `termCSR_sliceIteratorOrder` | 3.38 | 3.90 | 3.30 | 4.24 |
| `diagonalwise_memoryOrder` | 0.90 | 2.24 | 0.89 | 0.87 |
| `rowwise_memoryOrder` | 2.85 | 1.42 | 2.96 | 1.80 |
| `termCSR_memoryOrder` | 2.51 | 1.48 | 2.61 | 1.64 |
| `library_byDiagonals_memoryOrder` | 0.99 | 2.26 | 1.00 | 1.01 |
| `library_byRows_memoryOrder` | 1.23 | 0.99 | 1.39 | 0.98 |
| `eigen_sparse_kron` | 2.78 | 1.05 | 2.88 | 1.55 |


## e90ea79: `kernels-cxlr.D`, 4 KB pages (5 processes)

### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.49 | 5.56 | 1.15 | 4.29 |
| `diagonalwise_sliceIteratorOrder` | 1.39 | 8.08 | 1.06 | 3.59 |
| `rowwise_sliceIteratorOrder` | 3.42 | 8.97 | 3.28 | 6.79 |
| `termCSR_sliceIteratorOrder` | 2.56 | 8.47 | 2.40 | 6.59 |
| `library_memoryOrder` | 0.917 | 2.44 | 0.895 | 1.66 |
| `diagonalwise_memoryOrder` | 0.831 | 5.42 | 0.799 | 1.44 |
| `rowwise_memoryOrder` | 2.72 | 3.30 | 2.74 | 2.93 |
| `termCSR_memoryOrder` | 1.70 | 3.19 | 1.75 | 2.33 |
| `library_byDiagonals_memoryOrder` | 0.911 | 5.46 | 0.890 | 1.68 |
| `library_byRows_memoryOrder` | 1.12 | 2.39 | 1.24 | 1.63 |
| `eigen_sparse_kron` | 2.29 | 2.38 | 2.34 | 2.39 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.62 | 2.28 | 1.28 | 2.58 |
| `diagonalwise_sliceIteratorOrder` | 1.52 | 3.31 | 1.18 | 2.16 |
| `rowwise_sliceIteratorOrder` | 3.73 | 3.68 | 3.66 | 4.09 |
| `termCSR_sliceIteratorOrder` | 2.79 | 3.47 | 2.68 | 3.97 |
| `diagonalwise_memoryOrder` | 0.91 | 2.22 | 0.89 | 0.87 |
| `rowwise_memoryOrder` | 2.97 | 1.35 | 3.06 | 1.77 |
| `termCSR_memoryOrder` | 1.85 | 1.31 | 1.96 | 1.40 |
| `library_byDiagonals_memoryOrder` | 0.99 | 2.24 | 0.99 | 1.01 |
| `library_byRows_memoryOrder` | 1.22 | 0.98 | 1.39 | 0.98 |
| `eigen_sparse_kron` | 2.50 | 0.98 | 2.61 | 1.44 |


