# Session 2026-11-04-bench-caseH

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| caseH | kernels.F+H | thp | 5 | 192 | 0.2% | 2.1% | 10.3% | 5 | 0.994–1.008 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## caseH: `kernels.F+H`, pages as the system hands them out (5 processes)

### case `F_slices_2048`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.64 | 0.972 | 0.793 | 0.704 | 0.611 | 0.557 | 0.536 | 0.560 | 0.673 |
| `library_byRows` | 2.95 | 1.61 | 1.19 | 0.996 | 0.805 | 0.707 | 0.626 | 0.599 | 0.559 |


### case `F_slices_1048576`

| ns per nonzero | 8 | 16 | 24 | 32 | 48 | 64 | 96 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.80 | 2.77 | 0.950 | 2.61 | 0.792 | 2.48 | 0.758 | 2.44 | 2.48 |
| `library_byRows` | 3.01 | 2.19 | 1.36 | 1.75 | 1.15 | 1.77 | 1.36* | 1.68* | 1.45 |


### case `F_grid_8192`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 1.30 | 1.16 | 1.05 | 1.01 | 0.946 | 0.929 | 0.908 | 0.898 | 0.878 | 0.907 |
| `library_byRows` | 3.15 | 1.99 | 1.50 | 1.25 | 1.03 | 0.911 | 0.823 | 0.781 | 0.709 | 0.645 |


### case `F_grid_65536`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.16 | 2.10 | 1.32 | 1.27 | 1.22 | 1.21 | 1.18 | 1.18 | 1.18 | 1.22 |
| `library_byRows` | 3.33 | 2.48* | 1.63 | 1.44 | 1.22 | 1.10 | 1.25 | 1.33 | 1.25 | 1.06 |


### case `F_grid_1048576`

| ns per nonzero | 5 | 8 | 12 | 16 | 24 | 32 | 48 | 64 | 128 | 256 |
|---|---|---|---|---|---|---|---|---|---|---|
| `library_byDiagonals` | 2.88 | 2.89 | 2.28 | 2.66 | 2.22 | 2.55 | 2.12 | 2.56 | 2.49 | 2.51 |
| `library_byRows` | 3.49 | 2.58 | 1.94 | 1.97 | 1.80 | 1.75 | 1.94 | 2.20 | 2.11 | 1.95 |


### case `H_1048576_psi+0`

| ns per nonzero | 8 | 16 | 32 | 64 |
|---|---|---|---|---|
| `library_byDiagonals` | 2.48 | 1.80 | 0.832 | 0.742 |
| `library_byRows` | 3.20 | 2.02 | 1.11 | 1.01 |


### case `H_1048576_psi+16`

| ns per nonzero | 8 | 16 | 32 | 64 |
|---|---|---|---|---|
| `library_byDiagonals` | 2.12 | 1.44 | 0.879 | 0.787 |
| `library_byRows` | 3.28 | 2.03 | 1.15 | 1.03 |


### case `H_1048576_psi+2048`

| ns per nonzero | 8 | 16 | 32 | 64 |
|---|---|---|---|---|
| `library_byDiagonals` | 1.80 | 1.14 | 0.853 | 0.738 |
| `library_byRows` | 3.02 | 1.71 | 1.16 | 1.09 |


### case `H_1048576_psi+4080`

| ns per nonzero | 8 | 16 | 32 | 64 |
|---|---|---|---|---|
| `library_byDiagonals` | 1.80 | 1.11 | 0.855 | 0.758 |
| `library_byRows` | 2.98 | 1.66 | 1.14 | 1.04 |


### case `H_1048568_psi+0`

| ns per nonzero | 8 |
|---|---|
| `library_byDiagonals` | 2.49 |
| `library_byRows` | 3.19 |


### case `H_1048568_psi+16`

| ns per nonzero | 8 |
|---|---|
| `library_byDiagonals` | 2.12 |
| `library_byRows` | 3.27 |


### case `H_1048568_psi+2048`

| ns per nonzero | 8 |
|---|---|
| `library_byDiagonals` | 1.80 |
| `library_byRows` | 3.03 |


### case `H_1048568_psi+4080`

| ns per nonzero | 8 |
|---|---|
| `library_byDiagonals` | 1.80 |
| `library_byRows` | 2.99 |


### case `H_1048560_psi+0`

| ns per nonzero | 16 | 24 | 48 |
|---|---|---|---|
| `library_byDiagonals` | 1.81 | 1.56* | 0.789 |
| `library_byRows` | 2.02 | 1.65 | 1.15 |


### case `H_1048560_psi+16`

| ns per nonzero | 16 | 24 | 48 |
|---|---|---|---|
| `library_byDiagonals` | 1.44 | 1.20 | 0.799 |
| `library_byRows` | 2.03 | 1.67 | 1.12 |


### case `H_1048560_psi+2048`

| ns per nonzero | 16 | 24 | 48 |
|---|---|---|---|
| `library_byDiagonals` | 1.14 | 0.950 | 0.791 |
| `library_byRows` | 1.71 | 1.38 | 1.27 |


### case `H_1048560_psi+4080`

| ns per nonzero | 16 | 24 | 48 |
|---|---|---|---|
| `library_byDiagonals` | 1.12 | 0.942 | 0.802 |
| `library_byRows` | 1.68 | 1.34 | 1.14 |


### case `H_1048536_psi+0`

| ns per nonzero | 24 |
|---|---|
| `library_byDiagonals` | 1.56* |
| `library_byRows` | 1.66 |


### case `H_1048536_psi+16`

| ns per nonzero | 24 |
|---|---|
| `library_byDiagonals` | 1.21 |
| `library_byRows` | 1.67 |


### case `H_1048536_psi+2048`

| ns per nonzero | 24 |
|---|---|
| `library_byDiagonals` | 0.949 |
| `library_byRows` | 1.38 |


### case `H_1048536_psi+4080`

| ns per nonzero | 24 |
|---|---|
| `library_byDiagonals` | 0.937 |
| `library_byRows` | 1.33 |


### case `H_1048544_psi+0`

| ns per nonzero | 32 |
|---|---|
| `library_byDiagonals` | 0.832 |
| `library_byRows` | 1.11 |


### case `H_1048544_psi+16`

| ns per nonzero | 32 |
|---|---|
| `library_byDiagonals` | 0.878 |
| `library_byRows` | 1.15 |


### case `H_1048544_psi+2048`

| ns per nonzero | 32 |
|---|---|
| `library_byDiagonals` | 0.852 |
| `library_byRows` | 1.16 |


### case `H_1048544_psi+4080`

| ns per nonzero | 32 |
|---|---|
| `library_byDiagonals` | 0.856 |
| `library_byRows` | 1.14 |


### case `H_1048512_psi+0`

| ns per nonzero | 48 | 64 |
|---|---|---|
| `library_byDiagonals` | 0.787 | 0.740 |
| `library_byRows` | 1.15 | 1.01 |


### case `H_1048512_psi+16`

| ns per nonzero | 48 | 64 |
|---|---|---|
| `library_byDiagonals` | 0.797 | 0.789 |
| `library_byRows` | 1.13 | 1.02 |


### case `H_1048512_psi+2048`

| ns per nonzero | 48 | 64 |
|---|---|---|
| `library_byDiagonals` | 0.791 | 0.738 |
| `library_byRows` | 1.28 | 1.08 |


### case `H_1048512_psi+4080`

| ns per nonzero | 48 | 64 |
|---|---|---|
| `library_byDiagonals` | 0.801 | 0.761 |
| `library_byRows` | 1.14 | 1.05 |


