import sys
import json

import os

# print list of matrices to put in experiment file
matrices = []
if __name__ == "__main__":
        print '"table_matrices" : ['
	with open("registry.json") as g:
		matrices = json.load(g)
		for k, v in matrices.items():
			print '"' + k + '",'
	print '],'
