Tensor Fill Estimation: a comparison
======================

Organization
--------

deps/:
  Our implementation (and associated analysis) has some software dependencies.
To make our results more replicable, we have included the exact versions of
this software that we have used in the deps/ folder, and linked them into our
build system. Each subdirectory of deps/ contains a different software
dependency and associated Makefile. If you would like to provide custom build
instructions for some dependency $DEP, create and modify a copy of
deps/$DEP/Makefile.default named deps/$DEP/Makefile.$ARCH where $ARCH is the
name of your current architecture (The default value of $ARCH can be queried by
typing make arch at the top level directory of this project) or a custom name
you have given your machine (you can specify the value of the $ARCH variable
by invoking make as `make ARCH=$ARCH`).

Code, testing framework, and data generation can all be found in src/

oski-1.0.1h is the entire sparse kernel library. The tensor fill estimation algorithm is from oski-1.0.1h/src/heur/estfill.c.

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


