# Session 2026-11-04-bench-caseH2

Generated from the raw results in this directory by `benchmarks/reduce.py report`; not to be edited.

- A value is the median over the processes of its configuration.
- The main suite's values are times per application, those of `kernels` nanoseconds per nonzero.
- The second table of a section gives each implementation's time over that of the library: above 1 the library is faster.
- A star marks a value whose processes span more than 10% of their median.

## How well the processes agree

| build | suite | pages | processes | values | deviation from the median: half of the figures below | 90% below | largest | values spanning more than 10% | rounds |
|---|---|---|---|---|---|---|---|---|---|
| caseH | kernels.H | thp | 5 | 168 | 0.0% | 2.2% | 5.3% | 0 | 0.999–1.016 |

A figure is one process's result for one value, and its deviation is taken from the median over the processes. The last column is the range, over the rounds, of the geometric mean of the figures relative to their medians: a drift of the machine during the session shows there.

## caseH: `kernels.H`, pages as the system hands them out (5 processes)

### case `H_huge_2^24-4096`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.08 | 0.916 | 0.828 |
| `library_byRows` | 1.68 | 1.33 | 1.11 |


### case `H_huge_2^24-240`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.17 | 0.952 | 0.841 |
| `library_byRows` | 1.70 | 1.35 | 1.17 |


### case `H_huge_2^24-16`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.69 | 1.53 | 1.48 |
| `library_byRows` | 2.26 | 1.99 | 1.82 |


### case `H_huge_2^24+0`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 2.33 | 2.19 | 2.14 |
| `library_byRows` | 2.28 | 2.00 | 1.85 |


### case `H_huge_2^24+16`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 3.03 | 2.91 | 2.85 |
| `library_byRows` | 2.30 | 2.03 | 1.90 |


### case `H_huge_2^24+64`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 2.93 | 2.87 | 2.87 |
| `library_byRows` | 2.12 | 1.84 | 1.66 |


### case `H_huge_2^24+256`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.12 | 1.00 | 0.949 |
| `library_byRows` | 1.70 | 1.36 | 1.26 |


### case `H_huge_2^24+1024`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.13 | 0.962 | 0.858 |
| `library_byRows` | 1.71 | 1.37 | 1.17 |


### case `H_huge_2^24+2048`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.13 | 0.954 | 0.843 |
| `library_byRows` | 1.71 | 1.39 | 1.18 |


### case `H_huge_2^24+4112`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.12 | 0.928 | 0.871 |
| `library_byRows` | 1.70 | 1.34 | 1.17 |


### case `H_huge_2^24+65552`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.13 | 0.921 | 0.872 |
| `library_byRows` | 1.70 | 1.35 | 1.18 |


### case `H_huge_2^24+1048592`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 3.03 | 2.90 | 2.86 |
| `library_byRows` | 2.31 | 2.02 | 1.90 |


### case `H_huge_2^24+2097168`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 3.03 | 2.91 | 2.85 |
| `library_byRows` | 2.30 | 2.03 | 1.89 |


### case `H_huge_2^24+16777232`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 3.03 | 2.91 | 2.85 |
| `library_byRows` | 2.30 | 2.03 | 1.90 |


### case `H_4k_2^24-4096`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.11 | 0.926 | 0.842 |
| `library_byRows` | 1.68 | 1.33 | 1.13 |


### case `H_4k_2^24-240`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.10 | 0.933 | 0.850 |
| `library_byRows` | 1.71 | 1.35 | 1.18 |


### case `H_4k_2^24-16`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.11 | 0.925 | 0.854 |
| `library_byRows` | 1.68 | 1.34 | 1.15 |


### case `H_4k_2^24+0`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.10 | 0.926 | 0.838 |
| `library_byRows` | 1.68 | 1.33 | 1.13 |


### case `H_4k_2^24+16`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.14 | 0.933 | 0.886 |
| `library_byRows` | 1.71 | 1.36 | 1.19 |


### case `H_4k_2^24+64`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.14 | 0.961 | 0.902 |
| `library_byRows` | 1.71 | 1.39 | 1.16 |


### case `H_4k_2^24+256`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.13 | 0.928 | 0.893 |
| `library_byRows` | 1.71 | 1.37 | 1.27 |


### case `H_4k_2^24+1024`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.14 | 0.957 | 0.865 |
| `library_byRows` | 1.72 | 1.38 | 1.17 |


### case `H_4k_2^24+2048`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.15 | 0.957 | 0.860 |
| `library_byRows` | 1.72 | 1.39 | 1.18 |


### case `H_4k_2^24+4112`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.14 | 0.942 | 0.886 |
| `library_byRows` | 1.71 | 1.36 | 1.19 |


### case `H_4k_2^24+65552`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.14 | 0.938 | 0.884 |
| `library_byRows` | 1.71 | 1.36 | 1.19 |


### case `H_4k_2^24+1048592`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.14 | 0.939 | 0.886 |
| `library_byRows` | 1.72 | 1.35 | 1.19 |


### case `H_4k_2^24+2097168`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.14 | 0.937 | 0.886 |
| `library_byRows` | 1.72 | 1.36 | 1.19 |


### case `H_4k_2^24+16777232`

| ns per nonzero | 16 | 24 | 32 |
|---|---|---|---|
| `library_byDiagonals` | 1.14 | 0.939 | 0.889 |
| `library_byRows` | 1.72 | 1.36 | 1.19 |


