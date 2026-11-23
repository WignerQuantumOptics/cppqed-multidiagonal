# Session 2026-11-23-bench

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## machine.txt

```
host:     bench
kernel:   Linux 6.18.62-1-lts
pinning:  core 1
rounds:   5
suites:   main kernels kernels-cxlr
pages:    thp 4k
smoke:    no

Architecture:                            x86_64
CPU(s):                                  8
Model name:                              Intel(R) Core(TM) i7-7700 CPU @ 3.60GHz
Thread(s) per core:                      1
Core(s) per socket:                      4
Socket(s):                               1
CPU(s) scaling MHz:                      100%
CPU max MHz:                             3600.0000
L1d cache:                               128 KiB (4 instances)
L2 cache:                                1 MiB (4 instances)
L3 cache:                                8 MiB (1 instance)

── state at the start
time (UTC):                2026-11-23T09:52:38Z
load average:              1.15 0.26 0.09
SMT:                       off
turbo off (intel_pstate):  1
governor of cpu1:          performance
transparent huge pages:    [always] madvise never
steal ticks of cpu1:      0

── state at the end
time (UTC):                2026-11-23T10:34:55Z
load average:              1.01 1.02 1.00
SMT:                       off
turbo off (intel_pstate):  1
governor of cpu1:          performance
transparent huge pages:    [always] madvise never
steal ticks of cpu1:      0
failed processes:          0
```

## builds.txt

```
[134ddcf]
build directory:  /home/claude/build-134ddcf
source:           /home/claude/src/cppqed-multidiagonal-134ddcf
revision:         not a git checkout
build type:       Release
compiler:         /usr/bin/c++: c++ (GCC) 16.2.1 20260810
CMAKE_CXX_FLAGS:  
-march=native:    ON
Eigen:            5.0.1 (/usr/share/eigen3/cmake)
```

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| 134ddcf | main | thp | 5 | 257 | 0.1% | 2.7% | 74.0% | 36 | 0.992–1.014 |
| 134ddcf | main | 4k | 5 | 257 | 0.1% | 1.1% | 118.6% | 5 | 1.001–1.005 |
| 134ddcf | kernels | thp | 5 | 230 | 0.2% | 1.8% | 10.8% | 7 | 0.997–1.003 |
| 134ddcf | kernels | 4k | 5 | 230 | 0.2% | 2.9% | 52.2% | 45 | 0.989–1.017 |
| 134ddcf | kernels-cxlr | thp | 5 | 230 | 0.3% | 2.4% | 16.6% | 14 | 1.001–1.005 |
| 134ddcf | kernels-cxlr | 4k | 5 | 230 | 0.2% | 2.5% | 31.2% | 20 | 0.996–1.015 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## 134ddcf: `main`, pages as the system hands them out (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.7 ns | 171 ns | 695 ns | 3.08 µs | 14.6 µs | 58.5 µs | 257 µs* | 1.56 ms | 9.40 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.28 µs | 9.60 µs | 40.1 µs | 160 µs | 694 µs | 3.38 ms | 12.4 ms |
| `eigen_dense` | 87.3 ns | 1.27 µs* | 27.8 µs | 818 µs | 14.2 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.84 | 3.25 | 3.29 | 3.12 | 2.74 | 2.73 | 2.70* | 2.17 | 1.32 |
| `eigen_dense` | 1.22 | 7.42* | 40.0 | 266 | 970 |  |  |  |  |


### `compose1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 503 ns | 1.14 µs | 4.10 µs | 15.3 µs | 63.8 µs | 252 µs | 1.34 ms | 6.81 ms | 38.0 ms |
| `eigen_sparse` | 1.64 µs | 6.10 µs | 26.9 µs | 108 µs | 428 µs | 1.89 ms | 7.64 ms | 35.6 ms | 142 ms |
| `eigen_dense` | 1.59 µs | 79.6 µs | 5.04 ms | 343 ms |  |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 3.27 | 5.35 | 6.57 | 7.10 | 6.71 | 7.49 | 5.71 | 5.22 | 3.74 |
| `eigen_dense` | 3.15 | 69.8 | 1229 | 22462 |  |  |  |  |  |


### `construct1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 1.42 µs | 6.29 µs* | 28.7 µs* | 116 µs* | 465 µs* | 1.86 ms* | 7.45 ms* | 30.7 ms* | 125 ms* |
| `eigen_sparse` | 2.36 µs | 9.47 µs | 43.7 µs | 192 µs | 782 µs | 3.13 ms* | 14.4 ms | 72.9 ms | 263 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.66 | 1.51* | 1.53* | 1.66* | 1.68* | 1.69* | 1.94* | 2.37* | 2.10* |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 173 ns | 502 ns | 2.16 µs | 9.81 µs | 45.1 µs* | 190 µs | 1.25 ms* | 7.35 ms | 38.0 ms |
| `eigen_sparse` | 280 ns | 1.12 µs | 4.43 µs | 17.8 µs | 71.9 µs | 287 µs | 1.44 ms | 5.74 ms | 22.5 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.62 | 2.23 | 2.05 | 1.81 | 1.59* | 1.51 | 1.16* | 0.78 | 0.59 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 137 ns | 323 ns | 1.27 µs | 4.92 µs | 24.2 µs | 96.0 µs | 683 µs* | 3.22 ms | 15.5 ms |
| `eigen_sparse` | 270 ns | 1.11 µs | 4.43 µs | 17.8 µs | 73.5 µs | 291 µs | 1.74 ms | 5.77 ms | 23.8 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.98 | 3.44 | 3.49 | 3.62 | 3.04 | 3.03 | 2.55* | 1.79 | 1.53 |


### `composeJC`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 4.49 µs* | 15.2 µs | 58.3 µs | 231 µs | 938 µs | 3.74 ms | 16.0 ms | 66.5 ms | 259 ms |
| `eigen_sparse` | 4.24 µs | 20.0 µs | 90.5 µs | 371 µs | 1.48 ms | 6.44 ms | 27.2 ms | 122 ms | 518 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 0.94* | 1.32 | 1.55 | 1.61 | 1.58 | 1.72 | 1.70 | 1.84 | 2.00 |


