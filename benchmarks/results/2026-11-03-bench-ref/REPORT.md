# Session 2026-11-03-bench-ref

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| now | main | thp | 5 | 180 | 0.1% | 1.0% | 50.0% | 6 | 0.997–1.010 |
| now | kernels | thp | 5 | 92 | 0.2% | 1.9% | 16.4% | 3 | 0.994–1.008 |
| now | kernels-cxlr | thp | 5 | 92 | 0.2% | 1.9% | 16.9% | 1 | 0.997–1.006 |
| pre23 | main.apply1+applyJC | thp | 5 | 59 | 0.2% | 1.6% | 67.3% | 4 | 0.997–1.005 |
| pre23 | kernels.A+B+C | thp | 5 | 29 | 0.0% | 0.9% | 5.1% | 0 | 0.998–1.004 |
| pre23 | kernels-cxlr.A+B | thp | 5 | 21 | 0.3% | 2.0% | 41.2% | 6 | 0.998–1.058 |
| v040 | main.apply1+applyJC | thp | 5 | 59 | 0.1% | 1.5% | 60.0% | 3 | 1.000–1.011 |
| v040 | kernels.B+C | thp | 5 | 17 | 0.0% | 1.3% | 12.7% | 1 | 0.991–1.007 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## now: `main`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 68.3 ns | 171 ns | 835 ns | 3.59 µs | 16.3 µs | 82.6 µs | 633 µs* | 2.27 ms* | 17.8 ms |
| `eigen_sparse` | 131 ns | 549 ns | 2.27 µs | 9.49 µs | 39.7 µs | 158 µs | 683 µs | 3.32 ms | 12.3 ms |
| `eigen_dense` | 87.2 ns | 1.27 µs | 27.4 µs | 779 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.92 | 3.22 | 2.71 | 2.64 | 2.43 | 1.92 | 1.08* | 1.46* | 0.69 |
| `eigen_dense` | 1.28 | 7.44 | 32.9 | 217 | 818 |  |  |  |  |


### `compose1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 503 ns | 1.14 µs | 4.10 µs | 15.3 µs | 63.7 µs | 253 µs | 1.32 ms | 6.54 ms | 35.6 ms |
| `eigen_sparse` | 1.58 µs | 5.81 µs | 25.8 µs | 103 µs | 406 µs | 1.69 ms | 7.21 ms | 34.0 ms | 135 ms |
| `eigen_dense` | 1.71 µs | 81.6 µs | 4.98 ms | 329 ms |  |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 3.14 | 5.11 | 6.28 | 6.72 | 6.37 | 6.68 | 5.46 | 5.19 | 3.79 |
| `eigen_dense` | 3.40 | 71.7 | 1214 | 21543 |  |  |  |  |  |


### `construct1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 1.42 µs | 6.26 µs | 28.7 µs | 116 µs | 465 µs | 1.86 ms | 7.47 ms | 30.6 ms | 125 ms |
| `eigen_sparse` | 2.34 µs | 9.56 µs | 43.7 µs | 193 µs | 784 µs | 3.14 ms | 14.0 ms* | 72.7 ms | 260 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.64 | 1.53 | 1.53 | 1.66 | 1.69 | 1.69 | 1.87* | 2.38 | 2.08 |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 183 ns | 508 ns | 2.17 µs | 9.85 µs | 45.2 µs | 190 µs | 1.21 ms* | 6.94 ms | 36.1 ms |
| `eigen_sparse` | 276 ns | 1.11 µs | 4.43 µs | 17.8 µs | 71.9 µs | 287 µs | 1.43 ms | 5.62 ms | 22.2 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.51 | 2.18 | 2.04 | 1.81 | 1.59 | 1.51 | 1.18* | 0.81 | 0.61 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 126 ns | 328 ns | 1.34 µs | 6.14 µs | 30.8 µs | 134 µs | 1.01 ms* | 4.09 ms | 28.6 ms |
| `eigen_sparse` | 270 ns | 1.11 µs | 4.43 µs | 17.8 µs | 73.4 µs | 291 µs | 1.73 ms | 5.70 ms | 23.5 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 2.14 | 3.39 | 3.31 | 2.90 | 2.39 | 2.18 | 1.72* | 1.39 | 0.82 |


