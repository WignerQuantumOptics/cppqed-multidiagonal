# Session 2026-11-04-bench-protothp2

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| base | main.apply1+applyJC | thp | 5 | 59 | 0.1% | 1.9% | 71.2% | 4 | 0.997–1.019 |
| base | main.apply1+applyJC | 4k | 5 | 59 | 0.1% | 1.7% | 40.7% | 3 | 1.000–1.010 |
| base | kernels.B+C+D+F | thp | 5 | 167 | 0.2% | 2.7% | 25.8% | 13 | 0.998–1.009 |
| base | kernels.B+C+D+F | 4k | 5 | 167 | 0.0% | 1.4% | 53.8% | 4 | 0.999–1.017 |
| proto2 | main.apply1+applyJC | thp | 5 | 59 | 0.1% | 1.2% | 21.0% | 3 | 0.999–1.006 |
| proto2 | kernels.B+C+D+F | thp | 5 | 167 | 0.1% | 1.4% | 5.2% | 0 | 1.000–1.001 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## base: `main.apply1+applyJC`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.4 ns | 173 ns | 694 ns | 3.08 µs | 14.6 µs | 59.0 µs | 254 µs* | 1.49 ms | 9.17 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.29 µs | 9.79 µs | 40.5 µs | 162 µs | 692 µs | 3.46 ms | 12.5 ms |
| `eigen_dense` | 87.2 ns | 1.28 µs | 26.5 µs | 776 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.85 | 3.21 | 3.30 | 3.18 | 2.78 | 2.75 | 2.72* | 2.33 | 1.36 |
| `eigen_dense` | 1.22 | 7.37 | 38.2 | 252 | 914 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 174 ns | 485 ns | 2.04 µs | 10.6 µs* | 44.0 µs | 184 µs | 2.15 ms | 6.99 ms | 35.5 ms |
| `eigen_sparse` | 230 ns | 928 ns | 3.92 µs | 15.7 µs | 65.7 µs | 262 µs | 1.26 ms | 5.43 ms | 20.5 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.33 | 1.92 | 1.92 | 1.48* | 1.49 | 1.42 | 0.59 | 0.78 | 0.58 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 125 ns | 288 ns | 1.22 µs | 4.92 µs* | 25.4 µs | 101 µs | 696 µs* | 2.97 ms | 13.9 ms |
| `eigen_sparse` | 225 ns | 928 ns | 3.89 µs | 16.7 µs | 68.3 µs | 269 µs | 1.75 ms | 5.45 ms | 22.6 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.80 | 3.23 | 3.19 | 3.40* | 2.69 | 2.66 | 2.52* | 1.84 | 1.63 |


## base: `main.apply1+applyJC`, 4 KB pages (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.3 ns | 173 ns | 693 ns | 3.08 µs* | 14.6 µs* | 58.3 µs | 252 µs | 1.39 ms | 6.30 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.29 µs | 9.75 µs | 40.5 µs | 162 µs | 715 µs | 3.18 ms | 12.7 ms |
| `eigen_dense` | 87.3 ns | 1.28 µs | 27.5 µs | 814 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.85 | 3.21 | 3.31 | 3.17* | 2.77* | 2.77 | 2.84 | 2.29 | 2.01 |
| `eigen_dense` | 1.22 | 7.37 | 39.6 | 264* | 914* |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 174 ns | 486 ns | 2.04 µs* | 9.93 µs | 44.4 µs | 185 µs | 931 µs | 5.09 ms | 23.1 ms |
| `eigen_sparse` | 230 ns | 928 ns | 3.95 µs | 15.9 µs | 66.3 µs | 264 µs | 1.27 ms | 5.43 ms | 20.5 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.33 | 1.91 | 1.94* | 1.60 | 1.49 | 1.42 | 1.37 | 1.07 | 0.89 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 125 ns | 290 ns | 1.22 µs | 4.90 µs | 25.6 µs | 101 µs | 509 µs | 2.70 ms | 9.70 ms |
| `eigen_sparse` | 225 ns | 928 ns | 3.89 µs | 17.0 µs | 68.3 µs | 273 µs | 1.33 ms | 5.57 ms | 22.4 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.80 | 3.21 | 3.18 | 3.47 | 2.67 | 2.72 | 2.61 | 2.06 | 2.31 |


