# Session 2026-11-04-bench-padthp

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| base | main.apply1+applyJC | thp | 5 | 59 | 0.2% | 2.8% | 115.2% | 4 | 0.999–1.016 |
| base | main.apply1+applyJC | 4k | 5 | 59 | 0.1% | 1.4% | 36.8% | 1 | 1.000–1.007 |
| base | kernels.B+C+D+F | thp | 5 | 167 | 0.3% | 1.8% | 45.2% | 15 | 0.999–1.012 |
| base | kernels.B+C+D+F | 4k | 5 | 167 | 0.0% | 1.8% | 14.4% | 3 | 1.000–1.007 |
| pad | main.apply1+applyJC | thp | 5 | 59 | 0.1% | 1.4% | 10.8% | 2 | 1.000–1.002 |
| pad | kernels.B+C+D+F | thp | 5 | 167 | 0.3% | 2.3% | 76.2% | 10 | 0.996–1.018 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## base: `main.apply1+applyJC`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.4 ns | 174 ns | 695 ns* | 3.10 µs | 14.6 µs | 58.8 µs | 427 µs* | 1.42 ms | 9.26 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.29 µs | 9.76 µs | 40.5 µs | 162 µs | 698 µs | 3.46 ms | 12.5 ms |
| `eigen_dense` | 87.2 ns | 1.28 µs | 27.3 µs | 795 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.85 | 3.20 | 3.30* | 3.15 | 2.77 | 2.75 | 1.63* | 2.43 | 1.35 |
| `eigen_dense` | 1.22 | 7.36 | 39.3* | 256 | 909 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 174 ns | 486 ns | 2.04 µs | 9.81 µs | 43.8 µs | 182 µs | 2.20 ms | 6.96 ms* | 35.7 ms |
| `eigen_sparse` | 230 ns | 928 ns | 3.91 µs | 15.8 µs | 65.6 µs | 264 µs | 1.25 ms | 5.43 ms | 20.5 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.32 | 1.91 | 1.92 | 1.61 | 1.50 | 1.45 | 0.57 | 0.78* | 0.58 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 127 ns | 289 ns | 1.22 µs | 4.91 µs | 25.5 µs | 100 µs | 827 µs* | 2.93 ms | 13.9 ms |
| `eigen_sparse` | 225 ns | 928 ns | 3.89 µs | 16.8 µs | 68.2 µs | 272 µs | 1.75 ms | 5.48 ms | 22.6 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.78 | 3.21 | 3.18 | 3.43 | 2.68 | 2.71 | 2.12* | 1.87 | 1.63 |


## base: `main.apply1+applyJC`, 4 KB pages (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.4 ns | 173 ns | 692 ns | 3.08 µs | 14.5 µs | 58.1 µs | 252 µs | 1.39 ms | 6.30 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.29 µs | 9.75 µs | 40.5 µs | 162 µs | 711 µs | 3.17 ms | 12.7 ms |
| `eigen_dense` | 87.4 ns | 1.28 µs | 27.5 µs | 836 µs | 13.4 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.85 | 3.21 | 3.31 | 3.17 | 2.79 | 2.78 | 2.82 | 2.29 | 2.01 |
| `eigen_dense` | 1.22 | 7.37 | 39.8 | 272 | 919 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 173 ns | 486 ns | 2.04 µs | 9.84 µs | 44.8 µs | 189 µs | 931 µs | 5.09 ms | 23.0 ms |
| `eigen_sparse` | 230 ns | 928 ns | 3.91 µs | 15.8 µs | 65.5 µs | 262 µs | 1.27 ms | 5.44 ms | 20.6 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.33 | 1.91 | 1.92 | 1.60 | 1.46 | 1.39 | 1.36 | 1.07 | 0.89 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 125 ns | 292 ns | 1.23 µs | 4.93 µs* | 25.3 µs | 102 µs | 507 µs | 2.70 ms | 9.70 ms |
| `eigen_sparse` | 225 ns | 928 ns | 3.89 µs | 16.9 µs | 68.2 µs | 273 µs | 1.33 ms | 5.59 ms | 22.3 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.80 | 3.18 | 3.16 | 3.42* | 2.70 | 2.68 | 2.62 | 2.07 | 2.30 |


