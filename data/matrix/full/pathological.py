import sys
import numpy
import scipy.io
import scipy.sparse
import itertools

numpy.random.seed(0xDEADBEEF)

oski_size = 1000000
oski_block = 6
I = [i for i in range(oski_size)]
J = [0 for _ in range(oski_size)]
for i in range(oski_block):
  I += [i for _ in range(1,oski_size)]
  J += [j for j in range(1,oski_size)]

V = [1.0 for _ in I]
matrix = scipy.sparse.coo_matrix((V, (I,J)), dtype=numpy.float64)
matrix = scipy.sparse.csr_matrix(matrix)
scipy.io.mmwrite("pathological_oski.mtx", matrix)

phil_size = 10000
phil_entries = 100000
phil_block = 12
indices = [i * phil_size + j for (i, j) in itertools.product(range(phil_size), range(phil_size))]
indices = numpy.random.choice(indices, size=2 * phil_entries, replace=False)
I = []
J = []
for k in indices[0:phil_entries]:
  I.append((k / phil_size) * phil_block)
  J.append((k % phil_size) * phil_block)

for k in indices[phil_entries: -1]:
  i = (k / phil_size) * phil_block
  j = (k % phil_size) * phil_block
  for ii in range(i, i + phil_block):
    for jj in range(j, j + phil_block):
      I.append(ii)
      J.append(jj)

V = [1.0 for _ in I]
matrix = scipy.sparse.coo_matrix((V, (I, J)), dtype=numpy.float64)
matrix = scipy.sparse.csr_matrix(matrix)
scipy.io.mmwrite("pathological_phil.mtx", matrix)
