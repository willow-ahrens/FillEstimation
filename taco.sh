#!/bin/bash
mkdir -p taco
mkdir -p `pwd`/taco/tmp
mkdir -p `pwd`/taco/tmp0
mkdir -p `pwd`/taco/tmp1
mkdir -p `pwd`/taco/tmp2
mkdir -p `pwd`/taco/tmp3
mkdir -p `pwd`/taco/tmp4
mkdir -p `pwd`/taco/tmp5
mkdir -p `pwd`/taco/tmp6
mkdir -p `pwd`/taco/tmp7
mkdir -p `pwd`/taco/tmp8
mkdir -p `pwd`/taco/tmp9
mkdir -p `pwd`/taco/tmp10
mkdir -p `pwd`/taco/tmp11
mkdir -p `pwd`/taco/tmp12
mkdir -p `pwd`/taco/tmp13
mkdir -p `pwd`/taco/tmp14
mkdir -p `pwd`/taco/tmp15
mkdir -p `pwd`/taco/tmp16
mkdir -p `pwd`/taco/tmp17
mkdir -p `pwd`/taco/tmp18
mkdir -p `pwd`/taco/tmp19
mkdir -p `pwd`/taco/tmp20
mkdir -p `pwd`/taco/tmp21
mkdir -p `pwd`/taco/tmp22
mkdir -p `pwd`/taco/tmp23
mkdir -p `pwd`/taco/tmp24
mkdir -p `pwd`/taco/tmp25
mkdir -p `pwd`/taco/tmp26
mkdir -p `pwd`/taco/tmp27
mkdir -p `pwd`/taco/tmp28
mkdir -p `pwd`/taco/tmp29
mkdir -p `pwd`/taco/tmp30
mkdir -p `pwd`/taco/tmp31
mkdir -p `pwd`/taco/tmp32
MYDIR=`pwd`/taco
cd taco-oopsla2017
cmake -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=$MYDIR .
make -j
make install