### `constructJC`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 3.47 µs | 10.2 µs | 36.7 µs | 148 µs | 616 µs | 2.50 ms | 11.5 ms | 67.6 ms | 228 ms |
| `eigen_sparse` | 1.97 µs | 7.45 µs | 37.9 µs | 160 µs | 648 µs | 2.67 ms | 13.1 ms | 71.9 ms | 267 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 0.57 | 0.73 | 1.03 | 1.08 | 1.05 | 1.07 | 1.14 | 1.06 | 1.17 |


### `picture1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal_static` | 72.8 ns | 174 ns | 709 ns | 2.86 µs | 11.7 µs | 48.6 µs* | 449 µs | 1.80 ms* | 8.44 ms |
| `multidiagonal_scalarfreq` | 292 ns | 607 ns | 2.28 µs | 8.34 µs | 36.7 µs | 148 µs | 998 µs | 4.64 ms | 19.6 ms |
| `multidiagonal_tabfreq` | 1.61 µs | 6.32 µs | 25.8 µs | 97.5 µs | 360 µs | 1.45 ms | 5.88 ms* | 28.7 ms | 130 ms |
| `handwritten_scalarfreq` | 271 ns | 692 ns | 2.66 µs | 12.3 µs | 50.3 µs | 194 µs | 862 µs | 4.76 ms | 23.2 ms |
| `handwritten_tabfreq` | 1.56 µs* | 6.35 µs* | 25.8 µs* | 97.3 µs | 348 µs | 1.40 ms | 5.46 ms | 22.4 ms | 91.1 ms |
| `eigen_sparse_pernnz` | 1.57 µs* | 6.69 µs* | 27.2 µs* | 105 µs | 397 µs | 1.59 ms | 6.29 ms | 25.8 ms | 142 ms |
| `csr_pernnz` | 1.55 µs* | 6.59 µs* | 26.9 µs* | 103 µs | 375 µs | 1.50 ms | 5.83 ms | 23.6 ms | 93.3 ms |
| `eigen_sparse_sandwich` | 239 ns | 904 ns | 3.69 µs | 15.8 µs | 65.9 µs | 267 µs | 1.19 ms | 5.45 ms | 23.1 ms |


### `slices3_axis0`

| time per application | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `multidiagonal` | 19.6 µs | 99.7 µs | 123 µs | 2.59 ms* | 3.82 ms |
| `eigen_sparse_term` | 44.3 µs | 306 µs | 372 µs | 3.22 ms | 11.9 ms |
| `multidiagonal_picture_linear` | 20.4 µs | 101 µs | 124 µs | 2.59 ms* | 3.78 ms |
| `multidiagonal_picture_quadratic` | 20.5 µs | 101 µs | 124 µs | 2.59 ms* | 3.78 ms |
| `eigen_sparse_term_picture_pernnz` | 46.1 µs | 310 µs | 376 µs | 3.26 ms* | 11.9 ms |

| time ÷ that of `multidiagonal` | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `eigen_sparse_term` | 2.26 | 3.07 | 3.02 | 1.24* | 3.10 |
| `multidiagonal_picture_linear` | 1.04 | 1.01 | 1.01 | 1.00* | 0.99 |
| `multidiagonal_picture_quadratic` | 1.05 | 1.01 | 1.01 | 1.00* | 0.99 |
| `eigen_sparse_term_picture_pernnz` | 2.35 | 3.11 | 3.05 | 1.26* | 3.11 |


### `slices3_axis2`

| time per application | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `multidiagonal` | 40.3 µs | 160 µs* | 273 µs | 2.55 ms | 7.53 ms |
| `eigen_sparse_term` | 45.0 µs | 307 µs | 435 µs | 3.89 ms* | 16.7 ms |
| `multidiagonal_picture_linear` | 41.3 µs | 162 µs* | 274 µs | 2.55 ms | 7.52 ms |
| `multidiagonal_picture_quadratic` | 41.5 µs | 162 µs* | 275 µs | 2.58 ms | 7.52 ms |
| `eigen_sparse_term_picture_pernnz` | 46.6 µs | 311 µs | 433 µs | 3.81 ms* | 16.7 ms |

| time ÷ that of `multidiagonal` | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `eigen_sparse_term` | 1.12 | 1.92* | 1.59 | 1.53* | 2.22 |
| `multidiagonal_picture_linear` | 1.02 | 1.01* | 1.00 | 1.00 | 1.00 |
| `multidiagonal_picture_quadratic` | 1.03 | 1.01* | 1.01 | 1.01 | 1.00 |
| `eigen_sparse_term_picture_pernnz` | 1.15 | 1.94* | 1.59 | 1.49* | 2.22 |


## 134ddcf: `main`, 4 KB pages (5 processes)

### `apply1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 71.7 ns | 173 ns | 696 ns | 3.04 µs | 14.7 µs | 58.4 µs | 248 µs | 1.67 ms | 6.78 ms |
| `eigen_sparse` | 132 ns | 556 ns | 2.28 µs | 9.56 µs | 40.0 µs | 159 µs | 707 µs | 3.18 ms | 12.4 ms |
| `eigen_dense` | 87.6 ns | 1.27 µs | 27.0 µs | 880 µs* | 14.7 ms |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.84 | 3.22 | 3.28 | 3.15 | 2.73 | 2.73 | 2.85 | 1.91 | 1.84 |
| `eigen_dense` | 1.22 | 7.35 | 38.9 | 289* | 1001 |  |  |  |  |


