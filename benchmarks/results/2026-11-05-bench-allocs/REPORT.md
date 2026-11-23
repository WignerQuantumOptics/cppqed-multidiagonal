# Session 2026-11-05-bench-allocs

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| base | main.apply1+applyJC | thp | 5 | 59 | 0.1% | 3.0% | 79.0% | 8 | 0.995–1.020 |
| base | main.apply1+applyJC | 4k | 5 | 59 | 0.2% | 1.1% | 25.0% | 1 | 1.000–1.003 |
| base | kernels.B+C+D+F | thp | 5 | 167 | 0.3% | 2.1% | 18.4% | 8 | 0.995–1.006 |
| base | kernels.B+C+D+F | 4k | 5 | 167 | 0.0% | 1.4% | 7.2% | 0 | 1.000–1.005 |
| jemalloc | main.apply1+applyJC | thp | 5 | 59 | 0.3% | 2.4% | 19.5% | 4 | 0.997–1.005 |
| jemalloc | kernels.B+C+D+F | thp | 5 | 167 | 0.7% | 4.5% | 68.7% | 27 | 0.992–1.011 |
| proto2 | main.apply1+applyJC | thp | 5 | 59 | 0.1% | 1.4% | 42.3% | 4 | 1.000–1.012 |
| proto2 | kernels.B+C+D+F | thp | 5 | 167 | 0.1% | 1.4% | 6.7% | 0 | 0.999–1.002 |
| tcmalloc | main.apply1+applyJC | thp | 5 | 59 | 0.1% | 1.8% | 15.2% | 4 | 0.998–1.005 |
| tcmalloc | kernels.B+C+D+F | thp | 5 | 167 | 0.4% | 4.2% | 31.4% | 19 | 0.990–1.024 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## base: `main.apply1+applyJC`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.4 ns | 173 ns | 695 ns | 3.08 µs* | 14.6 µs* | 58.2 µs | 256 µs* | 1.50 ms* | 9.15 ms* |
| `eigen_sparse` | 132 ns | 556 ns | 2.29 µs | 9.71 µs | 40.5 µs | 162 µs | 695 µs | 3.46 ms | 12.5 ms |
| `eigen_dense` | 87.2 ns | 1.28 µs* | 27.4 µs | 794 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.85 | 3.21 | 3.30 | 3.16* | 2.78* | 2.79 | 2.71* | 2.30* | 1.37* |
| `eigen_dense` | 1.22 | 7.38* | 39.4 | 258* | 913* |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 174 ns | 486 ns | 2.04 µs | 9.75 µs | 43.7 µs | 182 µs | 2.15 ms | 6.95 ms | 35.3 ms |
| `eigen_sparse` | 230 ns | 928 ns | 3.95 µs | 15.7 µs | 66.4 µs | 263 µs | 1.26 ms | 5.42 ms | 20.4 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.32 | 1.91 | 1.94 | 1.61 | 1.52 | 1.44 | 0.59 | 0.78 | 0.58 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 125 ns | 290 ns | 1.22 µs | 4.91 µs* | 25.6 µs | 101 µs | 693 µs* | 2.95 ms | 13.8 ms |
| `eigen_sparse` | 225 ns | 928 ns | 3.89 µs | 16.7 µs | 68.2 µs | 272 µs | 1.76 ms | 5.46 ms | 22.6 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.80 | 3.20 | 3.19 | 3.39* | 2.67 | 2.71 | 2.53* | 1.85 | 1.64 |


