#!/bin/bash
#SBATCH --job-name=matrix_all
#SBATCH -J cpujob
#SBATCH -A MPHIL-DIS-SL2-CPU
#SBATCH -p icelake
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --time=03:00:00
#SBATCH --mail-type=NONE
#SBATCH --no-requeue

set -euo pipefail

# 1. Environment setup
. /etc/profile.d/modules.sh
module purge
module load rhel8/default-icl

# 2. Paths and runtime configuration
workdir="${SLURM_SUBMIT_DIR:-$(pwd)}"
# Default to build/matrix but allow override with BUILD_DIR
build_dir="${BUILD_DIR:-$workdir/build/matrix}"
matrix_dir="$build_dir/src/matrix"
export OMP_NUM_THREADS="${OMP_NUM_THREADS:-16}"
export OMP_DYNAMIC=FALSE
export OMP_PROC_BIND=spread
export OMP_PLACES=cores
build_jobs="${SLURM_CPUS_PER_TASK:-1}"

# 3. Execution Logic
cd "$workdir"

echo "JobID: ${SLURM_JOB_ID:-local}"
echo "Running on: $(hostname)"
echo "Working directory: $workdir"
echo "OMP_NUM_THREADS: $OMP_NUM_THREADS"

# 4. Build all targets
echo "Configuring build..."
cmake -S . -B "$build_dir" -DCMAKE_CXX_FLAGS="-O3"

echo "Building all targets..."
cmake --build "$build_dir" -j "$build_jobs"

# 5. Run all matrix executables
if [ ! -d "$matrix_dir" ]; then
  echo "ERROR: Matrix directory not found: $matrix_dir"
  exit 1
fi

echo "Running executables in $matrix_dir"
ran_any=0
for exe in "$matrix_dir"/*; do
  if [ -f "$exe" ] && [ -x "$exe" ]; then
    ran_any=1
    echo "----------------------------------------"
    echo "Executing: $exe"
    "$exe"
  fi
done

if [ "$ran_any" -eq 0 ]; then
  echo "ERROR: No executable files found in $matrix_dir"
  exit 1
fi

echo "All matrix executables completed."
