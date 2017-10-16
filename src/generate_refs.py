# generate times matrix

import numpy as np
from util import *
from blockutil import *
import os
from sys import argv
from test_meta import *

_, basedir, matrix_name = argv

set_matrix_storage(os.path.join(basedir, 'matrix'))

# generate references for these matrices
references = get_references([matrix_name])

# write reference fills each in a file
for reference in references:

    # generate output filename
    outdir = [basedir, ref_dir, matrix['name']]
    outfile = os.path.join(*outdir)

    # save
    np.save(outfile, [reference])