## base: `main.apply1+applyJC`, 4 KB pages (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.4 ns | 174 ns | 692 ns | 3.10 µs | 14.6 µs | 59.5 µs | 254 µs | 1.39 ms | 6.29 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.29 µs | 9.78 µs | 40.5 µs | 162 µs | 714 µs | 3.17 ms | 12.7 ms |
| `eigen_dense` | 87.3 ns | 1.28 µs | 27.4 µs | 832 µs | 13.4 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.85 | 3.20 | 3.31 | 3.15 | 2.78 | 2.72 | 2.81 | 2.28 | 2.01 |
| `eigen_dense` | 1.22 | 7.36 | 39.6 | 268 | 918 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 173 ns | 494 ns | 2.04 µs | 9.84 µs* | 44.2 µs | 191 µs | 926 µs | 5.11 ms | 23.0 ms |
| `eigen_sparse` | 230 ns | 928 ns | 3.91 µs | 15.9 µs | 65.6 µs | 263 µs | 1.26 ms | 5.43 ms | 20.6 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.33 | 1.88 | 1.92 | 1.62* | 1.49 | 1.38 | 1.37 | 1.06 | 0.89 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 126 ns | 295 ns | 1.22 µs | 4.90 µs | 25.7 µs | 102 µs | 508 µs | 2.70 ms | 9.68 ms |
| `eigen_sparse` | 225 ns | 928 ns | 3.89 µs | 16.8 µs | 68.3 µs | 273 µs | 1.33 ms | 5.56 ms | 22.4 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.79 | 3.15 | 3.18 | 3.43 | 2.66 | 2.68 | 2.61 | 2.06 | 2.31 |


## base: `kernels.B+C+D+F`, pages as the system hands them out (5 processes)

### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.595 | 0.643 | 1.49 |
| `library_byDiagonals` | 0.639 | 0.899 | 2.83 |
| `library_byRows` | 0.594 | 0.643 | 1.49 |
| `hoisted` | 0.630 | 0.899 | 2.83 |
| `eigen_sparse` | 2.14 | 2.22 | 4.25 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.07 | 1.40 | 1.90 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.06 | 1.40 | 1.90 |
| `eigen_sparse` | 3.60 | 3.45 | 2.85 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 3.02 | 1.97 |
| `blocked` | 4.00 | 1.59 |
| `library_materialized` | 2.41 | 1.63 |
| `library_materialized_byDiagonals` | 3.30 | 2.52 |
| `library_materialized_byRows` | 2.41 | 1.62 |
| `eigen_sparse_kron` | 2.73 | 2.52 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 3.21 | 4.82 | 1.03 | 3.51 |
| `diagonalwise_sliceIteratorOrder` | 3.08 | 7.76 | 0.933 | 2.75 |
| `rowwise_sliceIteratorOrder` | 3.25 | 7.95 | 3.13 | 6.10 |
| `termCSR_sliceIteratorOrder` | 3.42 | 8.04 | 2.84 | 6.20 |
| `library_memoryOrder` | 2.56 | 2.37 | 0.886 | 1.57 |
| `diagonalwise_memoryOrder` | 2.48 | 5.95 | 0.795 | 1.56 |
| `rowwise_memoryOrder` | 2.60 | 3.97 | 2.63 | 2.91 |
| `termCSR_memoryOrder` | 2.81 | 4.13 | 2.28 | 2.61 |
| `library_byDiagonals_memoryOrder` | 2.55 | 6.01 | 0.879 | 1.68 |
| `library_byRows_memoryOrder` | 1.85 | 2.33 | 1.24 | 1.54 |
| `eigen_sparse_kron` | 2.76 | 2.60 | 2.65 | 2.61 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.25 | 2.03 | 1.16 | 2.24 |
| `diagonalwise_sliceIteratorOrder` | 1.20 | 3.27 | 1.05 | 1.75 |
| `rowwise_sliceIteratorOrder` | 1.27 | 3.35 | 3.53 | 3.89 |
| `termCSR_sliceIteratorOrder` | 1.34 | 3.39 | 3.21 | 3.95 |
| `diagonalwise_memoryOrder` | 0.97 | 2.51 | 0.90 | 0.99 |
| `rowwise_memoryOrder` | 1.02 | 1.68 | 2.97 | 1.85 |
| `termCSR_memoryOrder` | 1.10 | 1.74 | 2.57 | 1.66 |
| `library_byDiagonals_memoryOrder` | 1.00 | 2.54 | 0.99 | 1.07 |
| `library_byRows_memoryOrder` | 0.72 | 0.98 | 1.40 | 0.98 |
| `eigen_sparse_kron` | 1.08 | 1.10 | 2.99 | 1.66 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.66 | 1.02 | 0.799 | 0.751 | 0.622 | 0.619 | 0.544 | 0.599 | 0.621 |
| `library_byRows` | 2.88 | 1.60 | 1.17 | 1.02 | 0.794 | 0.753 | 0.617 | 0.620 | 0.579 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.98* | 2.53* | 0.946 | 2.31* | 0.807 | 2.26* | 0.763 | 2.22* | 2.44* |
| `library_byRows` | 3.17 | 2.23 | 1.39 | 1.80* | 1.48 | 2.22 | 1.55 | 2.06 | 1.44* |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.30 | 1.16 | 1.04 | 1.00 | 0.950 | 0.925 | 0.903 | 0.902 | 0.876 | 0.901 |
| `library_byRows` | 3.13 | 1.98 | 1.49 | 1.25 | 1.01 | 0.911 | 0.821 | 0.779 | 0.704 | 0.684 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.15 | 2.09 | 1.35 | 1.93 | 1.26 | 1.85 | 1.18 | 1.82 | 1.83 | 1.83 |
| `library_byRows` | 3.35 | 2.49 | 1.65 | 1.78 | 1.21 | 1.43 | 1.19 | 1.52 | 1.44 | 1.16 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.89 | 2.84 | 2.32 | 2.60 | 2.16 | 2.56 | 2.09 | 2.52 | 2.48 | 2.54 |
| `library_byRows` | 3.49 | 2.55 | 1.95 | 1.95 | 1.76 | 1.90 | 1.85 | 2.24 | 2.17 | 1.53 |


