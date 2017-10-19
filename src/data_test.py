#!/bin/python

import time
import math
import util
import data_meta
import numpy as np

# generate performance matrix
# python data_profile.py [matrixdir] [outputdir] [architecture]

MAX_BLOCK = 12
baseline = 1
trials = 100

t = util.spmv_time(["dense"], r = 1, c = 1, trials = trials)[0]["time_mean"]
print t
