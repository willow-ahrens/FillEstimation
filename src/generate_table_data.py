import util
import argparse
import os
import json
import numpy

parser = argparse.ArgumentParser()
parser.add_argument("matrix", help="the matrix to use", type=str)
args = util.parse(parser)

results = {}
for name in ["phil", "oski"]:
  output = util.fill_estimates(name, args.matrix, trials = util.experiment["trials"], clock = True, errors = True, spmv_times = True)
  results[name] = {"mean_time" : output["mean_time"],
                   "mean_max_error" : numpy.mean(output["max_errors"]),
                   "mean_spmv_time" : numpy.mean(output["spmv_times"])}

table = os.path.join(util.experiment["experiment"], "table")
util.make_path(table)
path = os.path.join(table, "{}.json".format(args.matrix))

with open(path, 'w') as f:
  json.dump(results, f, indent=4, sort_keys=True)
