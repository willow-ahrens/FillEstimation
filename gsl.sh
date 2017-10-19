#!/bin/bash
mkdir -p gsl
MYDIR=`pwd`/gsl
cd gsl-2.4
./configure --prefix=$MYDIR
make -j
make install
