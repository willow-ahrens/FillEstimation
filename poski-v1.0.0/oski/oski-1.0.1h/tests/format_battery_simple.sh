#!/bin/sh
#
# General wrapper script that runs one of the hb<KERNEL>
# programs through a test-battery of different matrix
# formats.
#

if test x"$2" = x ; then
	echo ""
	echo "usage: $0 <test_num> <kernel>"
	echo ""
	exit 1
fi

NUM=${1}
TESTPROG=./test${NUM}-${2}

echo "*** Checking ${TESTPROG} using a custom script ***"

. ${top_srcdir}/tests/defs.sh
if ${top_srcdir}/tests/skip_test.sh "${NUM}" ; then
	exit 0
fi

if ${TESTPROG} "return GCSR(InputMat)"
then
	:
else
	echo "*** $0 GCSR failed ***" 2>&1 > ${NUM}.err
	exit 1
fi

if ${TESTPROG} "return DENSE(InputMat)"
then
	:
else
	echo "*** $0 DENSE failed ***" 2>&1 > ${NUM}.err
	exit 1
fi

for R in 5 10 ; do
	for C in 5 10 ; do
		if ${TESTPROG} "return CB(InputMat, ${R}, ${C})"
		then
			:
		else
			echo "*** $0 CB :: ${R} x ${C} failed ***" 2>&1 > ${NUM}.err
			exit 1
		fi
	done
done

for r in 1 2 3 4 5 6 7 8 ; do
	for c in 1 2 3 4 5 6 7 8 ; do

		if ${TESTPROG} "return BCSR(InputMat, ${r}, ${c})"
		then
			:
		else
			echo "*** $0 BCSR :: ${r} x ${c} failed ***" 2>&1 > ${NUM}.err
			exit 1
		fi

		if ${TESTPROG} "return MBCSR(InputMat, ${r}, ${c})"
		then
			:
		else
			echo "*** $0 MBCSR :: ${r} x ${c} failed ***" 2>&1 > ${NUM}.err
			exit 1
		fi

	done
done

exit 0

# eof
