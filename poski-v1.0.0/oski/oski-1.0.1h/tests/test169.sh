#!/bin/sh

. ${top_srcdir}/tests/defs.sh

NUM=`basename $0 | sed 's/[^0-9]//g'`

TESTPROG=./test${NUM}-trisolve_xform

echo "*** Checking ${TESTPROG} using a custom script ***"

if ${top_srcdir}/tests/skip_test.sh "${NUM}" ; then
	exit 0
fi

for r in 1 2 3 4 5 6 7 8 ; do
	for c in 1 2 3 4 5 6 7 8 ; do

		if ${TESTPROG} ${inmat} "return MBCSR(InputMat, ${r}, ${c})"
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
