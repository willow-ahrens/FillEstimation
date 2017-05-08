#!/bin/sh

if test x"$2" = x ; then
	echo ""
	echo "usage: $0 <n> <f>"
	echo ""
	echo "Creates the default test script test<n>.sh"
	echo "for test number <n>, echo based on '<f>.c'."
	echo ""
	exit 1
fi

test_num="$1"
test_base="$2"
test_script=test${test_num}.sh

echo "#!/bin/sh

\${top_srcdir}/tests/test.sh ${test_num} ${test_base}

# eof" > ${test_script}
chmod +x ${test_script}

exit 0

# eof
