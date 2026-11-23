# Session 2026-11-04-bench-fb8ca7f

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| prev | main.apply1+applyJC | thp | 5 | 59 | 0.1% | 3.3% | 56.7% | 9 | 0.995–1.009 |
| prev | kernels | thp | 5 | 92 | 0.0% | 1.6% | 10.2% | 1 | 0.997–1.003 |
| step3 | main | thp | 5 | 180 | 0.1% | 1.2% | 60.2% | 13 | 1.002–1.006 |
| step3 | kernels | thp | 5 | 134 | 0.2% | 1.6% | 9.0% | 1 | 0.998–1.006 |
| step3 | kernels-cxlr | thp | 5 | 134 | 0.2% | 1.9% | 22.6% | 7 | 1.000–1.007 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## prev: `main.apply1+applyJC`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 68.3 ns | 170 ns | 832 ns | 3.65 µs* | 16.8 µs | 83.3 µs | 819 µs* | 2.45 ms* | 18.0 ms* |
| `eigen_sparse` | 131 ns | 549 ns | 2.27 µs | 9.53 µs | 39.9 µs | 159 µs | 685 µs | 3.31 ms | 12.4 ms |
| `eigen_dense` | 88.0 ns | 1.27 µs | 27.5 µs | 777 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.92 | 3.24 | 2.73 | 2.61* | 2.38 | 1.91 | 0.84* | 1.35* | 0.69* |
| `eigen_dense` | 1.29 | 7.49 | 33.1 | 213* | 791 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 184 ns | 495 ns | 2.05 µs | 9.91 µs* | 43.4 µs | 182 µs | 2.18 ms | 7.07 ms* | 35.7 ms |
| `eigen_sparse` | 276 ns | 1.11 µs | 4.43 µs | 17.8 µs | 72.3 µs | 288 µs | 1.34 ms | 5.64 ms | 22.0 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.50 | 2.23 | 2.17 | 1.79* | 1.67 | 1.58 | 0.62 | 0.80* | 0.62 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 124 ns | 290 ns | 1.21 µs | 6.27 µs* | 30.3 µs* | 134 µs | 1.34 ms* | 4.07 ms | 28.3 ms |
| `eigen_sparse` | 270 ns | 1.11 µs | 4.43 µs | 17.8 µs | 73.0 µs | 290 µs | 1.73 ms | 5.71 ms | 23.3 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 2.17 | 3.84 | 3.67 | 2.83* | 2.40* | 2.17 | 1.29* | 1.40 | 0.82 |


## prev: `kernels`, pages as the system hands them out (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.646 | 0.807 | 1.54 |
| `hoisted` | 0.644 | 0.806 | 1.54 |
| `rowBlocked` | 0.640 | 0.803 | 1.20 |
| `eigen_sparse` | 1.80 | 1.95 | 2.36 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `hoisted` | 1.00 | 1.00 | 1.00 |
| `rowBlocked` | 0.99 | 1.00 | 0.78 |
| `eigen_sparse` | 2.79 | 2.42 | 1.53 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.592 | 0.935 | 3.33 |
| `hoisted` | 0.585 | 0.936 | 3.33 |
| `eigen_sparse` | 1.96 | 2.18 | 3.05 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `hoisted` | 0.99 | 1.00 | 1.00 |
| `eigen_sparse` | 3.31 | 2.33 | 0.92 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 2.21 | 1.69 |
| `blocked` | 3.39 | 1.27 |
| `library_materialized` | 2.65 | 2.31 |
| `eigen_sparse_kron` | 2.58 | 2.61 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 3.11 | 7.73 | 0.882 | 2.71 |
| `diagonalwise_sliceIteratorOrder` | 3.15 | 7.77 | 0.904 | 2.76 |
| `rowwise_sliceIteratorOrder` | 3.25 | 7.86 | 2.74 | 5.96 |
| `termCSR_sliceIteratorOrder` | 3.64 | 8.03 | 2.92 | 6.16 |
| `library_memoryOrder` | 2.53 | 5.95 | 0.783 | 1.55 |
| `diagonalwise_memoryOrder` | 2.54 | 5.94 | 0.797 | 1.59 |
| `rowwise_memoryOrder` | 2.33 | 3.82 | 2.08 | 2.59 |
| `termCSR_memoryOrder` | 2.82 | 4.09 | 2.32 | 2.64 |
| `eigen_sparse_kron` | 2.44 | 2.52 | 2.47 | 2.46 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.23 | 1.30 | 1.13 | 1.75 |
| `diagonalwise_sliceIteratorOrder` | 1.25 | 1.31 | 1.15 | 1.78 |
| `rowwise_sliceIteratorOrder` | 1.28 | 1.32 | 3.50 | 3.85 |
| `termCSR_sliceIteratorOrder` | 1.44 | 1.35 | 3.73 | 3.97 |
| `diagonalwise_memoryOrder` | 1.00 | 1.00 | 1.02 | 1.03 |
| `rowwise_memoryOrder` | 0.92 | 0.64 | 2.66 | 1.67 |
| `termCSR_memoryOrder` | 1.11 | 0.69 | 2.96 | 1.70 |
| `eigen_sparse_kron` | 0.96 | 0.42 | 3.15 | 1.59 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.935* | 1.34 | 1.28 | 1.35 | 1.30 | 1.27 | 1.17 | 1.00 | 0.919 |
| `hoisted` | 3.71 | 2.98 | 2.17 | 1.82 | 1.41 | 1.34 | 1.21 | 1.03 | 0.932 |
| `eigen_sparse` | 2.38 | 1.94 | 1.89 | 1.87 | 1.86 | 1.86 | 1.84 | 1.88 | 1.85 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `hoisted` | 3.97* | 2.22 | 1.70 | 1.35 | 1.08 | 1.06 | 1.03 | 1.03 | 1.01 |
| `eigen_sparse` | 2.55* | 1.45 | 1.48 | 1.39 | 1.43 | 1.46 | 1.57 | 1.88 | 2.01 |


