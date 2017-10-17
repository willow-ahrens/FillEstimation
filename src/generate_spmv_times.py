# generate times matrix

import numpy as np
from util import *
from blockutil import *
import os
from sys import argv
from test_meta import *

_, _, basedir, matrix_id = argv

matrix_name = matrices[int(matrix_id)]["name"]

# generate times matrix
matrix_times = generate_times_profile(matrix_name, trials, B)

outdir = os.path.join(basedir, spmv_times_dir)
if not os.path.isdir(outdir):
    os.mkdir(outdir)

outfile = os.path.join(outdir, matrix_name)

# save
np.save(outfile, matrix_times)