## base: `kernels.B+C+D+F`, pages as the system hands them out (5 processes)

### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.597 | 0.638 | 1.47* |
| `library_byDiagonals` | 0.639 | 0.901 | 2.83* |
| `library_byRows` | 0.594 | 0.640 | 1.47* |
| `hoisted` | 0.630 | 0.900 | 2.83* |
| `eigen_sparse` | 2.14 | 2.22 | 4.26* |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.07 | 1.41 | 1.93* |
| `library_byRows` | 0.99 | 1.00 | 1.00* |
| `hoisted` | 1.06 | 1.41 | 1.93* |
| `eigen_sparse` | 3.58 | 3.48 | 2.90* |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 3.01 | 1.98 |
| `blocked` | 3.98 | 1.58 |
| `library_materialized` | 2.40 | 1.63 |
| `library_materialized_byDiagonals` | 3.29 | 2.52 |
| `library_materialized_byRows` | 2.40 | 1.63 |
| `eigen_sparse_kron` | 2.73 | 2.52 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 3.22 | 4.82 | 1.04 | 3.51 |
| `diagonalwise_sliceIteratorOrder` | 3.09 | 7.76 | 0.939 | 2.75 |
| `rowwise_sliceIteratorOrder` | 3.26 | 7.98 | 3.13 | 6.09 |
| `termCSR_sliceIteratorOrder` | 3.42 | 8.06 | 2.85 | 6.18 |
| `library_memoryOrder` | 2.56 | 2.38 | 0.887 | 1.57 |
| `diagonalwise_memoryOrder` | 2.48 | 5.98 | 0.794 | 1.54 |
| `rowwise_memoryOrder` | 2.60 | 3.97 | 2.63 | 2.92 |
| `termCSR_memoryOrder` | 2.81 | 4.13 | 2.28 | 2.61 |
| `library_byDiagonals_memoryOrder` | 2.55 | 6.03 | 0.879 | 1.70 |
| `library_byRows_memoryOrder` | 1.84 | 2.33 | 1.24 | 1.54 |
| `eigen_sparse_kron` | 2.76 | 2.60 | 2.65 | 2.61 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.26 | 2.03 | 1.17 | 2.24 |
| `diagonalwise_sliceIteratorOrder` | 1.21 | 3.26 | 1.06 | 1.75 |
| `rowwise_sliceIteratorOrder` | 1.27 | 3.35 | 3.53 | 3.88 |
| `termCSR_sliceIteratorOrder` | 1.34 | 3.39 | 3.21 | 3.94 |
| `diagonalwise_memoryOrder` | 0.97 | 2.51 | 0.90 | 0.98 |
| `rowwise_memoryOrder` | 1.02 | 1.67 | 2.97 | 1.86 |
| `termCSR_memoryOrder` | 1.10 | 1.74 | 2.57 | 1.66 |
| `library_byDiagonals_memoryOrder` | 1.00 | 2.53 | 0.99 | 1.08 |
| `library_byRows_memoryOrder` | 0.72 | 0.98 | 1.40 | 0.98 |
| `eigen_sparse_kron` | 1.08 | 1.09 | 2.99 | 1.66 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.67 | 1.02 | 0.799 | 0.751 | 0.625 | 0.618 | 0.545 | 0.600 | 0.622 |
| `library_byRows` | 2.88 | 1.60 | 1.17 | 1.02 | 0.800 | 0.749 | 0.618 | 0.619 | 0.579 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.79 | 2.52* | 0.949 | 2.31* | 0.806 | 2.24* | 0.763 | 2.21* | 2.22* |
| `library_byRows` | 3.17 | 2.25 | 1.39 | 1.78* | 1.49 | 2.20 | 1.59 | 2.05 | 1.34* |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.30 | 1.15 | 1.04 | 1.01 | 0.963 | 0.940 | 0.917 | 0.909 | 0.890 | 0.920 |
| `library_byRows` | 3.13 | 1.95* | 1.50 | 1.25 | 1.01 | 0.912 | 0.816 | 0.779 | 0.701 | 0.686 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.15 | 2.10 | 1.34 | 1.94 | 1.26 | 1.85 | 1.18 | 1.82 | 1.83 | 1.84 |
| `library_byRows` | 3.34 | 2.50 | 1.65 | 1.79 | 1.21 | 1.41 | 1.20 | 1.51 | 1.42 | 1.16 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.88 | 2.84 | 2.30 | 2.68 | 2.17 | 2.54 | 2.14 | 2.50 | 2.47 | 2.50 |
| `library_byRows` | 3.49 | 2.55 | 1.94 | 1.96 | 1.76 | 1.91 | 1.85 | 2.32 | 2.17 | 1.54 |


