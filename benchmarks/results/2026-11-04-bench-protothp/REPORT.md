# Session 2026-11-04-bench-protothp

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| base | main.apply1+applyJC | thp | 5 | 59 | 0.1% | 2.5% | 68.2% | 3 | 0.998–1.003 |
| base | main.apply1+applyJC | 4k | 5 | 59 | 0.1% | 1.2% | 44.9% | 4 | 0.999–1.015 |
| base | kernels.B+C+D+F | thp | 5 | 167 | 0.2% | 2.0% | 30.1% | 15 | 0.998–1.016 |
| base | kernels.B+C+D+F | 4k | 5 | 167 | 0.0% | 1.1% | 41.0% | 7 | 1.000–1.010 |
| proto | main.apply1+applyJC | thp | 5 | 59 | 0.2% | 1.7% | 11.2% | 2 | 0.998–1.003 |
| proto | kernels.B+C+D+F | thp | 5 | 167 | 0.2% | 1.8% | 16.1% | 3 | 0.999–1.002 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## base: `main.apply1+applyJC`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.4 ns | 181 ns | 694 ns | 3.15 µs | 14.6 µs | 58.9 µs* | 409 µs* | 1.49 ms | 8.99 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.29 µs | 9.74 µs | 40.5 µs | 162 µs | 698 µs | 3.46 ms | 12.5 ms |
| `eigen_dense` | 87.2 ns | 1.28 µs | 27.1 µs | 788 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.85 | 3.07 | 3.31 | 3.10 | 2.78 | 2.74* | 1.71* | 2.31 | 1.39 |
| `eigen_dense` | 1.22 | 7.06 | 39.1 | 250 | 912 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 175 ns | 486 ns | 2.04 µs | 9.78 µs | 44.2 µs | 185 µs | 2.20 ms | 6.97 ms | 35.6 ms |
| `eigen_sparse` | 230 ns | 928 ns | 3.91 µs | 15.8 µs | 65.6 µs | 263 µs | 1.26 ms | 5.42 ms | 20.4 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.32 | 1.91 | 1.92 | 1.61 | 1.49 | 1.42 | 0.57 | 0.78 | 0.57 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 126 ns | 291 ns | 1.22 µs | 4.91 µs* | 25.3 µs | 100 µs | 824 µs | 3.01 ms | 13.8 ms |
| `eigen_sparse` | 225 ns | 928 ns | 3.89 µs | 16.7 µs | 68.2 µs | 272 µs | 1.75 ms | 5.46 ms | 22.6 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.80 | 3.19 | 3.18 | 3.41* | 2.69 | 2.71 | 2.13 | 1.82 | 1.64 |


## base: `main.apply1+applyJC`, 4 KB pages (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.4 ns | 173 ns | 692 ns* | 3.09 µs | 14.6 µs | 59.0 µs | 254 µs | 1.39 ms | 6.31 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.29 µs | 9.74 µs | 40.5 µs | 162 µs | 713 µs | 3.17 ms | 12.7 ms |
| `eigen_dense` | 87.3 ns | 1.28 µs | 27.4 µs | 834 µs | 13.4 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.85 | 3.21 | 3.31* | 3.15 | 2.78 | 2.75 | 2.81 | 2.28 | 2.01 |
| `eigen_dense` | 1.22 | 7.37 | 39.6* | 270 | 916 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 174 ns | 487 ns | 2.04 µs* | 9.82 µs* | 43.8 µs | 190 µs | 924 µs | 5.08 ms | 23.0 ms |
| `eigen_sparse` | 230 ns | 928 ns | 3.95 µs | 15.8 µs | 66.2 µs | 265 µs | 1.27 ms | 5.44 ms | 20.6 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.32 | 1.91 | 1.94* | 1.61* | 1.51 | 1.40 | 1.37 | 1.07 | 0.89 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 127 ns | 291 ns | 1.22 µs* | 4.94 µs | 26.0 µs | 102 µs | 504 µs | 2.69 ms | 9.70 ms |
| `eigen_sparse` | 225 ns | 928 ns | 3.89 µs | 16.8 µs | 68.3 µs | 273 µs | 1.33 ms | 5.56 ms | 22.5 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.78 | 3.19 | 3.19* | 3.40 | 2.63 | 2.68 | 2.63 | 2.07 | 2.32 |


