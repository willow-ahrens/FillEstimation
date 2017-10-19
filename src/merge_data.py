#!/bin/python

from sys import argv
import os

# input dir
# output file in the form
# matrix_name [asx_time] [oski_time] [asx_err] [oski_err] [asx_spmv] [oski_spmv]
_, matrix_list, in_dir1, in_dir2, outfile = argv

err_prefix = 'err_'
spmv_prefix = 'spmv_'
times_prefix = 'times_'
max_len = 5
sep = ' & '
better_prefix = r'\cellcolor{safe-peach!25}'
# better_suffix = '}'

def truncate(str_in):
    if len(str_in) > max_len:
        return str_in[:max_len]
    return str_in


def better_helper(s):
    return better_prefix + s # + better_suffix

# lower is better
def better_test(asx_str, oski_str):
    if float(oski_str) == float(asx_str):
        return asx_str, oski_str
    elif float(oski_str) > float(asx_str):
        return better_helper(asx_str), oski_str
    else:
        return asx_str, better_helper(oski_str)

    
if __name__ == "__main__":
    # get matrices in a list
    with open(matrix_list, 'r') as mfile:
        matrices = mfile.readlines()
    matrices = [m.strip() for m in matrices]

    print matrices
    with open(outfile, 'w') as out:
        for matrix in matrices:
            print matrix
            stuff = matrix.split(' ')
            # set the first thing
            matrix_name = stuff[0]
            if matrix_name.endswith('_conv'):
                matrix_name = stuff[0][:-5]
            matrix_name = matrix_name.replace('_','\_')

            # todo : put matrix meta here

            out_line = matrix_name + sep + stuff[1] + sep + stuff[2]
            

            # for now do this
            with open(os.path.join(in_dir1, times_prefix + stuff[0]), 'r') as timesfile:
                time = timesfile.readline().strip()
                parts = time.split(' ')

                # get normalized times
                asx_time, oski_time = better_test(truncate(parts[1]), truncate(parts[2]))
                out_line = out_line + sep + asx_time + sep + oski_time

            with open(os.path.join(in_dir1, err_prefix + stuff[0]), 'r') as errfile:
                err = errfile.readline().strip()
                parts = err.split(' ')
                asx_err, oski_err = better_test(truncate(parts[1]), truncate(parts[3]))
                out_line = out_line + sep + asx_err + sep + oski_err

            with open(os.path.join(in_dir1, spmv_prefix + stuff[0]), 'r') as spmvfile:
                spmv = spmvfile.readline().strip()
                parts = spmv.split(' ')
                asx_spmv, oski_spmv = better_test(truncate(parts[1]), truncate(parts[3]))
                out_line = out_line + sep + asx_spmv + sep + oski_spmv

            out_line = out_line + '&'

            # directory 2
            with open(os.path.join(in_dir2, times_prefix + stuff[0]), 'r') as timesfile:
                time = timesfile.readline().strip()
                parts = time.split(' ')

                # get normalized times
                asx_time, oski_time = better_test(truncate(parts[1]), truncate(parts[2]))
                out_line = out_line + sep + asx_time + sep + oski_time

            with open(os.path.join(in_dir2, err_prefix + stuff[0]), 'r') as errfile:
                err = errfile.readline().strip()
                parts = err.split(' ')
                asx_err, oski_err = better_test(truncate(parts[1]), truncate(parts[3]))
                out_line = out_line + sep + asx_err + sep + oski_err

            with open(os.path.join(in_dir2, spmv_prefix + stuff[0]), 'r') as spmvfile:
                spmv = spmvfile.readline().strip()
                parts = spmv.split(' ')
                asx_spmv, oski_spmv = better_test(truncate(parts[1]), truncate(parts[3]))
                out_line = out_line + sep + asx_spmv + sep + oski_spmv

            out.write(out_line +' \\\\ \n')

