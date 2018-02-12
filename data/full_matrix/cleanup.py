import sys
# this should be in src/matrix
# after process-inputs.py
# delete all tar file and untarred dirs

import os
import tarfile
import shutil

matrices = []
if __name__ == "__main__":
        for fname in os.listdir("."):
		# get all tar files and their unzipped dirs
                if fname.endswith('.tar.gz') and os.path.isfile(fname[:-7]+'_conv.mtx'):
			# delete tar
                        os.remove(fname)
                        matrices.append(fname[:-7])
        print matrices
	# delete unzipped dir
        for mfile in matrices:
                print mfile
		if os.path.isdir(mfile):
			shutil.rmtree(mfile)