### `compose1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 502 ns | 1.13 µs | 4.10 µs | 15.3 µs | 63.8 µs | 253 µs | 1.35 ms | 6.94 ms | 54.5 ms |
| `eigen_sparse` | 1.63 µs | 6.11 µs | 26.9 µs | 108 µs | 428 µs | 1.92 ms | 7.64 ms | 46.9 ms | 260 ms |
| `eigen_dense` | 1.59 µs | 79.6 µs | 5.04 ms | 344 ms |  |  |  |  |  |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 3.24 | 5.40 | 6.56 | 7.09 | 6.71 | 7.59 | 5.68 | 6.76 | 4.76 |
| `eigen_dense` | 3.16 | 70.3 | 1230 | 22548 |  |  |  |  |  |


### `construct1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 1.42 µs | 6.24 µs | 28.6 µs | 116 µs | 464 µs | 1.86 ms | 7.45 ms | 30.7 ms | 139 ms |
| `eigen_sparse` | 2.37 µs | 9.53 µs | 43.6 µs | 192 µs | 780 µs | 3.17 ms | 13.9 ms | 107 ms | 437 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.68 | 1.53 | 1.52 | 1.65 | 1.68 | 1.71 | 1.87 | 3.50 | 3.13 |


### `applyJC_qbitfast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 173 ns | 503 ns | 2.16 µs* | 9.92 µs | 44.6 µs | 189 µs | 931 µs | 5.43 ms | 26.7 ms |
| `eigen_sparse` | 278 ns | 1.12 µs | 4.43 µs | 17.8 µs | 71.9 µs | 287 µs | 1.37 ms | 5.70 ms | 22.6 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.61 | 2.22 | 2.05* | 1.79 | 1.61 | 1.52 | 1.47 | 1.05 | 0.85 |


### `applyJC_modefast`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 137 ns | 320 ns | 1.27 µs* | 4.93 µs* | 23.8 µs | 95.7 µs | 515 µs | 2.88 ms | 12.3 ms |
| `eigen_sparse` | 270 ns | 1.11 µs | 4.43 µs | 17.8 µs | 73.5 µs | 294 µs | 1.38 ms | 5.87 ms | 23.7 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 1.97 | 3.48 | 3.49* | 3.61* | 3.09 | 3.08 | 2.69 | 2.04 | 1.92 |


### `composeJC`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 4.49 µs | 15.2 µs | 57.9 µs | 231 µs | 937 µs | 3.78 ms | 16.0 ms | 64.8 ms | 260 ms |
| `eigen_sparse` | 4.27 µs | 20.1 µs | 90.9 µs | 370 µs | 1.48 ms | 6.46 ms | 26.9 ms | 206 ms | 843 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 0.95 | 1.32 | 1.57 | 1.60 | 1.57 | 1.71 | 1.68 | 3.18 | 3.23 |


### `constructJC`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal` | 3.46 µs | 10.2 µs | 36.7 µs | 148 µs | 616 µs | 2.50 ms | 11.5 ms | 90.3 ms | 373 ms |
| `eigen_sparse` | 1.96 µs | 7.42 µs | 38.0 µs | 159 µs | 646 µs | 2.68 ms | 13.1 ms | 115 ms | 521 ms |

| time ÷ that of `multidiagonal` | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `eigen_sparse` | 0.57 | 0.73 | 1.04 | 1.08 | 1.05 | 1.07 | 1.14 | 1.28 | 1.40 |


### `picture1`

| time per application | 16 | 64 | 256 | 1024 | 4096 | 16384 | 65536 | 262144 | 1048576 |
|---|---|---|---|---|---|---|---|---|---|
| `multidiagonal_static` | 72.8 ns | 179 ns | 710 ns | 2.83 µs | 11.7 µs* | 54.0 µs | 250 µs | 1.43 ms | 6.76 ms |
| `multidiagonal_scalarfreq` | 292 ns | 607 ns | 2.28 µs | 8.33 µs | 36.8 µs | 153 µs | 811 µs | 4.04 ms | 17.6 ms |
| `multidiagonal_tabfreq` | 1.61 µs | 6.33 µs | 25.9 µs | 97.6 µs | 361 µs | 1.46 ms | 5.72 ms | 23.2 ms | 93.5 ms |
| `handwritten_scalarfreq` | 271 ns | 692 ns | 2.66 µs | 12.3 µs | 49.5 µs | 194 µs | 750 µs | 3.80 ms | 24.6 ms |
| `handwritten_tabfreq` | 1.55 µs | 6.35 µs | 25.8 µs | 98.0 µs | 348 µs | 1.41 ms | 5.48 ms | 22.6 ms | 91.7 ms |
| `eigen_sparse_pernnz` | 1.57 µs | 6.70 µs | 27.2 µs | 105 µs | 397 µs | 1.59 ms | 6.33 ms | 25.5 ms | 105 ms |
| `csr_pernnz` | 1.55 µs | 6.60 µs | 26.9 µs | 103 µs | 376 µs | 1.50 ms | 5.84 ms | 23.6 ms | 93.6 ms |
| `eigen_sparse_sandwich` | 239 ns | 906 ns | 3.69 µs | 16.1 µs | 65.9 µs | 268 µs | 1.20 ms | 5.34 ms | 24.1 ms |


### `slices3_axis0`

| time per application | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `multidiagonal` | 19.6 µs | 99.3 µs | 123 µs | 934 µs | 3.80 ms |
| `eigen_sparse_term` | 44.3 µs | 306 µs | 372 µs | 3.02 ms | 11.9 ms |
| `multidiagonal_picture_linear` | 20.3 µs | 100 µs | 126 µs | 951 µs | 3.80 ms |
| `multidiagonal_picture_quadratic` | 20.5 µs | 101 µs | 125 µs | 953 µs | 3.80 ms |
| `eigen_sparse_term_picture_pernnz` | 46.0 µs | 310 µs | 376 µs | 3.03 ms | 11.9 ms |

