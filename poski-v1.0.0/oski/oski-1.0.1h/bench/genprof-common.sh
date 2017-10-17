#!/bin/sh
#
# This script detects the following environment variables, setting
# them to the default values shown if they do not exist. These
# variables are interpreted and re-exported as new variables which
# can be passed to the main benchmark program (oskibench_Txy).
#
#    Variable       (Default value)   Re-exported Variable
#    --------       ---------------   --------------------
#    row_multiple   (empty)           ROWMULT
#    col_multiple   (empty)           COLMULT
#    min_nnz        7000000           MINNNZ
#    testmat        dense             TESTMAT
#    numcalls       1                 NUMCALLS
#

if test -n "${row_multiple}" ; then
    ROWMULT="--row-multiple ${row_multiple}"
else
    ROWMULT=''
fi
export ROWMULT

if test -n "${col_multiple}" ; then
    COLMULT="--col-multiple ${col_multiple}"
else
    COLMULT=''
fi
export COLMULT

MINNNZ=${min_nnz-6000000}
export MINNNZ

case "${testmat}" in
"")
	TESTMAT="dense --min-nnz ${MINNNZ}"
	;;
symm*)
	TESTMAT="symm --min-nnz ${MINNNZ}"
	;;
*)
	TESTMAT="${testmat}"
	;;
esac
export TESTMAT

if test -n "${numcalls}" ; then
    NUMCALLS="${numcalls}"
else
    NUMCALLS=1
fi
export NUMCALLS

# eof
