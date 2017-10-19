#!/bin/bash -l

#SBATCH --nodes=1-1
#SBATCH --cpus-per-task=48
#SBATCH -t 02:00:00
#SBATCH --mail-user=pahrens@mit.edu
#SBATCH --mail-type=ALL
#SBATCH --array=0-31
#SBATCH -p lanka-v3


SCRATCH=/data/scratch/pahrens
MATRIX=$SCRATCH/matrix
HOME=$SCRATCH/FillEstimation
DATA=$HOME/data
source env.sh $HOME

export TACO_CFLAGS="-O3 -ffast-math -std=c99 -fopenmp -funroll-loops"
export OMP_NUM_THREADS=12
export DATA_SPMV_PREFIX="numactl -N 1"
export TACO_TMPDIR=$HOME/taco/tmp$SLURM_ARRAY_TASK_ID

python data_spmv_times.py $MATRIX $DATA lanka $SLURM_ARRAY_TASK_ID
