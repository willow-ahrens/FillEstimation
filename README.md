Tensor Fill Estimation: a comparison
======================

Organization
--------

`deps/`:
  Our implementation (and associated analysis) has some software dependencies.
To make our results more replicable, we have included the exact versions of
this software that we have used in the deps/ folder, and linked them into our
build system. Each subdirectory of deps/ contains a different software
dependency and associated Makefile. If you would like to provide custom build
instructions for some dependency $DEP, create and modify a copy of
deps/$DEP/Makefile.Default named deps/$DEP/Makefile.$ARCH where $ARCH is the
name of your current architecture (The default value of $ARCH can be queried by
typing make arch at the top level directory of this project) or a custom name
you have given your machine (you can specify the value of the $ARCH variable
by invoking make as `make ARCH=$ARCH`). The deps make target at the top level
directory of our project will build our software dependencies.

`src/`:
  The src directory contains the implementations of our algorithms and the
pipelines we use to generate results.
  Our [Phil]() algorithm is implemented in `src/phil.c` and built into
the executable `phil`. A different algorithm (described in the file
`oski-1.0.1h/src/heur/estfill.c` in the [OSKI]() library) is implemented in
`src/oski.c` and built into the executable `oski`. A reference algorithm is
implemented in `reference.c` and built into the executable `reference`. An
implementation of sparse (possibly blocked) matrix-vector multiply is provided
by the [TACO]() library in `spmv.cpp` and built into the executable `spmv`. All
executables take a `-h` option describing their usage, and provide their
output in [JSON]() format.

 Some of the scripts used to run our code were meant to be run using [SLURM].
Since the user likely does not have slurm installed, we also provide a
python script called slurm.py which can execute .sl scripts. Is this the right
approach?

Compilation
--------

Compilation is done from within the src/ directory. Compiler: gcc-7. Just run
```
$ make
```
to get the executables. Inputs are provided in matrix/. To run on a particular matrix, use
```
$ ./asx matrix/[name]
```

Results
--------

To run the test harness, make sure you have python installed. To do a comparison, run

```
$ python benchmark.py
```

For plots, run
```
$ python plots.py
```

How to generate plots
--------

First, generate the performance matrix local to your machine.

```
$ python performance-matrix.py [output]
```
where [output] is the name of the output file to put it in.

To do SpMV tests, do
```
$ python spmv-plot.py [output].npy
```
passing output file 

Testing on a fresh machine
--------
Install conda -> numpy and scipy (default with mkl)

make sure you have gcc7
```
sudo add-apt-repository ppa:jonathonf/gcc-7.1
sudo apt-get update
sudo apt-get install gcc-7 g++-7
```
and pkg-config + glib + libgsl-dev (rng)
```
sudo apt-get install pkg-config libglib2.0-dev libgsl-dev libgsl2
```


