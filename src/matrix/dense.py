import sys
import numpy
import scipy.io
import scipy.sparse
import itertools

size = 2000
I = []
J = []
for i in range(size):
  for j in range(size):
    I += [i]
    J += [j]

V = [1.0 for _ in I]
matrix = scipy.sparse.coo_matrix((V, (I,J)), dtype=numpy.float64)
matrix = scipy.sparse.csr_matrix(matrix)
scipy.io.mmwrite("dense.mtx", matrix)
