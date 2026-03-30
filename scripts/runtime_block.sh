#!/bin/bash
#SBATCH --job-name=blocking
#SBATCH -J cpujob
#SBATCH -A MPHIL-DIS-SL2-CPU
#SBATCH -p icelake
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --time=00:10:00
#SBATCH --mail-type=NONE
#SBATCH --no-requeue

set -euo pipefail

# 1. Environment setup
. /etc/profile.d/modules.sh
module purge
module load rhel8/default-icl

# 2. Paths and runtime configuration
workdir="${SLURM_SUBMIT_DIR:-$(pwd)}"
# Default to build/blocking but allow override with BUILD_DIR
build_dir="${BUILD_DIR:-$workdir/build/blocking}"
build_jobs="${SLURM_CPUS_PER_TASK:-1}"
targets=(
  "$build_dir/src/blocking/benchmark_v6_blocks"
  "$build_dir/src/blocking/benchmark_v7_serial_blocks"
)

# 3. Execution logic
cd "$workdir"

echo "JobID: ${SLURM_JOB_ID:-local}"
echo "Running on: $(hostname)"
echo "Working directory: $workdir"

# 4. Build targets
echo "Configuring build..."
cmake -S . -B "$build_dir" -DCMAKE_CXX_FLAGS="-O3"

echo "Building targets..."
cmake --build "$build_dir" -j "$build_jobs"

# 5. Run blocking benchmarks
for target in "${targets[@]}"; do
  if [ ! -x "$target" ]; then
    echo "ERROR: Expected executable not found: $target"
    exit 1
  fi

  echo "Executing: $target"
  "$target"
done

echo "Blocking benchmarks completed."
