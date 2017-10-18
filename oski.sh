#!/bin/bash
#use GNU prgenv
#use automake and autoconf modules
export CFLAGS="-O3"
mkdir oski_build
mkdir oski_out
cd oski-1.0.1h
#autoreconf
#automake --add-missing
cd ..
cd oski_build
../oski-1.0.1h/configure --prefix=$HOME/FillEstimation/oski_out