| time ÷ that of `multidiagonal` | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `eigen_sparse_term` | 2.26 | 3.08 | 3.01 | 3.23 | 3.12 |
| `multidiagonal_picture_linear` | 1.04 | 1.01 | 1.02 | 1.02 | 1.00 |
| `multidiagonal_picture_quadratic` | 1.04 | 1.01 | 1.01 | 1.02 | 1.00 |
| `eigen_sparse_term_picture_pernnz` | 2.35 | 3.12 | 3.05 | 3.24 | 3.12 |


### `slices3_axis2`

| time per application | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `multidiagonal` | 40.2 µs | 160 µs | 272 µs | 2.48 ms | 7.85 ms |
| `eigen_sparse_term` | 45.1 µs | 307 µs | 450 µs | 4.92 ms | 17.6 ms |
| `multidiagonal_picture_linear` | 40.4 µs | 161 µs | 272 µs | 2.48 ms | 7.90 ms |
| `multidiagonal_picture_quadratic` | 40.6 µs | 161 µs | 273 µs | 2.48 ms | 7.89 ms |
| `eigen_sparse_term_picture_pernnz` | 46.7 µs | 311 µs | 446 µs | 4.86 ms | 17.6 ms |

| time ÷ that of `multidiagonal` | 16 | 30 | 32 | 64 | 100 |
|---|---|---|---|---|---|
| `eigen_sparse_term` | 1.12 | 1.92 | 1.65 | 1.99 | 2.25 |
| `multidiagonal_picture_linear` | 1.01 | 1.01 | 1.00 | 1.00 | 1.01 |
| `multidiagonal_picture_quadratic` | 1.01 | 1.01 | 1.00 | 1.00 | 1.01 |
| `eigen_sparse_term_picture_pernnz` | 1.16 | 1.95 | 1.64 | 1.96 | 2.24 |


## 134ddcf: `kernels`, pages as the system hands them out (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.579 | 0.710 | 1.11 |
| `library_byDiagonals` | 0.661 | 0.812 | 1.61 |
| `library_byRows` | 0.577 | 0.710 | 1.11 |
| `hoisted` | 0.645 | 0.811 | 1.61 |
| `rowBlocked` | 0.642 | 0.800 | 1.24 |
| `eigen_sparse` | 2.02 | 2.06 | 2.52 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `library_byDiagonals` | 1.14 | 1.14 | 1.45 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.11 | 1.14 | 1.45 |
| `rowBlocked` | 1.11 | 1.13 | 1.12 |
| `eigen_sparse` | 3.49 | 2.90 | 2.27 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.605 | 0.702 | 1.77 |
| `library_byDiagonals` | 0.635 | 0.894 | 2.47 |
| `library_byRows` | 0.603 | 0.700 | 1.77 |
| `hoisted` | 0.632 | 0.895 | 2.49* |
| `eigen_sparse` | 1.95 | 2.14 | 2.71 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.05 | 1.27 | 1.40 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.04 | 1.27 | 1.41* |
| `eigen_sparse` | 3.22 | 3.05 | 1.53 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 3.10 | 2.12 |
| `blocked` | 3.97 | 1.64 |
| `library_materialized` | 2.37 | 1.71 |
| `library_materialized_byDiagonals` | 3.42 | 2.67 |
| `library_materialized_byRows` | 2.37 | 1.72 |
| `eigen_sparse_kron` | 2.55 | 2.57 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 3.26 | 5.03 | 1.07 | 3.63 |
| `diagonalwise_sliceIteratorOrder` | 3.15 | 7.95 | 0.910 | 2.83 |
| `rowwise_sliceIteratorOrder` | 3.32 | 8.31 | 3.21 | 6.29 |
| `termCSR_sliceIteratorOrder` | 3.35 | 8.48 | 2.97 | 6.41 |
| `library_memoryOrder` | 2.65 | 2.43 | 0.916 | 1.61 |
| `diagonalwise_memoryOrder` | 2.52 | 6.00 | 0.798 | 1.56 |
| `rowwise_memoryOrder` | 2.60 | 4.09 | 2.63 | 2.93 |
| `termCSR_memoryOrder` | 2.61 | 4.23 | 2.32 | 2.65 |
| `library_byDiagonals_memoryOrder` | 2.64 | 6.01 | 0.914 | 1.80 |
| `library_byRows_memoryOrder` | 1.77 | 2.40 | 1.25 | 1.57 |
| `eigen_sparse_kron` | 2.53 | 2.55 | 2.55 | 2.54 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.23 | 2.07 | 1.17 | 2.25 |
| `diagonalwise_sliceIteratorOrder` | 1.19 | 3.27 | 0.99 | 1.76 |
| `rowwise_sliceIteratorOrder` | 1.25 | 3.42 | 3.50 | 3.91 |
| `termCSR_sliceIteratorOrder` | 1.26 | 3.49 | 3.24 | 3.98 |
| `diagonalwise_memoryOrder` | 0.95 | 2.47 | 0.87 | 0.97 |
| `rowwise_memoryOrder` | 0.98 | 1.68 | 2.87 | 1.82 |
| `termCSR_memoryOrder` | 0.98 | 1.74 | 2.53 | 1.65 |
| `library_byDiagonals_memoryOrder` | 1.00 | 2.47 | 1.00 | 1.12 |
| `library_byRows_memoryOrder` | 0.67 | 0.99 | 1.36 | 0.98 |
| `eigen_sparse_kron` | 0.95 | 1.05 | 2.78 | 1.58 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.939 | 1.32 | 1.28 | 1.36 | 1.31 | 1.28 | 1.17 | 1.00 | 0.927 |
| `library_byDiagonals` | 0.939 | 1.33 | 1.27 | 1.36 | 1.30 | 1.27 | 1.17 | 1.00 | 0.922 |
| `library_byRows` | 10.7 | 6.47 | 5.67 | 4.06 | 3.36 | 2.71* | 2.09 | 1.29 | 0.927 |
| `hoisted` | 3.76 | 2.98 | 2.12 | 1.81 | 1.41 | 1.34 | 1.21 | 1.03 | 0.930 |
| `eigen_sparse` | 2.53 | 2.02 | 1.96 | 1.94 | 1.92 | 2.05 | 1.90 | 1.94 | 1.91 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.00 | 1.01 | 0.99 | 1.00 | 0.99 | 0.99 | 1.00 | 1.00 | 0.99 |
| `library_byRows` | 11.4 | 4.90 | 4.43 | 2.99 | 2.56 | 2.12* | 1.79 | 1.29 | 1.00 |
| `hoisted` | 4.00 | 2.26 | 1.66 | 1.33 | 1.08 | 1.05 | 1.03 | 1.03 | 1.00 |
| `eigen_sparse` | 2.69 | 1.53 | 1.53 | 1.43 | 1.47 | 1.60 | 1.62 | 1.94 | 2.06 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.78 | 1.08 | 0.831 | 0.784 | 0.653 | 0.638 | 0.552 | 0.579 | 0.633 |
| `library_byRows` | 2.87 | 1.60 | 1.17 | 1.02 | 0.798 | 0.759 | 0.616 | 0.631 | 0.591 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 3.16 | 2.62* | 0.981 | 2.41* | 0.860 | 2.29* | 0.800 | 2.23* | 2.25* |
| `library_byRows` | 3.15 | 2.11 | 1.36 | 1.70 | 1.47 | 1.60 | 1.58 | 2.10 | 1.35 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.31 | 1.17 | 1.05 | 1.01 | 0.953 | 0.920 | 0.900 | 0.888 | 0.875 | 0.894 |
| `library_byRows` | 3.36 | 2.09 | 1.55 | 1.30 | 1.06 | 0.928 | 0.831 | 0.779 | 0.704 | 0.663 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.17 | 2.13 | 1.35 | 1.97 | 1.28 | 1.87 | 1.19 | 1.84 | 1.84 | 1.87 |
| `library_byRows` | 3.50 | 2.60 | 1.69 | 1.86 | 1.25 | 1.47 | 1.32 | 1.43 | 1.49 | 1.21 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 3.06 | 2.96 | 2.48 | 2.83 | 2.39 | 2.77 | 2.32 | 2.66 | 2.66 | 2.71 |
| `library_byRows` | 3.71 | 2.70 | 2.02 | 2.05 | 1.81 | 1.94 | 2.06 | 2.01 | 2.19 | 1.63 |


