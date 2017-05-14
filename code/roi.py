import matplotlib.pyplot as plt
import numpy as np
from util import *

def plot_roi(matrix, methods, B = 12, trials = 100):
  for method in methods:
    references = get_references([matrix])
    times = []
    errors = []
    hi_bars = []
    lo_bars = []
    for point in method["points"]:
      results = fill_estimates(method["name"], matrices, B = B, results = True, trials = trials, **point)
      get_errors(results, references)
      times.append(results[0]["time_mean"])
      errors.append(np.median(results[0]["errors"]))
      #hi_bars.append(np.max(results[0]["errors"]) - errors[-1])
      #lo_bars.append(errors[-1] - np.min(results[0]["errors"]))
      hi_bars.append(np.std(results[0]["errors"]))
      lo_bars.append(np.std(results[0]["errors"]))
    plt.plot(times, errors, color = method["color"], label = method["name"])
    plt.errorbar(times, errors, yerr = [lo_bars, hi_bars], color = method["color"], linestyle="")

  plt.xlabel('Time To Compute Estimate (s)')
  plt.ylabel('Median Of (Maximum Relative Error Over All Block Sizes)')
  plt.title('Median Maximum Relative Error Vs. Time To Compute (%s)' % (matrix))
  plt.legend(loc='best')
  plt.savefig("roi_%s" % (matrix))
  plt.clf()


matrices = ["cont-300"]

asx_points = []
asx_epsilon_min = 4.0
asx_max = 4.0
for x in np.arange(-2.0, 3.0, 1.0):
  asx_points.append({"epsilon":1.0/(2.0**(x)), "delta":0.01})

oski_points = []
for x in np.arange(-10.0, -3.0, 1.0):
  oski_points.append({"delta":1.0/(2.0**(-x))})

methods = [{"name":"asx",
            "points":asx_points,
            "color":"red"},
           {"name":"oski",
            "points":oski_points,
            "color":"blue"}]

for matrix in matrices:
  plot_roi(matrix, methods, B = 12, trials = 100)
"""
matrices = ["freeFlyingRobot_5"]

asx_points = []
asx_epsilon_min = 4.0
asx_max = 4.0
for x in np.arange(-2.0, 2.0, 1.0):
  asx_points.append({"epsilon":1.0/(2.0**(x)), "delta":0.01})

oski_points = []
for x in np.arange(-6.0, -0.7, 0.6):
  oski_points.append({"delta":1.0/(2.0**(-x))})

methods = [{"name":"asx",
            "points":asx_points,
            "color":"red"},
           {"name":"oski",
            "points":oski_points,
            "color":"blue"}]

for matrix in matrices:
  plot_roi(matrix, methods, B = 12, trials = 100)
"""
