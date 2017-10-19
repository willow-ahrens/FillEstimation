#!/bin/bash -l

#SBATCH -N 1
#SBATCH -t 01:00:00
#SBATCH --mail-user=pahrens@mit.edu
#SBATCH --mail-type=ALL
#SBATCH -p lanka-v3

SCRATCH=/data/scratch/pahrens
MATRIX=$SCRATCH/matrix
HOME=$SCRATCH/FillEstimation
DATA=$HOME/data
source env.sh $HOME
python data_profile.py $MATRIX $DATA lanka
