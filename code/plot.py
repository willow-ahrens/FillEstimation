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
             "label": "pathological_asx",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(2, 1e-4, n)], "bound":True},
             "oski": {"points": [{"delta":d} for d in exprange(0.1, 1.0, n)]},
             "ymax": 0.5,
             "xmaxmax": True
            },
            {"name": "pathological_oski",
             "label": "pathological_oski",
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
    plt.plot(times, errors, color = method["color"], label = method["name"])
    if "bound" in matrix[method["name"]] and matrix[method["name"]]["bound"]:
      plt.plot(times, [method["bound"](point) for point in matrix[method["name"]]["points"]], color = method["bound_color"], label = "%s bound" % method["name"])
    plt.errorbar(times, errors, yerr = [lo_bars, hi_bars], color = method["color"], linestyle="")
    xmax = min(xmax, max(times))
    xmaxmax = max(xmaxmax, max(times))

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
            "color":"red",
            "point":{"epsilon":0.5, "delta":0.01},
            "bound" : lambda point : point["epsilon"],
            "bound_color": "green"
           },
           {"name":"oski",
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
             "label": "patho..._asx",
            },
            {"name": "pathological_oski",
             "label": "patho..._oski",
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
  error_rects.append(axarr[0].bar(ind, errors, width, color = method["color"], yerr=errors_std, label = method["name"])[0])
  time_rects.append(axarr[1].bar(ind, times, width, color = method["color"], label = method["name"])[0])
  ind += width

axarr[0].set_ylabel('Mean (Maximum Relative Error)')
axarr[0].set_ylim([0, 0.5])
axarr[1].set_ylabel('Time To Compute Estimate (s)')
axarr[0].set_title('Error and Time Vs. Matrix (Default asx And oski Settings)')
axarr[1].set_xlabel('Matrix')
axarr[0].set_xticks(ind - totalwidth/2)
axarr[0].set_xticklabels([matrix["label"] for matrix in matrices], rotation = rotation)
axarr[1].set_xticks(ind - totalwidth/2)
axarr[1].set_xticklabels([matrix["label"] for matrix in matrices], rotation = rotation)
axarr[0].legend(error_rects, [method["name"] for method in methods])
axarr[1].legend(time_rects, [method["name"] for method in methods])
f.savefig("bar_default_settings.pdf")
plt.clf()
