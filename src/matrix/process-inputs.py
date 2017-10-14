import sys
import numpy
import scipy.io
import scipy.sparse
from conv import *

# iterate through all .tar.gz
# keep a list of repos
import os
import tarfile


def convert_matrix(infile, outfile):
    with open(infile) as f:
        matrix = scipy.io.mmread(f)
        matrix = scipy.sparse.coo_matrix(matrix, dtype=numpy.float64)
        matrix = scipy.sparse.csr_matrix(matrix)
        
        print 'before write'
            scipy.io.mmwrite(outfile, matrix)

# guarantee that its a tar file
def untar(fname):
    tar = tarfile.open(fname)
    tar.extractall()
    tar.close()

matrices = []
if __name__ == "__main__":
    for fname in os.listdir("."):
        if fname.endswith('.tar.gz') and not os.path.isfile(fname[:-7]+'_conv.mtx'):
            if not os.path.isdir(fname[:-7]):
                untar(fname)
            matrices.append(fname[:-7])
    print matrices

    for mfile in matrices:
        print mfile

        # in the format name/name.mtx
        mpath = os.path.join(mfile, mfile + '.mtx')

        print mpath

        # output is in name_conv.mtx
        outpath = mfile + '_conv.mtx'
        print outpath
        convert_matrix(mpath, outpath)