## base: `kernels.B+C+D+F`, 4 KB pages (5 processes)

### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.598 | 0.641 | 1.11 |
| `library_byDiagonals` | 0.637 | 0.890 | 1.96 |
| `library_byRows` | 0.596 | 0.643 | 1.11 |
| `hoisted` | 0.627 | 0.891 | 1.97 |
| `eigen_sparse` | 2.14 | 2.23 | 2.79 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.07 | 1.39 | 1.77 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.05 | 1.39 | 1.77 |
| `eigen_sparse` | 3.58 | 3.48 | 2.51 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 1.60 | 1.48 |
| `blocked` | 2.99 | 1.00 |
| `library_materialized` | 2.09 | 1.39 |
| `library_materialized_byDiagonals` | 2.19 | 2.20 |
| `library_materialized_byRows` | 2.09 | 1.39 |
| `eigen_sparse_kron` | 2.46 | 2.59 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.43 | 5.47 | 1.15 | 4.28 |
| `diagonalwise_sliceIteratorOrder` | 1.38 | 7.97 | 1.06 | 3.58 |
| `rowwise_sliceIteratorOrder` | 3.21 | 8.94 | 3.19 | 6.92 |
| `termCSR_sliceIteratorOrder` | 2.93 | 9.03 | 2.91 | 7.01 |
| `library_memoryOrder` | 0.864 | 2.46 | 0.896 | 1.65 |
| `diagonalwise_memoryOrder` | 0.803 | 5.77 | 0.799 | 1.47 |
| `rowwise_memoryOrder` | 2.61 | 3.93 | 2.64 | 2.99 |
| `termCSR_memoryOrder` | 2.26 | 4.07 | 2.29 | 2.70 |
| `library_byDiagonals_memoryOrder` | 0.852 | 5.85 | 0.889 | 1.68 |
| `library_byRows_memoryOrder` | 1.26 | 2.42 | 1.25 | 1.62 |
| `eigen_sparse_kron` | 2.64 | 2.66 | 2.69 | 2.68 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.66 | 2.22 | 1.28 | 2.59 |
| `diagonalwise_sliceIteratorOrder` | 1.60 | 3.24 | 1.18 | 2.17 |
| `rowwise_sliceIteratorOrder` | 3.72 | 3.63 | 3.56 | 4.19 |
| `termCSR_sliceIteratorOrder` | 3.39 | 3.67 | 3.25 | 4.25 |
| `diagonalwise_memoryOrder` | 0.93 | 2.35 | 0.89 | 0.89 |
| `rowwise_memoryOrder` | 3.02 | 1.60 | 2.95 | 1.81 |
| `termCSR_memoryOrder` | 2.62 | 1.65 | 2.56 | 1.64 |
| `library_byDiagonals_memoryOrder` | 0.99 | 2.38 | 0.99 | 1.02 |
| `library_byRows_memoryOrder` | 1.46 | 0.98 | 1.40 | 0.98 |
| `eigen_sparse_kron` | 3.06 | 1.08 | 3.00 | 1.62 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.67 | 1.02 | 0.801 | 0.753 | 0.624 | 0.622 | 0.545 | 0.606 | 0.625 |
| `library_byRows` | 2.94 | 1.63 | 1.19 | 1.03 | 0.800 | 0.756 | 0.622 | 0.623 | 0.583 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.80 | 1.13 | 0.955 | 0.860 | 0.804 | 0.778 | 0.759 | 0.772 | 0.810 |
| `library_byRows` | 2.97 | 1.79 | 1.39 | 1.27 | 1.46 | 1.82 | 1.51 | 1.71 | 0.773 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.26 | 1.12 | 1.02 | 0.984 | 0.940 | 0.915 | 0.894 | 0.888 | 0.860 | 0.896 |
| `library_byRows` | 3.13 | 1.99 | 1.50 | 1.25 | 1.01 | 0.910 | 0.823 | 0.782 | 0.704 | 0.683 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.52 | 1.41 | 1.33 | 1.29 | 1.24 | 1.23 | 1.20 | 1.20 | 1.20 | 1.23 |
| `library_byRows` | 3.27 | 2.17 | 1.66 | 1.42 | 1.26 | 1.19 | 1.27 | 1.44 | 1.36 | 0.926 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.55 | 2.40 | 2.30 | 2.26 | 2.19 | 2.18 | 2.18 | 2.17 | 2.11 | 2.17 |
| `library_byRows` | 3.44 | 2.30 | 1.95 | 1.71 | 1.77 | 1.72 | 1.87 | 2.11 | 2.00 | 1.39 |