## base: `kernels.B+C+D+F`, pages as the system hands them out (5 processes)

### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.591 | 0.639 | 1.46 |
| `library_byDiagonals` | 0.641 | 0.899 | 2.85 |
| `library_byRows` | 0.590 | 0.641 | 1.46 |
| `hoisted` | 0.628 | 0.900 | 2.85 |
| `eigen_sparse` | 2.14 | 2.22 | 4.27 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.08 | 1.41 | 1.95 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.06 | 1.41 | 1.95 |
| `eigen_sparse` | 3.62 | 3.47 | 2.92 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 3.02 | 1.96 |
| `blocked` | 3.98 | 1.57 |
| `library_materialized` | 2.41 | 1.63 |
| `library_materialized_byDiagonals` | 3.30 | 2.51 |
| `library_materialized_byRows` | 2.41 | 1.62 |
| `eigen_sparse_kron` | 2.72 | 2.52 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 3.20 | 4.80 | 1.05 | 3.52 |
| `diagonalwise_sliceIteratorOrder` | 3.05 | 7.78 | 0.933 | 2.76 |
| `rowwise_sliceIteratorOrder` | 3.22 | 7.95 | 3.13 | 6.11 |
| `termCSR_sliceIteratorOrder` | 3.38 | 8.02 | 2.85 | 6.18 |
| `library_memoryOrder` | 2.56 | 2.37 | 0.887 | 1.58 |
| `diagonalwise_memoryOrder` | 2.47 | 5.96 | 0.794 | 1.56* |
| `rowwise_memoryOrder` | 2.60 | 3.97 | 2.63 | 2.91 |
| `termCSR_memoryOrder` | 2.81 | 4.13 | 2.28 | 2.61 |
| `library_byDiagonals_memoryOrder` | 2.55 | 6.01 | 0.877 | 1.70 |
| `library_byRows_memoryOrder` | 1.84 | 2.33 | 1.24 | 1.54 |
| `eigen_sparse_kron` | 2.76 | 2.60 | 2.65 | 2.61 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.25 | 2.03 | 1.18 | 2.23 |
| `diagonalwise_sliceIteratorOrder` | 1.19 | 3.28 | 1.05 | 1.75 |
| `rowwise_sliceIteratorOrder` | 1.26 | 3.35 | 3.53 | 3.87 |
| `termCSR_sliceIteratorOrder` | 1.32 | 3.38 | 3.21 | 3.91 |
| `diagonalwise_memoryOrder` | 0.96 | 2.51 | 0.90 | 0.99* |
| `rowwise_memoryOrder` | 1.02 | 1.68 | 2.97 | 1.84 |
| `termCSR_memoryOrder` | 1.10 | 1.74 | 2.57 | 1.65 |
| `library_byDiagonals_memoryOrder` | 1.00 | 2.54 | 0.99 | 1.08 |
| `library_byRows_memoryOrder` | 0.72 | 0.98 | 1.40 | 0.97 |
| `eigen_sparse_kron` | 1.08 | 1.10 | 2.99 | 1.65 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.66* | 1.02* | 0.799 | 0.752* | 0.622 | 0.617* | 0.544 | 0.600* | 0.622 |
| `library_byRows` | 2.88 | 1.60 | 1.17 | 1.03 | 0.791 | 0.750* | 0.617 | 0.621* | 0.579 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.79* | 2.30* | 0.943 | 2.08* | 0.800 | 2.00* | 0.755 | 1.99* | 2.00* |
| `library_byRows` | 3.18 | 2.17 | 1.40 | 1.71 | 1.48 | 2.15 | 1.60 | 1.99 | 1.25* |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.26 | 1.13 | 1.03 | 0.988 | 0.941 | 0.919 | 0.894 | 0.886 | 0.861 | 0.898 |
| `library_byRows` | 3.13 | 1.98 | 1.49 | 1.24 | 1.01 | 0.909 | 0.820 | 0.780 | 0.703 | 0.686 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.15 | 2.09 | 1.34 | 1.94 | 1.26 | 1.85 | 1.18 | 1.82 | 1.83 | 1.84 |
| `library_byRows` | 3.34 | 2.44 | 1.65 | 1.74 | 1.21 | 1.41 | 1.19 | 1.50 | 1.43 | 1.16 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.91 | 2.84 | 2.27 | 2.62 | 2.21 | 2.56 | 2.16 | 2.52 | 2.50 | 2.56 |
| `library_byRows` | 3.49 | 2.54 | 1.94 | 1.95 | 1.76 | 1.89 | 1.86 | 2.28 | 2.16 | 1.53 |


