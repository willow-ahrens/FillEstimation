import util
import argparse
import time
import sys

tic = time.time()

parser = argparse.ArgumentParser()
parser.add_argument("matrix", help="the matrix to use", type=str)
args = util.parse(parser)
util.get_spmv_record(args.matrix)

toc = time.time()
sys.stderr.write("generate_spmv_records({}) = {}\n".format(args.matrix, toc - tic))
