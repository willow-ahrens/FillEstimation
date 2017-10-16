cd matrix
source download-dataset.sh
cd ..

mkdir performance-matrices

python performance-matrix.py performance-matrices/perf-matrix

mkdir default-plot-inputs
mkdir matrix-times
python bar-input.py performance-matrices/perf-matrix.npy

mkdir plot-inputs
python error-no-plot.py 
