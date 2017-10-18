#!/bin/bash
mkdir -p taco
MYDIR=`pwd`/taco
cd taco-oopsla2017
cmake -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_DIR=$MYDIR .
make
make install