## 134ddcf: `kernels`, 4 KB pages (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.579 | 0.708 | 1.07 |
| `library_byDiagonals` | 0.661 | 0.820* | 1.70 |
| `library_byRows` | 0.577 | 0.707 | 1.07 |
| `hoisted` | 0.648 | 0.820* | 1.70 |
| `rowBlocked` | 0.641 | 0.802* | 1.27 |
| `eigen_sparse` | 2.02 | 2.06 | 2.55 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `library_byDiagonals` | 1.14 | 1.16* | 1.59 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.12 | 1.16* | 1.59 |
| `rowBlocked` | 1.11 | 1.13* | 1.19 |
| `eigen_sparse` | 3.49 | 2.91 | 2.38 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.608 | 0.703 | 1.42* |
| `library_byDiagonals` | 0.633 | 0.890 | 2.16 |
| `library_byRows` | 0.603 | 0.703 | 1.42* |
| `hoisted` | 0.632 | 0.890 | 2.15 |
| `eigen_sparse` | 1.95 | 2.15 | 2.79 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.04 | 1.27 | 1.52* |
| `library_byRows` | 0.99 | 1.00 | 1.00* |
| `hoisted` | 1.04 | 1.27 | 1.51* |
| `eigen_sparse` | 3.21 | 3.06 | 1.96* |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 1.82 | 1.70 |
| `blocked` | 2.99 | 1.10 |
| `library_materialized` | 2.07 | 1.49 |
| `library_materialized_byDiagonals` | 2.41 | 2.42 |
| `library_materialized_byRows` | 2.07 | 1.49 |
| `eigen_sparse_kron` | 2.58 | 2.61 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.68* | 5.74 | 1.21 | 4.34 |
| `diagonalwise_sliceIteratorOrder` | 1.54 | 8.60* | 1.07 | 3.63 |
| `rowwise_sliceIteratorOrder` | 3.29 | 9.33 | 3.25 | 6.93 |
| `termCSR_sliceIteratorOrder` | 3.08 | 9.54 | 3.02 | 7.09 |
| `library_memoryOrder` | 0.990 | 2.56* | 0.929 | 1.66 |
| `diagonalwise_memoryOrder` | 0.890 | 6.22* | 0.812 | 1.46 |
| `rowwise_memoryOrder` | 2.61 | 4.45* | 2.64 | 2.99 |
| `termCSR_memoryOrder` | 2.31 | 4.66* | 2.33 | 2.72 |
| `library_byDiagonals_memoryOrder` | 0.986 | 6.26* | 0.922 | 1.68 |
| `library_byRows_memoryOrder` | 1.18 | 2.52* | 1.25 | 1.62 |
| `eigen_sparse_kron` | 2.54 | 2.60 | 2.59 | 2.60 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.70* | 2.24* | 1.30 | 2.61 |
| `diagonalwise_sliceIteratorOrder` | 1.56 | 3.36* | 1.15 | 2.19 |
| `rowwise_sliceIteratorOrder` | 3.32 | 3.64* | 3.50 | 4.17 |
| `termCSR_sliceIteratorOrder` | 3.11 | 3.73* | 3.25 | 4.27 |
| `diagonalwise_memoryOrder` | 0.90 | 2.43* | 0.87 | 0.88 |
| `rowwise_memoryOrder` | 2.64 | 1.74* | 2.84 | 1.80 |
| `termCSR_memoryOrder` | 2.33 | 1.82* | 2.51 | 1.64 |
| `library_byDiagonals_memoryOrder` | 1.00 | 2.45* | 0.99 | 1.01 |
| `library_byRows_memoryOrder` | 1.19 | 0.98* | 1.35 | 0.98 |
| `eigen_sparse_kron` | 2.57 | 1.02* | 2.79 | 1.57 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.949* | 1.35* | 1.29 | 1.38* | 1.32* | 1.29* | 1.18* | 1.02* | 0.934 |
| `library_byDiagonals` | 0.949* | 1.35* | 1.29 | 1.38* | 1.32* | 1.28* | 1.18* | 1.01* | 0.935 |
| `library_byRows` | 10.7 | 6.59 | 5.71 | 4.08 | 3.37 | 2.77 | 2.10 | 1.30 | 0.935 |
| `hoisted` | 3.76 | 3.10 | 2.16 | 1.83* | 1.43* | 1.36* | 1.23* | 1.04* | 0.947 |
| `eigen_sparse` | 2.54 | 2.03 | 1.97 | 1.95 | 1.93 | 2.07* | 1.91 | 1.94 | 1.92 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.00* | 1.00* | 1.00 | 1.00* | 1.00* | 0.99* | 1.00* | 0.99* | 1.00 |
| `library_byRows` | 11.3* | 4.88* | 4.43 | 2.96* | 2.55* | 2.15* | 1.78* | 1.27* | 1.00 |
| `hoisted` | 3.96* | 2.30* | 1.67 | 1.33* | 1.08* | 1.05* | 1.04* | 1.02* | 1.01 |
| `eigen_sparse` | 2.68* | 1.50* | 1.53 | 1.41* | 1.46* | 1.60* | 1.62* | 1.90* | 2.06 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.78* | 1.08* | 0.830 | 0.785* | 0.653 | 0.644* | 0.551 | 0.569* | 0.632* |
| `library_byRows` | 2.88 | 1.61 | 1.17 | 1.02 | 0.798 | 0.760* | 0.617 | 0.631* | 0.591* |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.92 | 1.20 | 1.00 | 0.969 | 0.852 | 0.859 | 0.790 | 0.785 | 0.858 |
| `library_byRows` | 2.96 | 1.71 | 1.36 | 1.17 | 1.44 | 1.05 | 1.53 | 1.77 | 0.798 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.32* | 1.18* | 1.07 | 1.03 | 0.965 | 0.935 | 0.915 | 0.906 | 0.892 | 0.906 |
| `library_byRows` | 3.36 | 2.09 | 1.56 | 1.29 | 1.06 | 0.931 | 0.836 | 0.782 | 0.705 | 0.665 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.55 | 1.46 | 1.35 | 1.35 | 1.26 | 1.28 | 1.21 | 1.24 | 1.25 | 1.28 |
| `library_byRows` | 3.50 | 2.46 | 1.75 | 1.64* | 1.30 | 1.24 | 1.36 | 1.26 | 1.41 | 0.967 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.76 | 2.60 | 2.49 | 2.46 | 2.38 | 2.36 | 2.36 | 2.34 | 2.29 | 2.36 |
| `library_byRows` | 3.66 | 2.44 | 2.03 | 1.78 | 1.82 | 1.74 | 2.06 | 1.86 | 2.04 | 1.47 |


