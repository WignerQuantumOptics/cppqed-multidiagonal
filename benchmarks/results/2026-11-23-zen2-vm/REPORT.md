# Session 2026-11-23-zen2-vm

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## machine.txt

```
host:     zen2-vm
kernel:   Linux 5.15.0-204-generic
pinning:  core 10
rounds:   5
suites:   main kernels kernels-cxlr
pages:    4k
smoke:    no

Architecture:                            x86_64
CPU(s):                                  16
Model name:                              AMD EPYC-Rome Processor
Thread(s) per core:                      1
Core(s) per socket:                      1
Socket(s):                               16
Hypervisor vendor:                       KVM
Virtualization type:                     full
L1d cache:                               512 KiB (16 instances)
L2 cache:                                8 MiB (16 instances)
L3 cache:                                256 MiB (16 instances)

── state at the start
time (UTC):                2026-11-23T09:44:29Z
load average:              0.03 0.01 0.00
SMT:                       notsupported
turbo off (intel_pstate):  not available
governor of cpu10:          not available
transparent huge pages:    always [madvise] never
steal ticks of cpu10:      578

── state at the end
time (UTC):                2026-11-23T10:07:17Z
load average:              1.00 1.00 0.81
SMT:                       notsupported
turbo off (intel_pstate):  not available
governor of cpu10:          not available
transparent huge pages:    always [madvise] never
steal ticks of cpu10:      741
failed processes:          0
```

## builds.txt

```
[134ddcf]
build directory:  /home/ubuntu/build-release
source:           /home/ubuntu/cppqed-multidiagonal
revision:         134ddcf6893be0c3afac249b92c27bd0e95e42b9
build type:       Release
compiler:         /home/ubuntu/opt/toolchain/bin/g++: g++ (GCC) 16.2.0
CMAKE_CXX_FLAGS:  
-march=native:    ON
Eigen:            5.0.1 (/home/ubuntu/opt/toolchain/share/eigen3/cmake)
```

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| 134ddcf | main | 4k | 5 | 257 | 0.1% | 1.1% | 15.1% | 5 | 0.998–1.002 |
| 134ddcf | kernels | 4k | 5 | 230 | 0.3% | 2.1% | 27.4% | 7 | 0.999–1.004 |
| 134ddcf | kernels-cxlr | 4k | 5 | 230 | 0.4% | 2.6% | 15.8% | 10 | 1.000–1.008 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## 134ddcf: `main`, 4 KB pages (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 89.5 ns | 182 ns | 648 ns | 2.23 µs | 8.61 µs | 34.8 µs | 142 µs | 1.22 ms | 5.02 ms |
| `eigen_sparse` | 127 ns | 530 ns | 2.14 µs | 8.54 µs | 34.5 µs | 140 µs | 564 µs | 2.76 ms | 11.0 ms |
| `eigen_dense` | 79.3 ns* | 937 ns | 22.2 µs | 611 µs | 12.2 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.41 | 2.92 | 3.30 | 3.84 | 4.01 | 4.01 | 3.96 | 2.26 | 2.20 |
| `eigen_dense` | 0.89* | 5.16 | 34.3 | 274 | 1416 |  |  |  |  |


### `compose1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 540 ns | 1.09 µs | 3.84 µs | 13.2 µs | 51.5 µs | 205 µs | 907 µs | 6.41 ms | 40.9 ms |
| `eigen_sparse` | 3.60 µs | 13.9 µs | 55.6 µs | 221 µs | 880 µs | 3.52 ms | 14.3 ms | 72.9 ms | 349 ms |
| `eigen_dense` | 1.74 µs | 84.4 µs | 5.30 ms | 344 ms |  |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 6.68 | 12.8 | 14.5 | 16.7 | 17.1 | 17.2 | 15.7 | 11.4 | 8.52 |
| `eigen_dense` | 3.22 | 77.6 | 1382 | 26135 |  |  |  |  |  |


