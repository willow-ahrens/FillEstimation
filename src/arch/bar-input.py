import numpy as np
from util import *
from blockutil import *
import os
from sys import argv
from test_meta import *

n = 10
asx_delta = 0.01

# basedir = something like workspace/
_, _, basedir, perf_file, matrix_id = argv

matrix_name = matrices[int(matrix_id)]["name"]

oski_time = asx_time = oski_err = asx_err = oski_err_std =asx_err_std = oski_spmv = asx_spmv = oski_spmv_std = asx_spmv_std = 0

# make sure all the input files exist
ref_base = os.path.join(basedir, ref_dir)
reffile = os.path.join(ref_base, matrix_name + '.npy')
assert os.path.isfile(reffile)

perf_base = os.path.join(basedir, ref_dir)
perffile = os.path.join(perf_base, matrix_name + '.npy')
assert os.path.isfile(perffile)

# load in reference and spmv times
reference = np.load(reffile)

for method in methods:
    point = method["point"]
    
    # results = get errors from default fill estimate
    results = fill_estimates(method["name"], [matrix_name], B = B, results = True, trials = trials, **point)
    get_errors(results, [reference])
 
    if method["name"] == 'oski':
        # update all oski lists
        # normalized time to estimate
        oski_time = results[0]["time_mean"]
        
        # error and std
        oski_err = np.mean(results[0]["errors"])
        oski_err_std = np.std(results[0]["errors"])

    else:
        # update all asx lists
        # normalized time to estimate
        asx_time = results[0]["time_mean"]
        # error and std
        asx_err = np.mean(results[0]["errors"])
        asx_err_std = np.std(results[0]["errors"])

       
# outputfiles
# in the format 
# matrix_name asx_val asx_std oski_val oski_std


# for runtime, we just have val

outdir = os.path.join(basedir, bar_inputs_dir)
if not os.path.isdir(outdir):
    os.mkdir(outdir)

time_path = os.path.join(outdir, out_times_prefix + '_' + matrix_name)
err_path = os.path.join(outdir, out_err_prefix + '_' + matrix_name)
spmv_path = os.path.join(outdir, out_spmv_prefix + '_' + matrix_name)

with open(time_path, 'w') as time_out:
    time_out.write(matrix_name + ' ' + str(asx_time) + ' ' + str(oski_time) + '\n')

with open(err_path, 'w') as err_out:
    err_out.write(matrix_name + ' ' + str(asx_err) + ' ' + str(asx_err_std) + ' ' + str(oski_err) + ' ' + str(oski_err_std) + '\n')

