import util
import argparse
import os
args = util.parse(argparse.ArgumentParser())
prefix = "source {}; ".format(os.path.join(util.src, "env.sh"))

#cache references
util.experiment["create_script"](util.experiment["run"], "generate_references", prefix + "python {} -e \"{}\"".format(os.path.join(util.src, "generate_reference.py"), util.read_path(args.experiment)), util.experiment["matrix_registry"].keys())

#cache profile
util.experiment["create_script"](util.experiment["run"], "generate_profile", prefix + "python {} -e \"{}\"".format(os.path.join(util.src, "generate_profile.py"), util.read_path(args.experiment)), [])

#cache spmv_times
util.experiment["create_script"](util.experiment["run"], "generate_spmv_records", prefix + "python {} -e \"{}\"".format(os.path.join(util.src, "generate_spmv_record.py"), util.read_path(args.experiment)), util.experiment["matrix_registry"].keys())

#make table data
if "table_matrices" in util.experiment:
  util.experiment["create_script"](util.experiment["run"], "generate_table_data", prefix + "python {} -e \"{}\"".format(os.path.join(util.src, "generate_table_data.py"), util.read_path(args.experiment)), util.experiment["table_matrices"])

#make table
if "table_matrices" in util.experiment:
  util.experiment["create_script"](util.experiment["run"], "generate_table", prefix + "python {} -e \"{}\"".format(os.path.join(util.src, "generate_table.py"), util.read_path(args.experiment)), [])

#make plot
if "plot_points" in util.experiment:
  util.experiment["create_script"](util.experiment["run"], "generate_plots", prefix + "python {} -e \"{}\"".format(os.path.join(util.src, "generate_plot.py"), util.read_path(args.experiment)), util.experiment["plot_points"].keys())
