#!/bin/python

import time
import math
import util
import data_meta
import numpy as np
import os

# generate performance matrix
# python data_profile.py [matrixdir] [outputdir] [architecture]

MAX_BLOCK = data_meta.B
baseline = 1
trials = 1000
(n, m) = np.shape(util.matrix_read("dense"))

if __name__ == "__main__":
  result = np.ones((MAX_BLOCK, MAX_BLOCK))
  for i in range(1, MAX_BLOCK + 1):
    for j in range(1, MAX_BLOCK + 1):
      print "i: " + str(i)
      print "j: " + str(j)
      t = util.spmv_time(["dense"], r = i, c = j, trials = trials)[0]["time_mean"]
      print t
        
      r = (n * m) / float(t)
      result[i-1][j-1] = r
  
  print result
  outfile = os.path.join(data_meta.profile_dir, "profile")
  np.save(outfile, result)
  with open(outfile, 'w') as out:
    out.write(str(result))