## jemalloc: `main.apply1+applyJC`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.5 ns | 173 ns | 634 ns | 2.45 µs | 10.5 µs | 41.6 µs | 182 µs | 1.45 ms* | 6.25 ms* |
| `eigen_sparse` | 132 ns | 559 ns | 2.31 µs | 9.74 µs | 40.1 µs | 161 µs | 694 µs | 3.16 ms | 12.7 ms |
| `eigen_dense` | 87.2 ns | 1.28 µs | 24.9 µs | 788 µs | 14.1 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.85 | 3.23 | 3.65 | 3.97 | 3.82 | 3.86 | 3.81 | 2.18* | 2.04* |
| `eigen_dense` | 1.22 | 7.36 | 39.3 | 321 | 1341 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 173 ns | 486 ns | 2.03 µs | 8.16 µs | 40.0 µs | 166 µs | 826 µs | 5.18 ms | 23.7 ms |
| `eigen_sparse` | 230 ns | 927 ns | 4.04 µs | 15.7 µs | 64.3 µs | 256 µs | 1.26 ms | 5.16 ms | 20.7 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.33 | 1.91 | 1.99 | 1.92 | 1.61 | 1.54 | 1.53 | 0.99 | 0.87 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 127 ns | 291 ns | 1.23 µs | 4.22 µs | 19.0 µs | 74.8 µs | 471 µs | 2.78 ms* | 11.7 ms* |
| `eigen_sparse` | 225 ns | 938 ns | 4.09 µs | 16.1 µs | 66.7 µs | 266 µs | 1.26 ms | 5.40 ms | 22.2 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.77 | 3.23 | 3.33 | 3.81 | 3.50 | 3.56 | 2.68 | 1.95* | 1.89* |


## jemalloc: `kernels.B+C+D+F`, pages as the system hands them out (5 processes)

### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.600 | 0.577 | 1.39* |
| `library_byDiagonals` | 0.596 | 0.807 | 1.89* |
| `library_byRows` | 0.599 | 0.576 | 1.39* |
| `hoisted` | 0.593 | 0.806 | 1.90* |
| `eigen_sparse` | 2.05 | 2.08 | 2.61 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 0.99 | 1.40 | 1.36* |
| `library_byRows` | 1.00 | 1.00 | 1.00* |
| `hoisted` | 0.99 | 1.40 | 1.37* |
| `eigen_sparse` | 3.42 | 3.60 | 1.88* |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 1.74 | 1.60 |
| `blocked` | 2.98 | 0.920 |
| `library_materialized` | 2.05 | 1.29 |
| `library_materialized_byDiagonals` | 2.17 | 2.06 |
| `library_materialized_byRows` | 2.05 | 1.29 |
| `eigen_sparse_kron` | 2.44 | 2.50 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.44 | 4.55 | 1.00 | 3.15 |
| `diagonalwise_sliceIteratorOrder` | 1.34 | 7.24 | 0.881 | 2.73 |
| `rowwise_sliceIteratorOrder` | 3.39 | 7.75 | 3.13 | 5.29 |
| `termCSR_sliceIteratorOrder` | 3.10 | 7.80 | 2.84 | 5.40 |
| `library_memoryOrder` | 0.845 | 2.13 | 0.860 | 1.60 |
| `diagonalwise_memoryOrder` | 0.758 | 5.73 | 0.768 | 1.55 |
| `rowwise_memoryOrder` | 2.63 | 3.83* | 2.63 | 2.88 |
| `termCSR_memoryOrder` | 2.29 | 3.95* | 2.29 | 2.57 |
| `library_byDiagonals_memoryOrder` | 0.843 | 5.50 | 0.854 | 1.73 |
| `library_byRows_memoryOrder` | 1.25* | 2.09 | 1.29 | 1.57 |
| `eigen_sparse_kron` | 2.63 | 2.62 | 2.65 | 2.61 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.70 | 2.14 | 1.16 | 1.97 |
| `diagonalwise_sliceIteratorOrder` | 1.59 | 3.40 | 1.02 | 1.71 |
| `rowwise_sliceIteratorOrder` | 4.01 | 3.64 | 3.64 | 3.31 |
| `termCSR_sliceIteratorOrder` | 3.67 | 3.66 | 3.30 | 3.38 |
| `diagonalwise_memoryOrder` | 0.90 | 2.69 | 0.89 | 0.97 |
| `rowwise_memoryOrder` | 3.11 | 1.80* | 3.06 | 1.80 |
| `termCSR_memoryOrder` | 2.71 | 1.85* | 2.66 | 1.61 |
| `library_byDiagonals_memoryOrder` | 1.00 | 2.58 | 0.99 | 1.08 |
| `library_byRows_memoryOrder` | 1.48* | 0.98 | 1.50 | 0.98 |
| `eigen_sparse_kron` | 3.11 | 1.23 | 3.08 | 1.63 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.63 | 0.981 | 0.790 | 0.693 | 0.603 | 0.565 | 0.516 | 0.494 | 0.540 |
| `library_byRows` | 2.93 | 1.62 | 1.20 | 0.998 | 0.805 | 0.710 | 0.616 | 0.571 | 0.514 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.80 | 1.13 | 0.928 | 0.875 | 0.812* | 0.793 | 0.748 | 0.709 | 0.740 |
| `library_byRows` | 3.08 | 1.69 | 1.45 | 1.42* | 1.60* | 1.40* | 1.75* | 1.87 | 1.74* |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.24 | 1.06 | 0.984 | 0.940 | 0.914 | 0.891 | 0.886 | 0.871 | 0.855 | 0.857 |
| `library_byRows` | 3.13 | 1.98 | 1.51 | 1.27 | 1.04 | 0.930 | 0.837 | 0.789 | 0.716 | 0.669 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.50* | 1.33 | 1.28 | 1.23 | 1.21 | 1.19 | 1.18 | 1.18 | 1.17 | 1.17 |
| `library_byRows` | 3.25 | 2.11 | 1.64 | 1.44 | 1.27 | 1.22* | 1.27 | 1.30* | 1.35 | 1.11* |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.56 | 2.32 | 2.31* | 2.24* | 2.19 | 2.16 | 2.16* | 2.14* | 2.18* | 2.08 |
| `library_byRows` | 3.52 | 2.45 | 1.98* | 1.88* | 1.75 | 1.77* | 2.02* | 2.09* | 2.05 | 1.71 |


