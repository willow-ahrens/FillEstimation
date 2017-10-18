#!/bin/sh

. ${top_srcdir}/tests/defs.sh

inmat=${top_srcdir}/tests/raefsky6.pua
NUM=`basename $0 | sed 's/[^0-9]//g'`
freq=1000000 # Some large value...

TESTPROG=./test${NUM}-heur2

echo "*** Checking ${TESTPROG} using a custom script ***"

if ${top_srcdir}/tests/skip_test.sh "${NUM}" ; then
	exit 0
fi

if ${TESTPROG} ${inmat} ${freq} ; then
	:
else
	echo "*** $0 ***" 2>&1 > ${NUM}.err
	exit 1
fi

exit 0

# eof
