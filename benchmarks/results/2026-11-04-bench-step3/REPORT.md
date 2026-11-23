# Session 2026-11-04-bench-step3

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| prev | main.apply1+applyJC | thp | 5 | 59 | 0.1% | 1.9% | 67.6% | 6 | 0.985–1.015 |
| prev | kernels | thp | 5 | 92 | 0.1% | 1.4% | 8.6% | 0 | 0.999–1.003 |
| step3 | main | thp | 5 | 180 | 0.1% | 1.0% | 38.1% | 9 | 1.000–1.008 |
| step3 | kernels | thp | 5 | 134 | 0.2% | 2.3% | 29.4% | 10 | 0.980–1.003 |
| step3 | kernels-cxlr | thp | 5 | 134 | 0.2% | 2.2% | 16.6% | 5 | 0.996–1.008 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## prev: `main.apply1+applyJC`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 68.3 ns | 169 ns | 830 ns | 3.62 µs | 16.4 µs | 84.2 µs | 587 µs* | 2.08 ms* | 17.8 ms |
| `eigen_sparse` | 131 ns | 549 ns | 2.27 µs | 9.52 µs | 39.9 µs | 158 µs | 682 µs | 3.31 ms | 12.3 ms |
| `eigen_dense` | 87.4 ns | 1.27 µs* | 27.4 µs | 805 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.92 | 3.24 | 2.73 | 2.63 | 2.43 | 1.88 | 1.16* | 1.59* | 0.69 |
| `eigen_dense` | 1.28 | 7.50* | 33.0 | 223 | 810 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 184 ns | 494 ns | 2.04 µs | 9.78 µs | 43.7 µs | 184 µs | 2.16 ms | 6.98 ms | 35.3 ms |
| `eigen_sparse` | 276 ns | 1.11 µs | 4.43 µs | 17.8 µs | 72.3 µs | 288 µs | 1.34 ms | 5.61 ms | 22.0 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.50 | 2.24 | 2.17 | 1.82 | 1.65 | 1.57 | 0.62 | 0.80 | 0.62 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 125 ns | 297 ns | 1.20 µs | 6.21 µs* | 30.2 µs | 132 µs | 1.10 ms* | 4.11 ms* | 28.3 ms |
| `eigen_sparse` | 270 ns | 1.11 µs | 4.43 µs | 17.8 µs | 72.9 µs | 292 µs | 1.73 ms | 5.71 ms | 23.3 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 2.17 | 3.74 | 3.68 | 2.87* | 2.42 | 2.20 | 1.57* | 1.39* | 0.82 |


