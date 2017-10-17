#!/bin/python

from sys import argv
import os

# 
# input dir
# output file in the form
# matrix_name [asx_time] [oski_time] [asx_err] [oski_err] [asx_spmv] [oski_spmv]
_, matrix_list, in_dir, outfile = argv

err_prefix = 'err_'
spmv_prefix = 'spmv_'
times_prefix = 'times_'
max_len = 5
sep = ' & '
better_prefix = r'\textcolor{safe-peach}{'
better_suffix = '}'

def truncate(str_in):
    if len(str_in) > max_len:
        return str_in[:max_len]
    return str_in


def better_helper(s):
    return better_prefix + s + better_suffix

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
    counter = 1
    with open(outfile, 'w') as out:
        for matrix in matrices:
            with open(os.path.join(in_dir, times_prefix + matrix), 'r') as timesfile:
                time = timesfile.readline().strip()
                parts = time.split(' ')

                # get normalized times

                asx_time, oski_time = better_test(truncate(parts[1]), truncate(parts[2]))

            with open(os.path.join(in_dir, err_prefix + matrix), 'r') as errfile:
                err = errfile.readline().strip()
                parts = err.split(' ')

                asx_err, oski_err = better_test(truncate(parts[1]), truncate(parts[3]))

            with open(os.path.join(in_dir, spmv_prefix + matrix), 'r') as spmvfile:
                spmv = spmvfile.readline().strip()
                parts = spmv.split(' ')
 
                asx_spmv, oski_spmv = better_test(truncate(parts[1]), truncate(parts[3]))
               
            ind_string = str(counter) + '.'
            if counter < 10:
                ind_string = '0' + ind_string
            matrix_name = matrix
            if matrix.endswith('_conv'):
                matrix_name = matrix[:-5]
            matrix_name = ind_string + matrix_name
            matrix_name = matrix_name.replace('_','\_')

            out.write(matrix_name + sep + asx_time + sep + oski_time + sep + asx_err + sep + oski_err + sep + asx_spmv + sep + oski_spmv + ' \\\\ \n')
            counter = counter + 1