## proto2: `main.apply1+applyJC`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 72.5 ns | 174 ns | 732 ns* | 3.09 µs | 11.5 µs | 41.8 µs | 187 µs* | 1.29 ms | 5.46 ms |
| `eigen_sparse` | 157 ns | 658 ns | 2.64 µs | 10.7 µs | 43.2 µs | 173 µs | 739 µs | 3.29 ms | 14.9 ms |
| `eigen_dense` | 87.2 ns | 1.27 µs* | 27.0 µs | 777 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 2.16 | 3.78 | 3.61* | 3.46 | 3.76 | 4.14 | 3.96* | 2.55 | 2.73 |
| `eigen_dense` | 1.20 | 7.32* | 36.9* | 251 | 1159 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 174 ns | 486 ns | 2.03 µs | 9.81 µs | 39.6 µs | 167 µs | 848 µs | 5.15 ms | 23.2 ms |
| `eigen_sparse` | 230 ns | 925 ns | 3.91 µs | 15.8 µs | 66.3 µs | 261 µs | 1.44 ms | 5.77 ms | 23.3 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.32 | 1.91 | 1.92 | 1.61 | 1.67 | 1.57 | 1.70 | 1.12 | 1.00 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 159 ns | 296 ns | 1.23 µs | 4.92 µs* | 19.1 µs | 76.3 µs | 490 µs | 2.49 ms | 10.4 ms |
| `eigen_sparse` | 225 ns | 929 ns | 3.89 µs | 16.9 µs | 67.8 µs | 271 µs | 1.76 ms | 5.57 ms | 28.5 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.41 | 3.14 | 3.17 | 3.43* | 3.56 | 3.56 | 3.59 | 2.24 | 2.73 |


## proto2: `kernels.B+C+D+F`, pages as the system hands them out (5 processes)

### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.542 | 0.584 | 1.17 |
| `library_byDiagonals` | 0.590 | 0.807 | 1.94 |
| `library_byRows` | 0.539 | 0.585 | 1.16 |
| `hoisted` | 0.578 | 0.806 | 1.94 |
| `eigen_sparse` | 2.16 | 2.25 | 2.69 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.09 | 1.38 | 1.66 |
| `library_byRows` | 0.99 | 1.00 | 0.99 |
| `hoisted` | 1.07 | 1.38 | 1.66 |
| `eigen_sparse` | 3.99 | 3.85 | 2.30 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 1.69 | 1.45 |
| `blocked` | 2.97 | 0.905 |
| `library_materialized` | 1.97 | 1.31 |
| `library_materialized_byDiagonals` | 2.11 | 2.05 |
| `library_materialized_byRows` | 1.96 | 1.31 |
| `eigen_sparse_kron` | 2.73 | 2.51 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.38 | 4.65 | 1.01 | 3.19 |
| `diagonalwise_sliceIteratorOrder` | 1.26 | 7.29 | 0.888 | 2.74 |
| `rowwise_sliceIteratorOrder` | 3.16 | 7.84 | 3.17 | 5.36 |
| `termCSR_sliceIteratorOrder` | 2.89 | 7.95 | 2.87 | 5.47 |
| `library_memoryOrder` | 0.845 | 2.11 | 0.878 | 1.63 |
| `diagonalwise_memoryOrder` | 0.769 | 5.72 | 0.780 | 1.54 |
| `rowwise_memoryOrder` | 2.62 | 3.23 | 2.63 | 2.89 |
| `termCSR_memoryOrder` | 2.28 | 3.40 | 2.28 | 2.58 |
| `library_byDiagonals_memoryOrder` | 0.837 | 5.60 | 0.872 | 1.73 |
| `library_byRows_memoryOrder` | 1.26 | 2.08 | 1.31 | 1.61 |
| `eigen_sparse_kron` | 2.75 | 2.57 | 2.58 | 2.57 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.63 | 2.20 | 1.15 | 1.96 |
| `diagonalwise_sliceIteratorOrder` | 1.49 | 3.45 | 1.01 | 1.68 |
| `rowwise_sliceIteratorOrder` | 3.74 | 3.72 | 3.61 | 3.29 |
| `termCSR_sliceIteratorOrder` | 3.42 | 3.77 | 3.27 | 3.36 |
| `diagonalwise_memoryOrder` | 0.91 | 2.71 | 0.89 | 0.94 |
| `rowwise_memoryOrder` | 3.10 | 1.53 | 3.00 | 1.77 |
| `termCSR_memoryOrder` | 2.70 | 1.61 | 2.60 | 1.58 |
| `library_byDiagonals_memoryOrder` | 0.99 | 2.65 | 0.99 | 1.06 |
| `library_byRows_memoryOrder` | 1.49 | 0.99 | 1.49 | 0.99 |
| `eigen_sparse_kron` | 3.25 | 1.22 | 2.94 | 1.58 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.67 | 1.02 | 0.800 | 0.751 | 0.623 | 0.627 | 0.533 | 0.579 | 0.680 |
| `library_byRows` | 2.91 | 1.60 | 1.18 | 1.02 | 0.796 | 0.755 | 0.617 | 0.622 | 0.595 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.80 | 1.15 | 0.951 | 0.848 | 0.818 | 0.799 | 0.741 | 0.720 | 0.768 |
| `library_byRows` | 3.05 | 1.70 | 1.39 | 1.28 | 1.32 | 1.38 | 1.69 | 1.73 | 1.69 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.23 | 1.06 | 0.975 | 0.945 | 0.902 | 0.888 | 0.881 | 0.880 | 0.857 | 0.853 |
| `library_byRows` | 3.12 | 1.98 | 1.48 | 1.26 | 1.03 | 0.923 | 0.832 | 0.786 | 0.712 | 0.678 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.49 | 1.33 | 1.25 | 1.22 | 1.19 | 1.18 | 1.18 | 1.18 | 1.15 | 1.14 |
| `library_byRows` | 3.21 | 2.08 | 1.60 | 1.41 | 1.22 | 1.18 | 1.21 | 1.30 | 1.28 | 1.06 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.54 | 2.33 | 2.18 | 2.16 | 2.12 | 2.10 | 2.09 | 2.09 | 2.08 | 2.04 |
| `library_byRows` | 3.40 | 2.24 | 1.85 | 1.69 | 1.67 | 1.73 | 1.84 | 1.95 | 1.95 | 1.74 |


