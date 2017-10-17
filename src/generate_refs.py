# generate times matrix

import numpy as np
from util import *
from blockutil import *
import os
from sys import argv
from test_meta import *

_, _, basedir, matrix_id = argv

matrix_name = matrices[int(matrix_id)]["name"]
print(matrix_name)

# generate references for matrix_name
ref = get_references([matrix_name])

outdir = os.path.join(basedir, ref_dir)
if not os.path.isdir(outdir):
    os.mkdir(outdir)

outfile = os.path.join(outdir, matrix_name)

# save
np.save(outfile, ref)