### `construct1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 1.71 µs | 7.13 µs | 32.2 µs | 132 µs | 527 µs | 2.11 ms | 8.45 ms | 34.7 ms | 154 ms |
| `eigen_sparse` | 3.58 µs | 14.5 µs | 61.0 µs | 248 µs | 992 µs | 3.96 ms | 16.6 ms | 117 ms | 454 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 2.09 | 2.03 | 1.89 | 1.89 | 1.88 | 1.88 | 1.96 | 3.36 | 2.95 |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 165 ns | 517 ns | 2.10 µs | 8.64 µs | 35.0 µs | 142 µs | 653 µs | 5.22 ms | 22.9 ms |
| `eigen_sparse` | 221 ns | 889 ns | 3.58 µs | 14.3 µs | 58.6 µs | 233 µs | 997 µs | 4.56 ms | 18.2 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.34 | 1.72 | 1.70 | 1.65 | 1.68 | 1.64 | 1.53 | 0.87 | 0.80 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 136 ns | 284 ns | 1.25 µs | 3.80 µs | 14.9 µs | 58.2 µs* | 282 µs* | 2.48 ms | 10.0 ms |
| `eigen_sparse` | 217 ns | 895 ns | 3.61 µs | 14.3 µs | 58.3 µs | 234 µs | 1.00 ms | 4.55 ms | 17.9 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.59 | 3.15 | 2.89 | 3.77 | 3.92 | 4.03* | 3.55* | 1.83 | 1.79 |


### `composeJC`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 5.48 µs | 17.7 µs | 66.7 µs | 261 µs | 1.04 ms | 4.19 ms | 17.9 ms | 72.9 ms | 311 ms |
| `eigen_sparse` | 10.2 µs | 42.1 µs | 169 µs | 679 µs | 2.72 ms | 11.2 ms | 46.4 ms | 272 ms | 1.10 s |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.86 | 2.37 | 2.54 | 2.61 | 2.60 | 2.68 | 2.60 | 3.73 | 3.54 |


### `constructJC`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 4.08 µs | 9.18 µs | 28.0 µs | 104 µs | 414 µs | 1.66 ms | 8.32 ms | 66.9 ms | 290 ms |
| `eigen_sparse` | 3.93 µs | 13.7 µs | 53.9 µs | 212 µs | 841 µs | 3.39 ms | 15.3 ms | 118 ms | 505 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 0.96 | 1.49 | 1.92 | 2.03 | 2.03 | 2.04 | 1.84 | 1.76 | 1.74 |


### `picture1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal_static` | 89.6 ns | 182 ns | 641 ns | 2.42 µs | 9.11 µs | 35.5 µs | 142 µs | 1.21 ms | 4.76 ms |
| `multidiagonal_scalarfreq` | 306 ns | 646 ns | 2.22 µs | 7.94 µs | 30.8 µs | 121 µs | 542 µs | 3.53 ms | 14.1 ms |
| `multidiagonal_tabfreq` | 1.86 µs | 7.35 µs | 29.8 µs | 112 µs | 411 µs | 1.65 ms | 6.44 ms | 26.2 ms | 105 ms |
| `handwritten_scalarfreq` | 281 ns | 790 ns | 2.91 µs | 11.1 µs | 44.0 µs | 175 µs | 699 µs | 3.26 ms | 13.5 ms |
| `handwritten_tabfreq` | 1.79 µs | 7.40 µs | 29.8 µs | 113 µs | 425 µs | 1.69 ms | 6.64 ms | 26.7 ms | 107 ms |
| `eigen_sparse_pernnz` | 1.75 µs | 7.47 µs | 30.3 µs | 116 µs | 428 µs | 1.71 ms | 6.71 ms | 27.3 ms | 109 ms |
| `csr_pernnz` | 1.77 µs | 7.57 µs | 30.7 µs | 118 µs | 431 µs | 1.73 ms | 6.68 ms | 26.8 ms | 107 ms |
| `eigen_sparse_sandwich` | 220 ns | 794 ns | 3.19 µs | 12.7 µs | 51.4 µs | 209 µs | 896 µs | 4.64 ms | 19.4 ms |


