#!/bin/sh

if test x"$1" = x ; then
	echo "
usage: $0 <test_num>

This script checks to see if <test_num> is listed in the
environment variable, DISABLE_TESTS. Exits with code 0
if so, or 1 otherwise.

This script will also exit immediately with code 0 if
DISABLE_TESTS is set to 'all' or 'yes', and 0 if it is
set to 'none' or 'no'.
"
	exit 0
fi

TESTNUM=$1

case "${DISABLE_TESTS}" in
all | yes) echo "SKIPPING Test #${TESTNUM} ..." ; exit 0 ;;
none | no) exit 1 ;;
*) ;;
esac

if test x"${DISABLE_TESTS}" != x ; then
	for skip_num in ${DISABLE_TESTS} ; do
		if test x"${skip_num}" = x"${TESTNUM}" ; then
			echo "SKIPPING Test #${TESTNUM} ..."
			exit 0
		fi
	done
fi

exit 1

# eof