## base: `kernels.B+C+D+F`, 4 KB pages (5 processes)

### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.596* | 0.638 | 1.10 |
| `library_byDiagonals` | 0.643* | 0.890 | 1.96 |
| `library_byRows` | 0.593* | 0.638 | 1.10 |
| `hoisted` | 0.630* | 0.890 | 1.97 |
| `eigen_sparse` | 2.14 | 2.22 | 2.79 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.08* | 1.39 | 1.78 |
| `library_byRows` | 0.99* | 1.00 | 1.00 |
| `hoisted` | 1.06* | 1.39 | 1.79 |
| `eigen_sparse` | 3.59* | 3.48 | 2.54 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 1.60 | 1.49 |
| `blocked` | 2.98 | 1.00 |
| `library_materialized` | 2.09 | 1.39 |
| `library_materialized_byDiagonals` | 2.19 | 2.21 |
| `library_materialized_byRows` | 2.09 | 1.39 |
| `eigen_sparse_kron` | 2.46 | 2.59 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.43 | 5.43 | 1.15 | 4.29 |
| `diagonalwise_sliceIteratorOrder` | 1.36 | 7.92 | 1.06 | 3.57 |
| `rowwise_sliceIteratorOrder` | 3.21 | 8.90 | 3.19 | 6.90 |
| `termCSR_sliceIteratorOrder` | 2.93 | 9.02 | 2.92 | 6.99 |
| `library_memoryOrder` | 0.861 | 2.44 | 0.896 | 1.65 |
| `diagonalwise_memoryOrder` | 0.799 | 5.67 | 0.799 | 1.47 |
| `rowwise_memoryOrder` | 2.61 | 3.88 | 2.64 | 2.99 |
| `termCSR_memoryOrder` | 2.26 | 4.03 | 2.29 | 2.70 |
| `library_byDiagonals_memoryOrder` | 0.851 | 5.76 | 0.887 | 1.68 |
| `library_byRows_memoryOrder` | 1.26 | 2.40 | 1.25 | 1.62 |
| `eigen_sparse_kron` | 2.65 | 2.66 | 2.69 | 2.68 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.66 | 2.23 | 1.28 | 2.60 |
| `diagonalwise_sliceIteratorOrder` | 1.58 | 3.25 | 1.18 | 2.16 |
| `rowwise_sliceIteratorOrder` | 3.73 | 3.65 | 3.56 | 4.18 |
| `termCSR_sliceIteratorOrder` | 3.40 | 3.70 | 3.26 | 4.24 |
| `diagonalwise_memoryOrder` | 0.93 | 2.32 | 0.89 | 0.89 |
| `rowwise_memoryOrder` | 3.03 | 1.59 | 2.95 | 1.81 |
| `termCSR_memoryOrder` | 2.62 | 1.65 | 2.56 | 1.64 |
| `library_byDiagonals_memoryOrder` | 0.99 | 2.36 | 0.99 | 1.02 |
| `library_byRows_memoryOrder` | 1.46 | 0.98 | 1.40 | 0.98 |
| `eigen_sparse_kron` | 3.08 | 1.09 | 3.00 | 1.62 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.66 | 1.02 | 0.800 | 0.751 | 0.624 | 0.619 | 0.545 | 0.600 | 0.624 |
| `library_byRows` | 2.89 | 1.61 | 1.18 | 1.02 | 0.794 | 0.752 | 0.618 | 0.620 | 0.579 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.79 | 1.12 | 0.945 | 0.856 | 0.801 | 0.774 | 0.757 | 0.771 | 0.807 |
| `library_byRows` | 2.97 | 1.78 | 1.38 | 1.27 | 1.43 | 1.81 | 1.51 | 1.71 | 0.772 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.27 | 1.13 | 1.02 | 0.982 | 0.935 | 0.913 | 0.892 | 0.888 | 0.866 | 0.899 |
| `library_byRows` | 3.13 | 1.99 | 1.50 | 1.25 | 1.01 | 0.911 | 0.822 | 0.783 | 0.705 | 0.685 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.52 | 1.40 | 1.33 | 1.29 | 1.24 | 1.22 | 1.20 | 1.20 | 1.20 | 1.23 |
| `library_byRows` | 3.27 | 2.17 | 1.66 | 1.42 | 1.27 | 1.19 | 1.28 | 1.45 | 1.37 | 0.928 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.56 | 2.39 | 2.30 | 2.27 | 2.19 | 2.18 | 2.18 | 2.17 | 2.11 | 2.17 |
| `library_byRows` | 3.44 | 2.30 | 1.95 | 1.72 | 1.77 | 1.72 | 1.87 | 2.11 | 2.00 | 1.39 |


