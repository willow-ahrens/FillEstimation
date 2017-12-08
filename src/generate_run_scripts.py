import util
import argparse
import os
args = util.parse(argparse.ArgumentParser())

#cache references
util.experiment["create_script"](util.experiment["run"], "generate_references", "python {0} -e \"{1}\"".format(os.path.join(util.src, "generate_references.py"), util.read_path(args.experiment)), util.experiment["matrix_registry"].keys())

