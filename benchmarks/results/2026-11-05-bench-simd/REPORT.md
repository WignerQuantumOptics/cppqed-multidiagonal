# Session 2026-11-05-bench-simd

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| base | main.apply1+applyJC | 4k | 5 | 59 | 0.1% | 1.3% | 20.0% | 1 | 0.999–1.004 |
| base | kernels.A+B+E | 4k | 5 | 78 | 0.0% | 1.3% | 8.6% | 0 | 0.998–1.004 |
| native | main.apply1+applyJC | 4k | 5 | 59 | 0.1% | 1.3% | 30.0% | 2 | 0.999–1.007 |
| native | kernels.A+B+E | 4k | 5 | 78 | 0.0% | 1.4% | 14.1% | 2 | 0.999–1.007 |
| v3 | main.apply1+applyJC | 4k | 5 | 59 | 0.1% | 1.2% | 22.9% | 2 | 0.999–1.009 |
| v3 | kernels.A+B+E | 4k | 5 | 78 | 0.0% | 1.8% | 8.0% | 2 | 0.997–1.007 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## base: `main.apply1+applyJC`, 4 KB pages (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 120 ns | 402 ns | 1.65 µs | 6.66 µs | 32.2 µs | 128 µs | 529 µs | 2.36 ms | 9.76 ms |
| `eigen_sparse` | 164 ns | 690 ns | 2.81 µs | 11.9 µs | 49.2 µs | 196 µs | 848 µs | 3.69 ms | 14.9 ms |
| `eigen_dense` | 216 ns | 3.52 µs | 70.0 µs | 919 µs | 16.4 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.37 | 1.72 | 1.70 | 1.79 | 1.53 | 1.53 | 1.60 | 1.56 | 1.53 |
| `eigen_dense` | 1.81 | 8.77 | 42.4 | 138 | 510 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 217 ns | 760 ns | 2.98 µs | 12.5 µs | 55.0 µs | 231 µs | 1.09 ms | 5.64 ms | 25.3 ms |
| `eigen_sparse` | 324 ns | 1.31 µs | 5.28 µs | 21.3 µs | 87.1 µs | 346 µs | 1.59 ms | 6.29 ms | 25.0 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.49 | 1.73 | 1.77 | 1.71 | 1.58 | 1.50 | 1.46 | 1.12 | 0.99 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 197 ns* | 678 ns | 2.67 µs | 11.4 µs | 54.7 µs | 219 µs | 974 µs | 3.81 ms | 14.3 ms |
| `eigen_sparse` | 319 ns | 1.32 µs | 5.29 µs | 21.3 µs | 87.2 µs | 349 µs | 1.59 ms | 6.66 ms | 25.5 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.62* | 1.95 | 1.98 | 1.87 | 1.60 | 1.60 | 1.63 | 1.75 | 1.78 |


## base: `kernels.A+B+E`, 4 KB pages (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 1.26 | 1.50 | 1.57 |
| `library_byDiagonals` | 1.33 | 1.60 | 2.25 |
| `library_byRows` | 1.26 | 1.50 | 1.55 |
| `hoisted` | 1.32 | 1.60 | 2.25 |
| `rowBlocked` | 1.32 | 1.58 | 1.93 |
| `eigen_sparse` | 2.30 | 2.40 | 2.87 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `library_byDiagonals` | 1.06 | 1.07 | 1.43 |
| `library_byRows` | 1.00 | 1.00 | 0.99 |
| `hoisted` | 1.05 | 1.07 | 1.43 |
| `rowBlocked` | 1.05 | 1.05 | 1.23 |
| `eigen_sparse` | 1.83 | 1.60 | 1.83 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 1.30 | 1.47 | 1.82 |
| `library_byDiagonals` | 1.28 | 1.51 | 2.43 |
| `library_byRows` | 1.30 | 1.47 | 1.81 |
| `hoisted` | 1.28 | 1.52 | 2.42 |
| `eigen_sparse` | 2.72 | 2.71 | 3.25 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 0.98 | 1.03 | 1.34 |
| `library_byRows` | 1.00 | 1.00 | 0.99 |
| `hoisted` | 0.98 | 1.03 | 1.33 |
| `eigen_sparse` | 2.09 | 1.84 | 1.79 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 1.63 | 1.69 | 1.53 | 1.51 | 1.53 | 1.58 | 1.56 | 1.53 | 1.60 |
| `library_byDiagonals` | 1.62 | 1.69 | 1.53 | 1.51 | 1.53 | 1.58 | 1.56 | 1.53 | 1.51 |
| `library_byRows` | 10.5 | 7.27 | 5.70 | 4.25 | 3.55 | 3.16 | 2.65 | 1.93 | 1.60 |
| `hoisted` | 2.42 | 2.20 | 1.84 | 1.74 | 1.68 | 1.66 | 1.63 | 1.57 | 1.53 |
| `eigen_sparse` | 3.04 | 2.50 | 2.38 | 2.34 | 2.31 | 2.42 | 2.28 | 2.29 | 2.26 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 0.99 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 0.94 |
| `library_byRows` | 6.44 | 4.30 | 3.73 | 2.81 | 2.32 | 2.00 | 1.70 | 1.26 | 1.00 |
| `hoisted` | 1.48 | 1.30 | 1.20 | 1.15 | 1.10 | 1.05 | 1.04 | 1.03 | 0.96 |
| `eigen_sparse` | 1.87 | 1.48 | 1.56 | 1.55 | 1.51 | 1.53 | 1.46 | 1.50 | 1.41 |


