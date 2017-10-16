import numpy as np
from util import *
from blockutil import *
import os
from sys import argv
from test_meta import *

n = 10
asx_delta = 0.01
#outfile prefix

# basedir = something like workspace/
# perf_file = something like perf-matrix.npy
_, perf_file, basedir, matrix_name = argv

perf_matrix = np.load(perf_file)

oski_times = []
asx_times = []
oski_err = []
asx_err = []
oski_spmv = []
asx_spmv = []
oski_names = []
asx_names = []

mat_partition = get_input_matrices(num_processors, index)

# make sure all the input files exist
for matrix in matrices:
    ref_temp = [basedir, ref_dir, matrix['name']]
    times_temp = [basedir, spmv_times_dir, matrix['name']]
    reffile = os.path.join(*ref_temp)
    timesfile = os.path.join(*times_temp)
    assert os.isfile(reffile)
    assert os.isfile(timesfile)

for method in methods:
  for (reference, matrix) in zip(references, matrices):
    point = method["point"]
    
    # generate local performance matrix
    filename = matrix['name'] + '.npy'
    tfile = os.path.join(matrix_times_dir, filename)

    # generate matrix spmv times
    if os.path.isfile(tfile):
        matrix_times = np.load(tfile)
    else:
        matrix_times = generate_times_profile(matrix['name'], trials, B)
        np.save(tfile[:-4], matrix_times)
    
    print matrix_times    
    # get time to do spmv with (0,0)
    base_time = matrix_times[0][0]

    # results = get errors from default fill estimate
    results = fill_estimates(method["name"], [matrix["name"]], B = B, results = True, trials = trials, **point)
    get_errors(results, [reference])
 
    spmv_times = []
    
    # get spmv time
    for estimate in results[0]['results']:
      # get block size
      blocksize = get_blocksize(estimate, perf_matrix)
    
      # look up time that multiplying by with blocksize would take
      spmv_times.append(matrix_times[blocksize[0]][blocksize[1]] / base_time )
    
    if method["name"] == 'oski':
        # update all oski lists
        # normalized time to estimate
        oski_times.append(results[0]["time_mean"] / base_time)
        
        # error in tuple (error, std) 
        oski_err.append((np.mean(results[0]["errors"]), np.std(results[0]["errors"])))

        # spmv time in tuple (normalized_time, spmv)
        oski_spmv.append((np.mean(spmv_times), np.std(spmv_times)))
        oski_names.append(matrix['name'])
    else:
        # update all asx lists
        # normalized time to estimate
        asx_times.append(results[0]["time_mean"] / base_time)
        
        # error in tuple (error, std) 
        asx_err.append((np.mean(results[0]["errors"]), np.std(results[0]["errors"])))

        # spmv time in tuple (normalized_time, spmv)
        asx_spmv.append((np.mean(spmv_times), np.std(spmv_times)))

        # labels
        # this is mostly a sanity check
        asx_names.append(matrix['name'])
try:
  assert asx_names == oski_names
except:
    print asx_names
    print oski_names

assert len(oski_times) == len(asx_times)
assert len(oski_err) == len(asx_err)
assert len(oski_spmv) == len(asx_spmv)

# outputfiles
# in the format 
# for spmv times and 
# matrix_name asx_val asx_std oski_val oski_std


# for runtime, we just have val
# matrix_name asx_val 
# files (normalized time to estimate, error, normalized spmv time) in the form 
# matrix_id asx oski
outdir = os.path.join(basedir, bar_inputs_dir) 
time_path = os.path.join(outdir, out_times_prefix + '_' + matrix_name)
err_path = os.path.join(outdir, out_err_prefix + '_' + matrix_name)
spmv_path = os.path.join(outdir, out_spmv_prefix + '_' + matrix_name)

with open(time_path, 'w') as time_out:
    for i in range(0, len(oski_times)):
        time_out.write(str(oski_names[i]) + ' ' + str(asx_times[i]) + ' ' + str(oski_times[i]) + '\n')

with open(err_path, 'w') as err_out:
    for i in range(0, len(oski_err)):
        err_out.write(str(oski_names[i]) + ' ' + str(asx_err[i][0]) + ' ' + str(asx_err[i][1]) + ' ' + str(oski_err[i][0]) + ' ' + str(oski_err[i][1]) + '\n')

with open(spmv_path, 'w') as spmv_out:
    for i in range(0, len(oski_spmv)):
        spmv_out.write(str(oski_names[i]) + ' ' + str(asx_spmv[i][0]) + ' ' + str(asx_spmv[i][1]) + ' ' + str(oski_spmv[i][0]) + ' ' + str(oski_spmv[i][1]) + '\n')

