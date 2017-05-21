import matplotlib.pyplot as plt
import numpy as np
from util import *

B = 12
trials = 100

def exprange(a, b, n):
  r = (float(b) / float(a))**(1.0/(n - 1))
  return [a * r**i for i in range(n)]

n = 10
asx_delta = 0.01

methods = [{"name":"asx",
            "color":"red",
            "bound" : lambda point : point["epsilon"],
            "bound_color": "green"
            },
           {"name":"oski",
            "color":"blue"}]

matrices = [{"name": "3dtube_conv",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(7, 0.2, n)]},
             "oski": {"points": [{"delta":d} for d in exprange(0.001, 0.06, n)]},
             "ymax": 0.5
            },
            {"name": "gupta1_conv",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(7, 0.2, n)]},
             "oski": {"points": [{"delta":d} for d in exprange(0.001, 0.06, n)]},
             "ymax": 0.5
            },
            {"name": "ct20stif",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(7, 0.2, n)]},
             "oski": {"points": [{"delta":d} for d in exprange(0.001, 0.06, n)]},
             "ymax": 0.5
            },
            {"name": "pathological_asx",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(7, 0.2, n)], "bound":True},
             "oski": {"points": [{"delta":d} for d in exprange(0.14, 0.9, n)]},
             "ymax": 1.5
            },
            {"name": "pathological_oski",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(7, 0.05, n)], "bound":True},
             "oski": {"points": [{"delta":d} for d in exprange(0.1, 0.98, n)]},
             "ymax": 4
            }
           ]

references = get_references([matrix["name"] for matrix in matrices], B = B)
for (reference, matrix) in zip(references, matrices):
  print(matrix["name"])

  xmax = 100000
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
    plt.plot(times, errors, color = method["color"], label = method["name"])
    if "bound" in matrix[method["name"]] and matrix[method["name"]]["bound"]:
      plt.plot(times, [method["bound"](point) for point in matrix[method["name"]]["points"]], color = method["bound_color"], label = "%s bound" % method["name"])
    plt.errorbar(times, errors, yerr = [lo_bars, hi_bars], color = method["color"], linestyle="")
    xmax = min(xmax, max(times))

  plt.xlim([0, xmax])
  plt.ylim([0, matrix["ymax"]])
  plt.xlabel('Time To Compute Estimate (s)')
  plt.ylabel('Mean (Maximum Relative Error Over Block Sizes)')
  plt.title('Mean Maximum Relative Error Vs. Time To Compute (%s)' % (matrix["name"]))
  plt.legend(loc='best')
  plt.savefig("roi_%s" % (matrix["name"]))
  plt.clf()