### `composeJC`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 4.52 µs | 15.2 µs | 57.8 µs | 231 µs | 937 µs | 3.72 ms | 16.0 ms | 65.9 ms | 257 ms |
| `eigen_sparse` | 4.04 µs | 18.8 µs | 85.3 µs | 348 µs | 1.38 ms | 6.04 ms | 25.7 ms | 115 ms | 489 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 0.89 | 1.24 | 1.47 | 1.51 | 1.48 | 1.62 | 1.61 | 1.75 | 1.91 |


### `constructJC`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 3.35 µs | 10.1 µs | 36.6 µs | 147 µs | 615 µs | 2.50 ms | 11.3 ms | 67.8 ms | 224 ms |
| `eigen_sparse` | 2.04 µs | 7.74 µs | 39.2 µs | 164 µs | 665 µs | 2.74 ms | 12.9 ms | 71.6 ms | 265 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 0.61 | 0.77 | 1.07 | 1.11 | 1.08 | 1.10 | 1.14 | 1.06 | 1.18 |


### `picture1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal_static` | 182 ns | 726 ns | 2.92 µs | 11.5 µs | 46.6 µs | 188 µs | 771 µs | 3.66 ms | 16.3 ms |
| `multidiagonal_scalarfreq` | 278 ns | 726 ns | 2.79 µs | 11.3 µs | 46.5 µs | 187 µs | 764 µs | 3.71 ms | 16.9 ms |
| `multidiagonal_tabfreq` | 1.57 µs | 6.41 µs | 26.1 µs | 98.5 µs | 362 µs | 1.45 ms | 5.63 ms* | 22.7 ms | 92.5 ms |
| `eigen_sparse_pernnz` | 1.56 µs | 6.61 µs | 27.0 µs | 103 µs | 379 µs | 1.52 ms | 5.94 ms | 23.9 ms | 94.6 ms |
| `eigen_sparse_sandwich` | 238 ns | 904 ns | 3.68 µs | 15.9 µs | 65.3 µs | 262 µs | 1.16 ms | 5.47 ms | 23.2 ms |


