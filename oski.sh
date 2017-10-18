#!/bin/bash
#use GNU prgenv
export CFLAGS="-O3"
mkdir oski_build
mkdir oski_out
cd oski-1.0.1h
aclocal
autoconf
autoheader
automake --add-missing
cd ..
cd oski_build
../oski-1.0.1h/configure --prefix=$HOME/FillEstimation/oski_out

