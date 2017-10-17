#!/bin/sh

if test x"$1" = x || test x"$2" = x || test x"$3" = x || test x"$4" = x ; then
    echo \
"
usage: $0 <MATTYPE> <SHAPE> <PREC> <OP> [<R_LIST> <C_LIST>]
"
    exit 1
fi

MATTYPE="$1"
SHAPE="$2"
PREC="$3"
OP="$4"

if test -n "$5" && test -n "$6" ; then
    LOOP2D=yes
    R_LIST="$5"
    C_LIST="$6"
else
    LOOP2D=no
fi

KERNEL=MatTrisolve
case "${OP}" in
    [nN]* | [cC]*)
    OP=normal
    KERNEL_OP=MatTrisolve
    ;;
    [tT]* | [hH]*)
    OP=trans
    KERNEL_OP=MatTransTrisolve
    ;;
esac

case "${SHAPE}" in
    [lL]*) SHAPE=Lower ;;
    [uU]*) SHAPE=Upper ;;
    *) echo "*** Invalid shape, '${SHAPE}' ***" ; exit 1 ;;
esac

OUTFILE=./profile_${MATTYPE}_${SHAPE}${KERNEL_OP}_${PREC}.dat

if test x"${DISABLE_BENCH}" = xyes ; then
	touch "${OUTFILE}"
	exit 0
fi

rm -f "${OUTFILE}"

if test x"${LOOP2D}" = xyes ; then
    env r_list="${R_LIST}" c_list="${C_LIST}" \
	mattype=${MATTYPE} shape=${SHAPE} prec=${PREC} op=${OP} \
	xform="return ${MATTYPE}(InputMat, <r>, <c>)" \
	row_multiple="<r>" col_multiple="<c>" \
	outfile=${OUTFILE} \
	${srcdir}/genprof-loop2d.sh ${srcdir}/genprof-${KERNEL}.sh
else
    env mattype=${MATTYPE} shape=${SHAPE} prec=${PREC} op=${OP} \
	xform="return ${MATTYPE}(InputMat)" \
	row_multiple=1 col_multiple=1 \
	outfile=${OUTFILE} \
	${srcdir}/genprof-${KERNEL}.sh
fi

# eof
