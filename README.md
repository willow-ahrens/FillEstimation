Tensor Fill Estimation: a comparison
======================

Organization
--------

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


