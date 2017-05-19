import matplotlib.pyplot as plt
import numpy as np
from util import *

B = 12
trials = 100

"""
matrices = ["3dtube_conv", "gupta1_conv", "ct20stif", "cont-300", "freeFlyingRobot_5"]
show_bound = False
rate = 2.0**0.5
bottom_error = 0.2
oski_bottom_error = 0.2
asx_delta = 0.01
asx_epsilon_init = 0.5
oski_top_error = 0.05
oski_delta_init = 0.005
oski_override_points = []
override_ylim = 0.0
"""

matrices = ["pathological_asx"]
show_bound = True
rate = 2.0**0.25
bottom_error = 0.5
oski_bottom_error = 0.45
asx_delta = 0.01
asx_epsilon_init = 0.5
oski_top_error = 0.10
oski_delta_init = 0.1
oski_override_points = []
oski_override_points = [{"delta": delta} for delta in 2**0.9 * 2**np.arange(-1.0, -6.0, -1.0)]
override_ylim = 0.0

"""
matrices = ["pathological_oski"]
show_bound = True
rate = 2.0**0.5
bottom_error = 0.6
oski_bottom_error = 1.5
asx_delta = 0.01
asx_epsilon_init = 0.5
oski_top_error = 1.0
oski_delta_init = 0.9
oski_override_points = [{"delta": delta} for delta in 2**0.9 * 2**np.arange(-1.0, -6.0, -1.0)]
override_ylim = 3.0
"""

references = get_references(matrices, B = B)
for (reference, matrix) in zip(references, matrices):
  asx_points = [{'epsilon': 0.044194173824159196, 'delta': 0.01}, {'epsilon': 0.06249999999999997, 'delta': 0.01}, {'epsilon': 0.0883883476483184, 'delta': 0.01}, {'epsilon': 0.12499999999999996, 'delta': 0.01}, {'epsilon': 0.17677669529663684, 'delta': 0.01}, {'epsilon': 0.24999999999999994, 'delta': 0.01}, {'epsilon': 0.35355339059327373, 'delta': 0.01}, {'epsilon': 0.5, 'delta': 0.01}, {'epsilon': 0.7071067811865476, 'delta': 0.01}, {'epsilon': 1.0000000000000002, 'delta': 0.01}, {'epsilon': 1.4142135623730954, 'delta': 0.01}, {'epsilon': 2.0000000000000004, 'delta': 0.01}, {'epsilon': 2.8284271247461907, 'delta': 0.01}, {'epsilon': 4.000000000000001, 'delta': 0.01}, {'epsilon': 5.6568542494923815, 'delta': 0.01}, {'epsilon': 8.000000000000002, 'delta': 0.01}]
  oski_points = [{'delta': 0.93303299153680741}, {'delta': 0.46651649576840371}, {'delta': 0.23325824788420185}, {'delta': 0.11662912394210093}, {'delta': 0.058314561971050463}]

  methods = [{"name":"asx",
              "points":asx_points,
              "color":"red",
              },
             {"name":"oski",
              "points":oski_points,
              "color":"blue"}]
  if show_bound:
    methods[0]["bound"] = lambda point : point["epsilon"]
    methods[0]["bound_color"] = "green"

  xmax = 100000
  for method in methods:
    times = []
    errors = []
    hi_bars = []
    lo_bars = []
    for point in method["points"]:
      results = fill_estimates(method["name"], [matrix], B = B, results = True, trials = trials, **point)
      get_errors(results, [reference])
      times.append(results[0]["time_mean"])
      print(len(results[0]["errors"]))
      print(sorted(results[0]["errors"]))
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
  if override_ylim != 0.0:
    plt.ylim([0, override_ylim])
  plt.xlabel('Time To Compute Estimate (s)')
  plt.ylabel('Mean (Maximum Relative Error Over Block Sizes)')
  plt.title('Mean Maximum Relative Error Vs. Time To Compute (%s)' % (matrix))
  plt.legend(loc='best')
  plt.savefig("roi_%s" % (matrix))
  plt.clf()
