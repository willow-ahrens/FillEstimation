#!/bin/bash
#use GNU prgenv
#use automake and autoconf modules
export CFLAGS="-O3"
mkdir oski_build
mkdir oski_out
cd oski-1.0.1h
autoheader
aclocal
libtoolize --ltdl --copy --force
automake --add-missing --copy
autoconf 
cd ..
cd oski_build
../oski-1.0.1h/configure --prefix=$HOME/FillEstimation/oski_out