## base: `kernels.B+C+D+F`, pages as the system hands them out (5 processes)

### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.596 | 0.639 | 1.45* |
| `library_byDiagonals` | 0.636 | 0.917 | 2.85* |
| `library_byRows` | 0.594 | 0.638 | 1.45* |
| `hoisted` | 0.629 | 0.918 | 2.85* |
| `eigen_sparse` | 2.14 | 2.22 | 4.27* |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.07 | 1.44 | 1.97* |
| `library_byRows` | 1.00 | 1.00 | 1.00* |
| `hoisted` | 1.06 | 1.44 | 1.97* |
| `eigen_sparse` | 3.59 | 3.47 | 2.94* |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 3.00 | 1.95 |
| `blocked` | 3.98 | 1.59 |
| `library_materialized` | 2.40 | 1.63 |
| `library_materialized_byDiagonals` | 3.27 | 2.50 |
| `library_materialized_byRows` | 2.40 | 1.63 |
| `eigen_sparse_kron` | 2.73 | 2.51 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 3.22 | 4.85 | 1.03 | 3.51 |
| `diagonalwise_sliceIteratorOrder` | 3.07 | 7.79 | 0.932 | 2.74 |
| `rowwise_sliceIteratorOrder` | 3.26 | 8.00 | 3.13 | 6.09 |
| `termCSR_sliceIteratorOrder` | 3.41 | 8.07 | 2.85 | 6.17 |
| `library_memoryOrder` | 2.56 | 2.36 | 0.887 | 1.57 |
| `diagonalwise_memoryOrder` | 2.48 | 5.96 | 0.793 | 1.56 |
| `rowwise_memoryOrder` | 2.60 | 3.97 | 2.63 | 2.91 |
| `termCSR_memoryOrder` | 2.81 | 4.15 | 2.28 | 2.60 |
| `library_byDiagonals_memoryOrder` | 2.55 | 6.02 | 0.876 | 1.68 |
| `library_byRows_memoryOrder` | 1.84 | 2.33 | 1.24 | 1.54 |
| `eigen_sparse_kron` | 2.75 | 2.60 | 2.65 | 2.61 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.26 | 2.06 | 1.16 | 2.24 |
| `diagonalwise_sliceIteratorOrder` | 1.20 | 3.30 | 1.05 | 1.75 |
| `rowwise_sliceIteratorOrder` | 1.27 | 3.39 | 3.53 | 3.88 |
| `termCSR_sliceIteratorOrder` | 1.33 | 3.42 | 3.21 | 3.93 |
| `diagonalwise_memoryOrder` | 0.97 | 2.53 | 0.89 | 0.99 |
| `rowwise_memoryOrder` | 1.02 | 1.68 | 2.97 | 1.85 |
| `termCSR_memoryOrder` | 1.10 | 1.76 | 2.57 | 1.66 |
| `library_byDiagonals_memoryOrder` | 1.00 | 2.55 | 0.99 | 1.07 |
| `library_byRows_memoryOrder` | 0.72 | 0.99 | 1.40 | 0.98 |
| `eigen_sparse_kron` | 1.07 | 1.10 | 2.99 | 1.66 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.67 | 1.02 | 0.797 | 0.750 | 0.622 | 0.618 | 0.545 | 0.600 | 0.622 |
| `library_byRows` | 2.88 | 1.60 | 1.17 | 1.02 | 0.791 | 0.753 | 0.616 | 0.619 | 0.578* |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.79* | 2.29* | 0.944 | 2.31* | 0.800 | 2.24* | 0.758 | 2.22* | 2.22* |
| `library_byRows` | 3.13 | 2.21* | 1.39 | 1.76* | 1.47 | 2.18 | 1.57 | 2.01 | 1.34* |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.28 | 1.15 | 1.04 | 1.01 | 0.958 | 0.939 | 0.915 | 0.926 | 0.890 | 0.921 |
| `library_byRows` | 3.13 | 1.99 | 1.50 | 1.25 | 1.01 | 0.912 | 0.822 | 0.786 | 0.704 | 0.683 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.16 | 2.10 | 1.35 | 1.94 | 1.26 | 1.85 | 1.18 | 1.82 | 1.83 | 1.83 |
| `library_byRows` | 3.34 | 2.50 | 1.65 | 1.79 | 1.21 | 1.42 | 1.19 | 1.52 | 1.44 | 1.16 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.93 | 2.80 | 2.26 | 2.61 | 2.16 | 2.51 | 2.14 | 2.52 | 2.49 | 2.52 |
| `library_byRows` | 3.49 | 2.55 | 1.94 | 1.95 | 1.76 | 1.89 | 1.86 | 2.29 | 2.15 | 1.54 |


