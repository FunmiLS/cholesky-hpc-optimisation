#!/bin/bash
#SBATCH --job-name=threads_all
#SBATCH -J cpujob
#SBATCH -A MPHIL-DIS-SL2-CPU
#SBATCH -p icelake
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=70
#SBATCH --time=02:00:00
#SBATCH --mail-type=NONE
#SBATCH --no-requeue

set -euo pipefail

# 1. Environment setup
. /etc/profile.d/modules.sh
module purge
module load rhel8/default-icl

# 2. Paths and runtime configuration
workdir="${SLURM_SUBMIT_DIR:-$(pwd)}"
# Default to build/threads but allow override with BUILD_DIR
build_dir="${BUILD_DIR:-$workdir/build/threads}"
threads_dir="$build_dir/src/threads"
export OMP_NUM_THREADS="${OMP_NUM_THREADS:-70}"
export OMP_DYNAMIC=FALSE
export OMP_PROC_BIND=spread
export OMP_PLACES=cores
export OMP_DISPLAY_ENV=TRUE
build_jobs="${SLURM_CPUS_PER_TASK:-1}"

# 3. Execution logic
cd "$workdir"

echo "JobID: ${SLURM_JOB_ID:-local}"
echo "Running on: $(hostname)"
echo "Working directory: $workdir"
echo "SLURM_CPUS_PER_TASK: ${SLURM_CPUS_PER_TASK:-unset}"
echo "OMP_NUM_THREADS: $OMP_NUM_THREADS"
echo "OMP_PROC_BIND: $OMP_PROC_BIND"
echo "OMP_PLACES: $OMP_PLACES"

echo "========================================"
echo "CPU / Slurm allocation info"
echo "========================================"

echo "--- lscpu summary ---"
lscpu | egrep 'CPU\(s\):|On-line CPU\(s\) list:|Thread\(s\) per core:|Core\(s\) per socket:|Socket\(s\):|NUMA node\(s\):'

echo
echo "--- Slurm cpuset / affinity info ---"
echo "SLURM_JOB_CPUS_PER_NODE: ${SLURM_JOB_CPUS_PER_NODE:-unset}"
echo "SLURM_CPUS_ON_NODE: ${SLURM_CPUS_ON_NODE:-unset}"
echo "Available CPUs for this shell:"
taskset -cp $$ || true

echo
echo "--- /proc/self/status Cpus_allowed_list ---"
grep Cpus_allowed_list /proc/self/status || true

echo "========================================"

# 4. Build all targets
echo "Configuring build..."
cmake -S . -B "$build_dir" -DCMAKE_CXX_FLAGS="-O3"

echo "Building all targets..."
cmake --build "$build_dir" -j "$build_jobs"

# 5. Run all thread executables
if [ ! -d "$threads_dir" ]; then
  echo "ERROR: Threads directory not found: $threads_dir"
  exit 1
fi

echo "Running executables in $threads_dir"
ran_any=0
for exe in "$threads_dir"/*; do
  if [ -f "$exe" ] && [ -x "$exe" ]; then
    ran_any=1
    echo "----------------------------------------"
    echo "Executing: $exe"

    # Normal run:
    "$exe"
  fi
done

if [ "$ran_any" -eq 0 ]; then
  echo "ERROR: No executable files found in $threads_dir"
  exit 1
fi

echo "All thread executables completed."