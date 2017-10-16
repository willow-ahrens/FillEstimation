import math

trials = 100
B = 12
# matrix times in base-dir/matrix_times_[index]
spmv_times_dir = 'spmv_times'

# bar inputs in base-dir/default_bar_[index]
bar_inputs_dir = 'default_bar'

# ref dir
ref_dir = 'ref'

# files in default-bar
# each node should output one of these
# i.e. times_[index], err_[index], spmv_[index]
out_times_prefix = 'times'
out_err_prefix = 'err'
out_spmv_prefix = 'spmv'

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

# full matrix list
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