## now: `kernels`, pages as the system hands them out (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.647 | 0.834* | 1.56 |
| `hoisted` | 0.644 | 0.834* | 1.56 |
| `rowBlocked` | 0.639 | 0.805* | 1.20 |
| `eigen_sparse` | 1.80 | 1.95 | 2.37 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `hoisted` | 1.00 | 1.00* | 1.00 |
| `rowBlocked` | 0.99 | 0.97* | 0.77 |
| `eigen_sparse` | 2.78 | 2.34* | 1.52 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.593 | 0.957 | 3.34 |
| `hoisted` | 0.584 | 0.957 | 3.34 |
| `eigen_sparse` | 1.96 | 2.18 | 3.05 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `hoisted` | 0.98 | 1.00 | 1.00 |
| `eigen_sparse` | 3.31 | 2.28 | 0.91 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 2.21 | 1.68 |
| `blocked` | 3.39 | 1.28 |
| `library_materialized` | 2.64 | 2.31 |
| `eigen_sparse_kron` | 2.58 | 2.61 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 3.11 | 7.72 | 0.879 | 2.71 |
| `diagonalwise_sliceIteratorOrder` | 3.15 | 7.75 | 0.904 | 2.76 |
| `rowwise_sliceIteratorOrder` | 3.25 | 7.89 | 2.74 | 5.95 |
| `termCSR_sliceIteratorOrder` | 3.64 | 8.03 | 2.93 | 6.16 |
| `library_memoryOrder` | 2.53 | 5.95 | 0.784 | 1.54 |
| `diagonalwise_memoryOrder` | 2.54 | 5.94 | 0.797 | 1.58 |
| `rowwise_memoryOrder` | 2.33 | 3.80 | 2.08 | 2.58 |
| `termCSR_memoryOrder` | 2.81 | 4.09 | 2.32 | 2.64 |
| `eigen_sparse_kron` | 2.45 | 2.52 | 2.46 | 2.47 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.23 | 1.30 | 1.12 | 1.76 |
| `diagonalwise_sliceIteratorOrder` | 1.25 | 1.30 | 1.15 | 1.79 |
| `rowwise_sliceIteratorOrder` | 1.28 | 1.33 | 3.49 | 3.86 |
| `termCSR_sliceIteratorOrder` | 1.44 | 1.35 | 3.74 | 4.00 |
| `diagonalwise_memoryOrder` | 1.00 | 1.00 | 1.02 | 1.03 |
| `rowwise_memoryOrder` | 0.92 | 0.64 | 2.65 | 1.68 |
| `termCSR_memoryOrder` | 1.11 | 0.69 | 2.96 | 1.71 |
| `eigen_sparse_kron` | 0.97 | 0.42 | 3.14 | 1.60 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.927 | 1.34 | 1.29 | 1.32 | 1.27 | 1.24 | 1.14 | 0.994 | 0.924 |
| `hoisted` | 3.72 | 2.98 | 2.16 | 1.80 | 1.38 | 1.31 | 1.18 | 1.03 | 0.935 |
| `eigen_sparse` | 2.43 | 1.94 | 1.89 | 1.87 | 1.86 | 2.00 | 1.84 | 1.88 | 1.85 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `hoisted` | 4.01 | 2.22 | 1.67 | 1.36 | 1.09 | 1.06 | 1.04 | 1.04 | 1.01 |
| `eigen_sparse` | 2.62 | 1.45 | 1.47 | 1.42 | 1.46 | 1.61 | 1.61 | 1.89 | 2.00 |


## now: `kernels-cxlr`, pages as the system hands them out (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.646 | 0.805 | 1.53 |
| `hoisted` | 0.645 | 0.804 | 1.53 |
| `rowBlocked` | 0.639 | 0.805 | 1.19 |
| `eigen_sparse` | 1.62 | 1.77 | 2.19 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `hoisted` | 1.00 | 1.00 | 1.00 |
| `rowBlocked` | 0.99 | 1.00 | 0.78 |
| `eigen_sparse` | 2.51 | 2.20 | 1.43 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.594 | 0.935 | 3.33 |
| `hoisted` | 0.584 | 0.934 | 3.33 |
| `eigen_sparse` | 1.76 | 1.96 | 2.87 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `hoisted` | 0.98 | 1.00 | 1.00 |
| `eigen_sparse` | 2.96 | 2.10 | 0.86 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 2.22 | 1.69 |
| `blocked` | 3.39 | 1.28 |
| `library_materialized` | 2.65 | 2.31 |
| `eigen_sparse_kron` | 2.21 | 2.34 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 3.12 | 7.74 | 0.881 | 2.70 |
| `diagonalwise_sliceIteratorOrder` | 3.16 | 7.74 | 0.909 | 2.75 |
| `rowwise_sliceIteratorOrder` | 3.45 | 7.89 | 3.17 | 5.99 |
| `termCSR_sliceIteratorOrder` | 3.29 | 7.39 | 2.32 | 5.75 |
| `library_memoryOrder` | 2.53 | 5.94 | 0.784 | 1.55 |
| `diagonalwise_memoryOrder` | 2.55 | 5.93 | 0.799 | 1.59 |
| `rowwise_memoryOrder` | 2.60 | 3.84 | 2.62 | 2.80 |
| `termCSR_memoryOrder` | 2.48 | 3.69 | 1.69 | 2.16 |
| `eigen_sparse_kron` | 2.26 | 2.38 | 2.28 | 2.32 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.23 | 1.30 | 1.12 | 1.74 |
| `diagonalwise_sliceIteratorOrder` | 1.25 | 1.30 | 1.16 | 1.77 |
| `rowwise_sliceIteratorOrder` | 1.36 | 1.33 | 4.04 | 3.86 |
| `termCSR_sliceIteratorOrder` | 1.30 | 1.24 | 2.96 | 3.71 |
| `diagonalwise_memoryOrder` | 1.01 | 1.00 | 1.02 | 1.03 |
| `rowwise_memoryOrder` | 1.03 | 0.65 | 3.34 | 1.81 |
| `termCSR_memoryOrder` | 0.98 | 0.62 | 2.16 | 1.39 |
| `eigen_sparse_kron` | 0.89 | 0.40 | 2.91 | 1.50 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.924* | 1.36 | 1.29 | 1.35 | 1.27 | 1.23 | 1.16 | 0.988 | 0.915 |
| `hoisted` | 3.72 | 3.02 | 2.12 | 1.82 | 1.38 | 1.30 | 1.19 | 1.02 | 0.925 |
| `eigen_sparse` | 2.08 | 1.58 | 1.53 | 1.52 | 1.50 | 1.59 | 1.48 | 1.51 | 1.49 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `hoisted` | 4.03* | 2.22 | 1.64 | 1.35 | 1.09 | 1.06 | 1.03 | 1.03 | 1.01 |
| `eigen_sparse` | 2.25* | 1.16 | 1.19 | 1.13 | 1.18 | 1.29 | 1.28 | 1.53 | 1.63 |