## base: `kernels.B+C+D+F`, 4 KB pages (5 processes)

### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.596 | 0.639 | 1.10 |
| `library_byDiagonals` | 0.639 | 0.889 | 1.96 |
| `library_byRows` | 0.593 | 0.642 | 1.10 |
| `hoisted` | 0.627 | 0.888 | 1.96 |
| `eigen_sparse` | 2.14 | 2.23 | 2.79 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.07 | 1.39 | 1.78 |
| `library_byRows` | 0.99 | 1.00 | 1.00 |
| `hoisted` | 1.05 | 1.39 | 1.78 |
| `eigen_sparse` | 3.59 | 3.49 | 2.54 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 1.60 | 1.48 |
| `blocked` | 2.98 | 1.00 |
| `library_materialized` | 2.09 | 1.39 |
| `library_materialized_byDiagonals` | 2.20 | 2.21 |
| `library_materialized_byRows` | 2.09 | 1.40 |
| `eigen_sparse_kron` | 2.46 | 2.59 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.44 | 5.46 | 1.15 | 4.29 |
| `diagonalwise_sliceIteratorOrder` | 1.39 | 8.01 | 1.06 | 3.59 |
| `rowwise_sliceIteratorOrder` | 3.21 | 8.92 | 3.19 | 6.90 |
| `termCSR_sliceIteratorOrder` | 2.93 | 9.04 | 2.92 | 6.99 |
| `library_memoryOrder` | 0.863 | 2.45 | 0.897 | 1.65 |
| `diagonalwise_memoryOrder` | 0.804 | 5.78 | 0.798 | 1.47 |
| `rowwise_memoryOrder` | 2.61 | 3.95 | 2.64 | 2.99 |
| `termCSR_memoryOrder` | 2.26 | 4.10 | 2.29 | 2.70 |
| `library_byDiagonals_memoryOrder` | 0.850 | 5.89 | 0.888 | 1.70 |
| `library_byRows_memoryOrder` | 1.26 | 2.41 | 1.25 | 1.62 |
| `eigen_sparse_kron` | 2.65 | 2.66 | 2.69 | 2.68 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.67 | 2.23 | 1.28 | 2.60 |
| `diagonalwise_sliceIteratorOrder` | 1.61 | 3.27 | 1.18 | 2.18 |
| `rowwise_sliceIteratorOrder` | 3.72 | 3.64 | 3.56 | 4.18 |
| `termCSR_sliceIteratorOrder` | 3.40 | 3.69 | 3.26 | 4.24 |
| `diagonalwise_memoryOrder` | 0.93 | 2.36 | 0.89 | 0.89 |
| `rowwise_memoryOrder` | 3.02 | 1.61 | 2.94 | 1.81 |
| `termCSR_memoryOrder` | 2.62 | 1.67 | 2.55 | 1.64 |
| `library_byDiagonals_memoryOrder` | 0.98 | 2.40 | 0.99 | 1.03 |
| `library_byRows_memoryOrder` | 1.46 | 0.98 | 1.39 | 0.98 |
| `eigen_sparse_kron` | 3.07 | 1.09 | 3.00 | 1.62 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.67 | 1.02* | 0.800 | 0.752* | 0.623 | 0.617* | 0.546 | 0.610* | 0.624* |
| `library_byRows` | 2.91 | 1.63 | 1.16 | 1.02 | 0.790 | 0.756 | 0.617 | 0.620* | 0.579* |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.80 | 1.12 | 0.946 | 0.857 | 0.801 | 0.779 | 0.760 | 0.782 | 0.813 |
| `library_byRows` | 2.97 | 1.78 | 1.38 | 1.27 | 1.44 | 1.81 | 1.51 | 1.70 | 0.772 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.26 | 1.13 | 1.04 | 1.00 | 0.947 | 0.927 | 0.907 | 0.895 | 0.874 | 0.912 |
| `library_byRows` | 3.13 | 1.99 | 1.48 | 1.25 | 1.01 | 0.911 | 0.822 | 0.782 | 0.704 | 0.683 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.52 | 1.41 | 1.33 | 1.29 | 1.24 | 1.22 | 1.20 | 1.20 | 1.20 | 1.23 |
| `library_byRows` | 3.28 | 2.14 | 1.65 | 1.42 | 1.27 | 1.19 | 1.27 | 1.45 | 1.37 | 0.931 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.56 | 2.40 | 2.31 | 2.27 | 2.19 | 2.18 | 2.18 | 2.17 | 2.11 | 2.18 |
| `library_byRows` | 3.44 | 2.30 | 1.95 | 1.71 | 1.77 | 1.72 | 1.87 | 2.11 | 2.00 | 1.39 |


