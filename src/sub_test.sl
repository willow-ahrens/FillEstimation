#!/bin/bash -l

#SBATCH -N 1
#SBATCH -t 01:00:00
#SBATCH --mail-user=pahrens@mit.edu
#SBATCH --mail-type=ALL
#SBATCH -p research

export TACO_CFLAGS="-O3 -ffast-math -std=c99 -fopenmp"
export OMP_NUM_THREADS=12
export DATA_SPMV_PREFIX="numactl -N 1"

SCRATCH=/data/scratch/pahrens
MATRIX=$SCRATCH/matrix
HOME=$SCRATCH/FillEstimation
DATA=$HOME/data
source env.sh $HOME
python data_test.py $MATRIX $DATA lanka
