import util
import argparse
import time
import sys

tic = time.time()

args = util.parse(argparse.ArgumentParser())
util.get_profile()

toc = time.time()
sys.stderr.write("generate_profile() = {}\n".format(toc - tic))
