#!/bin/bash -l

#SBATCH -N 1         #Use 2 nodes
#SBATCH -t 02:00:00
#SBATCH --mail-user=pahrens@mit.edu
#SBATCH --mail-type=ALL
#SBATCH -p regular   #Submit to the regular 'partition'
#SBATCH -L SCRATCH   #Job requires $SCRATCH file system
#SBATCH -C knl   #Use KNL nodes in quad cache format (default, recommended)

export KMP_AFFINITY=disabled

module load python
python performance-matrix.py $SCRATCH/fill_estimation/knl/performance-matrix