## prev: `kernels`, pages as the system hands them out (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.648 | 0.809 | 1.58 |
| `hoisted` | 0.646 | 0.807 | 1.58 |
| `rowBlocked` | 0.640 | 0.800 | 1.21 |
| `eigen_sparse` | 1.80 | 1.94 | 2.37 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `hoisted` | 1.00 | 1.00 | 1.00 |
| `rowBlocked` | 0.99 | 0.99 | 0.77 |
| `eigen_sparse` | 2.78 | 2.40 | 1.50 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.590 | 0.926 | 3.30 |
| `hoisted` | 0.582 | 0.928 | 3.31 |
| `eigen_sparse` | 1.96 | 2.18 | 3.05 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `hoisted` | 0.99 | 1.00 | 1.00 |
| `eigen_sparse` | 3.32 | 2.35 | 0.92 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 2.21 | 1.68 |
| `blocked` | 3.38 | 1.28 |
| `library_materialized` | 2.65 | 2.31 |
| `eigen_sparse_kron` | 2.58 | 2.61 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 3.11 | 7.75 | 0.884 | 2.70 |
| `diagonalwise_sliceIteratorOrder` | 3.15 | 7.79 | 0.909 | 2.75 |
| `rowwise_sliceIteratorOrder` | 3.25 | 7.86 | 2.74 | 5.95 |
| `termCSR_sliceIteratorOrder` | 3.65 | 8.01 | 2.92 | 6.15 |
| `library_memoryOrder` | 2.52 | 5.95 | 0.784 | 1.55 |
| `diagonalwise_memoryOrder` | 2.54 | 5.94 | 0.797 | 1.57 |
| `rowwise_memoryOrder` | 2.33 | 3.82 | 2.08 | 2.58 |
| `termCSR_memoryOrder` | 2.81 | 4.10 | 2.32 | 2.63 |
| `eigen_sparse_kron` | 2.45 | 2.52 | 2.47 | 2.46 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.23 | 1.30 | 1.13 | 1.74 |
| `diagonalwise_sliceIteratorOrder` | 1.25 | 1.31 | 1.16 | 1.77 |
| `rowwise_sliceIteratorOrder` | 1.29 | 1.32 | 3.49 | 3.84 |
| `termCSR_sliceIteratorOrder` | 1.45 | 1.35 | 3.72 | 3.97 |
| `diagonalwise_memoryOrder` | 1.01 | 1.00 | 1.02 | 1.01 |
| `rowwise_memoryOrder` | 0.92 | 0.64 | 2.65 | 1.66 |
| `termCSR_memoryOrder` | 1.12 | 0.69 | 2.96 | 1.70 |
| `eigen_sparse_kron` | 0.97 | 0.42 | 3.15 | 1.59 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.924 | 1.31 | 1.27 | 1.37 | 1.31 | 1.27 | 1.17 | 1.00 | 0.922 |
| `hoisted` | 3.73 | 3.00 | 2.15 | 1.82 | 1.42 | 1.34 | 1.21 | 1.03 | 0.933 |
| `eigen_sparse` | 2.38 | 1.95 | 1.89 | 1.88 | 1.86 | 1.85 | 1.85 | 1.88 | 1.85 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `hoisted` | 4.04 | 2.29 | 1.69 | 1.33 | 1.08 | 1.06 | 1.03 | 1.03 | 1.01 |
| `eigen_sparse` | 2.58 | 1.49 | 1.49 | 1.37 | 1.42 | 1.46 | 1.58 | 1.88 | 2.01 |


## step3: `main`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 69.1 ns | 171 ns | 694 ns | 3.06 µs | 14.5 µs | 59.0 µs | 422 µs* | 1.43 ms | 9.07 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.29 µs | 9.75 µs | 40.3 µs | 161 µs | 700 µs | 3.46 ms | 12.5 ms |
| `eigen_dense` | 87.2 ns | 1.27 µs | 26.5 µs | 773 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.91 | 3.25 | 3.30 | 3.19 | 2.77 | 2.73 | 1.66* | 2.43 | 1.38 |
| `eigen_dense` | 1.26 | 7.44 | 38.2 | 253 | 918 |  |  |  |  |


### `compose1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 503 ns | 1.14 µs | 4.12 µs | 15.3 µs | 63.7 µs | 253 µs | 1.32 ms | 6.58 ms | 35.5 ms |
| `eigen_sparse` | 1.61 µs | 6.00 µs | 26.9 µs | 108 µs | 424 µs | 1.76 ms | 7.53 ms | 34.6 ms | 140 ms |
| `eigen_dense` | 1.59 µs | 79.7 µs | 4.93 ms | 327 ms |  |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 3.19 | 5.29 | 6.54 | 7.02 | 6.65 | 6.98 | 5.70 | 5.26 | 3.94 |
| `eigen_dense` | 3.15 | 70.3 | 1198 | 21360 |  |  |  |  |  |


### `construct1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 1.43 µs | 6.25 µs | 28.6 µs* | 116 µs* | 465 µs* | 1.86 ms* | 7.46 ms* | 30.6 ms* | 125 ms* |
| `eigen_sparse` | 2.39 µs | 9.50 µs | 43.6 µs | 193 µs | 785 µs | 3.13 ms | 14.2 ms | 71.6 ms | 260 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.67 | 1.52 | 1.52* | 1.66* | 1.69* | 1.68* | 1.90* | 2.34* | 2.08* |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 173 ns | 510 ns | 2.16 µs | 9.84 µs | 45.2 µs | 188 µs | 1.59 ms* | 7.00 ms | 36.4 ms |
| `eigen_sparse` | 277 ns | 1.11 µs | 4.43 µs | 17.8 µs | 73.1 µs | 287 µs | 1.44 ms | 5.62 ms | 22.2 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.60 | 2.17 | 2.05 | 1.81 | 1.62 | 1.53 | 0.91* | 0.80 | 0.61 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 126 ns | 324 ns | 1.27 µs | 4.91 µs | 23.8 µs | 91.4 µs | 826 µs | 2.96 ms | 14.8 ms |
| `eigen_sparse` | 270 ns | 1.11 µs | 4.44 µs | 17.8 µs | 73.4 µs | 291 µs | 1.76 ms | 5.70 ms | 23.4 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 2.15 | 3.44 | 3.51 | 3.62 | 3.09 | 3.19 | 2.13 | 1.93 | 1.59 |


