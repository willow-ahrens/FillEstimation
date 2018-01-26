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

def exprange(a, b, n):
  r = (float(b) / float(a))**(1.0/(n - 1))
  return [a * r**i for i in range(n)]

def make_path(path):
  if not os.path.exists(path):
    try:
      os.makedirs(path)
    except OSError as e:
      if not os.path.exists(path):
        raise(e)

def create_bash_script(path, name, command, parallel):
  make_path(path)
  path = os.path.join(path, "{0}.sh".format(name))
  with open(path, "w") as f:
    f.write("#!/bin/bash\n")
    if parallel:
      for thing in parallel:
        f.write("{0} {1}\n".format(command, thing))
    else:
      f.write("{0}\n".format(command))

def create_create_slurm_script(preamble):
  def create_slurm_script(path, name, command, parallel):
    make_path(path)
    path = os.path.join(path, "{0}.sl".format(name))
    with open(path, "w") as f:
      f.write("#!/bin/bash\n")
      f.write(preamble)
      if parallel:
        f.write("#SBATCH --array=0-{}\n".format(len(parallel) - 1))
        for (i, thing) in enumerate(parallel):
          f.write("if test $SLURM_ARRAY_TASK_ID -eq {0}; then \n".format(i))
          f.write("{0} {1};\n".format(command, thing))
          f.write("fi\n")
      else:
        f.write("{0}\n".format(command))
  return create_slurm_script

experiment = {
  "experiment_name" : "DefaultExperiment",
  "data_path" : os.path.join(top, "data"),
  "matrix_path" : os.path.join(top, "data/matrix"),
  "matrix_registry_path" : os.path.join(top, "data/matrix/registry.json"),
  "run_path" : os.path.join(top, "run"),
  "fill_prefix" : "",
  "fill_vars" : {},
  "spmv_prefix" : "",
  "spmv_vars" : {},
  "create_script" : create_bash_script,
  "B" : 12,
  "epsilon" : 0.5,
  "delta" : 0.01,
  "sigma" : 0.02,
  "trials" : 100,
  "profile_m" : 1000,
  "profile_n" : 1000,
  "profile_trials" : 1000,
  "verbose" : False
}

def read_path(path):
  return os.path.realpath(os.path.expandvars(path))

def read_params(path):
  try:
    with open(read_path(path)) as f:
      params = eval(f.read())
  except Exception as e:
    print("Parameter file ({0}) not found or invalid python format.".format(path))
    raise(e)
  assert isinstance(params, dict), "Parameter file ({0}) must evaluate to a dictionary.".format(path)
  return params

def read_matrix_entry(matrix):
  entry = {"name" : matrix,
           "relpath" : "{0}.mtx".format(matrix),
           "domain" : "unknown"}
  if matrix in experiment["matrix_registry"]:
    newentry = experiment["matrix_registry"][matrix]
    assert isinstance(newentry, dict), "Matrix registry entry for ({0}) must evaluate to a dictionary.".format(matrix)
    entry.update(newentry)
  else:
    if verbose:
      print("Warning: matrix {0} not found in matrix registry".format(matrix))
  if "path" not in entry:
    entry["path"] = read_path(os.path.join(experiment["matrix"], entry["relpath"]))
  return entry

def parse(parser):
  global verbose
  parser.add_argument("-e", "--experiment", help="an experiment parameters file (default = {0})".format(os.path.join(top, "src/experiment_default.py")), type=str, default = os.path.join(top, "src/experiment_default.py"))
  parser.add_argument("-v", "--verbose", help="increase verbosity", action="store_true")
  args = parser.parse_args()

  verbose = experiment["verbose"] and args.verbose

  experiment.update(read_params(args.experiment))

  experiment["matrix_registry"] = read_params(experiment["matrix_registry_path"])

  experiment["data"] = read_path(experiment["data_path"])

  experiment["matrix"] = read_path(experiment["matrix_path"])

  experiment["run"] = os.path.join(read_path(experiment["run_path"]), experiment["experiment_name"])

  experiment["experiment"] = os.path.join(experiment["data"], "experiment", experiment["experiment_name"])
  experiment["references"] = os.path.join(experiment["experiment"], "references")
  experiment["profile"] = os.path.join(experiment["experiment"], "profile")
  experiment["spmv_records"] = os.path.join(experiment["experiment"], "spmv_records")

  assert isinstance(experiment["fill_vars"], dict), "Fill environment variables must evaluate to a dictionary."

  assert isinstance(experiment["spmv_vars"], dict), "SPMV environment variables must evaluate to a dictionary."

  assert isinstance(experiment["fill_prefix"], str), "Fill command prefix must evaluate to a string."

  assert isinstance(experiment["spmv_prefix"], str), "SPMV command prefix must evaluate to a string."

  return args

def matrix_path(matrix):
  entry = read_matrix_entry(matrix)
  path = entry["path"]
  assert os.path.isfile(path), "Matrix ({0}) not found at {1}.".format(matrix, os.path.join(experiment["matrix_path"], entry["relpath"]))
  return path

def matrix_name(matrix):
  return read_matrix_entry(matrix)["name"]

def matrix_domain(matrix):
  return read_matrix_entry(matrix)["domain"]

def matrix_size(matrix):
  return os.path.getsize(matrix_path(matrix))

