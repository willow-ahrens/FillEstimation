import matplotlib.pyplot as plt
import numpy as np
from util import *

B = 12
trials = 100

matrices = ["cont-300", "freeFlyingRobot_5", "3dtube_conv", "gupta1_conv", "ct20stif"]

rate = 2.0**0.5
bottom_error = 0.2

asx_delta = 0.01
asx_epsilon_init = 0.5

oski_top_error = 0.05
oski_delta_init = 0.005

references = get_references(matrices)
for (reference, matrix) in zip(references, matrices):
  oski_points = []
  oski_delta = oski_delta_init

  oski_error = 10000.0
  results = fill_estimates("oski", [matrix], B = B, results = True, trials = trials, delta = oski_delta)
  get_errors(results, [reference])
  oski_error = np.mean(results[0]["errors"])
  while oski_error > oski_top_error:
    oski_delta *= rate
    if oski_delta >= 1.0:
      oski_delta /= rate
      break
    results = fill_estimates("oski", [matrix], B = B, results = True, trials = trials, delta = oski_delta)
    get_errors(results, [reference])
    oski_error = np.mean(results[0]["errors"])
    print("decreasing oski error...", oski_error)

  oski_time = benchmark("oski", [matrix], B = B, delta = oski_delta)[0]

  oski_error = 0.0
  while oski_error < bottom_error:
    oski_delta /= rate
    oski_points.append({"delta":oski_delta})
    results = fill_estimates("oski", [matrix], B = B, results = True, trials = trials, delta = oski_delta)
    get_errors(results, [reference])
    oski_error = np.mean(results[0]["errors"])
    print("increasing oski error...", oski_error)

  asx_points = []
  asx_epsilon = asx_epsilon_init
  while benchmark("asx", [matrix], B = B, epsilon = asx_epsilon, delta = asx_delta)[0] < oski_time:
    asx_epsilon /= rate
    print("increasing asx runtime", asx_epsilon)
  asx_error = 0.0
  while asx_error < bottom_error:
    asx_points.append({"epsilon":asx_epsilon, "delta":asx_delta})
    results = fill_estimates("asx", [matrix], B = B, results = True, trials = trials, epsilon = asx_epsilon, delta = asx_delta)
    get_errors(results, [reference])
    asx_error = np.mean(results[0]["errors"])
    asx_epsilon *= rate
    print("increasing asx error", asx_error)

  methods = [{"name":"asx",
              "points":asx_points,
              "color":"red",
              #"bound": (lambda point : point["epsilon"]),
              #"bound_color":"green"
              },
             {"name":"oski",
              "points":oski_points,
              "color":"blue"}]

  xmax = 100000
  for method in methods:
    times = []
    errors = []
    hi_bars = []
    lo_bars = []
    for point in method["points"]:
      results = fill_estimates(method["name"], matrices, B = B, results = True, trials = trials, **point)
      get_errors(results, [reference])
      times.append(results[0]["time_mean"])
      errors.append(np.mean(results[0]["errors"]))
      hi_bars.append(np.std(results[0]["errors"]))
      lo_bars.append(np.std(results[0]["errors"]))
      if len(times) > 1:
        if times[0] < times[-1]:
          times = times[0:-1]
          errors = errors[0:-1]
          hi_bars = hi_bars[0:-1]
          lo_bars = lo_bars[0:-1]
          break
    plt.plot(times, errors, color = method["color"], label = method["name"])
    if "bound" in method:
      plt.plot(times, [method["bound"](point) for point in method["points"]], color = method["bound_color"], label = "%s bound" % method["name"])
    plt.errorbar(times, errors, yerr = [lo_bars, hi_bars], color = method["color"], linestyle="")
    xmax = min(xmax, times[0])

  plt.xlim([0, xmax])
  plt.xlabel('Time To Compute Estimate (s)')
  plt.ylabel('Median Of (Maximum Relative Error Over All Block Sizes)')
  plt.title('Median Maximum Relative Error Vs. Time To Compute (%s)' % (matrix))
  plt.legend(loc='best')
  plt.savefig("roi_%s" % (matrix))
  plt.clf()
