import math
import os

import sys

trials = 100
if sys.argv[3] == "lanka12":
  B = 12
  methods = [{"name":"asx",
             "label":"ASX",
             "color":"red",
             "point":{"epsilon":0.3, "delta":0.01},
             },
             {"name":"oski",
             "label":"OSKI",
             "color":"blue",
             "point":{"delta":0.02}
             }]

if sys.argv[3] == "lanka4":
  B = 4
  methods = [{"name":"asx",
             "label":"ASX",
             "color":"red",
             "point":{"epsilon":0.1, "delta":0.01},
             },
             {"name":"oski",
             "label":"OSKI",
             "color":"blue",
             "point":{"delta":0.02}
             }]
  
#_, _, data_dir, platform  = sys.argv

spmv_times_dir = os.path.join(sys.argv[2], sys.argv[3], 'spmv_times')

profile_dir = os.path.join(sys.argv[2], sys.argv[3], 'profile')

table_dir = os.path.join(sys.argv[2], sys.argv[3], 'table')

roi_dir = os.path.join(sys.argv[2], sys.argv[3], 'roi')

ref_dir = os.path.join(sys.argv[2], sys.argv[3], 'ref')

if False:
  for d in [spmv_times_dir, profile_dir, bar_dir, roi_dir, ref_dir]:
    if not os.path.isdir(d):
      os.mkdir(d)

# files in default-bar
# each node should output one of these
# i.e. times_[index], err_[index], spmv_[index]
out_times_prefix = 'times'
out_err_prefix = 'err'
out_spmv_prefix = 'spmv'

# full matrix list
matrices = [{"name": "cfd2_conv"},
            {"name": "parabolic_fem_conv"},
            {"name": "Ga41As41H72_conv"},
            {"name": "ASIC_680k_conv"},
            {"name": "G3_circuit_conv"},
            {"name": "Hamrle3_conv"},
            {"name": "rajat24_conv"},
            {"name": "cage13_conv"},
            {"name": "cnr-2000_conv"},
            {"name": "Stanford_conv"},
            {"name": "degme_conv"},
            {"name": "rail4284_conv"},
            {"name": "pds-100_conv"},
            {"name": "exdata_1_conv"},
            {"name": "kkt_power_conv"},
            {"name": "largebasis_conv"},
            {"name": "TSOPF_RS_b2383_conv"},
            {"name": "af_shell10_conv"},
            {"name": "bmw7st_1_conv"},
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
'''

# short test
matrices = [ {"name": "3dtube_conv"},
            {"name": "gupta1_conv"},
            {"name": "ct20stif"},
            {"name": "pathological_asx"},
            {"name": "pathological_oski"}
            ]
'''
# given filename, ret filename_[index]
def join_with_index(filename, index):
    return filename + '_' + str(index)

# indices 0...p-1
def get_input_matrices(num_processors, index):
    num_matrices = math.ceil(num_processors, index)
    if index < num_processors - 1:
        return matrices[index * num_matrices:index * (num_matrices + 1)]
    else:
        # if the last one, just get the rest
        return matrices[index * num_matrices:]
