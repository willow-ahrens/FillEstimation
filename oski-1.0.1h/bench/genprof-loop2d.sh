#!/bin/sh
#
# Executes a two dimensional loop around a core benchmark script.
#

if test x"${r_list}" = x \
    || test x"${c_list}" = x \
    || test x"${mattype}" = x \
    || test x"${xform}" = x \
    || test x"$1" = x \
; then
    echo \
"
usage: $0 <script> [script-args ...]

Executes a two-dimensional loop around a core benchmark script.
The caller must set the following environment variables:

  mattype
    Matrix type (e.g., BCSR, MBCSR, CB, ...)

  xform
    Parameterized transformation in which the substring
    <r>, <c> will be replaced by the current iteration
    variable.

  r_list
    Specifies the 'row' iteration space.

  c_list
    Specifies the 'column' iteration space.
"
    exit 1
fi

for r in ${r_list} ; do
    for c in ${c_list} ; do
	XFORM="`echo \"${xform}\" | sed \"s,<r>,${r},g\" | sed \"s,<c>,${c},g\"`"
	if test x"${row_multiple}" != x ; then
	    ROWMULT="row_multiple=`echo ${row_multiple} | sed \"s,<r>,${r},g\"`"
	fi
	if test x"${col_multiple}" != x ; then
	    COLMULT="col_multiple=`echo ${col_multiple} | sed \"s,<c>,${c},g\"`"
	fi

	env xform="${XFORM}" ${ROWMULT} ${COLMULT} "$@"
    done
done

# eof