## proto: `main.apply1+applyJC`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 72.5 ns | 176 ns | 731 ns | 3.13 µs | 12.4 µs | 47.5 µs | 214 µs | 1.28 ms | 7.89 ms |
| `eigen_sparse` | 157 ns | 658 ns | 2.64 µs | 10.6 µs | 43.1 µs | 172 µs | 738 µs | 3.27 ms | 14.9 ms |
| `eigen_dense` | 87.7 ns | 1.27 µs* | 27.3 µs | 785 µs* | 13.4 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 2.17 | 3.74 | 3.61 | 3.40 | 3.48 | 3.63 | 3.45 | 2.56 | 1.89 |
| `eigen_dense` | 1.21 | 7.25* | 37.4 | 251* | 1082 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 174 ns | 486 ns | 2.04 µs | 9.80 µs | 40.6 µs | 168 µs | 847 µs | 5.12 ms | 22.7 ms |
| `eigen_sparse` | 230 ns | 925 ns | 3.93 µs | 15.9 µs | 66.2 µs | 260 µs | 1.42 ms | 5.58 ms | 23.3 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.32 | 1.90 | 1.93 | 1.62 | 1.63 | 1.55 | 1.68 | 1.09 | 1.03 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 161 ns | 295 ns | 1.22 µs | 4.92 µs | 21.4 µs | 80.6 µs | 476 µs | 2.35 ms | 10.4 ms |
| `eigen_sparse` | 225 ns | 933 ns | 3.90 µs | 16.7 µs | 67.3 µs | 270 µs | 1.74 ms | 5.37 ms | 28.5 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.40 | 3.16 | 3.18 | 3.40 | 3.15 | 3.35 | 3.67 | 2.28 | 2.74 |


## proto: `kernels.B+C+D+F`, pages as the system hands them out (5 processes)

### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.548 | 0.563 | 1.19 |
| `library_byDiagonals` | 0.589 | 0.848 | 1.95 |
| `library_byRows` | 0.546 | 0.561 | 1.19 |
| `hoisted` | 0.579 | 0.846 | 1.95 |
| `eigen_sparse` | 2.15 | 2.22 | 3.22* |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.07 | 1.51 | 1.64 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.06 | 1.50 | 1.64 |
| `eigen_sparse` | 3.92 | 3.94 | 2.71* |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 1.66 | 1.47 |
| `blocked` | 2.97 | 0.918 |
| `library_materialized` | 2.07 | 1.30 |
| `library_materialized_byDiagonals` | 2.13 | 2.08 |
| `library_materialized_byRows` | 2.07 | 1.30 |
| `eigen_sparse_kron` | 2.73 | 2.53 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.44 | 4.65 | 1.02 | 3.52 |
| `diagonalwise_sliceIteratorOrder` | 1.33 | 7.33 | 0.905 | 2.76 |
| `rowwise_sliceIteratorOrder` | 3.29 | 7.87 | 3.13 | 5.98 |
| `termCSR_sliceIteratorOrder` | 2.99 | 7.97 | 2.85 | 6.18 |
| `library_memoryOrder` | 0.872 | 2.24 | 0.888 | 1.57 |
| `diagonalwise_memoryOrder` | 0.788 | 5.63 | 0.790 | 1.57 |
| `rowwise_memoryOrder` | 2.62 | 3.67 | 2.63 | 2.83 |
| `termCSR_memoryOrder` | 2.27 | 3.89 | 2.28 | 2.62 |
| `library_byDiagonals_memoryOrder` | 0.865 | 5.60 | 0.880 | 1.68 |
| `library_byRows_memoryOrder` | 1.25 | 2.19 | 1.25 | 1.53 |
| `eigen_sparse_kron` | 2.92 | 2.56 | 2.46 | 2.48 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.65 | 2.08 | 1.15 | 2.24 |
| `diagonalwise_sliceIteratorOrder` | 1.53 | 3.27 | 1.02 | 1.76 |
| `rowwise_sliceIteratorOrder` | 3.77 | 3.51 | 3.52 | 3.81 |
| `termCSR_sliceIteratorOrder` | 3.43 | 3.56 | 3.21 | 3.94 |
| `diagonalwise_memoryOrder` | 0.90 | 2.51 | 0.89 | 1.00 |
| `rowwise_memoryOrder` | 3.00 | 1.64 | 2.96 | 1.80 |
| `termCSR_memoryOrder` | 2.60 | 1.74 | 2.57 | 1.67 |
| `library_byDiagonals_memoryOrder` | 0.99 | 2.50 | 0.99 | 1.07 |
| `library_byRows_memoryOrder` | 1.43 | 0.98 | 1.41 | 0.97 |
| `eigen_sparse_kron` | 3.35 | 1.14 | 2.77 | 1.58 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.66 | 1.02 | 0.798 | 0.749 | 0.623 | 0.627 | 0.535 | 0.579 | 0.681 |
| `library_byRows` | 2.97 | 1.62 | 1.19 | 1.03 | 0.807 | 0.758 | 0.620 | 0.624 | 0.594 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.74 | 1.14 | 0.941 | 0.863 | 0.823 | 0.776 | 2.21* | 0.723 | 0.769 |
| `library_byRows` | 3.08 | 1.81 | 1.42 | 1.27 | 1.45 | 1.87 | 1.94 | 1.71* | 0.766 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.23 | 1.10 | 1.01 | 0.956 | 0.927 | 0.904 | 0.890 | 0.909 | 0.877 | 0.848 |
| `library_byRows` | 3.36 | 2.08 | 1.55 | 1.30 | 1.05 | 0.932 | 0.837 | 0.813 | 0.713 | 0.639 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.47 | 1.36 | 1.26 | 1.23 | 1.21 | 1.19 | 1.85 | 1.85 | 1.16 | 1.16 |
| `library_byRows` | 3.46 | 2.15 | 1.64 | 1.42 | 1.24 | 1.24 | 1.41 | 1.51 | 1.35 | 1.05 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.56 | 2.36 | 2.22 | 2.15 | 2.12 | 2.09 | 2.50 | 2.51 | 2.09 | 2.07 |
| `library_byRows` | 3.67 | 2.36 | 1.93 | 1.76 | 1.73 | 1.80 | 2.06 | 2.18 | 1.98 | 1.74 |


