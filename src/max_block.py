#!/bin/python

import numpy as np
import scipy
from scipy.sparse import bsr_matrix
import time
import math
from sys import argv
from util import *
import json

oski_prefix = 'oski_'
asx_prefix = 'asx_'
ref_prefix = 'ref_'
matrices = ["freeFlyingRobot_5"]
key = 'flat_results'

# arg1 = performance profile of this architecture
_, perf_file = argv

#input = name of json file
def get_estimate(estimate_file):
    # calculate estimate
    with open(estimate_file, 'r') as f:
        results = json.load(f)

    estimate = np.zeros((12,12))
    trials = 0
    results = results[0]
    for result in results['results']:
        trials = trials + 1
        estimate = estimate + result
    estimate = [[float(j)/trials for j in i] for i in estimate]
    # print estimate
    # print trials
    return estimate

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

        oski_max = np.unravel_index(oski.argmax(), oski.shape)
        asx_max = np.unravel_index(asx.argmax(), asx.shape)