### `slices3_axis0`

| time per application | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `multidiagonal` | 23.2 µs | 106 µs | 125 µs | 791 µs | 3.59 ms |
| `eigen_sparse_term` | 34.4 µs | 239 µs | 289 µs | 2.32 ms | 8.94 ms |
| `multidiagonal_picture_linear` | 23.4 µs | 107 µs | 127 µs | 803 µs | 3.58 ms |
| `multidiagonal_picture_quadratic` | 23.7 µs | 108 µs | 128 µs | 800 µs | 3.55 ms |
| `eigen_sparse_term_picture_pernnz` | 36.2 µs | 242 µs | 293 µs | 2.33 ms | 8.96 ms |

| time ÷ that of `multidiagonal` | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `eigen_sparse_term` | 1.48 | 2.25 | 2.32 | 2.93 | 2.49 |
| `multidiagonal_picture_linear` | 1.01 | 1.01 | 1.02 | 1.01 | 1.00 |
| `multidiagonal_picture_quadratic` | 1.02 | 1.01 | 1.03 | 1.01 | 0.99 |
| `eigen_sparse_term_picture_pernnz` | 1.56 | 2.28 | 2.35 | 2.95 | 2.50 |


### `slices3_axis2`

| time per application | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `multidiagonal` | 71.4 µs | 168 µs | 490 µs | 3.58 ms | 5.55 ms |
| `eigen_sparse_term` | 51.3 µs | 278 µs* | 421 µs | 3.79 ms | 12.2 ms |
| `multidiagonal_picture_linear` | 71.6 µs | 168 µs | 490 µs | 3.57 ms | 5.43 ms |
| `multidiagonal_picture_quadratic` | 72.0 µs | 163 µs | 489 µs | 3.57 ms | 5.46 ms |
| `eigen_sparse_term_picture_pernnz` | 54.6 µs | 253 µs* | 421 µs | 3.81 ms | 12.2 ms |

| time ÷ that of `multidiagonal` | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `eigen_sparse_term` | 0.72 | 1.65* | 0.86 | 1.06 | 2.20 |
| `multidiagonal_picture_linear` | 1.00 | 1.00 | 1.00 | 1.00 | 0.98 |
| `multidiagonal_picture_quadratic` | 1.01 | 0.97 | 1.00 | 1.00 | 0.98 |
| `eigen_sparse_term_picture_pernnz` | 0.77 | 1.50* | 0.86 | 1.07 | 2.19 |