## proto2: `main.apply1+applyJC`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 72.5 ns | 175 ns | 730 ns | 3.07 µs | 11.5 µs | 41.8 µs | 197 µs* | 1.29 ms | 5.43 ms |
| `eigen_sparse` | 157 ns | 658 ns | 2.64 µs | 10.7 µs | 43.3 µs | 173 µs | 740 µs | 3.30 ms | 15.0 ms |
| `eigen_dense` | 87.3 ns | 1.27 µs | 27.8 µs | 765 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 2.16 | 3.77 | 3.61 | 3.49 | 3.75 | 4.14 | 3.76* | 2.55 | 2.76 |
| `eigen_dense` | 1.20 | 7.29 | 38.1 | 249 | 1153 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 174 ns | 485 ns | 2.04 µs | 9.84 µs* | 39.4 µs | 166 µs | 837 µs | 5.13 ms | 23.5 ms |
| `eigen_sparse` | 230 ns | 925 ns | 3.93 µs | 16.0 µs | 66.6 µs | 261 µs | 1.44 ms | 5.77 ms | 23.3 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.32 | 1.91 | 1.93 | 1.62* | 1.69 | 1.57 | 1.72 | 1.12 | 0.99 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 159 ns | 295 ns | 1.22 µs | 4.99 µs | 19.1 µs | 76.3 µs | 490 µs | 2.46 ms | 10.6 ms* |
| `eigen_sparse` | 225 ns | 929 ns | 3.89 µs | 16.8 µs | 67.9 µs | 270 µs | 1.76 ms | 5.58 ms | 28.6 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.41 | 3.15 | 3.18 | 3.37 | 3.56 | 3.54 | 3.59 | 2.27 | 2.69* |


## proto2: `kernels.B+C+D+F`, pages as the system hands them out (5 processes)

### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.542 | 0.583 | 1.17 |
| `library_byDiagonals` | 0.590 | 0.804 | 1.92 |
| `library_byRows` | 0.539 | 0.584 | 1.17 |
| `hoisted` | 0.580 | 0.803 | 1.92 |
| `eigen_sparse` | 2.16 | 2.22 | 2.69 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.09 | 1.38 | 1.64 |
| `library_byRows` | 0.99 | 1.00 | 1.00 |
| `hoisted` | 1.07 | 1.38 | 1.64 |
| `eigen_sparse` | 3.99 | 3.81 | 2.30 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 1.69 | 1.47 |
| `blocked` | 2.97 | 0.916 |
| `library_materialized` | 1.97 | 1.31 |
| `library_materialized_byDiagonals` | 2.11 | 2.06 |
| `library_materialized_byRows` | 1.96 | 1.31 |
| `eigen_sparse_kron` | 2.73 | 2.51 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.37 | 4.61 | 1.01 | 3.17 |
| `diagonalwise_sliceIteratorOrder` | 1.26 | 7.25 | 0.882 | 2.73 |
| `rowwise_sliceIteratorOrder` | 3.15 | 7.79 | 3.17 | 5.33 |
| `termCSR_sliceIteratorOrder` | 2.88 | 7.91 | 2.88 | 5.44 |
| `library_memoryOrder` | 0.842 | 2.08 | 0.876 | 1.63 |
| `diagonalwise_memoryOrder` | 0.768 | 5.69 | 0.779 | 1.53 |
| `rowwise_memoryOrder` | 2.62 | 3.21 | 2.63 | 2.89 |
| `termCSR_memoryOrder` | 2.28 | 3.36 | 2.28 | 2.57 |
| `library_byDiagonals_memoryOrder` | 0.831 | 5.56 | 0.871 | 1.72 |
| `library_byRows_memoryOrder` | 1.26 | 2.05 | 1.30 | 1.61 |
| `eigen_sparse_kron` | 2.75 | 2.57 | 2.58 | 2.57 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.63 | 2.22 | 1.15 | 1.94 |
| `diagonalwise_sliceIteratorOrder` | 1.50 | 3.49 | 1.01 | 1.67 |
| `rowwise_sliceIteratorOrder` | 3.74 | 3.75 | 3.62 | 3.27 |
| `termCSR_sliceIteratorOrder` | 3.42 | 3.80 | 3.29 | 3.34 |
| `diagonalwise_memoryOrder` | 0.91 | 2.74 | 0.89 | 0.94 |
| `rowwise_memoryOrder` | 3.11 | 1.54 | 3.00 | 1.77 |
| `termCSR_memoryOrder` | 2.71 | 1.62 | 2.60 | 1.58 |
| `library_byDiagonals_memoryOrder` | 0.99 | 2.67 | 0.99 | 1.06 |
| `library_byRows_memoryOrder` | 1.50 | 0.99 | 1.48 | 0.99 |
| `eigen_sparse_kron` | 3.27 | 1.24 | 2.95 | 1.58 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.66 | 1.02 | 0.798 | 0.750 | 0.623 | 0.628 | 0.533 | 0.579 | 0.682 |
| `library_byRows` | 2.91 | 1.60 | 1.17 | 1.02 | 0.797 | 0.755 | 0.616 | 0.622 | 0.594 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.80 | 1.14 | 0.946 | 0.846 | 0.817 | 0.790 | 0.742 | 0.718 | 0.762 |
| `library_byRows` | 3.05 | 1.70 | 1.38 | 1.27 | 1.32 | 1.38 | 1.68 | 1.73 | 1.70 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.23 | 1.05 | 0.971 | 0.941 | 0.902 | 0.892 | 0.881 | 0.888 | 0.862 | 0.855 |
| `library_byRows` | 3.12 | 1.98 | 1.48 | 1.26 | 1.02 | 0.921 | 0.830 | 0.784 | 0.712 | 0.677 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.49 | 1.33 | 1.25 | 1.22 | 1.19 | 1.18 | 1.17 | 1.18 | 1.15 | 1.14 |
| `library_byRows` | 3.21 | 2.09 | 1.60 | 1.42 | 1.21 | 1.19 | 1.21 | 1.30 | 1.27 | 1.05 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.54 | 2.28 | 2.17 | 2.16 | 2.10 | 2.09 | 2.07 | 2.06 | 2.08 | 2.04 |
| `library_byRows` | 3.40 | 2.24 | 1.85 | 1.69 | 1.67 | 1.73 | 1.84 | 1.94 | 1.94 | 1.73 |