## native: `main.apply1+applyJC`, 4 KB pages (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.4 ns | 173 ns | 698 ns | 3.13 µs* | 14.6 µs | 58.9 µs | 255 µs | 1.39 ms | 6.31 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.29 µs | 9.73 µs | 40.5 µs | 162 µs | 717 µs | 3.18 ms | 12.7 ms |
| `eigen_dense` | 87.2 ns | 1.28 µs* | 27.6 µs | 798 µs | 13.2 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.85 | 3.21 | 3.28 | 3.11* | 2.77 | 2.74 | 2.81 | 2.28 | 2.01 |
| `eigen_dense` | 1.22 | 7.38* | 39.5 | 255* | 903 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 174 ns | 484 ns | 2.03 µs | 9.82 µs | 43.8 µs | 188 µs | 930 µs | 5.09 ms | 23.0 ms |
| `eigen_sparse` | 230 ns | 928 ns | 3.91 µs | 15.9 µs | 66.4 µs | 262 µs | 1.26 ms | 5.42 ms | 20.5 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.32 | 1.92 | 1.92 | 1.62 | 1.51 | 1.39 | 1.36 | 1.07 | 0.89 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 125 ns | 292 ns | 1.22 µs | 4.89 µs | 25.4 µs | 101 µs | 514 µs | 2.69 ms | 9.69 ms |
| `eigen_sparse` | 225 ns | 929 ns | 3.89 µs | 16.8 µs | 68.2 µs | 273 µs | 1.33 ms | 5.57 ms | 22.3 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.80 | 3.18 | 3.19 | 3.44 | 2.68 | 2.71 | 2.58 | 2.07 | 2.30 |


## native: `kernels.A+B+E`, 4 KB pages (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.579 | 0.712 | 1.02 |
| `library_byDiagonals` | 0.648 | 0.830 | 1.63 |
| `library_byRows` | 0.577 | 0.709 | 1.02 |
| `hoisted` | 0.646 | 0.831 | 1.63 |
| `rowBlocked` | 0.639 | 0.803 | 1.24 |
| `eigen_sparse` | 2.02 | 2.06 | 2.49 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `library_byDiagonals` | 1.12 | 1.17 | 1.60 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.12 | 1.17 | 1.60 |
| `rowBlocked` | 1.10 | 1.13 | 1.22 |
| `eigen_sparse` | 3.49 | 2.89 | 2.44 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.606 | 0.705 | 1.26 |
| `library_byDiagonals` | 0.644 | 0.890 | 2.01 |
| `library_byRows` | 0.605 | 0.706 | 1.26 |
| `hoisted` | 0.635 | 0.891 | 2.00 |
| `eigen_sparse` | 1.95 | 2.15 | 2.72 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.06 | 1.26 | 1.60 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.05 | 1.26 | 1.59 |
| `eigen_sparse` | 3.22 | 3.05 | 2.16 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.929* | 1.36 | 1.29 | 1.32 | 1.27 | 1.23 | 1.14 | 0.990 | 0.908 |
| `library_byDiagonals` | 0.929* | 1.36 | 1.29 | 1.31 | 1.27 | 1.23 | 1.14 | 0.989 | 0.912 |
| `library_byRows` | 9.71 | 5.94 | 5.24 | 3.79 | 3.15 | 2.55 | 2.00 | 1.25 | 0.909 |
| `hoisted` | 3.76 | 3.00 | 2.13 | 1.79 | 1.37 | 1.30 | 1.18 | 1.02 | 0.921 |
| `eigen_sparse` | 2.40 | 1.96 | 1.90 | 1.88 | 1.87 | 2.01 | 1.86 | 1.89 | 1.87 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.00* | 1.00 | 1.00 | 0.99 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 |
| `library_byRows` | 10.5* | 4.37 | 4.06 | 2.87 | 2.48 | 2.07 | 1.75 | 1.26 | 1.00 |
| `hoisted` | 4.05* | 2.21 | 1.65 | 1.36 | 1.08 | 1.06 | 1.04 | 1.03 | 1.01 |
| `eigen_sparse` | 2.58* | 1.44 | 1.47 | 1.42 | 1.47 | 1.63 | 1.63 | 1.91 | 2.06 |


