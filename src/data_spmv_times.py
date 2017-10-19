# generate times matrix

import numpy as np
import util
from blockutil import *
import os
from sys import argv
from data_meta import *

# generate performance matrix
# python data_spmv_times.py [matrixdir] [outputdir] [architecture] [matrix_id]
matrix_id = argv[4]


matrix_name = matrices[int(matrix_id)]["name"]

# generate times matrix
result = np.zeros((B, B))
for i in range(1, B+1):
  for j in range(1, B+1):
    print (i,j)
    result[i-1][j-1] = util.spmv_time([matrix_name], r = i, c = j, trials = trials)[0]["time_mean"]

print result
outfile = os.path.join(spmv_times_dir, matrix_name)
np.save(outfile, result)
with open(outfile, 'w') as out:
  out.write(str(result))
