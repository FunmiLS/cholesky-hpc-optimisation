# C2 Cholesky Factorisation Optimisation

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

C++ project to perform serial optimisation and OpenMP parallelisation on Cholesky factorisation algorithms.

## Repository Structure


```
include/            # public headers
src/                # implementation sources
example/            # example program
test/               # correctness tests
scripts/            # example CSD3 submission scripts
output/             # generated benchmark CSV files
report/             # written report 
figs/               # output figures for report
requirements.txt    # python dependencies
LICENSE             # MIT License
Makefile            # CMake wrapper
```

## Build

```bash
git clone https://gitlab.developers.cam.ac.uk/phy/data-intensive-science-mphil/assessments/c2_coursework/ol306.git

cd ol306
```

From the repository root:

```bash
cmake -S . -B build
cmake --build build -j
```

### Build directory structure

This repository contains a build folder with subdirectories:

- **build/threads** — thread scaling experiments
- **build/matrix** — matrix-size experiments
- **build/blocking** — blocking benchmarks
- **build/example**  — example program
- **build/test**   — test build


## Example Code

The example/main.cpp file illustrates the use of my naive OpenMP implementation (V11). 

The following parameters were set:
- Block Size: 64
- Matrix Size: 2
- Number of Threads: 4 (default)

Build and run:

```bash
cmake -S . -B build
cmake --build build --target example -j
./build/example/example
```

## Tests

Configure and run:

```bash
cmake -S . -B build/test -DBUILD_TESTING=ON -DBUILD_BENCHMARKS=OFF -DBUILD_EXAMPLE=OFF
cmake --build build/test -j
ctest --test-dir build/test --output-on-failure
```

## Benchmarks

### Matrix-size experiments

- block size: 64
- thread count: 16 for OpenMP parallelised files
- matrix sizes: 128, 256, 512, 768, 1024, 2048, 4096
- cores: --cpus-per-task=16

Run on CSD3 with:

```bash
sbatch scripts/runtime_matrix.sh
```

### Block-size experiments

- matrix sizes: 1024
- block sizes: 8, 16, 32, 64, 128, 256, 512
- cores: --cpus-per-task=1

Run with:

```bash
sbatch scripts/runtime_block.sh
```

### Thread-scaling experiments

- matrix size: 4096
- block size: 64
- thread counts: 1 to 70
- cores: --cpus-per-task=70

Run with:

```bash
sbatch scripts/runtime_threads.sh
```

## Example CSD3 Scripts

Example SLURM scripts are provided in scripts/:

- scripts/runtime_matrix.sh
- scripts/runtime_block.sh
- scripts/runtime_threads.sh

## HPC Reproducibility

- Platform: Cambridge CSD3 Cluster, icelake partition (Intel Xeon Ice Lake)
- Compiler Flag: -O3

Further details related to HPC use is outlined in the report.pdf

## Use of Auto-Generative Tools
This project utilised auto-generative tools in the development of plotting functions, testing and error handling, writing results to CSVs, commenting style and LaTeX formatting. It was also used to assess the project for software development best practices.

Example prompts used for this project:
- Generate Python code for a plot.
- Generate doc-strings and type hints for this function.
- Generate a README template for this project.
- Generate a Makefile.
- Generate LaTeX formula for this equation. 