## 134ddcf: `kernels-cxlr`, pages as the system hands them out (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.579 | 0.711 | 1.14* |
| `library_byDiagonals` | 0.661 | 0.784* | 1.63 |
| `library_byRows` | 0.576 | 0.712 | 1.14* |
| `hoisted` | 0.647 | 0.786* | 1.62 |
| `rowBlocked` | 0.641 | 0.802* | 1.25 |
| `eigen_sparse` | 1.58 | 1.71 | 2.15 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `library_byDiagonals` | 1.14 | 1.10* | 1.43* |
| `library_byRows` | 0.99 | 1.00 | 1.00* |
| `hoisted` | 1.12 | 1.11* | 1.42* |
| `rowBlocked` | 1.11 | 1.13* | 1.10* |
| `eigen_sparse` | 2.73 | 2.41 | 1.89* |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.607 | 0.710 | 1.81 |
| `library_byDiagonals` | 0.632 | 0.899 | 2.48 |
| `library_byRows` | 0.604 | 0.709 | 1.81 |
| `hoisted` | 0.634 | 0.899 | 2.49 |
| `eigen_sparse` | 1.77 | 1.95 | 2.53 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.04 | 1.27 | 1.37 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.04 | 1.27 | 1.38 |
| `eigen_sparse` | 2.92 | 2.75 | 1.40 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 3.12 | 2.14 |
| `blocked` | 3.96 | 1.65 |
| `library_materialized` | 2.37 | 1.72 |
| `library_materialized_byDiagonals` | 3.41 | 2.68 |
| `library_materialized_byRows` | 2.37 | 1.72 |
| `eigen_sparse_kron` | 2.28 | 2.37 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 3.27 | 5.09 | 1.07 | 3.65 |
| `diagonalwise_sliceIteratorOrder` | 3.15 | 7.99 | 0.911 | 2.83 |
| `rowwise_sliceIteratorOrder` | 3.40 | 8.25 | 3.32 | 6.22 |
| `termCSR_sliceIteratorOrder` | 3.21 | 7.79 | 2.40 | 5.97 |
| `library_memoryOrder` | 2.65 | 2.44 | 0.916 | 1.61 |
| `diagonalwise_memoryOrder` | 2.52 | 6.08 | 0.798 | 1.57 |
| `rowwise_memoryOrder` | 2.71 | 4.03 | 2.74 | 2.88 |
| `termCSR_memoryOrder` | 2.49 | 3.87 | 1.74 | 2.18 |
| `library_byDiagonals_memoryOrder` | 2.64 | 6.07 | 0.913 | 1.78 |
| `library_byRows_memoryOrder` | 1.77 | 2.40 | 1.25 | 1.58 |
| `eigen_sparse_kron` | 2.29 | 2.39 | 2.32 | 2.35 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.23 | 2.09 | 1.17 | 2.27 |
| `diagonalwise_sliceIteratorOrder` | 1.19 | 3.27 | 0.99 | 1.76 |
| `rowwise_sliceIteratorOrder` | 1.28 | 3.38 | 3.62 | 3.86 |
| `termCSR_sliceIteratorOrder` | 1.21 | 3.19 | 2.62 | 3.71 |
| `diagonalwise_memoryOrder` | 0.95 | 2.49 | 0.87 | 0.98 |
| `rowwise_memoryOrder` | 1.02 | 1.65 | 2.99 | 1.79 |
| `termCSR_memoryOrder` | 0.94 | 1.59 | 1.90 | 1.35 |
| `library_byDiagonals_memoryOrder` | 1.00 | 2.49 | 1.00 | 1.11 |
| `library_byRows_memoryOrder` | 0.67 | 0.98 | 1.36 | 0.98 |
| `eigen_sparse_kron` | 0.86 | 0.98 | 2.53 | 1.46 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.938 | 1.32 | 1.27 | 1.36 | 1.31 | 1.28 | 1.17 | 1.01 | 0.928 |
| `library_byDiagonals` | 0.938 | 1.32 | 1.27 | 1.36 | 1.31 | 1.28 | 1.17 | 1.00 | 0.919 |
| `library_byRows` | 10.7* | 6.48 | 5.64 | 4.08 | 3.36 | 2.67 | 2.09 | 1.29 | 0.927 |
| `hoisted` | 3.75 | 2.98 | 2.12 | 1.82 | 1.42 | 1.35 | 1.22 | 1.03 | 0.933 |
| `eigen_sparse` | 2.08 | 1.59 | 1.54 | 1.53 | 1.51 | 1.69* | 1.49 | 1.53 | 1.50 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 0.99 | 0.99 |
| `library_byRows` | 11.4* | 4.91 | 4.44 | 3.00 | 2.56 | 2.09 | 1.79 | 1.28 | 1.00 |
| `hoisted` | 4.00 | 2.26 | 1.67 | 1.34 | 1.08 | 1.05 | 1.04 | 1.02 | 1.01 |
| `eigen_sparse` | 2.22 | 1.20 | 1.21 | 1.12 | 1.15 | 1.32* | 1.27 | 1.51 | 1.62 |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.78 | 1.08 | 0.830 | 0.786 | 0.654 | 0.638 | 0.551 | 0.569 | 0.644 |
| `library_byRows` | 2.87 | 1.61 | 1.18 | 1.03 | 0.803 | 0.761 | 0.619 | 0.634 | 0.597 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 3.16 | 2.62 | 0.979 | 2.41* | 0.862 | 2.29* | 0.800 | 2.23* | 2.25* |
| `library_byRows` | 3.15 | 2.12 | 1.35 | 1.71* | 1.47 | 1.59* | 1.60 | 2.11 | 1.35* |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.31 | 1.17 | 1.06 | 1.02 | 0.949 | 0.922 | 0.899 | 0.883 | 0.866 | 0.882 |
| `library_byRows` | 3.36 | 2.09 | 1.56 | 1.29 | 1.06 | 0.925 | 0.833 | 0.782 | 0.704 | 0.658 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.17 | 2.13 | 1.36 | 1.97 | 1.28 | 1.87 | 1.19 | 1.84 | 1.84 | 1.87 |
| `library_byRows` | 3.50 | 2.57 | 1.69 | 1.85 | 1.26 | 1.47 | 1.28 | 1.41 | 1.48 | 1.21 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 3.04 | 2.97 | 2.43 | 2.85 | 2.36 | 2.73 | 2.38 | 2.70 | 2.66 | 2.75 |
| `library_byRows` | 3.69 | 2.70 | 2.02 | 2.06 | 1.82 | 1.94 | 2.08 | 2.03 | 2.27 | 1.63 |


