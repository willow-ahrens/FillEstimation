# generate times matrix

import numpy as np
from util import *
from blockutil import *
import os
from sys import argv
from test_meta import *

# basedir = working dir
_, basedir, matrix_name = argv

set_matrix_storage(os.path.join(basedir, 'matrix'))

# generate references for matrix_name
ref = get_references([matrix_name])

outdir = os.path.join(basedir, ref_dir)
if not os.path.isdir(outdir):
    os.mkdir(outdir)

outfile = os.path.join(outdir, matrix_name)

# save
np.save(outfile, ref)
