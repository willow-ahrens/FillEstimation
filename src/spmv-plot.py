import matplotlib.pyplot as plt
import numpy as np
from util import *
from sys import argv
from blockutil import *

B = 12
trials = 100


def exprange(a, b, n):
  r = (float(b) / float(a))**(1.0/(n - 1))
  return [a * r**i for i in range(n)]

n = 10
asx_delta = 0.01

_, perf_file = argv

# read in performance matrix
perf_matrix = np.load(perf_file)

methods = [{"name":"asx",
            "label":"ASX",
            "color":"red",
            "bound" : lambda point : point["epsilon"],
            "bound_color": "green"
            },
           {"name":"oski",
            "label":"OSKI",
            "color":"blue"}]
matrices = [
            {"name": "pathological_oski",
             "label": "pathological_OSKI",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(2, 1e-4, n)], "bound":True},
             "oski": {"points": [{"delta":d} for d in exprange(0.1, 1.0, n)]},
             # "ymax": 4,
             "xmaxmax": True
            }
           ]
'''           
matrices = [{"name": "3dtube_conv",
             "label": "3dtube",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(7, 0.2, n)]},
             "oski": {"points": [{"delta":d} for d in exprange(0.001, 0.06, n)]},
             # "ymax": 5
            },
            {"name": "gupta1_conv",
             "label": "gupta1",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(7, 0.2, n)]},
             "oski": {"points": [{"delta":d} for d in exprange(0.001, 0.06, n)]},
             #"ymax": 5
            },
            {"name": "ct20stif",
             "label": "ct20stif",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(7, 0.2, n)]},
             "oski": {"points": [{"delta":d} for d in exprange(0.001, 0.06, n)]},
            # "ymax": 5
            },
            {"name": "pathological_asx",
             "label": "pathological_ASX",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(2, 1e-4, n)], "bound":True},
             "oski": {"points": [{"delta":d} for d in exprange(0.1, 1.0, n)]},
             # "ymax": 5,
             "xmaxmax": True
            },
            {"name": "pathological_oski",
             "label": "pathological_OSKI",
             "asx": {"points": [{"epsilon":e, "delta":asx_delta} for e in exprange(2, 1e-4, n)], "bound":True},
             "oski": {"points": [{"delta":d} for d in exprange(0.1, 1.0, n)]},
             # "ymax": 4,
             "xmaxmax": True
            }
           ]
'''
# references = get_references([matrix["name"] for matrix in matrices], B = B)
# for (reference, matrix) in zip(references, matrices):
for matrix in matrices:
  # generate local performance matrix 
  spmv_runtimes = generate_spmv_runtimes(matrix['name'], trials, B)

  print(matrix["name"])
  
  xmax = 100000
  xmaxmax = 0
  for method in methods:
    print(method["name"])
    times = []
    spmv_times = []
    
    for point in matrix[method["name"]]["points"]:
      print(point)
      trial_time = 0
      blocksizes = []
      spmv_time = 0

      # estimate the fill + look up time to do spmv with the corresponding blocksize
      for i in range(0, trials):
          results = fill_estimates(method["name"], [matrix["name"]], B = B, results = True, **point)
          # get_errors(results, [reference])
          # time to estimate
          trial_time = trial_time + results[0]["time_mean"] 

          # get block size
          blocksize = get_blocksize(results, perf_matrix)

          # look up time that multiplying by with blocksize would take
          spmv_time = spmv_time + spmv_runtimes[blocksize[0]-1][blocksize[1] - 1]

      # time to estimate
      times.append(trial_time / trials)
      # 
      spmv_times.append(spmv_time / trials)
      # spmv time
      print("spmv time: %g, estimate time: %g" % (spmv_times[-1], times[-1]))
    plt.plot(times, spmv_times, color = method["color"], label = method["label"])
    xmax = min(xmax, max(times))
    xmaxmax = max(xmaxmax, max(times))
    outfile = method["name"] + '_' + matrix['name']

    # output space separated in form
    # [time to estimate] [spmv time] [block size] 
    # with open(outfile, 'w') as out:
    #    for i in range(0, len(spmv_times)):
    #        out.write(str(times[i]) + ' ' + str(spmv_times[i]) + ' ' + str(blocksizes[i]))

  if "xmaxmax" in matrix and matrix["xmaxmax"]:
    plt.xlim([0, xmaxmax])
  else:
    plt.xlim([0, xmax])
  # plt.ylim([0, matrix["ymax"]])
  plt.xlabel('Time To Compute Estimate (s)')
  plt.ylabel('Mean spmv time (s)')
  plt.title('Mean spmv time Vs. Time To Compute (%s)' % (matrix["label"]))
  plt.legend(loc='best')
  plt.savefig("spmv_%s.pdf" % (matrix["name"]))
  plt.clf()

# down here is the bar plot
'''
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

# references = get_references([matrix["name"] for matrix in matrices], B = B)
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
  error_rects.append(axarr[0].bar(ind, errors, width, color = method["color"], yerr=errors_std, label = method["label"])[0])
  time_rects.append(axarr[1].bar(ind, times, width, color = method["color"], label = method["label"])[0])
  ind += width

axarr[0].set_ylabel('Mean (Maximum Relative Error)')
axarr[0].set_ylim([0, 0.5])
axarr[1].set_ylabel('Time To Compute Estimate (s)')
axarr[0].set_title('Error and Time Vs. Matrix (Default ASX And OSKI Settings)')
axarr[1].set_xlabel('Matrix')
axarr[0].set_xticks(ind - totalwidth/2)
axarr[0].set_xticklabels([matrix["label"] for matrix in matrices], rotation = rotation)
axarr[1].set_xticks(ind - totalwidth/2)
axarr[1].set_xticklabels([matrix["label"] for matrix in matrices], rotation = rotation)
axarr[0].legend(error_rects, [method["label"] for method in methods])
axarr[1].legend(time_rects, [method["label"] for method in methods])
f.savefig("bar_default_settings.pdf")
plt.clf()
'''
