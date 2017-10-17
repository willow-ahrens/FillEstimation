#!/bin/sh

. ${top_srcdir}/tests/defs.sh

TESTNUM="$1"
TESTBASE="$2"
TESTPROG="./test${TESTNUM}-${TESTBASE}"

echo "*** Checking ${TESTPROG} ***"

if ${top_srcdir}/tests/skip_test.sh "${TESTNUM}" ; then
	exit 0
fi

ERRLOG="$1.err"
OUTLOG="$1.out"

if "${TESTPROG}" 2> "${ERRLOG}" > "${OUTLOG}" ; then
	exit 0
else
	exit 1
fi

# eof