## base: `kernels.B+C+D+F`, 4 KB pages (5 processes)

### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.593 | 0.647 | 1.10 |
| `library_byDiagonals` | 0.638 | 0.891 | 1.96 |
| `library_byRows` | 0.591 | 0.649 | 1.09 |
| `hoisted` | 0.626 | 0.891 | 1.96 |
| `eigen_sparse` | 2.14 | 2.22 | 2.79 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.08 | 1.38 | 1.78 |
| `library_byRows` | 1.00 | 1.00 | 0.99 |
| `hoisted` | 1.06 | 1.38 | 1.78 |
| `eigen_sparse` | 3.61 | 3.43 | 2.54 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 1.60 | 1.49 |
| `blocked` | 2.98 | 1.00 |
| `library_materialized` | 2.09 | 1.39 |
| `library_materialized_byDiagonals` | 2.19 | 2.20 |
| `library_materialized_byRows` | 2.09 | 1.39 |
| `eigen_sparse_kron` | 2.47 | 2.60 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.45 | 5.42 | 1.15 | 4.29 |
| `diagonalwise_sliceIteratorOrder` | 1.41 | 8.02 | 1.06 | 3.58 |
| `rowwise_sliceIteratorOrder` | 3.21 | 8.95 | 3.19 | 6.90 |
| `termCSR_sliceIteratorOrder` | 2.93 | 9.06 | 2.92 | 7.00 |
| `library_memoryOrder` | 0.867 | 2.46 | 0.898 | 1.65 |
| `diagonalwise_memoryOrder` | 0.805 | 5.80 | 0.799 | 1.47 |
| `rowwise_memoryOrder` | 2.61 | 3.95 | 2.64 | 2.99 |
| `termCSR_memoryOrder` | 2.26 | 4.12 | 2.29 | 2.70 |
| `library_byDiagonals_memoryOrder` | 0.854 | 5.91 | 0.888 | 1.69 |
| `library_byRows_memoryOrder` | 1.26 | 2.41 | 1.25 | 1.62 |
| `eigen_sparse_kron` | 2.65 | 2.66 | 2.69 | 2.68 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.67 | 2.20 | 1.28 | 2.60 |
| `diagonalwise_sliceIteratorOrder` | 1.63 | 3.26 | 1.18 | 2.17 |
| `rowwise_sliceIteratorOrder` | 3.70 | 3.64 | 3.55 | 4.18 |
| `termCSR_sliceIteratorOrder` | 3.38 | 3.68 | 3.25 | 4.24 |
| `diagonalwise_memoryOrder` | 0.93 | 2.36 | 0.89 | 0.89 |
| `rowwise_memoryOrder` | 3.01 | 1.61 | 2.94 | 1.81 |
| `termCSR_memoryOrder` | 2.61 | 1.67 | 2.55 | 1.64 |
| `library_byDiagonals_memoryOrder` | 0.99 | 2.40 | 0.99 | 1.02 |
| `library_byRows_memoryOrder` | 1.45 | 0.98 | 1.39 | 0.98 |
| `eigen_sparse_kron` | 3.06 | 1.08 | 3.00 | 1.62 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.67* | 1.02* | 0.799 | 0.751 | 0.625 | 0.638 | 0.549 | 0.610 | 0.622 |
| `library_byRows` | 2.89 | 1.60 | 1.17 | 1.02 | 0.794 | 0.753 | 0.617 | 0.620 | 0.583 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.79* | 1.12 | 0.950 | 0.857 | 0.806 | 0.785 | 0.762 | 0.776 | 0.805 |
| `library_byRows` | 2.98 | 1.77 | 1.38 | 1.27 | 1.43 | 1.82 | 1.56 | 1.71 | 0.770 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.26 | 1.12 | 1.03 | 0.986 | 0.937 | 0.920 | 0.898 | 0.893 | 0.870 | 0.900 |
| `library_byRows` | 3.13 | 1.99 | 1.49 | 1.25 | 1.02 | 0.911 | 0.821 | 0.781 | 0.704 | 0.684 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.53 | 1.41 | 1.33 | 1.28 | 1.24 | 1.23 | 1.20 | 1.20 | 1.20 | 1.22 |
| `library_byRows` | 3.28 | 2.14 | 1.66 | 1.42 | 1.27 | 1.20 | 1.29 | 1.45 | 1.37 | 0.929 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.56 | 2.40 | 2.31 | 2.26 | 2.19 | 2.19 | 2.18 | 2.17 | 2.11 | 2.17 |
| `library_byRows` | 3.44 | 2.31 | 1.95 | 1.71 | 1.77 | 1.72 | 1.87 | 2.11 | 2.00 | 1.39 |


