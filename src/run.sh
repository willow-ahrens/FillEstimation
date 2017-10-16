# generate performance matrix
python performance_matrix.py perf-matrix

# the correct one is in perf-matrix.npy
# this part you can do in parallel 

# for each matrix_name in matrix
# also need to edit this python file to generate the directory basedir / spmv_times_dir if does not exist
# genrate_spmvtimes and generate_refs needs to happen before respective bar-input
python generate_spmv_times.py [basedir] [matrix_name]

# edit file directory to generate if not exist
python generate_refs.py [basedir] [matrix_name]

python bar-input.py [matrix_name]

