import util
import argparse
import os

parser = argparse.ArgumentParser()
parser.add_argument("matrix", help="the matrix to use", type=str)
args = util.parse(parser)

phil_results = util.fill_estimates("phil", args.matrix, trials = util.experiment["trials"], clock = True, errors = True, spmv_times = True)
oski_results = util.fill_estimates("oski", args.matrix, trials = util.experiment["trials"], clock = True, errors = True, spmv_times = True)
results = {"phil" : phil_results,
           "oski" : oski_results,
           "normal_spmv_time" : get_spmv_record(args.matrix)[0][0]}

table = os.path.join(util.experiment["experiment"], "table")
util.make_path(table)
path = os.path.join("{}.json".format(args.matrix))

with open(path) as f:
  json.dump(results, f, indent=4, sort_keys=True)