## tcmalloc: `main.apply1+applyJC`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.5 ns | 173 ns | 633 ns | 2.75 µs | 11.5 µs | 51.2 µs* | 503 µs | 2.45 ms* | 9.65 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.32 µs | 10.0 µs | 41.4 µs | 165 µs | 704 µs | 3.18 ms | 12.9 ms |
| `eigen_dense` | 87.2 ns | 1.28 µs | 27.4 µs | 789 µs* | 13.4 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.85 | 3.22 | 3.66 | 3.65 | 3.59 | 3.23* | 1.40 | 1.30* | 1.33 |
| `eigen_dense` | 1.22 | 7.42 | 43.2 | 287* | 1161 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 173 ns | 490 ns | 2.03 µs | 9.07 µs | 42.9 µs | 179 µs | 1.17 ms | 6.19 ms | 27.8 ms |
| `eigen_sparse` | 230 ns | 925 ns | 3.85 µs | 17.2 µs | 70.3 µs | 280 µs | 1.32 ms | 5.35 ms | 21.5 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.33 | 1.89 | 1.90 | 1.90 | 1.64 | 1.56 | 1.13 | 0.86 | 0.77 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 126 ns | 294 ns | 1.23 µs | 4.87 µs* | 21.8 µs | 87.5 µs | 474 µs | 4.43 ms | 16.2 ms |
| `eigen_sparse` | 225 ns | 931 ns | 3.92 µs | 18.3 µs | 74.3 µs | 297 µs | 1.39 ms | 9.33 ms | 23.5 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.79 | 3.17 | 3.19 | 3.76* | 3.41 | 3.40 | 2.93 | 2.10 | 1.45 |


## tcmalloc: `kernels.B+C+D+F`, pages as the system hands them out (5 processes)

### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.598 | 0.713 | 1.79 |
| `library_byDiagonals` | 0.602 | 0.892 | 2.73 |
| `library_byRows` | 0.596 | 0.714 | 1.79 |
| `hoisted` | 0.590 | 0.892 | 2.73 |
| `eigen_sparse` | 1.95 | 2.33 | 2.78 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.01 | 1.25 | 1.53 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 0.99 | 1.25 | 1.53 |
| `eigen_sparse` | 3.26 | 3.27 | 1.55 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 2.13 | 1.46 |
| `blocked` | 3.55 | 0.955 |
| `library_materialized` | 1.98 | 1.39 |
| `library_materialized_byDiagonals` | 2.64 | 2.18 |
| `library_materialized_byRows` | 1.98 | 1.39 |
| `eigen_sparse_kron` | 2.49 | 2.46 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 2.08 | 5.26 | 1.03 | 3.40 |
| `diagonalwise_sliceIteratorOrder` | 1.97 | 9.47 | 0.911 | 2.90 |
| `rowwise_sliceIteratorOrder` | 3.43 | 8.41 | 3.18 | 5.56 |
| `termCSR_sliceIteratorOrder` | 3.36 | 8.59 | 2.90 | 5.68 |
| `library_memoryOrder` | 1.51 | 2.79 | 0.847 | 1.61 |
| `diagonalwise_memoryOrder` | 1.43 | 6.53 | 0.767 | 1.56 |
| `rowwise_memoryOrder` | 2.65 | 3.55 | 2.63 | 2.91 |
| `termCSR_memoryOrder` | 2.57 | 3.91 | 2.29 | 2.62 |
| `library_byDiagonals_memoryOrder` | 1.50 | 6.60 | 0.834 | 1.74 |
| `library_byRows_memoryOrder` | 1.48 | 2.76 | 1.24 | 1.59 |
| `eigen_sparse_kron` | 2.65 | 2.58 | 2.64 | 2.62 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.38 | 1.89 | 1.22 | 2.11 |
| `diagonalwise_sliceIteratorOrder` | 1.30 | 3.39 | 1.08 | 1.80 |
| `rowwise_sliceIteratorOrder` | 2.27 | 3.01 | 3.75 | 3.45 |
| `termCSR_sliceIteratorOrder` | 2.23 | 3.08 | 3.42 | 3.53 |
| `diagonalwise_memoryOrder` | 0.95 | 2.34 | 0.91 | 0.97 |
| `rowwise_memoryOrder` | 1.75 | 1.27 | 3.11 | 1.81 |
| `termCSR_memoryOrder` | 1.70 | 1.40 | 2.70 | 1.63 |
| `library_byDiagonals_memoryOrder` | 0.99 | 2.37 | 0.98 | 1.08 |
| `library_byRows_memoryOrder` | 0.98 | 0.99 | 1.46 | 0.99 |
| `eigen_sparse_kron` | 1.75 | 0.92 | 3.12 | 1.63 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.63 | 0.983 | 0.799 | 0.700 | 0.619 | 0.572 | 0.538 | 0.520 | 0.571 |
| `library_byRows` | 2.89 | 1.57 | 1.18 | 0.982 | 0.801 | 0.704 | 0.620 | 0.572 | 0.561 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.34* | 1.88* | 1.72* | 1.66* | 1.58* | 1.58* | 1.52* | 1.53* | 1.57* |
| `library_byRows` | 3.28 | 2.05* | 1.76* | 1.58* | 1.61* | 1.45* | 1.75* | 1.46* | 1.19* |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.28 | 1.11 | 1.04 | 1.02 | 0.939 | 0.931 | 0.900 | 0.903 | 0.911 | 0.956 |
| `library_byRows` | 3.12 | 1.98 | 1.50 | 1.27 | 1.05 | 0.927 | 0.834 | 0.779 | 0.709 | 0.638 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.17* | 1.55 | 1.39 | 1.33 | 1.24 | 1.50 | 1.49 | 1.49 | 1.50 | 1.52 |
| `library_byRows` | 3.33 | 2.44 | 1.70 | 1.63 | 1.20 | 1.41 | 1.39 | 1.37 | 1.31 | 1.16 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.97 | 2.50 | 2.50* | 2.36 | 2.35 | 2.33 | 2.30 | 2.30 | 2.27 | 2.36 |
| `library_byRows` | 3.43 | 2.45 | 2.03 | 1.82 | 1.69 | 1.63 | 1.86 | 1.86 | 1.82 | 1.50 |


