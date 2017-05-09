#!/bin/sh

. ${top_srcdir}/tests/defs.sh

num=`basename "$0" | sed 's,[^0-9],,g'`
echo "*** Checking ./test${num}-debug using a custom script ***"

# Should print nothing on stderr, and messages 1..3 on stdout.
./test${num}-debug 3 0 2> ${num}.err > ${num}.out

# Check stderr output
cat <<\EOF >${num}.err-ans
EOF

if cmp -s ${num}.err ${num}.err-ans ; then
	:
else
	echo "*** Wrong output on standard error! ***"
	cat ${num}.err
	exit 1
fi

# Check stdout output
if test -z "`cat ${num}.out`" ; then
	echo "*** Did not generate any output on stdout! ***"
	exit 1
fi

cat <<\EOF >${num}.out-ans
!   A debug message on stdout: 1
!      A debug message on stdout: 2
!         A debug message on stdout: 3
EOF

if cmp -s ${num}.out ${num}.out-ans ; then
	:
else
	echo "*** Wrong output on standard out! ***"
	cat ${num}.out
	exit 1
fi

exit 0

# eof
