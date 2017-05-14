import json
import os
from subprocess import check_output
import numpy as np

def fill_estimate(name, matrix, B = 12, trials = 1, clock = True, results = False):
  command = [os.path.join(os.path.dirname(os.path.realpath(__file__)), name)]
  command += ["-B", "%d" % B]
  command += ["-t", "%d" % trials]
  if clock:
    command += ["-c"]
  else:
    command += ["-d"]
  if results:
    command += ["-r"]
  else:
    command += ["-s"]
  command += [matrix]
  output = check_output(command)
  try:
    return json.loads(output)
  except ValueError as e:
    print(output)
    raise(e)


def matrix_path(matrix):
  return os.path.join(os.path.dirname(os.path.realpath(__file__)), "matrix", matrix)

def benchmark(name, matrix):
  timeout = 0.1
  trials = 1
  while True:
    output = fill_estimate(name, matrix, trials = trials)
    if output["time_total"] >= 0.1:
      break
    trials *= 10

  return output["time_mean"]

def flat_results(name, matrix, trials = 1):
  results = fill_estimate(name, matrix, clock = False, results = True, trials = trials)["output"]
  return [[fill for a in result for fill in a] for result in results]

matrices = ["tp-6.mtx"]
#matrices = ["freeFlyingRobot_5.mtx"]
reference = np.array([benchmark("reference", matrix_path(matrix)) for matrix in matrices])
oski = np.array([benchmark("oski", matrix_path(matrix)) for matrix in matrices])
asx = np.array([benchmark("asx", matrix_path(matrix)) for matrix in matrices])

print("       Reference Time: %g" % np.mean(reference))
print("            OSKI Time: %g" % np.mean(oski))
print("             ASX Time: %g" % np.mean(asx))
print("     ASX/OSKI Speedup: %g" % np.mean(oski/asx))
print("ASX/Reference Speedup: %g" % np.mean(reference/asx))

trials = 100;
reference = []
asx = []
oski = []
for matrix in matrices:
  reference_results = flat_results("reference", matrix_path(matrix), trials = 1)
  reference += [reference_results[0] for _ in range(trials)]
  asx += flat_results("asx", matrix_path(matrix), trials = trials)
  oski += flat_results("oski", matrix_path(matrix), trials = trials)
reference = np.array(reference)
asx = np.array(asx)
oski = np.array(oski)
print(type(asx[1]))
print("       Over %d trials..." % trials)
print(" Median asx max error: %g" % np.median(np.max(np.abs(reference - asx)/reference, axis=1)))
print("Median oski max error: %g" % np.median(np.max(np.abs(reference - oski)/reference, axis=1)))
print("   Mean asx max error: %g" % np.mean(np.max(np.abs(reference - asx)/reference, axis=1)))
print("  Mean oski max error: %g" % np.mean(np.max(np.abs(reference - oski)/reference, axis=1)))