## step3: `main`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.7 ns | 171 ns | 695 ns | 3.04 µs* | 15.2 µs | 59.1 µs | 314 µs* | 1.42 ms | 9.10 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.29 µs | 9.73 µs | 40.3 µs | 161 µs | 703 µs | 3.48 ms | 12.4 ms |
| `eigen_dense` | 87.2 ns | 1.28 µs | 26.4 µs | 772 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.84 | 3.25 | 3.29 | 3.20* | 2.66 | 2.73 | 2.24* | 2.45 | 1.37 |
| `eigen_dense` | 1.22 | 7.47 | 38.0 | 254* | 879 |  |  |  |  |


### `compose1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 503 ns | 1.14 µs | 4.11 µs | 15.3 µs | 63.7 µs | 253 µs | 1.32 ms | 6.59 ms | 35.4 ms |
| `eigen_sparse` | 1.61 µs | 6.02 µs | 26.9 µs | 108 µs | 424 µs | 1.76 ms | 7.52 ms | 34.6 ms | 140 ms |
| `eigen_dense` | 1.57 µs | 79.8 µs | 4.94 ms | 327 ms |  |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 3.20 | 5.30 | 6.56 | 7.03 | 6.66 | 6.96 | 5.69 | 5.25 | 3.96 |
| `eigen_dense` | 3.13 | 70.2 | 1202 | 21357 |  |  |  |  |  |


### `construct1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 1.42 µs | 6.27 µs | 28.6 µs | 116 µs | 466 µs | 1.86 ms | 7.47 ms | 30.6 ms | 125 ms |
| `eigen_sparse` | 2.32 µs | 9.48 µs | 43.8 µs | 192 µs | 782 µs | 3.13 ms | 13.5 ms | 71.6 ms | 260 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.64 | 1.51 | 1.53 | 1.65 | 1.68 | 1.68 | 1.81 | 2.34 | 2.08 |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 174 ns | 503 ns | 2.17 µs* | 9.94 µs* | 45.2 µs | 189 µs | 1.64 ms* | 7.03 ms | 36.0 ms |
| `eigen_sparse` | 233 ns | 925 ns | 3.93 µs | 16.1 µs | 66.0 µs | 264 µs | 1.43 ms | 5.46 ms | 21.3 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.34 | 1.84 | 1.81* | 1.61* | 1.46 | 1.39 | 0.87* | 0.78 | 0.59 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 125 ns | 323 ns | 1.27 µs* | 4.96 µs | 24.5 µs | 95.4 µs | 830 µs* | 2.95 ms | 14.7 ms |
| `eigen_sparse` | 225 ns | 929 ns | 4.10 µs | 16.9 µs | 70.9 µs | 283 µs | 1.73 ms | 5.43 ms | 22.6 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.80 | 2.88 | 3.23* | 3.40 | 2.89 | 2.97 | 2.09* | 1.84 | 1.54 |