## pre23: `main.apply1+applyJC`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 105 ns | 376 ns | 1.80 µs | 7.69 µs | 32.4 µs | 134 µs | 824 µs* | 2.88 ms* | 20.9 ms* |
| `eigen_sparse` | 157 ns | 657 ns | 2.64 µs | 10.7 µs | 43.3 µs | 173 µs | 744 µs | 3.42 ms | 13.3 ms |
| `eigen_dense` | 87.2 ns | 1.28 µs | 27.5 µs | 780 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.50 | 1.75 | 1.47 | 1.39 | 1.34 | 1.30 | 0.90* | 1.19* | 0.63* |
| `eigen_dense` | 0.83 | 3.40 | 15.3 | 101 | 410 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 232 ns | 915 ns | 3.59 µs | 15.8 µs | 65.4 µs | 264 µs | 2.53 ms | 8.18 ms | 40.5 ms |
| `eigen_sparse` | 260 ns | 1.05 µs | 4.26 µs | 17.0 µs | 70.3 µs | 275 µs | 1.29 ms | 5.62 ms | 21.0 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.12 | 1.14 | 1.19 | 1.07 | 1.08 | 1.04 | 0.51 | 0.69 | 0.52 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 176 ns | 642 ns | 2.69 µs | 12.7 µs | 50.4 µs | 204 µs | 1.87 ms* | 5.00 ms | 36.8 ms |
| `eigen_sparse` | 253 ns | 1.01 µs | 4.05 µs | 17.2 µs | 69.6 µs | 278 µs | 1.89 ms | 5.51 ms | 22.8 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.43 | 1.57 | 1.50 | 1.36 | 1.38 | 1.36 | 1.01* | 1.10 | 0.62 |