def matrix_read(matrix):
  try:
    return scipy.io.mmread(matrix_path(matrix))
  except Exception as e:
    print("Problem reading matrix ({0}) at {1}. Double check that matrix is in matrixmarket format.".format(matrix, os.path.join(entry["relpath"], experiment["matrix_path"])))
    raise(e)

def matrix_nnz(matrix):
  return matrix_read(matrix).nnz

def matrix_n(matrix):
  return matrix_read(matrix).shape[0]

def matrix_m(matrix):
  return matrix_read(matrix).shape[1]

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

  try:
    output = check_output(command, env=myenv)
  except Exception as e:
    print("error executing command ({0})".format(command))
    raise(e)

  try:
    parsed = json.loads(output)
  except ValueError as e:
    print("Output of command ({0}) must be valid json. Got:".format(command))
    print(output)
    raise(e)

  return parsed

def get_profile(B = None, m = None, n = None, trials = None):
  if not B:
    B = experiment["B"]
  if not m:
    m = experiment["profile_m"]
  if not n:
    n = experiment["profile_n"]
  if not trials:
    trials = experiment["profile_trials"]

  make_path(experiment["profile"])

  dense_path = os.path.join(experiment["profile"], "dense_{}_{}.mtx".format(m, n))
  profile_path = os.path.join(experiment["profile"], "profile_{}_{}_{}_{}.npy".format(B, m, n, trials))

  if not os.path.isfile(dense_path):
    I = []
    J = []
    for i in range(m):
      for j in range(n):
        I += [i]
        J += [j]
    V = [1.0 for _ in I]
    dense = scipy.sparse.coo_matrix((V, (I,J)), dtype=numpy.float64)
    dense = scipy.sparse.csr_matrix(dense)
    scipy.io.mmwrite(dense_path, dense)

  experiment["matrix_registry"]["__P3T3R_IS_TH3_UB3R_HAX0R__"] = {"name" : "dense_{}_{}.mtx".format(m, n),
                                                                  "domain" : "synthetic",
                                                                  "path" : dense_path}

  if not os.path.isfile(profile_path):
    profile = numpy.ones((B, B))
    for r in range(1, B + 1):
      for c in range(1, B + 1):
        t = spmv_time("__P3T3R_IS_TH3_UB3R_HAX0R__", r = r, c = c, trials = trials)["mean_time"]
        profile[r-1][c-1] = float(n) * float(m) / float(t)
    numpy.save(profile_path, profile)

  experiment["matrix_registry"].pop("__P3T3R_IS_TH3_UB3R_HAX0R__", None)

  return numpy.load(profile_path)

def get_spmv_record(matrix, B = None, trials = None):
  if not B:
    B = experiment["B"]
  if not trials:
    trials = experiment["profile_trials"]

  make_path(experiment["spmv_records"])

  path = os.path.join(experiment["spmv_records"], "{}_{}_{}.npy".format(matrix, B, trials))

  if not os.path.isfile(path):
    record = numpy.ones((B, B))
    for r in range(1, B + 1):
      for c in range(1, B + 1):
        t = spmv_time(matrix, r = r, c = c, trials = trials)["mean_time"]
        record[r-1][c-1] = float(t)
    numpy.save(path, record)
  return numpy.load(path)

def fill_estimates(name, matrix, B = None, epsilon = None, delta = None, sigma = None, trials = 1, clock = True, results = False, errors = False, blocks = False, spmv_times = False):
  if not B:
    B = experiment["B"]
  if not epsilon:
    epsilon = experiment["epsilon"]
  if not delta:
    delta = experiment["delta"]
  if not sigma:
    sigma = experiment["sigma"]
  if errors:
    results = True
  if spmv_times:
    blocks = True

  myenv = os.environ.copy()
  myenv["GSL_RNG_SEED"] = str(random.randrange(sys.maxint))
  myenv.update(experiment["fill_vars"])

  prefix = experiment["fill_prefix"]
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

  try:
    output = check_output(command, env=myenv)
  except Exception as e:
    print("error executing command ({0})".format(command))
    raise(e)

  try:
    parsed = json.loads(output)
  except Exception as e:
    print("Output of command ({0}) must be valid json. Got:".format(command))
    print(output)
    raise(e)

  try:
    parsed["results"] = [numpy.array(result) for result in parsed["results"]]
  except Exception as e:
    print("Could not read result estimates as numpy arrays. Got:")
    print(parsed["results"])
    raise(e)

  if errors:
    reference = get_reference(matrix, B = B)
    parsed["errors"] = [numpy.abs(result - reference) / reference for result in parsed["results"]]
    parsed["max_errors"] = [numpy.max(error) for error in parsed["errors"]]

  if blocks:
    profile = get_profile(B = B)
    parsed["blocks"] = [numpy.unravel_index((profile/fill).argmax(), profile.shape) for fill in parsed["results"]]

  if spmv_times:
    record = get_spmv_record(matrix, B = B)
    parsed["spmv_times"] = [record[block] for block in parsed["blocks"]]

  return parsed

def get_reference(matrix, B = None):
  if not B:
    B = experiment["B"]

  make_path(experiment["references"])
  path = os.path.join(experiment["references"], "{0}_reference_{1}.npy".format(matrix, B))
  if not os.path.isfile(path):
    numpy.save(path, fill_estimates("reference", matrix, B = B, trials = 1, clock = False, results = True, errors = False)["results"][0])
  try:
    reference = numpy.load(path)
  except e:
    print("Could not read reference file at ({0})".format(path))
  return reference