## 134ddcf: `kernels-cxlr`, 4 KB pages (5 processes)

### case `A_rank1`

| ns per nonzero | 256 | 4096 | 262144 |
|---|---|---|---|
| `library` | 0.579 | 0.711 | 1.07 |
| `library_byDiagonals` | 0.659 | 0.803 | 1.71 |
| `library_byRows` | 0.577 | 0.711 | 1.07 |
| `hoisted` | 0.645 | 0.803 | 1.71 |
| `rowBlocked` | 0.642 | 0.801 | 1.27 |
| `eigen_sparse` | 1.58 | 1.71 | 2.17 |

| time ÷ that of `library` | 256 | 4096 | 262144 |
|---|---|---|---|
| `library_byDiagonals` | 1.14 | 1.13 | 1.60 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.11 | 1.13 | 1.60 |
| `rowBlocked` | 1.11 | 1.13 | 1.19 |
| `eigen_sparse` | 2.73 | 2.41 | 2.03 |


### case `B_JC_modefast`

| ns per nonzero | 512 | 8192 | 524288 |
|---|---|---|---|
| `library` | 0.604 | 0.703 | 1.44 |
| `library_byDiagonals` | 0.635 | 0.889 | 2.17 |
| `library_byRows` | 0.601 | 0.705 | 1.44 |
| `hoisted` | 0.633 | 0.889 | 2.18 |
| `eigen_sparse` | 1.77 | 1.95 | 2.57 |

| time ÷ that of `library` | 512 | 8192 | 524288 |
|---|---|---|---|
| `library_byDiagonals` | 1.05 | 1.26 | 1.51 |
| `library_byRows` | 1.00 | 1.00 | 1.00 |
| `hoisted` | 1.05 | 1.26 | 1.51 |
| `eigen_sparse` | 2.93 | 2.77 | 1.78 |


### cases `C_…`

