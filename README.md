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

