#!/bin/sh

if test x"$1" = x || test x"$2" = x || test x"$3" = x ; then
    echo \
"
usage: $0 <MATTYPE> <PREC> <OP> [<R_LIST> <C_LIST>]
"
    exit 1
fi

MATTYPE="$1"
PREC="$2"
OP="$3"

if test -n "$4" && test -n "$5" ; then
    LOOP2D=yes
    R_LIST="$4"
    C_LIST="$5"
else
    LOOP2D=no
fi

KERNEL=MatTransMatMult
case "${OP}" in
    [aA][tT][aA] | [aA][hH][aA])
    OP=ata
    KERNEL_OP=MatTransMatMult
    ;;
    [aA][aA][tT] | [aA][aA][hH])
    OP=aat
    KERNEL_OP=MatMatTransMult
    ;;
esac

OUTFILE=./profile_${MATTYPE}_${KERNEL_OP}_${PREC}.dat

if test x"${DISABLE_BENCH}" = xyes ; then
	touch "${OUTFILE}"
	exit 0
fi

rm -f "${OUTFILE}"

if test x"${LOOP2D}" = xyes ; then
    env r_list="${R_LIST}" c_list="${C_LIST}" \
	mattype=${MATTYPE} prec=${PREC} op=${OP} \
	xform="return ${MATTYPE}(InputMat, <r>, <c>)" \
	row_multiple="<r>" col_multiple="<c>" \
	outfile=${OUTFILE} \
	${srcdir}/genprof-loop2d.sh ${srcdir}/genprof-${KERNEL}.sh
else
    env mattype=${MATTYPE} prec=${PREC} op=${OP} \
	xform="return ${MATTYPE}(InputMat)" \
	row_multiple=1 col_multiple=1 \
	outfile=${OUTFILE} \
	${srcdir}/genprof-${KERNEL}.sh
fi

# eof