### `composeJC`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 4.49 µs | 15.2 µs | 57.8 µs | 231 µs | 937 µs | 3.75 ms | 16.1 ms | 65.6 ms | 257 ms |
| `eigen_sparse` | 4.25 µs | 20.2 µs | 90.2 µs | 373 µs | 1.49 ms | 6.39 ms | 26.8 ms | 122 ms | 508 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 0.95 | 1.33 | 1.56 | 1.62 | 1.59 | 1.70 | 1.67 | 1.85 | 1.98 |


### `constructJC`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 3.35 µs | 10.1 µs | 36.6 µs | 147 µs | 616 µs | 2.50 ms | 11.3 ms | 67.7 ms | 224 ms |
| `eigen_sparse` | 2.01 µs | 7.68 µs | 39.0 µs | 164 µs | 664 µs | 2.73 ms | 12.9 ms | 71.3 ms | 266 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 0.60 | 0.76 | 1.07 | 1.11 | 1.08 | 1.09 | 1.14 | 1.05 | 1.19 |


### `picture1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal_static` | 183 ns | 726 ns | 2.92 µs | 11.5 µs | 46.8 µs | 189 µs | 772 µs | 3.66 ms | 16.4 ms |
| `multidiagonal_scalarfreq` | 278 ns | 727 ns | 2.78 µs | 11.3 µs | 46.5 µs | 186 µs | 761 µs | 3.69 ms | 16.9 ms |
| `multidiagonal_tabfreq` | 1.56 µs* | 6.43 µs* | 26.1 µs* | 98.8 µs | 366 µs | 1.47 ms | 5.67 ms | 23.0 ms | 93.2 ms |
| `eigen_sparse_pernnz` | 1.56 µs* | 6.61 µs* | 27.0 µs* | 103 µs | 380 µs | 1.52 ms | 5.97 ms | 24.0 ms | 95.2 ms |
| `eigen_sparse_sandwich` | 237 ns | 902 ns | 3.67 µs | 15.9 µs | 65.2 µs | 262 µs | 1.16 ms | 5.48 ms | 23.3 ms |