## 134ddcf: `kernels`, 4 KB pages (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.485 | 0.425 | 0.936 |
| `library_byDiagonals` | 0.547 | 0.635 | 1.67 |
| `library_byRows` | 0.483 | 0.425 | 0.936 |
| `hoisted` | 0.542 | 0.628 | 1.66 |
| `rowBlocked` | 0.548 | 0.735 | 1.81 |
| `eigen_sparse` | 1.72 | 1.72 | 2.14 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `library_byDiagonals` | 1.13 | 1.49 | 1.78 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.12 | 1.48 | 1.77 |
| `rowBlocked` | 1.13 | 1.73 | 1.93 |
| `eigen_sparse` | 3.55 | 4.05 | 2.29 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.538 | 0.424 | 1.18 |
| `library_byDiagonals` | 0.629 | 0.807 | 2.05 |
| `library_byRows` | 0.540 | 0.471* | 1.18 |
| `hoisted` | 0.619 | 0.823 | 2.05 |
| `eigen_sparse` | 1.90 | 1.89 | 2.23 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.17 | 1.90 | 1.74 |
| `library_byRows` | 1.00 | 1.11* | 1.00 |
| `hoisted` | 1.15 | 1.94 | 1.74 |
| `eigen_sparse` | 3.53 | 4.46 | 1.89 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 1.90 | 1.97 |
| `blocked` | 2.78 | 1.18 |
| `library_materialized` | 1.64 | 1.37 |
| `library_materialized_byDiagonals` | 2.32 | 2.37 |
| `library_materialized_byRows` | 1.63 | 1.37 |
| `eigen_sparse_kron` | 2.15 | 2.15 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.75 | 9.64 | 1.26 | 3.95 |
| `diagonalwise_sliceIteratorOrder` | 1.65 | 42.6 | 1.15 | 3.78 |
| `rowwise_sliceIteratorOrder` | 2.86* | 17.2 | 2.08* | 4.69 |
| `termCSR_sliceIteratorOrder` | 2.92* | 17.5 | 2.18 | 4.91 |
| `library_memoryOrder` | 0.884 | 3.19 | 0.805 | 1.50 |
| `diagonalwise_memoryOrder` | 0.775 | 44.7 | 0.727 | 1.94 |
| `rowwise_memoryOrder` | 1.76 | 2.42 | 1.76 | 2.17 |
| `termCSR_memoryOrder` | 1.85 | 2.63 | 1.87 | 2.31 |
| `library_byDiagonals_memoryOrder` | 0.841 | 38.9 | 0.777 | 1.95 |
| `library_byRows_memoryOrder` | 1.20 | 3.16 | 1.17 | 1.52 |
| `eigen_sparse_kron` | 2.15 | 2.15 | 2.12 | 2.14 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.98 | 3.02 | 1.57 | 2.63 |
| `diagonalwise_sliceIteratorOrder` | 1.87 | 13.4 | 1.43 | 2.52 |
| `rowwise_sliceIteratorOrder` | 3.24* | 5.39 | 2.58* | 3.13 |
| `termCSR_sliceIteratorOrder` | 3.30* | 5.49 | 2.71 | 3.27 |
| `diagonalwise_memoryOrder` | 0.88 | 14.0 | 0.90 | 1.29 |
| `rowwise_memoryOrder` | 1.99 | 0.76 | 2.19 | 1.45 |
| `termCSR_memoryOrder` | 2.09 | 0.82 | 2.32 | 1.54 |
| `library_byDiagonals_memoryOrder` | 0.95 | 12.2 | 0.97 | 1.30 |
| `library_byRows_memoryOrder` | 1.36 | 0.99 | 1.45 | 1.01 |
| `eigen_sparse_kron` | 2.43 | 0.67 | 2.63 | 1.43 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.656 | 1.01 | 1.29 | 1.29 | 1.19 | 1.34 | 1.12 | 0.765 | 0.863 |
| `library_byDiagonals` | 0.653 | 1.02 | 1.32 | 1.30 | 1.20 | 1.35 | 1.11 | 0.773 | 0.672 |
| `library_byRows` | 12.4 | 6.03 | 5.48 | 3.96 | 3.23 | 2.71 | 2.07 | 1.25 | 0.873 |
| `hoisted` | 5.25 | 3.80 | 2.33 | 1.99 | 1.41 | 1.34 | 1.07 | 0.760 | 0.700 |
| `eigen_sparse` | 2.15 | 1.72 | 1.64 | 1.62 | 1.60 | 1.59 | 1.59 | 1.61 | 1.58 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.00 | 1.01 | 1.02 | 1.01 | 1.01 | 1.01 | 0.99 | 1.01 | 0.78 |
| `library_byRows` | 18.9 | 5.97 | 4.25 | 3.07 | 2.71 | 2.02 | 1.85 | 1.63 | 1.01 |
| `hoisted` | 8.00 | 3.76 | 1.81 | 1.54 | 1.18 | 1.00 | 0.96 | 0.99 | 0.81 |
| `eigen_sparse` | 3.28 | 1.70 | 1.27 | 1.26 | 1.34 | 1.19 | 1.42 | 2.10 | 1.83 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.65 | 0.989 | 0.776 | 0.705* | 0.595 | 0.585* | 0.519 | 0.533 | 0.573 |
| `library_byRows` | 2.74 | 1.46 | 1.10 | 0.916 | 0.746 | 0.661 | 0.577 | 0.541 | 0.487 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.68 | 1.03 | 0.838 | 0.781* | 0.689 | 0.736 | 0.755 | 0.772 | 0.727 |
| `library_byRows` | 2.77 | 1.50 | 1.14 | 1.17 | 1.19 | 1.07 | 1.01 | 0.947 | 0.772 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.20 | 1.10 | 0.854 | 0.775 | 0.675 | 0.663 | 0.613 | 0.614 | 0.594 | 0.616 |
| `library_byRows` | 3.23 | 2.03 | 1.47 | 1.21 | 0.984 | 0.859 | 0.733 | 0.664 | 0.581 | 0.557 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.27 | 1.20 | 0.981 | 0.926 | 0.848 | 0.858 | 0.797 | 0.839 | 0.839 | 0.973 |
| `library_byRows` | 3.22 | 2.04 | 1.48 | 1.23 | 1.05 | 1.01 | 0.847 | 0.781 | 0.721 | 0.629 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.51 | 2.36 | 2.21 | 2.22 | 2.16 | 2.16 | 2.11 | 2.13 | 2.16 | 2.19 |
| `library_byRows` | 3.28 | 2.20 | 1.61 | 1.38 | 1.63 | 1.69 | 1.64 | 1.51 | 1.37 | 1.17 |