## v3: `main.apply1+applyJC`, 4 KB pages (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 69.8 ns | 171 ns | 710 ns | 3.10 µs | 14.6 µs | 59.1 µs | 255 µs | 1.39 ms | 6.30 ms |
| `eigen_sparse` | 132 ns | 554 ns | 2.28 µs | 9.73 µs | 40.7 µs | 162 µs | 715 µs | 3.17 ms | 12.6 ms |
| `eigen_dense` | 86.9 ns | 1.27 µs | 26.3 µs | 819 µs | 13.2 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.89 | 3.23 | 3.22 | 3.14 | 2.78 | 2.74 | 2.80 | 2.28 | 2.01 |
| `eigen_dense` | 1.25 | 7.41 | 37.1 | 264 | 903 |  |  |  |  |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 175 ns | 485 ns | 2.01 µs | 9.82 µs* | 43.7 µs | 186 µs | 924 µs | 5.10 ms | 23.1 ms |
| `eigen_sparse` | 230 ns | 929 ns | 3.89 µs | 15.9 µs | 66.0 µs | 263 µs | 1.26 ms | 5.44 ms | 20.5 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.31 | 1.91 | 1.93 | 1.62* | 1.51 | 1.41 | 1.36 | 1.07 | 0.89 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 124 ns | 286 ns | 1.23 µs | 4.95 µs* | 25.4 µs | 101 µs | 507 µs | 2.69 ms | 9.68 ms |
| `eigen_sparse` | 225 ns | 930 ns | 3.88 µs | 16.9 µs | 68.0 µs | 272 µs | 1.33 ms | 5.58 ms | 22.4 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.82 | 3.25 | 3.16 | 3.41* | 2.68 | 2.68 | 2.63 | 2.07 | 2.31 |


## v3: `kernels.A+B+E`, 4 KB pages (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.584 | 0.710 | 1.02 |
| `library_byDiagonals` | 0.649 | 0.828 | 1.63 |
| `library_byRows` | 0.584 | 0.708 | 1.02 |
| `hoisted` | 0.646 | 0.829 | 1.63 |
| `rowBlocked` | 0.641 | 0.820 | 1.24 |
| `eigen_sparse` | 2.13 | 2.15 | 2.57 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `library_byDiagonals` | 1.11 | 1.17 | 1.60 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.11 | 1.17 | 1.60 |
| `rowBlocked` | 1.10 | 1.15 | 1.22 |
| `eigen_sparse` | 3.65 | 3.03 | 2.52 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.607 | 0.704 | 1.27 |
| `library_byDiagonals` | 0.632 | 0.896 | 2.01 |
| `library_byRows` | 0.604 | 0.705 | 1.26 |
| `hoisted` | 0.639 | 0.896 | 2.01 |
| `eigen_sparse` | 1.95 | 2.16 | 2.72 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.04 | 1.27 | 1.58 |
| `library_byRows` | 1.00 | 1.00 | 0.99 |
| `hoisted` | 1.05 | 1.27 | 1.58 |
| `eigen_sparse` | 3.21 | 3.07 | 2.14 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.966* | 1.36 | 1.26 | 1.26 | 1.26 | 1.23 | 1.14 | 0.991 | 0.920 |
| `library_byDiagonals` | 0.965* | 1.36 | 1.26 | 1.26 | 1.26 | 1.23 | 1.14 | 0.987 | 0.916 |
| `library_byRows` | 9.79 | 6.30 | 5.43 | 3.90 | 3.22 | 2.64 | 2.07 | 1.27 | 0.920 |
| `hoisted` | 3.73 | 3.07 | 2.15 | 1.84 | 1.46 | 1.34 | 1.17 | 0.999 | 0.918 |
| `eigen_sparse` | 2.32 | 1.86 | 1.78 | 1.76 | 1.74 | 1.74 | 1.71 | 1.76 | 1.73 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.00* | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 |
| `library_byRows` | 10.1* | 4.63 | 4.31 | 3.10 | 2.56 | 2.15 | 1.82 | 1.28 | 1.00 |
| `hoisted` | 3.86* | 2.26 | 1.71 | 1.46 | 1.16 | 1.09 | 1.03 | 1.01 | 1.00 |
| `eigen_sparse` | 2.40* | 1.37 | 1.41 | 1.40 | 1.38 | 1.41 | 1.50 | 1.78 | 1.88 |


