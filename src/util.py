import json
import os
import scipy.io
from subprocess import check_output
import numpy as np
import random
import sys
from sys import argv

# _, matrix_dir, ...
wherematrix = argv[1]

def matrix_path(matrix):
  return os.path.join(wherematrix, "%s.mtx" % (matrix))

def matrix_read(matrix):
  return scipy.io.mmread(matrix_path(matrix))

def spmv_time(matrices, r = 1, c = 1, trials = 1):
  outputs = []
  for matrix in matrices:
    myenv = os.environ.copy()
    myenv["GSL_RNG_SEED"] = str(random.randrange(sys.maxint))
    prefix = os.getenv("DATA_SPMV_PREFIX") 
    if prefix:
      command = prefix.split(" ")
    else:
      command = []
    command += [os.path.join(os.path.dirname(os.path.realpath(__file__)), "spmv")]
    command += ["-r", "%d" % r]
    command += ["-c", "%d" % c]
    command += ["-t", "%d" % trials]
    command += [matrix_path(matrix)]
    print(command)
    output = check_output(command, env=myenv)
    try:
      outputs.append(json.loads(output))
    except ValueError as e:
      print(output)
      raise(e)
  return outputs

def fill_estimates(name, matrices, B = 12, epsilon = 0.1, delta = 0.01, trials = 1, clock = True, results = False):
  outputs = []
  for matrix in matrices:
    myenv = os.environ.copy()
    myenv["GSL_RNG_SEED"] = str(random.randrange(sys.maxint))
    prefix = os.getenv("DATA_FILL_PREFIX") 
    if prefix:
      command = prefix.split(" ")
    else:
      command = []
    command = [os.path.join(os.path.dirname(os.path.realpath(__file__)), name)]
    command += ["-B", "%d" % B]
    command += ["-e", "%g" % epsilon]
    command += ["-d", "%g" % delta]
    command += ["-t", "%d" % trials]
    if clock:
      command += ["-c"]
    else:
      command += ["-C"]
    if results:
      command += ["-r"]
    else:
      command += ["-R"]
    command += [matrix_path(matrix)]
    print(command)
    output = check_output(command, env=myenv)
    try:
      outputs.append(json.loads(output))
    except ValueError as e:
      print(output)
      raise(e)
  return outputs

def get_flat_results(outputs):
  for i in range(len(outputs)):
    outputs[i]["flat_results"] = [[fill for a in output for fill in a] for output in outputs[i]["results"]]

def get_references(matrices, B = 12):
  outputs = fill_estimates("reference", matrices, B = B, trials = 1, clock = False, results = True)
  get_flat_results(outputs)
  references = [np.array(output["flat_results"][0]) for output in outputs]
  return references

def get_errors(outputs, references):
  get_flat_results(outputs)
  for (i, reference) in enumerate(references):
    fill_errors = np.abs(np.array(outputs[i]["flat_results"]) - reference[np.newaxis,:])/reference[np.newaxis,:]
    outputs[i]["errors"] = np.max(fill_errors, axis = 1)

def benchmark(name, matrices, B = 12, epsilon = 0.1, delta = 0.01):
  timeout = 0.1
  trials = 1
  times = []
  for matrix in matrices:
    while True:
      result = fill_estimates(name, [matrix], B = B, epsilon = epsilon, delta = delta, trials = trials)[0]
      if result["time_total"] >= 0.1:
        break
      trials *= 10
    times.append(result["time_mean"])
  return np.array(times)