## 134ddcf: `kernels-cxlr`, 4 KB pages (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.488 | 0.423 | 0.943 |
| `library_byDiagonals` | 0.549 | 0.628 | 1.67 |
| `library_byRows` | 0.486 | 0.423 | 0.933 |
| `hoisted` | 0.542 | 0.640 | 1.67 |
| `rowBlocked` | 0.547 | 0.730 | 1.79 |
| `eigen_sparse` | 1.51 | 1.50 | 2.00 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `library_byDiagonals` | 1.13 | 1.48 | 1.77 |
| `library_byRows` | 1.00 | 1.00 | 0.99 |
| `hoisted` | 1.11 | 1.51 | 1.77 |
| `rowBlocked` | 1.12 | 1.73 | 1.90 |
| `eigen_sparse` | 3.09 | 3.55 | 2.12 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.545 | 0.432 | 1.18 |
| `library_byDiagonals` | 0.619 | 0.814 | 2.05 |
| `library_byRows` | 0.544 | 0.454* | 1.18 |
| `hoisted` | 0.614 | 0.806 | 2.05 |
| `eigen_sparse` | 1.62 | 1.62 | 2.05 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.14 | 1.88 | 1.74 |
| `library_byRows` | 1.00 | 1.05* | 1.00 |
| `hoisted` | 1.13 | 1.87 | 1.74 |
| `eigen_sparse` | 2.97 | 3.75 | 1.74 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 1.90 | 1.97 |
| `blocked` | 2.77 | 1.18 |
| `library_materialized` | 1.74 | 1.38 |
| `library_materialized_byDiagonals` | 2.31 | 2.37 |
| `library_materialized_byRows` | 1.75 | 1.38 |
| `eigen_sparse_kron` | 2.00 | 2.04 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.72 | 9.61* | 1.26 | 3.95 |
| `diagonalwise_sliceIteratorOrder` | 1.63 | 42.6 | 1.15 | 3.80 |
| `rowwise_sliceIteratorOrder` | 2.80 | 17.1* | 2.11 | 4.61 |
| `termCSR_sliceIteratorOrder` | 2.57 | 16.6* | 1.87 | 4.37 |
| `library_memoryOrder` | 0.886 | 3.19 | 0.806 | 1.50 |
| `diagonalwise_memoryOrder` | 0.781 | 44.6 | 0.719 | 1.94 |
| `rowwise_memoryOrder` | 1.79 | 2.34 | 1.80 | 2.05 |
| `termCSR_memoryOrder` | 1.56 | 2.33 | 1.56 | 2.07* |
| `library_byDiagonals_memoryOrder` | 0.844 | 38.7 | 0.778 | 1.94 |
| `library_byRows_memoryOrder` | 1.12 | 3.13 | 1.11 | 1.44 |
| `eigen_sparse_kron` | 2.02 | 2.04 | 1.98 | 2.00 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.94 | 3.01* | 1.56 | 2.63 |
| `diagonalwise_sliceIteratorOrder` | 1.84 | 13.4 | 1.43 | 2.53 |
| `rowwise_sliceIteratorOrder` | 3.16 | 5.36* | 2.62 | 3.07 |
| `termCSR_sliceIteratorOrder` | 2.90 | 5.20* | 2.32 | 2.91 |
| `diagonalwise_memoryOrder` | 0.88 | 14.0 | 0.89 | 1.29 |
| `rowwise_memoryOrder` | 2.02 | 0.73 | 2.23 | 1.37 |
| `termCSR_memoryOrder` | 1.76 | 0.73 | 1.94 | 1.38* |
| `library_byDiagonals_memoryOrder` | 0.95 | 12.1 | 0.97 | 1.29 |
| `library_byRows_memoryOrder` | 1.26 | 0.98 | 1.38 | 0.96 |
| `eigen_sparse_kron` | 2.28 | 0.64 | 2.46 | 1.33 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.653 | 1.01 | 1.30 | 1.29 | 1.19 | 1.36 | 1.13 | 0.768 | 0.848 |
| `library_byDiagonals` | 0.658 | 1.00 | 1.30 | 1.29 | 1.20 | 1.37 | 1.11 | 0.765 | 0.670* |
| `library_byRows` | 12.5* | 6.02 | 5.47 | 3.92 | 3.21 | 2.60 | 2.02 | 1.21 | 0.847 |
| `hoisted` | 5.25 | 3.80 | 2.33 | 1.99 | 1.41 | 1.34 | 1.07 | 0.767 | 0.671 |
| `eigen_sparse` | 1.88 | 1.39 | 1.34 | 1.32 | 1.30 | 1.30 | 1.28 | 1.31 | 1.29 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.01 | 0.99 | 1.00 | 1.00 | 1.01 | 1.01 | 0.98 | 1.00 | 0.79* |
| `library_byRows` | 19.1* | 5.96 | 4.21 | 3.04 | 2.70 | 1.91 | 1.79 | 1.58 | 1.00 |
| `hoisted` | 8.04 | 3.76 | 1.79 | 1.54 | 1.18 | 0.99 | 0.95 | 1.00 | 0.79 |
| `eigen_sparse` | 2.88 | 1.38 | 1.03 | 1.02 | 1.09 | 0.96 | 1.13 | 1.71 | 1.52 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.65 | 0.992 | 0.781 | 0.714* | 0.596 | 0.589 | 0.519 | 0.531 | 0.570 |
| `library_byRows` | 2.69 | 1.47 | 1.10 | 0.916 | 0.746 | 0.660 | 0.577 | 0.541 | 0.488 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.68 | 1.03 | 0.839 | 0.791* | 0.689 | 0.753 | 0.761 | 0.783 | 0.727 |
| `library_byRows` | 2.73 | 1.50 | 1.14 | 1.16 | 1.17 | 1.08 | 1.01 | 0.944 | 0.763 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.20 | 1.11 | 0.872 | 0.794 | 0.698* | 0.678 | 0.623 | 0.630 | 0.597 | 0.623 |
| `library_byRows` | 3.21 | 2.05 | 1.48 | 1.26 | 0.982 | 0.877 | 0.728 | 0.666 | 0.575 | 0.552 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.28 | 1.20 | 0.989 | 0.918 | 0.852 | 0.859 | 0.812 | 0.849 | 0.845 | 0.980 |
| `library_byRows` | 3.22 | 2.05 | 1.49 | 1.25 | 1.05 | 1.00 | 0.848 | 0.781 | 0.717 | 0.630 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.53 | 2.37 | 2.23 | 2.24 | 2.16 | 2.14 | 2.11 | 2.12 | 2.17 | 2.19 |
| `library_byRows` | 3.28 | 2.17 | 1.63 | 1.40 | 1.64 | 1.69 | 1.65 | 1.50 | 1.37 | 1.18 |


