import util
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("matrix", help="the matrix to use", type=str)
args = util.parse(parser)
util.get_reference(args.matrix)
