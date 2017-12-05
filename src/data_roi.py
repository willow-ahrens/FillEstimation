import matplotlib.pyplot as plt
import numpy as np
from util import *
from sys import argv
from data_meta import *

# python data_roi.py [matrixdir] [outputdir] [architecture]

trials = 100
def exprange(a, b, n):
  r = (float(b) / float(a))**(1.0/(n - 1))
  return [a * r**i for i in range(n)]

n = 10
phil_delta = 0.01

methods = [{"name":"phil",
            "label":"PHIL",
            "color":"red",
            "bound" : lambda point : point["epsilon"],
            "bound_color": "green"
            },
           {"name":"oski",
            "label":"OSKI",
            "color":"blue"}]

matrices = [{"name": "3dtube_conv",
             "label": "3dtube",
             "phil": {"points": [{"epsilon":e, "delta":phil_delta} for e in exprange(7, 0.2, n)]},
             "oski": {"points": [{"delta":d} for d in exprange(0.001, 0.06, n)]},
             "ymax": 0.5
            },
            {"name": "gupta1_conv",
             "label": "gupta1",
             "phil": {"points": [{"epsilon":e, "delta":phil_delta} for e in exprange(7, 0.2, n)]},
             "oski": {"points": [{"delta":d} for d in exprange(0.001, 0.06, n)]},
             "ymax": 0.5
            },
            {"name": "ct20stif",
             "label": "ct20stif",
             "phil": {"points": [{"epsilon":e, "delta":phil_delta} for e in exprange(7, 0.2, n)]},
             "oski": {"points": [{"delta":d} for d in exprange(0.001, 0.06, n)]},
             "ymax": 0.5
            },
            {"name": "pathological_phil",
             "label": "pathological_PHIL",
             "phil": {"points": [{"epsilon":e, "delta":phil_delta} for e in exprange(2, 0.07, n)], "bound":True},
             "oski": {"points": [{"delta":d} for d in exprange(0.1, 1.0, n)]},
             "ymax": 0.5,
             "xmaxmax": True
            },
            {"name": "pathological_oski",
             "label": "pathological_OSKI",
             "phil": {"points": [{"epsilon":e, "delta":phil_delta} for e in exprange(2, 0.07, n)], "bound":True},
             "oski": {"points": [{"delta":d} for d in exprange(0.1, 1.0, n)]},
             "ymax": 4,
             "xmaxmax": True
            }
           ]

references = get_references([matrix["name"] for matrix in matrices], B = B)
for (reference, matrix) in zip(references, matrices):
    
    # generate local performance matrix
  filename =  matrix['name'] + '.npy'
  tfile = os.path.join(spmv_times_dir, filename)
  assert os.path.isfile(tfile)
  matrix_times = np.load(tfile)

  # get time to do spmv with (0,0)
  base_time = matrix_times[0][0]
  # print(matrix["name"])
  for method in methods:
    # print(method["name"])
    times = []
    errors = []
    hi_bars = []
    lo_bars = []
    for point in matrix[method["name"]]["points"]:
      # print(point)
      results = fill_estimates(method["name"], [matrix["name"]], B = B, results = True, trials = trials, **point)
      get_errors(results, [reference])
      times.append(results[0]["time_mean"] / base_time)
      errors.append(np.mean(results[0]["errors"]))
      hi_bars.append(np.std(results[0]["errors"]))
      lo_bars.append(np.std(results[0]["errors"]))

    outfile = 'error_' + method['name'] + '_' + matrix['name']
    out_path = os.path.join(roi_dir, outfile)

    # output in form
    # x y ylow yhigh
    with open(out_path, 'w') as out:
        for i in range(0, len(times)):
            out.write(str(times[i]) + ' ' + str(errors[i]) + ' ' + str(lo_bars[i]) + ' ' + str(hi_bars[i]) + '\n')
