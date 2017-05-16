import sys
import numpy
import scipy.io
import scipy.sparse

with open(sys.argv[1]) as f:
  matrix = scipy.io.mmread(f)
  matrix = scipy.sparse.coo_matrix(matrix, dtype=numpy.float64)
  scipy.io.mmwrite(sys.argv[2], matrix)
