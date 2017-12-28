import util
import argparse
import os
import json
import numpy

parser = argparse.ArgumentParser()
parser.add_argument("matrix", help="the matrix to use", type=str)
args = util.parse(parser)

results = []
for point in util.experiment["plot_points"][args.matrix]:
  row = {}
  row["matrix"] = args.matrix
  row["matrix_name"] = util.matrix_name(args.matrix)
  row["matrix_domain"] = util.matrix_domain(args.matrix)
  row["matrix_size"] = util.matrix_size(args.matrix)
  row["matrix_nnz"] = util.matrix_nnz(args.matrix)
  row["normal_spmv_time"] = util.get_spmv_record(args.matrix)[0][0]
  row.update(point)
  for name in ["phil", "oski"]:
    output = util.fill_estimates(name, args.matrix, trials = util.experiment["trials"], clock = True, errors = True, spmv_times = True, **point)
    row["{}_mean_time".format(name)] = output["mean_time"]
    row["{}_mean_max_error".format(name)] = numpy.mean(output["max_errors"])
    row["{}_std_max_error".format(name)] = numpy.std(output["max_errors"])
    row["{}_mean_spmv_time".format(name)] = numpy.mean(output["spmv_times"])
  results.append(row)

plots = os.path.join(util.experiment["experiment"], "plots")
util.make_path(plots)
path = os.path.join(plots, "{}.json".format(args.matrix))

with open(path, 'w') as f:
  json.dump(results, f, indent=4, sort_keys=True)
