# Session 2026-11-04-bench-remeasure

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| remeasure | main.apply1+applyJC | thp | 5 | 59 | 0.2% | 2.0% | 36.5% | 3 | 0.997–1.005 |
| remeasure | main.apply1+applyJC | 4k | 5 | 59 | 0.1% | 1.5% | 7.4% | 0 | 1.000–1.005 |
| remeasure | kernels | thp | 5 | 230 | 0.2% | 1.7% | 47.3% | 19 | 0.998–1.005 |
| remeasure | kernels | 4k | 5 | 230 | 0.0% | 1.9% | 41.5% | 4 | 0.997–1.008 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## remeasure: `main.apply1+applyJC`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.4 ns | 174 ns | 697 ns | 3.09 µs | 14.6 µs | 59.9 µs | 398 µs* | 1.41 ms | 9.05 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.29 µs | 9.70 µs | 40.5 µs | 162 µs | 699 µs | 3.46 ms | 12.5 ms |
| `eigen_dense` | 88.5 ns | 1.28 µs | 27.8 µs | 781 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.85 | 3.20 | 3.29 | 3.14 | 2.78 | 2.70 | 1.76* | 2.44 | 1.39 |
| `eigen_dense` | 1.24 | 7.36 | 39.9 | 253 | 914 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 177 ns | 486 ns | 2.04 µs | 9.86 µs | 43.9 µs | 185 µs | 2.21 ms | 6.98 ms* | 35.5 ms |
| `eigen_sparse` | 230 ns | 928 ns | 3.95 µs | 15.9 µs | 66.3 µs | 262 µs | 1.26 ms | 5.43 ms | 20.5 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.30 | 1.91 | 1.94 | 1.61 | 1.51 | 1.42 | 0.57 | 0.78* | 0.58 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 127 ns | 292 ns | 1.22 µs | 4.92 µs | 25.5 µs | 99.8 µs | 830 µs* | 2.96 ms | 13.9 ms |
| `eigen_sparse` | 225 ns | 928 ns | 3.89 µs | 16.7 µs | 68.2 µs | 271 µs | 1.75 ms | 5.45 ms | 22.6 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.77 | 3.18 | 3.18 | 3.40 | 2.68 | 2.72 | 2.11* | 1.84 | 1.62 |


## remeasure: `main.apply1+applyJC`, 4 KB pages (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.5 ns | 174 ns | 702 ns | 3.04 µs | 14.5 µs | 58.8 µs | 253 µs | 1.39 ms | 6.31 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.29 µs | 9.72 µs | 40.5 µs | 162 µs | 713 µs | 3.18 ms | 12.7 ms |
| `eigen_dense` | 87.2 ns | 1.28 µs | 27.8 µs | 741 µs | 13.3 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.84 | 3.19 | 3.27 | 3.19 | 2.79 | 2.75 | 2.82 | 2.29 | 2.01 |
| `eigen_dense` | 1.22 | 7.35 | 39.6 | 243 | 920 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 174 ns | 485 ns | 2.03 µs | 9.87 µs | 43.6 µs | 187 µs | 930 µs | 5.09 ms | 23.0 ms |
| `eigen_sparse` | 230 ns | 928 ns | 3.95 µs | 15.9 µs | 66.3 µs | 264 µs | 1.26 ms | 5.45 ms | 20.6 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.32 | 1.91 | 1.94 | 1.61 | 1.52 | 1.42 | 1.36 | 1.07 | 0.89 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 127 ns | 293 ns | 1.23 µs | 4.96 µs | 25.5 µs | 101 µs | 505 µs | 2.69 ms | 9.70 ms |
| `eigen_sparse` | 225 ns | 928 ns | 3.89 µs | 16.8 µs | 68.2 µs | 273 µs | 1.33 ms | 5.55 ms | 22.3 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.77 | 3.17 | 3.17 | 3.38 | 2.68 | 2.72 | 2.63 | 2.06 | 2.30 |


