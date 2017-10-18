#!/bin/sh

if test x"$1" = x \
    || test x"$2" = x \
    || test x"$3" = x \
    || test x"$4" = x \
    || test x"$5" = x \
; then
    echo \
"
usage: $0 <MATTYPE> <PREC> <OP> <R_LIST> <C_LIST>
"
    exit 1
fi

MATTYPE="$1"
PREC="$2"
OP="$3"
R_LIST="$4"
C_LIST="$5"

KERNEL=MatMult
case "${OP}" in
    [nN]* | [cC]*)
    OP=normal
    KERNEL_OP=MatMult
    ;;
    [tT]* | [hH]*)
    OP=trans
    KERNEL_OP=MatTransMult
    ;;
esac

OUTFILE=./profile_${MATTYPE}_${KERNEL_OP}_${PREC}.dat
if test x"${DISABLE_BENCH}" = xyes ; then
	touch "${OUTFILE}"
	exit 0
fi

rm -f "${OUTFILE}"

env r_list="${R_LIST}" c_list="${C_LIST}" \
    mattype=${MATTYPE} prec=${PREC} op=${OP} \
    xform="return ${MATTYPE}(InputMat, <r>, <c>)" \
	row_multiple="<r>" col_multiple="<c>" \
	testmat="randrect" numcalls=10 \
    outfile=${OUTFILE} \
    ${srcdir}/genprof-loop2d.sh ${srcdir}/genprof-${KERNEL}.sh

# eof
