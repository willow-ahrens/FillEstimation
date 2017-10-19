# generate times matrix

import numpy as np
from util import *
from blockutil import *
import os
from sys import argv
import data_meta

# generate performance matrix
# python data_spmv_times.py [matrixdir] [outputdir] [architecture] [matrix_id]
matrix_id = argv[4]

matrix_name = matrices[int(matrix_id)]["name"]

# generate times matrix
output = np.zeros((B, B))
for i in range(1, B+1):
  for j in range(1, B+1):
    print (i,j)
    output[i-1][j-1] = util.spmv_time([matrix_name], r = i, c = j, trials = trials)[0]["time_mean"]

# save
np.save(outfile, output)
