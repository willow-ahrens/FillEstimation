#!/bin/python

import numpy as np
import scipy
from scipy.sparse import bsr_matrix
import time
import math
from sys import argv
from util import *
import json
import os
import math
import time
import scipy.io

oski_prefix = 'oski_'
asx_prefix = 'asx_'
ref_prefix = 'ref_'
matrices = ["freeFlyingRobot_5"]
matrix_path = 'matrix/'

# arg1 = performance profile of this architecture
_, perf_file = argv

#input = name of json file
def get_estimate_from_file(estimate_file):
    # calculate estimate
    with open(estimate_file, 'r') as f:
        results = json.load(f)

    estimate = np.zeros((12,12))
    trials = 0
    results = results[0]
    for result in results['results']:
        trials = trials + 1
        estimate = estimate + result

    # invert fill because 1/fill * perf matrix => argmax
    estimate = [[1/(float(j)/trials) for j in i] for i in estimate]
    # print estimate
    # print trials
    return estimate

# average estimate from a single run
def get_estimate(results):
    # calculate estimate
    # invert fill because 1/fill * perf matrix => argmax
    estimate = [[1/float(j) for j in i] for i in results]
    return estimate

# input = matrix, blocksize
# output where x,y dimensions of matrix % x,y of blocksize == 0
# basically pad rows, cols with 0 if necessary
def reshape_for_blocks(mat, blocksize):
    matrix = mat
    x_dim = matrix.shape[0]
    y_dim = matrix.shape[1]
    # if we need to pad rows
    if blocksize[0] != 1 and matrix.shape[0] % blocksize[0] != 0:
        k = math.ceil(matrix.shape[0] / blocksize[0]) + 1
        x_dim = int(k*blocksize[0])
        assert x_dim > matrix.shape[0]
        z = scipy.sparse.coo_matrix(np.zeros((x_dim - matrix.shape[0], matrix.shape[1])))
        matrix = scipy.sparse.vstack((matrix, z))
        assert matrix.shape[0] == x_dim
    # if we need to pad cols
    if matrix.shape[1] % blocksize[1] != 0:
        k = math.ceil(matrix.shape[1] / blocksize[1]) + 1
        y_dim = int(k*blocksize[1])
        assert y_dim > matrix.shape[1]
        z = scipy.sparse.coo_matrix(np.zeros((matrix.shape[0], y_dim - matrix.shape[1])))
        matrix = scipy.sparse.hstack((matrix, z))
        assert matrix.shape[1] == y_dim
    return matrix

# input = coo matrix, blocksize
# output flops of spmv with all ones vector for matrix w/ blocksize
def mul_test(mat, block, trials):
    # reshape
    reshape = reshape_for_blocks(mat, block)

    # dense vector
    v = np.ones(reshape.shape[1])
    
    # block and multiply
    s_m = bsr_matrix(reshape, blocksize = block)

    # warmup
    for i in range(0, 5):
        s_m.dot(v)

    # testing
    runtime = 0
    x_dim = s_m.shape[0]
    y_dim = s_m.shape[1]

    for i in range(0, trials):
        t0 = time.time()
        s_m.dot(v)
        t1 = time.time()
        # flops
        runtime = runtime + float(t1 - t0)
        
    # total flops / trials
    return runtime / trials


# input = coo matrix, blocksize
# output flops of spmv with all ones vector for matrix w/ blocksize
def mul_flops_test(mat, block, trials):
    # reshape
    reshape = reshape_for_blocks(mat, block)

    # dense vector
    v = np.ones(reshape.shape[1])
    
    # block and multiply
    s_m = bsr_matrix(reshape, blocksize = block)

    # warmup
    for i in range(0, 5):
        s_m.dot(v)

    # testing
    runtime = 0
    x_dim = s_m.shape[0]
    y_dim = s_m.shape[1]

    for i in range(0, trials):
        t0 = time.time()
        s_m.dot(v)
        t1 = time.time()
        # flops
        diff = (x_dim * y_dim) / float(t1 - t0)
        runtime = runtime + diff
        
    # total flops / trials
    return runtime / trials

def block_from_index(blocksize):
    return (blocksize[0] + 1, blocksize[1] + 1)

# input = matrix name, num trials, block dimension B
# output = spmv times with blocking (B*B)
def generate_times_profile(matrix_name, trials, B):
    output = np.zeros((B, B))
    mat = scipy.io.mmread(os.path.join(matrix_path, matrix_name))
    for i in range(1, B+1):
        for j in range(1, B+1):
            print (i,j)
            output[i-1][j-1] = mul_test(mat, (i,j), trials)
    print output
    return output


# input = matrix name, num trials, block dimension B
# output = spmv times with blocking (B*B)
def generate_flops_profile(matrix_name, trials, B):
    output = np.zeros((B, B))
    mat = scipy.io.mmread(os.path.join(matrix_path, matrix_name))
    for i in range(1, B+1):
        for j in range(1, B+1):
            print (i,j)
            output[i-1][j-1] = mul_test(mat, (i,j), trials)
    print output
    return output

# given the fill estimate and performance matrix
# return blocksize 
def get_blocksize(fill_estimate, perf_matrix):
    estimate = get_estimate(fill_estimate)
    
    # perf matrix * fill estimate
    perf_profile = np.multiply(perf_matrix, fill_estimate)

    blocksize = np.unravel_index(perf_profile.argmax(), perf_profile.shape)
    block = block_from_index(blocksize) # (blocksize[0] + 1, blocksize[1] + 1)
    return block


# run spmv for all trials
def spmv_test(mat_name, fill_estimate, perf_matrix, trials):
    # get blocksize
    block = get_blocksize(fill_estimate, perf_matrix)

    # read in from infile
    mat = scipy.io.mmread(os.path.join(matrix_path, mat_name))
    spmv_time = mul_test(mat, block, trials)
    return spmv_time

'''
if __name__ == "__main__":
    # read in performance matrix
    perf_matrix = np.load(perf_file)
    
    for matrix in matrices:
        oski_file = oski_prefix + matrix
        asx_file = asx_prefix + matrix
        ref_file = ref_prefix + matrix

        # get fill estimates
        oski_estimate = get_estimate(oski_file)
        asx_estimate = get_estimate(asx_file)
        
        # performance matrix * fill estimate
        oski = np.multiply(perf_matrix, oski_estimate)
        asx = np.multiply(perf_matrix, asx_estimate)

        oski_blocksize = np.unravel_index(oski.argmax(), oski.shape)
        oski_block = (oski_blocksize[0] + 1, oski_blocksize[1] + 1)
        asx_blocksize = np.unravel_index(asx.argmax(), asx.shape)
        asx_block = (asx_blocksize[0] + 1, asx_blocksize[1] + 1)

        # read in from infile
        mat = scipy.io.mmread(os.path.join(matrix_path, matrix))
        oski_time = mul_test(mat, oski_block)
        print oski_time
        if oski_block != asx_block:
            asx_time = mul_test(mat, asx_block)
''' 
