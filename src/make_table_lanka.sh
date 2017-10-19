#!/bin/bash -l

#SBATCH --nodes=1-1
#SBATCH --cpus-per-task=48
#SBATCH -t 01:00:00
#SBATCH --mail-user=pahrens@mit.edu
#SBATCH --mail-type=ALL
#SBATCH -p lanka-v3

SCRATCH=/data/scratch/pahrens
MATRIX=$SCRATCH/matrix
HOME=$SCRATCH/FillEstimation
DATA=$HOME/data

python make_table.py $MATRIX $DATA lanka $HOME/src/matrix_meta $HOME/src/table_lanka.tex