### `composeJC`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 4.56 µs | 15.2 µs | 58.0 µs | 231 µs | 936 µs | 3.75 ms | 16.2 ms | 65.6 ms | 256 ms |
| `eigen_sparse` | 4.23 µs | 20.1 µs | 90.2 µs | 368 µs | 1.46 ms | 6.38 ms | 26.7 ms | 121 ms | 508 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 0.93 | 1.32 | 1.56 | 1.59 | 1.56 | 1.70 | 1.65 | 1.85 | 1.98 |


### `constructJC`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 3.35 µs | 10.1 µs | 36.6 µs | 147 µs | 615 µs | 2.50 ms | 11.3 ms | 67.7 ms | 224 ms |
| `eigen_sparse` | 2.01 µs | 7.66 µs | 39.0 µs | 164 µs | 663 µs | 2.73 ms | 12.9 ms | 71.3 ms | 266 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 0.60 | 0.76 | 1.07 | 1.11 | 1.08 | 1.09 | 1.14 | 1.05 | 1.19 |


### `picture1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal_static` | 183 ns | 734 ns | 2.92 µs | 11.5 µs | 46.8 µs | 189 µs | 773 µs | 3.66 ms | 16.3 ms |
| `multidiagonal_scalarfreq` | 278 ns | 727 ns | 2.78 µs | 11.3 µs | 46.5 µs | 186 µs | 761 µs | 3.69 ms | 16.8 ms |
| `multidiagonal_tabfreq` | 1.57 µs | 6.43 µs | 26.1 µs | 98.7 µs | 365 µs | 1.46 ms | 5.65 ms | 22.9 ms | 92.8 ms |
| `eigen_sparse_pernnz` | 1.56 µs | 6.61 µs | 27.0 µs | 103 µs | 380 µs | 1.52 ms | 5.96 ms | 23.9 ms | 94.8 ms |
| `eigen_sparse_sandwich` | 237 ns | 904 ns | 3.68 µs | 15.9 µs | 65.3 µs | 262 µs | 1.16 ms | 5.48 ms | 23.3 ms |