## pre23: `kernels.A+B+C`, pages as the system hands them out (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 1.50 | 1.58 | 2.14 |
| `hoisted` | 1.37 | 1.59 | 2.14 |
| `rowBlocked` | 1.33 | 1.56 | 1.90 |
| `eigen_sparse` | 2.02 | 2.06 | 2.47 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `hoisted` | 0.91 | 1.01 | 1.00 |
| `rowBlocked` | 0.89 | 0.99 | 0.89 |
| `eigen_sparse` | 1.35 | 1.30 | 1.15 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 1.27 | 1.63 | 4.34 |
| `hoisted` | 1.24 | 1.63 | 4.34 |
| `eigen_sparse` | 2.31 | 2.35 | 3.11 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `hoisted` | 0.98 | 1.00 | 1.00 |
| `eigen_sparse` | 1.82 | 1.44 | 0.72 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 3.02 | 2.63 |
| `blocked` | 2.92 | 2.31 |
| `library_materialized` | 3.37 | 3.25 |
| `eigen_sparse_kron` | 2.67 | 2.73 |


## pre23: `kernels-cxlr.A+B`, pages as the system hands them out (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.642 | 0.784* | 1.55 |
| `hoisted` | 0.632 | 0.788* | 1.57 |
| `rowBlocked` | 0.638 | 0.795* | 1.20 |
| `eigen_sparse` | 1.65 | 1.79 | 2.19 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `hoisted` | 0.98 | 1.01* | 1.01 |
| `rowBlocked` | 0.99 | 1.01* | 0.77 |
| `eigen_sparse` | 2.57 | 2.28* | 1.41 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.594 | 0.937 | 3.35* |
| `hoisted` | 0.571 | 0.935 | 3.32* |
| `eigen_sparse` | 1.76 | 1.96 | 2.88* |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `hoisted` | 0.96 | 1.00 | 0.99* |
| `eigen_sparse` | 2.96 | 2.09 | 0.86* |


## v040: `main.apply1+applyJC`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 127 ns | 429 ns | 1.83 µs | 7.83 µs | 32.7 µs | 138 µs | 574 µs* | 3.05 ms* | 20.7 ms* |
| `eigen_sparse` | 132 ns | 556 ns | 2.29 µs | 9.59 µs | 40.0 µs | 159 µs | 688 µs | 3.31 ms | 12.5 ms |
| `eigen_dense` | 90.0 ns | 1.28 µs | 27.0 µs | 777 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.04 | 1.30 | 1.25 | 1.23 | 1.22 | 1.16 | 1.20* | 1.09* | 0.60* |
| `eigen_dense` | 0.71 | 2.97 | 14.7 | 99.3 | 407 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 1.04 µs | 3.71 µs | 14.6 µs | 58.7 µs | 238 µs | 954 µs | 4.15 ms | 17.2 ms | 72.1 ms |
| `eigen_sparse` | 230 ns | 928 ns | 3.95 µs | 15.7 µs | 65.8 µs | 266 µs | 1.26 ms | 5.43 ms | 20.5 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 0.22 | 0.25 | 0.27 | 0.27 | 0.28 | 0.28 | 0.30 | 0.31 | 0.28 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 922 ns | 3.52 µs | 14.0 µs | 57.2 µs | 238 µs | 952 µs | 3.89 ms | 15.9 ms | 65.5 ms |
| `eigen_sparse` | 228 ns | 929 ns | 3.91 µs | 16.8 µs | 68.1 µs | 272 µs | 1.73 ms | 5.47 ms | 22.6 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 0.25 | 0.26 | 0.28 | 0.29 | 0.29 | 0.29 | 0.44 | 0.34 | 0.35 |


## v040: `kernels.B+C`, pages as the system hands them out (5 processes)

### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 6.95 | 7.24 | 7.77 |
| `hoisted` | 1.44 | 1.59 | 3.85* |
| `eigen_sparse` | 2.11 | 2.30 | 4.21 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `hoisted` | 0.21 | 0.22 | 0.50* |
| `eigen_sparse` | 0.30 | 0.32 | 0.54 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 3.82 | 3.24 |
| `blocked` | 3.78 | 2.93 |
| `library_materialized` | 47.7 | 46.6 |
| `eigen_sparse_kron` | 2.75 | 2.57 |


