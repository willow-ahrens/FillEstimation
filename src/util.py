import json
import os
import scipy.io
from subprocess import check_output
import numpy
import random
import sys
import argparse

top = os.path.realpath(os.path.join(os.path.dirname(os.path.realpath(__file__)), "../"))
src = os.path.join(top, "src")

experiment = {
  "data_path" = os.path.join(top, "data"),
  "matrix_path" = os.path.join(top, "data/matrix"),
  "machine_path" = os.path.join(src, "default_machine.py"),
  "matrix_registry_path" = os.path.join(top, "data/matrix/registry.json"),
  "run_path" = os.path.join(top, "run"),
  "fill_prefix" = "",
  "fill_vars" = {},
  "spmv_prefix" = "",
  "spmv_vars" = {},
  "machine_name" = "DefaultMachine",
  "experiment_name" = "DefaultExperiment",
  "verbose" = False
}

def make_path(path):
  if not os.path.exists(path):
    try:
      os.makedirs(path)
    except OSError as e:
      if not os.path.exists(path):
        raise(e)

def read_path(path):
  return os.path.realpath(os.path.expandvars(path))

def read_params(path):
  try
    with open(read_path(path)) as f:
      params = eval(r.read())
  except IOError as e:
    print("Parameter file ({0}) not found.".format(path))
    raise(e)
  assert isinstance(params, dict), "Parameter file ({0}) must evaluate to a dictionary.".format(path)
  return params

def read_matrix_entry(matrix):
  entry = {"name" = matrix,
           "relpath" = "{0}.mtx".format(matrix)
           "domain" = "unknown"}
  if matrix in experiment["matrix_registry"], "No matrix entry for {0}".format(matrix)
    newentry = experiment["matrix_registry"][matrix]
    assert isinstance(newentry, dict), "Matrix registry entry for ({0}) must evaluate to a dictionary.".format(matrix)
    entry.update(newentry)
  else
    print("Warning: matrix {0} not found in matrix registry".format(matrix))
  entry["path"] = read_path(os.path.join(experiment["matrix"], entry["relpath"]))
  return entry

def parse(parser):
  global verbose
  parser.add_argument("-e", "--experiment", help="an experiment parameters file (default = {0})".format(os.path.join(top, "src/default_experiment.py")), type=str, default = os.path.join(top, "src/default_experiment.py"))
  parser.add_argument("-v", "--verbose", help="increase verbosity", action="store_true")
  args = parser.parse_args()

  verbose = experiment["verbose"] and args.verbose

  experiment.update(read_params(args.experiment))

  if "machine_path" in experiment:
    experiment.update(read_params(experiment["machine_path"]))

  try
    try
      experiment["matrix_registry"] = json.loads(read_path(experiment["matrix_registry_path"]))
    except IOError as e:
      print("Matrix registry ({0}) not found.".format(experiment["matrix_registry_path"]))
      raise(e)
  except TypeError as e:
    print("Matrix registry ({0}) is not valid json.").format(experiment[matrix_registry_path])
    raise(e)
  assert isinstance(experiment["matrix_registry"], dict), "Matrix registry in ({0}) must evaluate to a dictionary.".format(experiment["matrix_registry_path"])

  experiment["data"] = read_path(experiment["data_path"])

  experiment["matrix"] = read_path(experiment["matrix_path"])

  experiment["run"] = read_path(experiment["run_path"])

  experiment["experiment"] = os.joinpath(experiment["data"], experiment["experiment_name"])
  experiment["reference"] = os.joinpath(experiment["experiment"], "reference")
  experiment["machine"] = os.joinpath(experiment["experiment"], experiment["machine_name"])

  assert isinstance(experiment["fill_vars"], dict), "Fill environment variables must evaluate to a dictionary."

  assert isinstance(experiment["spmv_vars"], dict), "SPMV environment variables must evaluate to a dictionary."

  assert isinstance(experiment["fill_prefix"], str), "Fill command prefix must evaluate to a string."

  assert isinstance(experiment["spmv_prefix"], str), "SPMV command prefix must evaluate to a string."

  return args