## remeasure: `kernels`, pages as the system hands them out (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.579 | 0.713 | 1.04* |
| `library_byDiagonals` | 0.646 | 0.781 | 1.54 |
| `library_byRows` | 0.576 | 0.712 | 1.04* |
| `hoisted` | 0.644 | 0.781 | 1.56 |
| `rowBlocked` | 0.639 | 0.802 | 1.20 |
| `eigen_sparse` | 2.02 | 2.06 | 2.48 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `library_byDiagonals` | 1.12 | 1.10 | 1.48* |
| `library_byRows` | 0.99 | 1.00 | 1.00* |
| `hoisted` | 1.11 | 1.10 | 1.50* |
| `rowBlocked` | 1.10 | 1.12 | 1.15* |
| `eigen_sparse` | 3.49 | 2.89 | 2.38* |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.607 | 0.726 | 1.67 |
| `library_byDiagonals` | 0.642 | 0.908 | 2.36 |
| `library_byRows` | 0.605 | 0.728 | 1.67* |
| `hoisted` | 0.633 | 0.908 | 2.37* |
| `eigen_sparse` | 1.95 | 2.15 | 2.67 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.06 | 1.25 | 1.41 |
| `library_byRows` | 1.00 | 1.00 | 1.00* |
| `hoisted` | 1.04 | 1.25 | 1.42* |
| `eigen_sparse` | 3.21 | 2.96 | 1.60 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 3.02 | 2.00 |
| `blocked` | 3.95 | 1.60 |
| `library_materialized` | 2.28 | 1.66 |
| `library_materialized_byDiagonals` | 3.31 | 2.51 |
| `library_materialized_byRows` | 2.28 | 1.66 |
| `eigen_sparse_kron` | 2.43 | 2.51 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 3.35 | 4.87 | 1.05 | 3.51 |
| `diagonalwise_sliceIteratorOrder` | 3.18 | 7.78 | 0.906 | 2.75 |
| `rowwise_sliceIteratorOrder` | 3.49 | 8.06 | 3.18 | 6.10 |
| `termCSR_sliceIteratorOrder` | 3.67 | 8.10 | 2.88 | 6.19 |
| `library_memoryOrder` | 2.65 | 2.36 | 0.896 | 1.57 |
| `diagonalwise_memoryOrder` | 2.54 | 5.97 | 0.797 | 1.55 |
| `rowwise_memoryOrder` | 2.61 | 3.98 | 2.63 | 2.91 |
| `termCSR_memoryOrder` | 2.81 | 4.15 | 2.28 | 2.60 |
| `library_byDiagonals_memoryOrder` | 2.64 | 6.04 | 0.885 | 1.70 |
| `library_byRows_memoryOrder` | 1.87 | 2.33 | 1.25 | 1.54 |
| `eigen_sparse_kron` | 2.61 | 2.63 | 2.64 | 2.61 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.26 | 2.06 | 1.17 | 2.24 |
| `diagonalwise_sliceIteratorOrder` | 1.20 | 3.30 | 1.01 | 1.75 |
| `rowwise_sliceIteratorOrder` | 1.32 | 3.42 | 3.55 | 3.89 |
| `termCSR_sliceIteratorOrder` | 1.38 | 3.43 | 3.21 | 3.94 |
| `diagonalwise_memoryOrder` | 0.96 | 2.53 | 0.89 | 0.99 |
| `rowwise_memoryOrder` | 0.98 | 1.69 | 2.94 | 1.85 |
| `termCSR_memoryOrder` | 1.06 | 1.76 | 2.54 | 1.66 |
| `library_byDiagonals_memoryOrder` | 1.00 | 2.56 | 0.99 | 1.08 |
| `library_byRows_memoryOrder` | 0.71 | 0.99 | 1.40 | 0.98 |
| `eigen_sparse_kron` | 0.98 | 1.11 | 2.95 | 1.66 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.956 | 1.35 | 1.28 | 1.31 | 1.27 | 1.23 | 1.14 | 0.988 | 0.902 |
| `library_byDiagonals` | 0.954 | 1.35 | 1.28 | 1.31 | 1.27 | 1.23 | 1.14 | 0.990 | 0.910 |
| `library_byRows` | 9.78 | 5.93 | 5.24 | 3.79 | 3.15 | 2.54 | 1.98 | 1.25 | 0.903 |
| `hoisted` | 3.73 | 3.00 | 2.14 | 1.79 | 1.37 | 1.30 | 1.18 | 1.02 | 0.920 |
| `eigen_sparse` | 2.38 | 1.95 | 1.89 | 1.88 | 1.86 | 1.99 | 1.85 | 1.88 | 1.85 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.01 |
| `library_byRows` | 10.2 | 4.39 | 4.09 | 2.89 | 2.48 | 2.07 | 1.74 | 1.27 | 1.00 |
| `hoisted` | 3.90 | 2.22 | 1.67 | 1.37 | 1.08 | 1.06 | 1.04 | 1.03 | 1.02 |
| `eigen_sparse` | 2.49 | 1.44 | 1.48 | 1.44 | 1.46 | 1.62 | 1.62 | 1.90 | 2.05 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.70* | 1.04* | 0.802 | 0.763* | 0.634 | 0.625* | 0.541 | 0.562* | 0.629 |
| `library_byRows` | 2.90 | 1.61 | 1.16 | 1.03 | 0.795 | 0.766* | 0.615 | 0.639* | 0.593 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 3.02* | 2.50* | 0.946 | 2.33* | 0.809 | 2.21* | 0.756 | 2.16* | 2.18* |
| `library_byRows` | 3.19 | 2.10 | 1.37 | 1.77 | 1.18 | 1.64 | 1.56 | 2.01 | 1.31* |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.26 | 1.14 | 1.03 | 0.991 | 0.928 | 0.908 | 0.885 | 0.879 | 0.865 | 0.880 |
| `library_byRows` | 3.14 | 1.99 | 1.48 | 1.24 | 1.02 | 0.901 | 0.817 | 0.772 | 0.698 | 0.650 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.16 | 2.12 | 1.34 | 1.96 | 1.26 | 1.87 | 1.18 | 1.84 | 1.84 | 1.87 |
| `library_byRows` | 3.34 | 2.46 | 1.63 | 1.77 | 1.22 | 1.43 | 1.25 | 1.40 | 1.44 | 1.18 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.92 | 2.84 | 2.35* | 2.66 | 2.15 | 2.57 | 2.15 | 2.56 | 2.49 | 2.56 |
| `library_byRows` | 3.50 | 2.54 | 1.93 | 1.96 | 1.76 | 1.79 | 1.91 | 1.92 | 2.10 | 1.56 |


