#! /bin/bash
##
# This file builds pOSKI.
##

## pOSKI default prefix
poskiprefix=/usr/local/poski
poskibuild=`pwd`
OSKIDIR=$OSKIDIR
if [ x"${OSKIBUILD}" == x"" ]; then
	OSKIBUILD=${OSKIDIR}
else
	OSKIBUILD=${OSKIBUILD}
fi
echo "OSKIBUILD = ${OSKIBUILD}"
echo "OSKIDIR = ${OSKIDIR}"

## Grap pOSKI source directory.
poskipath=`echo $0 | sed -e "s/INSTALL.sh//"`

## Grap argument.
oskiflag=0
poskicoptions=
benchmarkflag=0
checkflag=0
numaflag=0

report_usage="\
pOSKI Usage: ./INSTALL.sh [options...] 

+ Configuration:
	--help			display this help and exit
	--version		display version information and exit

+ Installation:
	--prefix=PREFIX		install architecture-independent files in PREFIX
				[default]=/usr/local/poski
	--oskidir={path_to_OSKI_install_directory}
				oski install directory
	--oskibld={path_to_OSKI_build_directory}	
				oski build directory
	--with-oski=<opt>	build oski; <opt> in {yes,no}
				[default]=no
	--with-tune=<opt>	off-line tuning; <opt> in {yes,no}
				[default]=no
	--with-check=<opt>	check; <opt> in {yes,no}
				[default]=no
	--with-numa=<opt>	numa; <opt> in {yes,no}
				[default]=no

+ Compilation:
	--cc=<CC>		C compiler command; <CC> in {icc, gcc, cc}
	--cflag=<CFLAG>		C compiler flags		
"

# FIXME: more efficient way. 
for arg in "$@"
do 

flag=`echo "$arg" | sed -e "s/--oskidir=//"`
if [ "$flag" != "$arg" ]; then
	OSKIDIR="$flag"
	if [ x"$OSKIBUILD" == x"" ]; then
		OSKIBUILD=${OSKIDIR}
	fi
fi

flag=`echo "$arg" | sed -e "s/--oskibld=//"`
if [ "$flag" != "$arg" ]; then
	OSKIBUILD="$flag"
fi


flag=`echo "$arg" | sed -e "s/--help//"`
if [ "$flag" != "$arg" ]; then
	echo "$report_usage"
	exit
fi

flag=`echo "$arg" | sed -e "s/--version//"`
if [ "$flag" != "$arg" ]; then
	echo "pOSKI-v1.0.0"
	exit
fi

flag=`echo "$arg" | sed -e "s/--prefix=//"`
if [ "$flag" != "$arg" ]; then
	if [ "$flag" != "" ]; then
		poskiprefix="$flag"
	fi
fi

flag=`echo "$arg" | sed -e "s/--with-oski=//"`
if [ "$flag" != "$arg" ]; then
	if [ "$flag" == "yes" -o "$flag" == "Yes" -o "$flag" == "YES" ]; then
		oskiflag=1
	fi
fi

flag=`echo "$arg" | sed -e "s/--with-tune=//"`
if [ "$flag" != "$arg" ]; then
	if [ "$flag" == "yes" -o "$flag" == "Yes" -o "$flag" == "YES" ]; then
		tuneflag=1
	fi
fi

flag=`echo "$arg" | sed -e "s/--with-check=//"`
if [ "$flag" != "$arg" ]; then
	if [ "$flag" == "yes" -o "$flag" == "Yes" -o "$flag" == "YES" ]; then
		checkflag=1
	fi
fi

flag=`echo "$arg" | sed -e "s/--with-numa=//"`
if [ "$flag" != "$arg" ]; then
	if [ "$flag" == "yes" -o "$flag" == "Yes" -o "$flag" == "YES" ]; then
		numaflag=1
	fi
fi

flag=`echo "$arg" | sed -e "s/--cc=//"`
if [ "$flag" != "$arg" ]; then
	if [ "$flag" != "" ]; then
		poskicoptions="${poskicoptions} --CC=$flag"
	fi
fi

flag=`echo "$arg" | sed -e "s/--cflags=//"`
if [ "$flag" != "$arg" ]; then
	if [ "$flag" == "" ]; then
		poskicoptions="${poskicoptions} --CFLAGS=$flag"
	fi
fi

done

if [ -f Makefile ]; then
echo "
[Uninstalling prior pOSKI library...]"
time make uninstall
fi

if [ x"$oskiflag" == x"1" ]; then
## Install OSKI
echo "
[Installing OSKI library...]"
OSKIDIR="${poskiprefix}/build_oski"
OSKIBUILD="${poskibuild}/build_oski"
poskicoptions="${poskicoptions} --with-oski=yes"
	if [ x"$tuneflag" == x"1" ]; then
		time ${poskipath}/oski/install_oski.sh ${poskicoptions} --prefix=${OSKIDIR}
	else
		time ${poskipath}/oski/install_full_oski.sh ${poskicoptions} --prefix=${OSKIDIR}
	fi
fi
echo "OSKIBUILD = $OSKIBUILD"
echo "OSKIDIR = $OSKIDIR"

## Run Configure script for your platform.
echo "
[Runing pOSKI configuration...]"
if [ x"$numaflag" == x"1" ]; then
	enablenuma="--with-numa=yes"
fi

time ${poskipath}/configure ${poskicoptions} --prefix=${poskiprefix} $enablenuma
. ${poskibuild}/poski-var.sh

## Compile source codes.
echo "
[Compling source codes...]"
time make 

## Run "off-line tuning".
if [ x"$tuneflag" == x"1" ]; then
echo "
[Runing off-line tuning...]"
echo "OSKIBUILD = $OSKIBUILD"
echo "OSKIDIR = $OSKIDIR"
time make benchmarks
else
echo "
[Skip off-line tuning...]"
fi

## Run your build of pOSKI through an extensive series of tests.
if [ x"$checkflag" == x"1" ]; then
echo "
[Checking pOSKI...]"
time make check
else
echo "
[Skip checking...]"
fi

## Install all of the pOSKI files and  benchmarking data.
echo "
[Installing pOSKI library...]"
time make install

echo "
All processing is done!"