## pad: `main.apply1+applyJC`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.5 ns | 176 ns | 685 ns | 3.02 µs | 12.8 µs | 48.1 µs | 217 µs* | 1.27 ms | 5.64 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.29 µs | 9.75 µs | 40.3 µs | 160 µs | 689 µs | 3.18 ms | 12.7 ms |
| `eigen_dense` | 87.2 ns | 1.28 µs | 27.9 µs | 774 µs* | 13.5 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.85 | 3.17 | 3.34 | 3.22 | 3.15 | 3.33 | 3.18* | 2.51 | 2.25 |
| `eigen_dense` | 1.22 | 7.28 | 40.7 | 256* | 1057 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 174 ns | 485 ns | 2.04 µs | 9.80 µs | 39.9 µs | 165 µs | 831 µs | 5.14 ms | 23.6 ms |
| `eigen_sparse` | 230 ns | 928 ns | 3.95 µs | 15.7 µs | 65.2 µs | 260 µs | 1.27 ms | 5.36 ms | 20.8 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.33 | 1.92 | 1.94 | 1.60 | 1.64 | 1.58 | 1.52 | 1.04 | 0.88 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 125 ns | 292 ns | 1.22 µs | 4.92 µs | 20.7 µs | 80.7 µs | 475 µs | 2.38 ms | 11.0 ms |
| `eigen_sparse` | 225 ns | 928 ns | 3.96 µs | 16.8 µs | 67.3 µs | 267 µs | 1.25 ms | 5.52 ms | 22.0 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.80 | 3.18 | 3.24 | 3.41 | 3.26 | 3.31 | 2.64 | 2.32 | 2.00 |


## pad: `kernels.B+C+D+F`, pages as the system hands them out (5 processes)

### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.597* | 0.574 | 1.19 |
| `library_byDiagonals` | 0.647* | 0.841 | 2.00 |
| `library_byRows` | 0.596* | 0.576 | 1.18 |
| `hoisted` | 0.629* | 0.842 | 1.99 |
| `eigen_sparse` | 2.14 | 2.12 | 2.64 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.08* | 1.47 | 1.68 |
| `library_byRows` | 1.00* | 1.00 | 0.99 |
| `hoisted` | 1.05* | 1.47 | 1.67 |
| `eigen_sparse` | 3.58* | 3.69 | 2.22 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 1.67* | 1.48 |
| `blocked` | 2.99 | 0.920 |
| `library_materialized` | 2.07 | 1.32 |
| `library_materialized_byDiagonals` | 2.16 | 2.08 |
| `library_materialized_byRows` | 2.07 | 1.32 |
| `eigen_sparse_kron` | 2.44 | 2.50 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.43 | 4.64 | 1.03 | 3.51 |
| `diagonalwise_sliceIteratorOrder` | 1.32 | 7.32 | 0.932 | 2.73 |
| `rowwise_sliceIteratorOrder` | 3.26 | 7.89 | 3.13 | 6.05 |
| `termCSR_sliceIteratorOrder` | 2.96 | 7.94 | 2.85 | 6.13 |
| `library_memoryOrder` | 0.869 | 2.21 | 0.888 | 1.57 |
| `diagonalwise_memoryOrder` | 0.788 | 5.62 | 0.792 | 1.55 |
| `rowwise_memoryOrder` | 2.62 | 3.72 | 2.63 | 2.93 |
| `termCSR_memoryOrder` | 2.28 | 3.83 | 2.28 | 2.63 |
| `library_byDiagonals_memoryOrder` | 0.860 | 5.56 | 0.882 | 1.67 |
| `library_byRows_memoryOrder` | 1.26 | 2.17 | 1.26 | 1.55 |
| `eigen_sparse_kron` | 2.64 | 2.62 | 2.66 | 2.61 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.65 | 2.10 | 1.16 | 2.24 |
| `diagonalwise_sliceIteratorOrder` | 1.52 | 3.31 | 1.05 | 1.74 |
| `rowwise_sliceIteratorOrder` | 3.75 | 3.57 | 3.52 | 3.85 |
| `termCSR_sliceIteratorOrder` | 3.41 | 3.59 | 3.21 | 3.90 |
| `diagonalwise_memoryOrder` | 0.91 | 2.54 | 0.89 | 0.99 |
| `rowwise_memoryOrder` | 3.01 | 1.68 | 2.96 | 1.87 |
| `termCSR_memoryOrder` | 2.62 | 1.73 | 2.57 | 1.68 |
| `library_byDiagonals_memoryOrder` | 0.99 | 2.52 | 0.99 | 1.06 |
| `library_byRows_memoryOrder` | 1.45 | 0.98 | 1.42 | 0.99 |
| `eigen_sparse_kron` | 3.04 | 1.19 | 3.00 | 1.66 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.67 | 1.02 | 0.800 | 0.751 | 0.623 | 0.618 | 0.547 | 0.605 | 0.623 |
| `library_byRows` | 2.89 | 1.60 | 1.17 | 1.02 | 0.793 | 0.751 | 0.617 | 0.622 | 0.578 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.74 | 1.13 | 0.945 | 0.872 | 0.826 | 0.780 | 2.46* | 0.741 | 0.789 |
| `library_byRows` | 3.01 | 1.77 | 1.39 | 1.27 | 1.44 | 1.90 | 2.00 | 1.73 | 1.72 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.23 | 1.08 | 1.00 | 0.953 | 0.931 | 0.909 | 0.895* | 0.915* | 0.873* | 0.856* |
| `library_byRows` | 3.13 | 1.96 | 1.50 | 1.26 | 1.02 | 0.917 | 0.823 | 0.806 | 0.730 | 0.639 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.49 | 1.35 | 1.28 | 1.24 | 1.22 | 1.20 | 1.21 | 1.21 | 1.17 | 1.17 |
| `library_byRows` | 3.30 | 2.13 | 1.65 | 1.43 | 1.26 | 1.29 | 1.29 | 1.48 | 1.40 | 1.15 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.61 | 2.34 | 2.23 | 2.19 | 2.12 | 2.11 | 2.54 | 2.51 | 2.06 | 2.07 |
| `library_byRows` | 3.45 | 2.29 | 1.89 | 1.69 | 1.71 | 1.80 | 2.00 | 2.35 | 2.09 | 1.82 |