## remeasure: `kernels`, 4 KB pages (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.578 | 0.709 | 1.02 |
| `library_byDiagonals` | 0.647 | 0.810 | 1.64 |
| `library_byRows` | 0.576 | 0.710 | 1.02 |
| `hoisted` | 0.642 | 0.811 | 1.64 |
| `rowBlocked` | 0.638 | 0.801 | 1.24 |
| `eigen_sparse` | 2.02 | 2.06 | 2.48 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `library_byDiagonals` | 1.12 | 1.14 | 1.61 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.11 | 1.14 | 1.61 |
| `rowBlocked` | 1.10 | 1.13 | 1.22 |
| `eigen_sparse` | 3.49 | 2.91 | 2.43 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.605 | 0.725 | 1.27 |
| `library_byDiagonals` | 0.642 | 0.927 | 2.01 |
| `library_byRows` | 0.604 | 0.728 | 1.26 |
| `hoisted` | 0.634 | 0.928 | 2.01 |
| `eigen_sparse` | 1.95 | 2.15 | 2.72 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.06 | 1.28 | 1.58 |
| `library_byRows` | 1.00 | 1.00 | 0.99 |
| `hoisted` | 1.05 | 1.28 | 1.58 |
| `eigen_sparse` | 3.22 | 2.97 | 2.14 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 1.61 | 1.51 |
| `blocked` | 2.96 | 1.04 |
| `library_materialized` | 1.95 | 1.45 |
| `library_materialized_byDiagonals` | 2.20 | 2.21 |
| `library_materialized_byRows` | 1.95 | 1.45 |
| `eigen_sparse_kron` | 2.45 | 2.57 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.54 | 5.45 | 1.15 | 4.30 |
| `diagonalwise_sliceIteratorOrder` | 1.47 | 7.97 | 1.07 | 3.58 |
| `rowwise_sliceIteratorOrder` | 3.34 | 8.92 | 3.21 | 6.91 |
| `termCSR_sliceIteratorOrder` | 3.06 | 9.01 | 2.94 | 7.00 |
| `library_memoryOrder` | 0.894 | 2.45 | 0.905 | 1.64 |
| `diagonalwise_memoryOrder` | 0.827 | 5.77 | 0.805 | 1.46 |
| `rowwise_memoryOrder` | 2.61 | 4.05 | 2.64 | 2.98 |
| `termCSR_memoryOrder` | 2.26 | 4.24* | 2.29 | 2.69 |
| `library_byDiagonals_memoryOrder` | 0.888 | 5.88* | 0.897 | 1.68 |
| `library_byRows_memoryOrder` | 1.28 | 2.41 | 1.26 | 1.61 |
| `eigen_sparse_kron` | 2.64 | 2.68 | 2.69 | 2.68 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.72 | 2.22 | 1.27 | 2.62 |
| `diagonalwise_sliceIteratorOrder` | 1.64 | 3.25 | 1.18 | 2.18 |
| `rowwise_sliceIteratorOrder` | 3.74 | 3.64 | 3.55 | 4.21 |
| `termCSR_sliceIteratorOrder` | 3.42 | 3.68 | 3.25 | 4.27 |
| `diagonalwise_memoryOrder` | 0.93 | 2.36 | 0.89 | 0.89 |
| `rowwise_memoryOrder` | 2.92 | 1.65 | 2.92 | 1.82 |
| `termCSR_memoryOrder` | 2.53 | 1.73* | 2.53 | 1.64 |
| `library_byDiagonals_memoryOrder` | 0.99 | 2.40* | 0.99 | 1.02 |
| `library_byRows_memoryOrder` | 1.43 | 0.98 | 1.39 | 0.98 |
| `eigen_sparse_kron` | 2.95 | 1.09 | 2.97 | 1.63 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.939 | 1.35 | 1.29 | 1.31 | 1.26 | 1.23 | 1.15 | 1.01 | 0.907 |
| `library_byDiagonals` | 0.938 | 1.35 | 1.29 | 1.31 | 1.26 | 1.23 | 1.15 | 1.01 | 0.947 |
| `library_byRows` | 9.75 | 5.93 | 5.22 | 3.80 | 3.15 | 2.54 | 1.99 | 1.25 | 0.907 |
| `hoisted` | 3.71 | 2.99 | 2.13 | 1.79 | 1.37 | 1.29 | 1.19 | 1.04 | 0.956 |
| `eigen_sparse` | 2.40 | 1.96 | 1.90 | 1.89 | 1.87 | 2.03 | 1.86 | 1.89 | 1.87 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.04 |
| `library_byRows` | 10.4 | 4.39 | 4.05 | 2.90 | 2.50 | 2.07 | 1.73 | 1.24 | 1.00 |
| `hoisted` | 3.95 | 2.21 | 1.65 | 1.37 | 1.09 | 1.05 | 1.03 | 1.03 | 1.05 |
| `eigen_sparse` | 2.56 | 1.45 | 1.47 | 1.44 | 1.48 | 1.65 | 1.62 | 1.87 | 2.06 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.71 | 1.03 | 0.802 | 0.758 | 0.633 | 0.624 | 0.541 | 0.563 | 0.629* |
| `library_byRows` | 2.90 | 1.61 | 1.17 | 1.05 | 0.796 | 0.766 | 0.620 | 0.640 | 0.595* |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.81 | 1.14 | 0.958 | 0.891 | 0.817 | 0.809 | 0.760 | 0.747 | 0.810 |
| `library_byRows` | 2.98 | 1.67 | 1.37 | 1.28 | 1.16 | 1.18 | 1.49 | 1.70 | 0.774 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.26 | 1.16 | 1.05 | 1.02 | 0.943 | 0.945 | 0.898 | 0.906 | 0.897 | 0.912 |
| `library_byRows` | 3.14 | 2.01 | 1.50 | 1.25 | 1.03 | 0.904 | 0.817 | 0.772 | 0.701 | 0.651 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.53 | 1.42 | 1.33 | 1.30 | 1.25 | 1.23 | 1.20 | 1.20 | 1.21 | 1.24 |
| `library_byRows` | 3.26 | 2.15 | 1.64 | 1.43 | 1.28 | 1.17 | 1.32 | 1.25 | 1.38 | 0.929 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.56 | 2.40 | 2.30 | 2.27 | 2.19 | 2.18 | 2.17 | 2.16 | 2.12 | 2.18 |
| `library_byRows` | 3.42 | 2.30 | 1.95 | 1.69 | 1.76 | 1.57 | 1.92 | 1.80 | 1.94 | 1.40 |


