import matplotlib.pyplot as plt
import numpy as np
from util import *
from blockutil import *
import os
from sys import argv
from conv import *

B = 12
trials = 100
outdir = 'default-plot-inputs/'
n = 10
asx_delta = 0.01
matrix_times_dir = 'matrix-times/'
_, perf_file = argv
perf_matrix = np.load(perf_file)

methods = [{"name":"asx",
            "label":"ASX",
            "color":"red",
            "point":{"epsilon":0.5, "delta":0.01},
           },
           {"name":"oski",
            "label":"OSKI",
            "color":"blue",
            "point":{"delta":0.02}
           }
          ]
'''
matrices = [{"name": "3dtube_conv",
             "label": "3dtube",
            },
            {"name": "gupta1_conv",
             "label": "gupta1",
            },
            {"name": "ct20stif",
             "label": "ct20stif",
            },
            {"name": "cfd2_conv",
             "label": "cfd2"
            }
            {"name": "pathological_asx",
             "label": "patho..._ASX",
            },
            {"name": "pathological_oski",
             "label": "patho..._OSKI",
            }

#index by numbers
matrices = [{"name": "3dtube_conv",
             "label": "1",
            },
            {"name": "gupta1_conv",
             "label": "2",
            },
            {"name": "ct20stif",
             "label": "3",
            },
            {"name": "cfd2_conv",
             "label": "4"
            },
            {"name": "pathological_asx",
             "label": "5",
            },
            {"name": "pathological_oski",
             "label": "6",
            }
'''
#index by numbers
matrices = [{"name": "cfd2_conv"},
            {"name": "parabolic_fem_conv"},
            {"name": "Ga41As41H72_conv"},
            {"name": "ASIC_680k_conv"},
            {"name": "G3_circuit_conv"},
            {"name": "Hamrle3_conv"},
            {"name": "rajat31_conv"},
            {"name": "cage15_conv"},
            {"name": "wb-edu_conv"},
            {"name": "wikipedia-20061104_conv"},
            {"name": "degme_conv"},
            {"name": "rail4284_conv"},
            {"name": "spal_004_conv"},
            {"name": "bone010_conv"},
            {"name": "kkt_power_conv"},
            {"name": "largebasis_conv"},
            {"name": "TSOPF_RS_b2383_conv"},
            {"name": "af_shell10_conv"},
            {"name": "audikw_1_conv"},
            {"name": "F1_conv"},
            {"name": "gearbox_conv"},
            {"name": "inline_1_conv"},
            {"name": "ldoor_conv"},
            {"name": "pwtk_conv"},
            {"name": "thermal2_conv"},
            {"name": "nd24k_conv"},
            {"name": "stomach_conv"},
            {"name": "3dtube_conv"},
            {"name": "gupta1_conv"},
            {"name": "ct20stif"},
            {"name": "pathological_asx"},
            {"name": "pathological_oski"}
            ]


oski_times = []
asx_times = []
oski_err = []
asx_err = []
oski_spmv = []
asx_spmv = []
oski_names = []
asx_names = []

references = get_references([matrix["name"] for matrix in matrices], B = B)

for method in methods:
  for (reference, matrix) in zip(references, matrices):
    point = method["point"]
    
    # generate local performance matrix
    filename = 'times_' + matrix['name'] + '.npy'
    tfile = os.path.join(matrix_times_dir, filename)
    print tfile

    print os.path.isfile(tfile) 
    # generate matrix spmv times
    if os.path.isfile(tfile):
        print 'isfile'
        matrix_times = np.load(tfile)
    else:
        print 'isnot'
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
time_path = os.path.join(outdir, 'default_times')
err_path = os.path.join(outdir, 'default_err')
spmv_path = os.path.join(outdir, 'default_spmv')

with open(time_path, 'w') as time_out:
    for i in range(0, len(oski_times)):
        time_out.write(str(oski_names[i]) + ' ' + str(asx_times[i]) + ' ' + str(oski_times[i]) + '\n')

with open(err_path, 'w') as err_out:
    for i in range(0, len(oski_err)):
        err_out.write(str(oski_names[i]) + ' ' + str(asx_err[i][0]) + ' ' + str(asx_err[i][1]) + ' ' + str(oski_err[i][0]) + ' ' + str(oski_err[i][1]) + '\n')

with open(spmv_path, 'w') as spmv_out:
    for i in range(0, len(oski_spmv)):
        spmv_out.write(str(oski_names[i]) + ' ' + str(asx_spmv[i][0]) + ' ' + str(asx_spmv[i][1]) + ' ' + str(oski_spmv[i][0]) + ' ' + str(oski_spmv[i][1]) + '\n')

