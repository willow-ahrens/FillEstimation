import sys
import numpy
import scipy.io
import scipy.sparse

# this should be in src/matrix
# after download-dataset.sh

# iterate through all .tar.gz
# keep a list of repos
import os
import tarfile

# convert type to real and format to csr
def convert_matrix(infile, outfile):
        with open(infile) as f:
                matrix = scipy.io.mmread(f)
                matrix = scipy.sparse.coo_matrix(matrix, dtype=numpy.float64)
                matrix = scipy.sparse.csr_matrix(matrix)
                
                scipy.io.mmwrite(outfile, matrix)

# guarantee that its a tar file
def untar(fname):
        tar = tarfile.open(fname)
        tar.extractall()
        tar.close()

# for all tar files that have not yet been processed
# untar them, get the matrix with the same name as the directory
# convert it with the name [original name]_conv.mtx
matrices = []
if __name__ == "__main__":
        for fname in os.listdir("."):
                if fname.endswith('.tar.gz') and not os.path.isfile(fname[:-7]+'_conv.mtx'):
                        if not os.path.isdir(fname[:-7]):
				print fname
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
