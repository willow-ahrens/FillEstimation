Tensor Fill Estimation: A Comparison
======================

Build
--------
Our makefiles can be run with [GNU Make](https://www.gnu.org/software/make/).
Each directory with a subdirectory that has an executable or other dependency
that needs to be made has a makefile with an 'all' make target and a 'clean'
make target which operate on the current directory and any subdirectories.

Organization
--------

`deps/`:
  Our implementation (and associated analysis) has some software dependencies.
To make our results more replicable, we have included the exact versions of
this software that we have used in the deps/ folder, and linked them into our
build system. Each subdirectory of deps/ contains a different software
dependency and associated Makefile. If you would like to provide custom build
instructions for some dependency $DEP, create and modify a copy of
deps/$DEP/Makefile.Default named `deps/$DEP/Makefile.$ARCH` where $ARCH is the
name of your current architecture (The default value of $ARCH can be queried by
typing make arch at the top level directory of this project) or a custom name
you have given your machine (you can specify the value of the $ARCH variable
by invoking make as `make ARCH=$ARCH`). The deps make target at the top level
directory of our project will build our software dependencies.

`src/`:
  The src directory contains the implementations of our algorithms and the
test harnesses we use to generate results.

  Our [Phil]() algorithm is implemented in `src/phil.c` and built into the
executable `phil`. A different algorithm (described in the file
`oski-1.0.1h/src/heur/estfill.c` in the
[OSKI](https://bebop.cs.berkeley.edu/oski/) library) is implemented in
`src/oski.c` and built into the executable `oski`. A reference algorithm is
implemented in `reference.c` and built into the executable `reference`. An
implementation of sparse (possibly blocked) matrix-vector multiply is provided
by the [TACO](http://tensor-compiler.org/) library in `spmv.cpp` and built into
the executable `spmv`. All executables take a `-h` option describing their
usage, and provide their output in [JSON](https://www.json.org/) format. If you
would like to provide custom build instructions for the executables in `src/`,
create and modify a copy of `src/Makefile.Default` named `src/Makefile.$ARCH`
where `$ARCH` is the name of your current architecture (The default value of
`$ARCH` can be queried by typing make arch at the top level directory of this
project) or a custom name you have given your machine (you can specify the
value of the `$ARCH` variable by invoking make as `make ARCH=$ARCH`). The
makefile also generates a file called `src/env.sh` which sets important
environment variables necessary to run the code. The `src` make target at the
top level directory of our project will build our software dependencies.

  The test harnesses are controlled by a parameter file which describes the
settings to run a particular experiment on a particular machine. The parameter
file is written in python and must evaluate to a dictionary. An example
parameter file is given in `src/experiment_default.py`. The `"data_path"`
parameter specifies where results will be stored. The test harness files are
named after the type of results they generate. `src/generate_spmv_records.py`
records the time taken to perform a blocked sparse matrix vector multiply of a
sparse matrix with several different block sizes. `src/generate_profile.py`
investigates how the relationship between performance and block size on this
machine.  `src/generate_reference.py` records reference fill values of a sparse
matrix for several different block sizes. `src/generate_plot.py` examines the
performance of our fill estimation algorithms on a sparse matrix as a function
of different parameters (storing the output as a
[csv](https://en.wikipedia.org/wiki/Comma-separated_values) file), and
`src/generate_table_data.py` examines the performance of our fill estimation
algorithm on a sparse matrix for default parameter values (storing the output
in [JSON](https://www.json.org/) format for each matrix).
`src/generate_table.py` combines each output of `src/generate_table_data.py`
into a single [csv](https://en.wikipedia.org/wiki/Comma-separated_values)
table. because running all of these python files individually can become
tedious, you can use the `src/generate_run_scripts.py` file to generate sh or
[slurm](https://slurm.schedmd.com/) scripts which call these test harnesses
with the appropriate arguments. These files show up in the `"run_path"`
directory as specified in the experiment parameter file, which defaults to
`run/`

`run/`:
  This is the default directory for the scripts generated by
`src/generate_run_scripts.py` generator. `generate_table.sh` requires you to
run `generate_table_data.sh` first, and both `generate_table_data.sh` and
`generate_plots.sh` can be accelerated by running `generate_spmv_records.sh`,
`generate_references.sh` and `generate_profile.sh` first.

`data/experiment`:
  This is the default directory for the data generated by the test harnesses.

`data/matrix/`:
  The `default` directory few toy matrices to test the code on. The `full`
directory contains scripts to download matrices from the UF Sparse Matrix
Collection and process and convert them to `real` format. We currently only
accept Matrix Market format with `real` data.

  To download the dataset from the paper, you can download and convert it from
source using the `download_dataset.sh` script then processing all the tar files
with `process_inputs.py.`

  To add matrices to the test set for some experiment, add them to a
`registry.json` file as well as the `experiment_*.py` file in `src/` that you
want to run. You will need to regenerate the run scripts after editing the
`registry.json` file, which stores information about the set of matrices
contained in a test set.