## step3: `kernels`, pages as the system hands them out (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.579 | 0.708 | 1.05 |
| `library_byDiagonals` | 0.645 | 0.804 | 1.54 |
| `library_byRows` | 0.578 | 0.709 | 1.05 |
| `hoisted` | 0.645 | 0.803 | 1.54 |
| `rowBlocked` | 0.641 | 0.801 | 1.21 |
| `eigen_sparse` | 2.02 | 2.06 | 2.47 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `library_byDiagonals` | 1.11 | 1.14 | 1.47 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.11 | 1.13 | 1.47 |
| `rowBlocked` | 1.11 | 1.13 | 1.15 |
| `eigen_sparse` | 3.49 | 2.91 | 2.35 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.577 | 0.751 | 2.47* |
| `library_byDiagonals` | 0.585 | 0.957 | 4.71* |
| `library_byRows` | 0.576 | 0.752 | 2.47* |
| `hoisted` | 0.583 | 0.956 | 4.72* |
| `eigen_sparse` | 2.17 | 2.25 | 3.45* |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.01 | 1.27 | 1.91* |
| `library_byRows` | 1.00 | 1.00 | 1.00* |
| `hoisted` | 1.01 | 1.27 | 1.91* |
| `eigen_sparse` | 3.76 | 3.00 | 1.40* |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 2.83* | 1.93* |
| `blocked` | 3.80* | 1.52* |
| `library_materialized` | 2.23 | 1.62 |
| `library_materialized_byDiagonals` | 3.15* | 2.46 |
| `library_materialized_byRows` | 2.23 | 1.62 |
| `eigen_sparse_kron` | 2.54 | 2.57 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 3.31 | 4.75 | 1.03 | 3.52 |
| `diagonalwise_sliceIteratorOrder` | 3.13 | 7.79 | 0.903 | 2.76 |
| `rowwise_sliceIteratorOrder` | 3.45 | 7.93 | 3.18 | 6.09 |
| `termCSR_sliceIteratorOrder` | 3.48 | 8.09 | 2.97 | 6.19 |
| `library_memoryOrder` | 2.64 | 2.38 | 0.894 | 1.58 |
| `diagonalwise_memoryOrder` | 2.54 | 6.00 | 0.797 | 1.56 |
| `rowwise_memoryOrder` | 2.60 | 3.94 | 2.63 | 2.91 |
| `termCSR_memoryOrder` | 2.64 | 4.06 | 2.37 | 2.68 |
| `library_byDiagonals_memoryOrder` | 2.63 | 6.06 | 0.887 | 1.72 |
| `library_byRows_memoryOrder` | 1.83 | 2.33 | 1.24 | 1.54 |
| `eigen_sparse_kron` | 2.66 | 2.67 | 2.69 | 2.65 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.25 | 2.00 | 1.15 | 2.23 |
| `diagonalwise_sliceIteratorOrder` | 1.19 | 3.27 | 1.01 | 1.75 |
| `rowwise_sliceIteratorOrder` | 1.31 | 3.33 | 3.56 | 3.85 |
| `termCSR_sliceIteratorOrder` | 1.32 | 3.40 | 3.32 | 3.92 |
| `diagonalwise_memoryOrder` | 0.96 | 2.52 | 0.89 | 0.99 |
| `rowwise_memoryOrder` | 0.98 | 1.66 | 2.94 | 1.84 |
| `termCSR_memoryOrder` | 1.00 | 1.71 | 2.65 | 1.70 |
| `library_byDiagonals_memoryOrder` | 1.00 | 2.55 | 0.99 | 1.09 |
| `library_byRows_memoryOrder` | 0.69 | 0.98 | 1.39 | 0.97 |
| `eigen_sparse_kron` | 1.01 | 1.12 | 3.01 | 1.68 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.942 | 1.35 | 1.28 | 1.31 | 3.22 | 2.60 | 2.02 | 1.26 | 0.913 |
| `library_byDiagonals` | 0.942 | 1.35 | 1.28 | 1.31 | 1.27 | 1.29 | 1.18 | 1.01 | 0.929 |
| `library_byRows` | 9.65 | 6.10 | 5.30 | 3.89 | 3.22 | 2.61 | 2.02 | 1.26 | 0.914 |
| `hoisted` | 3.79 | 2.99 | 2.15 | 1.79 | 1.42 | 1.36 | 1.22 | 1.04 | 0.944 |
| `eigen_sparse` | 2.29 | 1.85 | 1.76 | 1.75 | 1.72 | 1.72 | 1.69 | 1.72 | 1.70 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.00 | 1.00 | 1.00 | 1.00 | 0.39 | 0.50 | 0.58 | 0.80 | 1.02 |
| `library_byRows` | 10.2 | 4.52 | 4.14 | 2.97 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 |
| `hoisted` | 4.02 | 2.21 | 1.68 | 1.37 | 0.44 | 0.52 | 0.60 | 0.83 | 1.03 |
| `eigen_sparse` | 2.43 | 1.37 | 1.38 | 1.34 | 0.53 | 0.66 | 0.84 | 1.37 | 1.86 |


