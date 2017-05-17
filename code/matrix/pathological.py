import sys
import numpy
import scipy.io
import scipy.sparse
import itertools

oski_size = 10000
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

asx_size = 100
asx_entries = 50
asx_block = 12
indices = [i * asx_size + j for (i, j) in itertools.product(range(asx_size), range(asx_size))]
indices = numpy.random.choice(indices, size=2 * asx_entries, replace=False)
I = []
J = []
for k in indices[0:asx_entries]:
  I.append((k / asx_size) * asx_block)
  J.append((k % asx_size) * asx_block)

for k in indices[asx_entries: -1]:
  i = (k / asx_size) * asx_block
  j = (k % asx_size) * asx_block
  for ii in range(i, i + asx_block):
    for jj in range(j, j + asx_block):
      I.append(ii)
      J.append(jj)

V = [1.0 for _ in I]
matrix = scipy.sparse.coo_matrix((V, (I, J)), dtype=numpy.float64)
matrix = scipy.sparse.csr_matrix(matrix)
scipy.io.mmwrite("pathological_asx.mtx", matrix)