def matrix_path(matrix):
  entry = read_matrix_entry(matrix)
  path = entry["path"]
  assert os.path.isfile(path), "Matrix ({0}) not found at {1}.".format(matrix, os.path.join(entry["relpath"], experiment["matrix_path"]))
  return path

def matrix_name(matrix):
  return read_matrix_entry(matrix)["name"]

def matrix_domain(matrix):
  return read_matrix_entry(matrix)["domain"]

def matrix_size(matrix):
  return os.path.getsize(matrix_path(matrix))

def matrix_read(matrix):
  try:
    scipy.io.mmread(matrix_path(matrix))
  except e:
    print("Problem reading matrix ({0}) at {1}. Double check that matrix is in matrixmarket format.".format(matrix, os.path.join(entry["relpath"], experiment["matrix_path"]))
    raise(e)

def matrix_nnz(matrix):
  return matrix_read(matrix).nnz

def spmv_time(matrix, r = 1, c = 1, trials = 1):
  myenv = os.environ.copy()
  myenv["GSL_RNG_SEED"] = str(random.randrange(sys.maxint))
  myenv.update(experiment["spmv_vars"])

  prefix = experiment["spmv_prefix"]
  if prefix:
    command = prefix.split(" ")
  else:
    command = []
  command += [os.path.join(os.path.dirname(os.path.realpath(__file__)), "spmv")]
  command += ["-r", "%d" % r]
  command += ["-c", "%d" % c]
  command += ["-t", "%d" % trials]
  command += [matrix_path(matrix)]

  if verbose:
    print(command)

  try
    output = check_output(command, env=myenv)
  except e:
    print("error executing command ({0})".format(command))
    raise(e)

  try:
    parsed = append(json.loads(output))
  except ValueError as e:
    print("Output of command ({0}) must be valid json. Got:".format(command))
    print(output)
    raise(e)

  return parsed

def fill_estimates(name, matrices, B = 12, epsilon = 0.1, delta = 0.01, sigma = 0.01, trials = 1, clock = True, results = False, errors = False):
  if errors:
    results = True

  myenv = os.environ.copy()
  myenv["GSL_RNG_SEED"] = str(random.randrange(sys.maxint))
  myenv.update(experiment["spmv_vars"])

  prefix = experiment["spmv_prefix"]
  if prefix:
    command = prefix.split(" ")
  else:
    command = []
  command += [os.path.join(os.path.dirname(os.path.realpath(__file__)), name)]
  command += ["-B", "%d" % B]
  command += ["-e", "%g" % epsilon]
  command += ["-d", "%g" % delta]
  command += ["-s", "%g" % sigma]
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

  if verbose:
    print(command)

  try
    output = check_output(command, env=myenv)
  except e:
    print("error executing command ({0})".format(command))
    raise(e)

  try:
    parsed = append(json.loads(output))
  except ValueError as e:
    print("Output of command ({0}) must be valid json. Got:".format(command))
    print(output)
    raise(e)

  try:
    parsed["results"] = [np.array(result) for result in parsed["results"]]
  except ValueError as e:
    print("Could not read result estimates as numpy arrays. Got:")
    print(parsed["results"])
    raise(e)

  if errors:
    reference = get_reference["errors"]
    parsed["errors"] = [np.abs(result - reference) ./ reference for result in parsed["results"]]
    parsed["max_errors"] = [max(error) for error in parsed["errors"]]

  return parsed

def get_reference(matrix, B = 12):
  make_path(experiment["reference"])
  path = os.path.join(experiment["reference"], "{0}_reference.npy".format(matrix))
  if not os.path.isfile(path):
    np.save(path, fill_estimates("reference", matrix, B = B, trials = 1, clock = False, results = True, errors = False)["results"][0])
  try:
    reference = np.load(path)
  except e:
    print("error reading reference file at ({0})".format(path))
  return reference

"""
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
"""