## step3: `kernels-cxlr`, pages as the system hands them out (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.577 | 0.714 | 1.07* |
| `library_byDiagonals` | 0.646 | 0.813* | 1.57 |
| `library_byRows` | 0.577 | 0.712 | 1.07* |
| `hoisted` | 0.642 | 0.809* | 1.57 |
| `rowBlocked` | 0.639 | 0.802* | 1.21 |
| `eigen_sparse` | 1.64 | 1.79 | 2.20 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `library_byDiagonals` | 1.12 | 1.14* | 1.47* |
| `library_byRows` | 1.00 | 1.00 | 1.00* |
| `hoisted` | 1.11 | 1.13* | 1.47* |
| `rowBlocked` | 1.11 | 1.12* | 1.13* |
| `eigen_sparse` | 2.84 | 2.51 | 2.06* |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.579 | 0.746 | 1.92 |
| `library_byDiagonals` | 0.582 | 0.957 | 3.33 |
| `library_byRows` | 0.578 | 0.746 | 1.92 |
| `hoisted` | 0.583 | 0.958 | 3.33 |
| `eigen_sparse` | 1.80 | 1.98 | 3.07 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.01 | 1.28 | 1.73 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.01 | 1.28 | 1.73 |
| `eigen_sparse` | 3.11 | 2.65 | 1.60 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 2.20 | 1.69 |
| `blocked` | 3.39 | 1.28 |
| `library_materialized` | 2.07 | 1.52 |
| `library_materialized_byDiagonals` | 2.65 | 2.31 |
| `library_materialized_byRows` | 2.07 | 1.52 |
| `eigen_sparse_kron` | 2.25 | 2.35 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 3.30 | 4.79 | 1.04 | 3.53 |
| `diagonalwise_sliceIteratorOrder` | 3.11 | 7.75 | 0.902 | 2.75 |
| `rowwise_sliceIteratorOrder` | 3.22 | 7.88 | 2.73 | 5.93 |
| `termCSR_sliceIteratorOrder` | 3.27 | 7.42 | 2.37 | 5.73 |
| `library_memoryOrder` | 2.64 | 2.38 | 0.893 | 1.60 |
| `diagonalwise_memoryOrder` | 2.54 | 5.95 | 0.797 | 1.53 |
| `rowwise_memoryOrder` | 2.32 | 3.82 | 2.07 | 2.63 |
| `termCSR_memoryOrder` | 2.49 | 3.73 | 1.73 | 2.15 |
| `library_byDiagonals_memoryOrder` | 2.63 | 6.01 | 0.886 | 1.71 |
| `library_byRows_memoryOrder` | 1.83 | 2.34 | 1.24 | 1.55 |
| `eigen_sparse_kron` | 2.28 | 2.39 | 2.31 | 2.32 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.25 | 2.01 | 1.16 | 2.21 |
| `diagonalwise_sliceIteratorOrder` | 1.18 | 3.26 | 1.01 | 1.72 |
| `rowwise_sliceIteratorOrder` | 1.22 | 3.31 | 3.06 | 3.71 |
| `termCSR_sliceIteratorOrder` | 1.24 | 3.12 | 2.65 | 3.58 |
| `diagonalwise_memoryOrder` | 0.96 | 2.50 | 0.89 | 0.96 |
| `rowwise_memoryOrder` | 0.88 | 1.61 | 2.32 | 1.64 |
| `termCSR_memoryOrder` | 0.94 | 1.57 | 1.94 | 1.34 |
| `library_byDiagonals_memoryOrder` | 1.00 | 2.53 | 0.99 | 1.07 |
| `library_byRows_memoryOrder` | 0.69 | 0.98 | 1.39 | 0.97 |
| `eigen_sparse_kron` | 0.86 | 1.00 | 2.59 | 1.45 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.940 | 1.32 | 1.27 | 1.37 | 3.23 | 2.59 | 2.01 | 1.26 | 0.913 |
| `library_byDiagonals` | 0.939 | 1.32 | 1.27 | 1.37 | 1.31 | 1.27 | 1.17 | 1.01 | 0.921 |
| `library_byRows` | 9.76 | 6.12 | 5.30 | 3.91 | 3.23 | 2.59 | 2.02 | 1.26 | 0.913 |
| `hoisted` | 3.71 | 2.97 | 2.12 | 1.81 | 1.42 | 1.34 | 1.21 | 1.03 | 0.930 |
| `eigen_sparse` | 2.10 | 1.60 | 1.54 | 1.52 | 1.50 | 1.61 | 1.48 | 1.50 | 1.48 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.00 | 1.00 | 1.00 | 1.00 | 0.41 | 0.49 | 0.58 | 0.80 | 1.01 |
| `library_byRows` | 10.4 | 4.64 | 4.17 | 2.85 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 |
| `hoisted` | 3.95 | 2.25 | 1.67 | 1.32 | 0.44 | 0.52 | 0.60 | 0.82 | 1.02 |
| `eigen_sparse` | 2.23 | 1.21 | 1.21 | 1.11 | 0.46 | 0.62 | 0.74 | 1.19 | 1.62 |


