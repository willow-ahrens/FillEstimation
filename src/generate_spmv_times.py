# generate times matrix

import numpy as np
from util import *
from blockutil import *
import os
from sys import argv
from test_meta import *

_, basedir, matrix_name = argv

set_matrix_storage(os.path.join(basedir, 'matrix'))

# generate times matrix
matrix_times = generate_times_profile(matrix_name, trials, B)

# generate output filename
outdir = [basedir, spmv_times_dir, matrix['name']]
outfile = os.path.join(*outdir)

# save
np.save(outfile, spmv_times_dir)
