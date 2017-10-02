#!/bin/python

from sys import argv
import numpy as np
import scipy
from scipy.sparse import bsr_matrix
import time
import math

# generate performance matrix
_, outfile = argv

MIN_X = 3000
MIN_Y = 3000
MAX_BLOCK = 12
baseline = 1
# dense matrix, vector
def lcm(x, y):
    if x > y:
        greater = x
    else:
        greater = y
    while True:
        if((greater % x == 0) and (greater % y == 0)):
            lcm = greater
            break
        greater = greater + 1
    return lcm

if __name__ == "__main__":
    result = np.ones((MAX_BLOCK, MAX_BLOCK))
    for i in range(1, MAX_BLOCK + 1):
        for j in range(1, MAX_BLOCK + 1):
            print "i: " + str(i)
            print "j: " + str(j)
            m = lcm(i,j)
            x_dim = MIN_X
            y_dim = MIN_Y
            if MIN_X % m != 0:
                k = math.ceil(MIN_X / m) + 1
                x_dim = int(k * m)
            if MIN_Y % m != 0:
                k = math.ceil(MIN_Y / m) + 1
                y_dim = int(k*m)
            assert x_dim >= MIN_X
            assert y_dim >= MIN_Y

            mat = np.ones((x_dim, y_dim))
            v = np.ones(y_dim)

            s_m = bsr_matrix(mat, blocksize = (i,j))
            s_m.dot(v)
            t0 = time.time()
            s_m.dot(v)
            t1 = time.time()
            diff = (x_dim * y_dim) / float(t1 - t0)
            if i == 1 and j == 1:
                baseline = diff
            r = diff / baseline
            result[i-1][j-1] = r
    print result
    np.save(outfile, result)
    with open(outfile, 'w') as out:
        out.write(str(result))
