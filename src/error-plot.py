import matplotlib.pyplot as plt
import numpy as np
from util import *

B = 12
trials = 100
outdir = 'plot-inputs/'
def exprange(a, b, n):
  r = (float(b) / float(a))**(1.0/(n - 1))
  return [a * r**i for i in range(n)]

n = 10
asx_delta = 0.01

methods = [{"name":"asx",
            "label":"ASX",
            "color":"red",
            "bound" : lambda point : point["epsilon"],
            "bound_color": "green"
            },
           {"name":"oski",
            "label":"OSKI",
            "color":"blue"}]

matrices = [{"name": "3dtube_conv",
             "label": "3dtube",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(7, 0.2, n)]},
             "oski": {"points": [{"delta":d} for d in exprange(0.001, 0.06, n)]},
             "ymax": 0.5
            },
            {"name": "gupta1_conv",
             "label": "gupta1",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(7, 0.2, n)]},
             "oski": {"points": [{"delta":d} for d in exprange(0.001, 0.06, n)]},
             "ymax": 0.5
            },
            {"name": "ct20stif",
             "label": "ct20stif",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(7, 0.2, n)]},
             "oski": {"points": [{"delta":d} for d in exprange(0.001, 0.06, n)]},
             "ymax": 0.5
            },
            {"name": "pathological_asx",
             "label": "pathological_ASX",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(2, 1e-4, n)], "bound":True},
             "oski": {"points": [{"delta":d} for d in exprange(0.1, 1.0, n)]},
             "ymax": 0.5,
             "xmaxmax": True
            },
            {"name": "pathological_oski",
             "label": "pathological_OSKI",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(2, 1e-4, n)], "bound":True},
             "oski": {"points": [{"delta":d} for d in exprange(0.1, 1.0, n)]},
             "ymax": 4,
             "xmaxmax": True
            }
           ]

references = get_references([matrix["name"] for matrix in matrices], B = B)
for (reference, matrix) in zip(references, matrices):
  print(matrix["name"])

  xmax = 100000
  xmaxmax = 0
  for method in methods:
    print(method["name"])
    times = []
    errors = []
    hi_bars = []
    lo_bars = []
    for point in matrix[method["name"]]["points"]:
      print(point)
      results = fill_estimates(method["name"], [matrix["name"]], B = B, results = True, trials = trials, **point)
      get_errors(results, [reference])
      times.append(results[0]["time_mean"])
      errors.append(np.mean(results[0]["errors"]))
      hi_bars.append(np.std(results[0]["errors"]))
      lo_bars.append(np.std(results[0]["errors"]))
      print("Error: %g, Time: %g" % (errors[-1], times[-1]))

    outfile = 'error_' + method['name'] + '_' + matrix['name']
    out_path = os.path.join(outdir, outfile)

    # output in form
    # x y ylow yhigh
    with open(out_path, 'w') as out:
        for i in range(0, len(times)):
            out.write(str(times[i]) + ' ' + str(errors[i]) + ' ' + str(lo_bars[i]) + ' ' + str(hi_bars[i]) + '\n')
    

  if "xmaxmax" in matrix and matrix["xmaxmax"]:
    plt.xlim([0, xmaxmax])
  else:
    plt.xlim([0, xmax])
  plt.ylim([0, matrix["ymax"]])
  plt.xlabel('Time To Compute Estimate (s)')
  plt.ylabel('Mean (Maximum Relative Error Over Block Sizes)')
  plt.title('Mean Maximum Relative Error Vs. Time To Compute (%s)' % (matrix["label"]))
  plt.legend(loc='best')
  plt.savefig("roi_%s.pdf" % (matrix["name"]))
  plt.clf()

methods = [{"name":"asx",
            "label":"ASX",
            "color":"red",
            "point":{"epsilon":0.5, "delta":0.01},
            "bound" : lambda point : point["epsilon"],
            "bound_color": "green"
           },
           {"name":"oski",
            "label":"OSKI",
            "color":"blue",
            "point":{"delta":0.02}
           }
          ]

matrices = [{"name": "3dtube_conv",
             "label": "3dtube",
            },
            {"name": "gupta1_conv",
             "label": "gupta1",
            },
            {"name": "ct20stif",
             "label": "ct20stif",
            },
            {"name": "pathological_asx",
             "label": "patho..._ASX",
            },
            {"name": "pathological_oski",
             "label": "patho..._OSKI",
            }
           ]



references = get_references([matrix["name"] for matrix in matrices], B = B)
f, axarr = plt.subplots(nrows=2, ncols=1)#, sharex=True)
ind = np.arange(float(len(matrices)))
totalwidth = 0.8
rotation = 0
width = totalwidth / len(matrices)
time_rects = []
error_rects = []
outfile = 'bar_input'
out_path = os.path.join(outdir, outfile)

with open(out_path, 'w') as out:
    for method in methods:
      print(method["name"])
      times = []
      errors = []
      errors_std = []
      for (reference, matrix) in zip(references, matrices):
        print(matrix["name"])
        point = method["point"]
        print(point)
        results = fill_estimates(method["name"], [matrix["name"]], B = B, results = True, trials = trials, **point)
        get_errors(results, [reference])
        times.append(results[0]["time_mean"])
        errors.append(np.mean(results[0]["errors"]))
        errors_std.append(np.std(results[0]["errors"]))
        print("Error: %g, Time: %g" % (errors[-1], times[-1]))
        out.write(method['name'] + ' ' + matrix['name'] + ' ' + str(times[-1]) + ' ' + str(errors[-1]) + ' ' + str(errors_std[-1] + '\n')