## step3: `kernels`, pages as the system hands them out (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.576 | 0.709 | 1.06 |
| `library_byDiagonals` | 0.643 | 0.797 | 1.56 |
| `library_byRows` | 0.577 | 0.708 | 1.07 |
| `hoisted` | 0.642 | 0.796 | 1.57 |
| `rowBlocked` | 0.639 | 0.800 | 1.21 |
| `eigen_sparse` | 2.02 | 2.07 | 2.47 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `library_byDiagonals` | 1.12 | 1.12 | 1.47 |
| `library_byRows` | 1.00 | 1.00 | 1.01 |
| `hoisted` | 1.11 | 1.12 | 1.48 |
| `rowBlocked` | 1.11 | 1.13 | 1.14 |
| `eigen_sparse` | 3.51 | 2.92 | 2.33 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.576 | 0.751 | 1.99 |
| `library_byDiagonals` | 0.582 | 0.961 | 3.33 |
| `library_byRows` | 0.574 | 0.752 | 2.00 |
| `hoisted` | 0.582 | 0.961 | 3.33 |
| `eigen_sparse` | 2.01 | 2.23 | 3.22 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.01 | 1.28 | 1.67 |
| `library_byRows` | 1.00 | 1.00 | 1.01 |
| `hoisted` | 1.01 | 1.28 | 1.67 |
| `eigen_sparse` | 3.49 | 2.97 | 1.62 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 2.21* | 1.70 |
| `blocked` | 3.40 | 1.28 |
| `library_materialized` | 2.07 | 1.52 |
| `library_materialized_byDiagonals` | 2.66 | 2.32 |
| `library_materialized_byRows` | 2.07 | 1.52 |
| `eigen_sparse_kron` | 2.71 | 2.73 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 3.33 | 4.84 | 1.04 | 3.50 |
| `diagonalwise_sliceIteratorOrder` | 3.15 | 7.77 | 0.904 | 2.74 |
| `rowwise_sliceIteratorOrder` | 3.46 | 8.00 | 3.18 | 6.08 |
| `termCSR_sliceIteratorOrder` | 3.49 | 8.15 | 2.98 | 6.17 |
| `library_memoryOrder` | 2.64 | 2.36 | 0.897 | 1.57 |
| `diagonalwise_memoryOrder` | 2.55 | 5.94 | 0.797 | 1.53 |
| `rowwise_memoryOrder` | 2.60 | 3.96 | 2.63 | 2.91 |
| `termCSR_memoryOrder` | 2.64 | 4.07 | 2.38 | 2.67 |
| `library_byDiagonals_memoryOrder` | 2.63 | 6.00 | 0.886 | 1.70 |
| `library_byRows_memoryOrder` | 1.84 | 2.32 | 1.24 | 1.54 |
| `eigen_sparse_kron` | 2.57 | 2.61 | 2.58 | 2.57 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.26 | 2.05 | 1.16 | 2.23 |
| `diagonalwise_sliceIteratorOrder` | 1.19 | 3.29 | 1.01 | 1.75 |
| `rowwise_sliceIteratorOrder` | 1.31 | 3.39 | 3.55 | 3.87 |
| `termCSR_sliceIteratorOrder` | 1.32 | 3.45 | 3.32 | 3.93 |
| `diagonalwise_memoryOrder` | 0.97 | 2.52 | 0.89 | 0.97 |
| `rowwise_memoryOrder` | 0.98 | 1.68 | 2.93 | 1.85 |
| `termCSR_memoryOrder` | 1.00 | 1.72 | 2.65 | 1.70 |
| `library_byDiagonals_memoryOrder` | 1.00 | 2.54 | 0.99 | 1.08 |
| `library_byRows_memoryOrder` | 0.70 | 0.98 | 1.38 | 0.98 |
| `eigen_sparse_kron` | 0.97 | 1.11 | 2.88 | 1.64 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.939 | 1.32 | 1.27 | 1.38 | 1.31 | 1.28 | 1.18 | 1.01 | 0.914 |
| `library_byDiagonals` | 0.939 | 1.32 | 1.28 | 1.38 | 1.31 | 1.28 | 1.17 | 1.01 | 0.929 |
| `library_byRows` | 9.65 | 6.21 | 5.32 | 3.88 | 3.21 | 2.61 | 2.02 | 1.26 | 0.912 |
| `hoisted` | 3.74 | 2.98 | 2.13 | 1.82 | 1.42 | 1.35 | 1.21 | 1.04 | 0.940 |
| `eigen_sparse` | 2.38 | 2.05 | 1.95 | 1.92 | 1.90 | 1.89 | 1.88 | 1.91 | 1.88 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.00 | 1.00 | 1.01 | 1.00 | 1.00 | 1.00 | 0.99 | 1.00 | 1.02 |
| `library_byRows` | 10.3 | 4.70 | 4.19 | 2.81 | 2.45 | 2.04 | 1.71 | 1.25 | 1.00 |
| `hoisted` | 3.98 | 2.26 | 1.68 | 1.32 | 1.08 | 1.05 | 1.03 | 1.03 | 1.03 |
| `eigen_sparse` | 2.53 | 1.55 | 1.54 | 1.39 | 1.45 | 1.48 | 1.59 | 1.89 | 2.06 |


