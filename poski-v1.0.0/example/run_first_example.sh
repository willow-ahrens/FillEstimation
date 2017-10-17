#! /bin/bash

PROGRAM=first_example-shared

if [ -f $PROGRAM ]; then
make clean
fi

make

./$PROGRAM

