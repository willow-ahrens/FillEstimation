# generate performance matrix
python performance_matrix.py perf-matrix

# the correct one is in perf-matrix.npy
# this part you can do in parallel 

# for each matrix_name in matrix
python generate_spmv_times.py [basedir] [matrix_name]

# for example python generate_refs.py workspace ct20stif
python generate_refs.py [basedir] [matrix_name]

python bar-input.py perf-matrix.npy [basedir] [matrix_name]

# past here in serial
# make sure the script is ok to run esp. paths pointing to the right place (needs to be modified to take in basedir)
# generates roi plot inputs in basedir/outdir
python error-no-plot.py [basedir] [outdir]
