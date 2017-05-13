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
  return json.loads(output)

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

matrices = ["1138_bus.mtx"]
reference = np.array([benchmark("reference", matrix_path(matrix)) for matrix in matrices])
oski = np.array([benchmark("oski", matrix_path(matrix)) for matrix in matrices])
asx = np.array([benchmark("asx", matrix_path(matrix)) for matrix in matrices])

print("       Reference Time: %g" % np.mean(reference))
print("            OSKI Time: %g" % np.mean(oski))
print("             ASX Time: %g" % np.mean(asx))
print("     ASX/OSKI Speedup: %g" % np.mean(oski/asx))
print("ASX/Reference Speedup: %g" % np.mean(reference/asx))
