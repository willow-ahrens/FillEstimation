#! /bin/bash
##
# This file installs OSKI.
##

## Grap OSKI source directory.
echo "[Checking OSKI source directory...]"
if [ "$poskipath" != "" ]; then
	oskipath=$poskipath/oski
else
	oskipath=`echo $0 | sed -e "s/install_oski.sh//"`
fi
if [ ! -d "$oskipath" ]; then
	echo "Error: can't find $oskipath!"
	exit
else
	cd $oskipath
	oskipath=`pwd`
	cd -
fi

## Create OSKI build directory
echo "[Creating OSKI build directory...]"
oskibuild=build_oski
if [ "$poskiblddir" != "" ]; then
	cd $poskiblddir
	if [ ! -d "$oskibuild" ]; then
		mkdir $oskibuild
	fi
	cd $oskibuild
	oskiblddir=`pwd`
	cd -
else
	if [ ! -d "$oskibuild" ]; then
		mkdir $oskibuild
	fi
	oskiblddir=`pwd`/$oskibuild
fi

if [ ! -d "$oskiblddir" ]; then
	echo "Error: can't find $oskibuild!"
	exit
fi

## Grab arguments.
echo "[Checking configuration arguments...]"
oskiprefix=$oskiblddir
for arg in "$@"
do
	flag=`echo "$arg" | sed -e "s/--prefix=//"`
	if [ "$flag" != "$arg" ]; then
		oskiprefix="$flag"
	fi

	flag=`echo "$arg" | sed -e "s/--cc=//"`
	if [ "$flag" != "$arg" ]; then
		oskicc="$flag"
	fi
done

## Build OSKI
echo "[Building OSKI library...]"
cd $oskiblddir
if [ "$oskicc" != "" ]; then
	time env CC=$oskicc $oskipath/oski-1.0.1h/configure --prefix=$oskiprefix --with-papi=no
else
	time $oskipath/oski-1.0.1h/configure --prefix=$oskiprefix --with-papi=no
fi
echo "OSKI is built in $oskiprefix"

## Compile OSKI
echo "[Compiling OSKI source codes...]"
time make 

## Run OSKI benchmarks 
#echo "Running OSKI benchmarks..."
#make benchmarks

## Check OSKI
#echo "Runing OSKI check..."
#make check

## Install OSKI
echo "[Installing OSKI library...]"
time make install
cd -