| ns per nonzero | `C_axis0` | `C_axis2` |
|---|---|---|
| `diagonalwise` | 1.83 | 1.69 |
| `blocked` | 2.99 | 1.11 |
| `library_materialized` | 2.07 | 1.49 |
| `library_materialized_byDiagonals` | 2.42 | 2.42 |
| `library_materialized_byRows` | 2.07 | 1.49 |
| `eigen_sparse_kron` | 2.29 | 2.42 |


### cases `D_…`

| ns per nonzero | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.63 | 5.70 | 1.21 | 4.34 |
| `diagonalwise_sliceIteratorOrder` | 1.51 | 8.51 | 1.08 | 3.62 |
| `rowwise_sliceIteratorOrder` | 3.38 | 9.17 | 3.37 | 6.82 |
| `termCSR_sliceIteratorOrder` | 2.58 | 8.75 | 2.43 | 6.59 |
| `library_memoryOrder` | 0.966 | 2.55 | 0.929 | 1.66 |
| `diagonalwise_memoryOrder` | 0.863 | 6.09 | 0.813 | 1.46 |
| `rowwise_memoryOrder` | 2.71 | 4.10 | 2.75 | 2.93 |
| `termCSR_memoryOrder` | 1.73 | 3.95* | 1.75 | 2.32 |
| `library_byDiagonals_memoryOrder` | 0.958 | 6.10 | 0.922 | 1.68 |
| `library_byRows_memoryOrder` | 1.16 | 2.52 | 1.25 | 1.63 |
| `eigen_sparse_kron` | 2.30 | 2.44 | 2.36 | 2.41 |

| time ÷ that of `library_memoryOrder` | `D_n32_axis0` | `D_n32_axis2` | `D_n30_axis0` | `D_n30_axis2` |
|---|---|---|---|---|
| `library_sliceIteratorOrder` | 1.69 | 2.24 | 1.30 | 2.61 |
| `diagonalwise_sliceIteratorOrder` | 1.56 | 3.34 | 1.16 | 2.18 |
| `rowwise_sliceIteratorOrder` | 3.50 | 3.60 | 3.63 | 4.11 |
| `termCSR_sliceIteratorOrder` | 2.67 | 3.43 | 2.62 | 3.97 |
| `diagonalwise_memoryOrder` | 0.89 | 2.39 | 0.88 | 0.88 |
| `rowwise_memoryOrder` | 2.81 | 1.61 | 2.96 | 1.77 |
| `termCSR_memoryOrder` | 1.79 | 1.55* | 1.88 | 1.40 |
| `library_byDiagonals_memoryOrder` | 0.99 | 2.39 | 0.99 | 1.01 |
| `library_byRows_memoryOrder` | 1.20 | 0.99 | 1.35 | 0.98 |
| `eigen_sparse_kron` | 2.38 | 0.96 | 2.54 | 1.45 |


### cases `E_…`

| ns per nonzero | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library` | 0.939 | 1.37* | 1.29 | 1.38 | 1.33 | 1.29 | 1.18 | 1.01* | 0.936* |
| `library_byDiagonals` | 0.939 | 1.37* | 1.29 | 1.38 | 1.32 | 1.29 | 1.17 | 1.01* | 0.942* |
| `library_byRows` | 10.7 | 6.60 | 5.71 | 4.09 | 3.37 | 2.77 | 2.09 | 1.29 | 0.936* |
| `hoisted` | 3.75 | 3.04 | 2.15 | 1.83 | 1.44 | 1.36 | 1.22 | 1.04* | 0.948* |
| `eigen_sparse` | 2.10 | 1.60 | 1.55 | 1.54 | 1.52 | 1.67 | 1.51 | 1.54 | 1.52 |

| time ÷ that of `library` | `E_axis0_1` | `E_axis0_2` | `E_axis0_3` | `E_axis0_4` | `E_axis0_5` | `E_axis0_6` | `E_axis0_8` | `E_axis0_16` | `E_axis0_32` |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.00 | 1.00* | 1.00 | 1.00 | 0.99 | 1.00 | 0.99 | 1.00* | 1.01* |
| `library_byRows` | 11.4 | 4.82* | 4.43 | 2.96 | 2.53 | 2.15 | 1.77 | 1.28* | 1.00* |
| `hoisted` | 3.99 | 2.22* | 1.67 | 1.33 | 1.08 | 1.05 | 1.03 | 1.03* | 1.01* |
| `eigen_sparse` | 2.24 | 1.17* | 1.20 | 1.12 | 1.14 | 1.29 | 1.28 | 1.52* | 1.62* |


### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.78 | 1.08 | 0.831 | 0.786 | 0.654 | 0.638 | 0.551 | 0.569 | 0.632 |
| `library_byRows` | 2.88 | 1.62 | 1.18 | 1.03 | 0.800 | 0.761 | 0.617 | 0.632 | 0.593 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.92 | 1.22 | 1.00 | 0.969 | 0.854 | 0.862 | 0.793 | 0.787 | 0.860 |
| `library_byRows` | 2.95 | 1.69 | 1.36 | 1.17 | 1.44 | 1.05 | 1.54 | 1.78 | 0.800 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.32 | 1.17* | 1.07 | 1.03* | 0.967 | 0.937* | 0.917 | 0.907* | 0.893* | 0.905* |
| `library_byRows` | 3.37 | 2.09 | 1.56 | 1.30 | 1.06 | 0.934* | 0.835 | 0.786* | 0.710* | 0.671* |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.60 | 1.48 | 1.34 | 1.35 | 1.26 | 1.28 | 1.20 | 1.24 | 1.25 | 1.28 |
| `library_byRows` | 3.49 | 2.47 | 1.75 | 1.65 | 1.30 | 1.24 | 1.36 | 1.27 | 1.41 | 0.971 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.77 | 2.59 | 2.49 | 2.45 | 2.38 | 2.36 | 2.36 | 2.34 | 2.29 | 2.36 |
| `library_byRows` | 3.66 | 2.44 | 2.03 | 1.78 | 1.82 | 1.73 | 2.06 | 1.86 | 2.04 | 1.48 |


