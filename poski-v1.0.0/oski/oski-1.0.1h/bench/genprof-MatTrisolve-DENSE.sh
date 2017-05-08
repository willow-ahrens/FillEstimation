#!/bin/sh

if test x"$1" = x \
    || test x"$2" = x \
    || test x"$3" = x \
    || test x"$4" = x \
    || test x"$5" = x \
; then
    echo \
"
usage: $0 <MATTYPE> <SHAPE> <PREC> <OP> <N_LIST>
"
    exit 1
fi

MATTYPE="$1"
SHAPE="$2"
PREC="$3"
OP="$4"
N_LIST="$5"

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

for n_test in ${N_LIST} ; do
    rm -f tmp-${n_test}
    env mattype=${MATTYPE} shape=${SHAPE} prec=${PREC} op=${OP} \
	xform="return ${MATTYPE}(InputMat)" \
	min_nnz=`expr ${n_test} \* ${n_test}` \
	outfile=tmp-${n_test} \
	${srcdir}/genprof-${KERNEL}.sh
    echo ${n_test} `cat tmp-${n_test}` >> "${OUTFILE}"
    rm -f tmp-${n_test}
done

# eof