## step3: `kernels-cxlr`, pages as the system hands them out (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.577 | 0.712 | 1.07* |
| `library_byDiagonals` | 0.647 | 0.814 | 1.56 |
| `library_byRows` | 0.577 | 0.711 | 1.06* |
| `hoisted` | 0.643 | 0.813 | 1.56* |
| `rowBlocked` | 0.641 | 0.807 | 1.23 |
| `eigen_sparse` | 1.63 | 1.79 | 2.20 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `library_byDiagonals` | 1.12 | 1.14 | 1.46* |
| `library_byRows` | 1.00 | 1.00 | 0.99* |
| `hoisted` | 1.11 | 1.14 | 1.46* |
| `rowBlocked` | 1.11 | 1.13 | 1.15* |
| `eigen_sparse` | 2.82 | 2.51 | 2.06* |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.577 | 0.731 | 1.92 |
| `library_byDiagonals` | 0.580 | 0.931* | 3.31 |
| `library_byRows` | 0.576 | 0.732 | 1.92 |
| `hoisted` | 0.582 | 0.931* | 3.31 |
| `eigen_sparse` | 1.80 | 1.98 | 3.07* |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.01 | 1.27* | 1.72 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.01 | 1.27* | 1.72 |
| `eigen_sparse` | 3.12 | 2.71 | 1.60* |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 2.19* | 1.69 |
| `blocked` | 3.38 | 1.28 |
| `library_materialized` | 2.07 | 1.52 |
| `library_materialized_byDiagonals` | 2.64 | 2.31 |
| `library_materialized_byRows` | 2.07 | 1.52 |
| `eigen_sparse_kron` | 2.25 | 2.35 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 3.28 | 4.81 | 1.04 | 3.53 |
| `diagonalwise_sliceIteratorOrder` | 3.12 | 7.76 | 0.905 | 2.76 |
| `rowwise_sliceIteratorOrder` | 3.22 | 7.82 | 2.73 | 5.96 |
| `termCSR_sliceIteratorOrder` | 3.28 | 7.38 | 2.35 | 5.75 |
| `library_memoryOrder` | 2.64 | 2.37 | 0.895 | 1.58 |
| `diagonalwise_memoryOrder` | 2.54 | 5.95 | 0.797 | 1.55 |
| `rowwise_memoryOrder` | 2.32 | 3.80 | 2.07 | 2.63 |
| `termCSR_memoryOrder` | 2.49 | 3.71 | 1.73 | 2.16 |
| `library_byDiagonals_memoryOrder` | 2.63 | 6.01 | 0.887 | 1.71 |
| `library_byRows_memoryOrder` | 1.83 | 2.33 | 1.24 | 1.55 |
| `eigen_sparse_kron` | 2.29 | 2.39 | 2.30 | 2.33 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.24 | 2.03 | 1.16 | 2.23 |
| `diagonalwise_sliceIteratorOrder` | 1.18 | 3.27 | 1.01 | 1.75 |
| `rowwise_sliceIteratorOrder` | 1.22 | 3.30 | 3.05 | 3.77 |
| `termCSR_sliceIteratorOrder` | 1.24 | 3.11 | 2.63 | 3.64 |
| `diagonalwise_memoryOrder` | 0.96 | 2.51 | 0.89 | 0.98 |
| `rowwise_memoryOrder` | 0.88 | 1.60 | 2.31 | 1.66 |
| `termCSR_memoryOrder` | 0.94 | 1.57 | 1.93 | 1.37 |
| `library_byDiagonals_memoryOrder` | 1.00 | 2.54 | 0.99 | 1.08 |
| `library_byRows_memoryOrder` | 0.69 | 0.98 | 1.39 | 0.98 |
| `eigen_sparse_kron` | 0.87 | 1.01 | 2.57 | 1.47 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.932 | 1.34 | 1.29 | 1.33 | 1.27 | 1.24 | 1.15 | 0.997 | 0.913 |
| `library_byDiagonals` | 0.932 | 1.34 | 1.29 | 1.33 | 1.27 | 1.24 | 1.15 | 0.996 | 0.924 |
| `library_byRows` | 9.76 | 6.10 | 5.30 | 3.89 | 3.22 | 2.58 | 2.03 | 1.27 | 0.914 |
| `hoisted` | 3.72 | 2.98 | 2.12 | 1.80 | 1.38 | 1.31 | 1.19 | 1.03 | 0.936 |
| `eigen_sparse` | 2.09 | 1.58 | 1.53 | 1.52 | 1.50 | 1.66 | 1.48 | 1.52 | 1.50 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.01 |
| `library_byRows` | 10.5 | 4.55 | 4.11 | 2.92 | 2.54 | 2.08 | 1.77 | 1.27 | 1.00 |
| `hoisted` | 3.99 | 2.22 | 1.64 | 1.35 | 1.09 | 1.06 | 1.03 | 1.03 | 1.03 |
| `eigen_sparse` | 2.24 | 1.18 | 1.19 | 1.14 | 1.18 | 1.34 | 1.29 | 1.52 | 1.64 |


