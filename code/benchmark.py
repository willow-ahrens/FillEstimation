from util import *
import numpy as np

def benchmark(name, matrices, B = 12, epsilon = 0.1, delta = 0.01):
  timeout = 0.1
  trials = 1
  times = []
  for matrix in matrices:
    while True:
      result = fill_estimates(name, [matrix_path(matrix)], B = B, epsilon = epsilon, delta = delta, trials = trials)[0]
      if result["time_total"] >= 0.1:
        break
      trials *= 10
    times.append(result["time_mean"])
  return np.array(times)

matrices = ["freeFlyingRobot_5"]
oski_kwargs= {"delta": 0.02}
asx_kwargs= {"epsilon": 0.3, "delta": 0.01}

reference = benchmark("reference", matrices)
oski = benchmark("oski", matrices, **oski_kwargs)
asx = benchmark("asx", matrices, **asx_kwargs)

print("       Reference Time: %g" % np.mean(reference))
print("            OSKI Time: %g" % np.mean(oski))
print("             ASX Time: %g" % np.mean(asx))
print("     ASX/OSKI Speedup: %g" % np.mean(oski/asx))
print("ASX/Reference Speedup: %g" % np.mean(reference/asx))

trials = 100;
references = get_references(matrices)
asx = fill_estimates("asx", matrices, results = True, trials = trials, **asx_kwargs)
oski = fill_estimates("oski", matrices, results = True, trials = trials, **asx_kwargs)
get_errors(asx, references)
get_errors(oski, references)
asx = np.concatenate([result["errors"] for result in asx], axis = 0)
oski = np.concatenate([result["errors"] for result in oski], axis = 0)
print("       Over %d trials..." % trials)
print(" Median asx max error: %g" % np.median(asx))
print("Median oski max error: %g" % np.median(oski))
print("   Mean asx max error: %g" % np.mean(asx))
print("  Mean oski max error: %g" % np.mean(oski))
