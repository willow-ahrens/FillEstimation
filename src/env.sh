#!/bin/bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$1/gsl/lib:$1/taco/lib
export TACO_TMPDIR=$1/taco/tmp
