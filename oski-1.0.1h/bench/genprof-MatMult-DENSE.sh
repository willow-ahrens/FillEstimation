#!/bin/sh

if test x"$1" = x \
    || test x"$2" = x \
    || test x"$3" = x \
    || test x"$4" = x \
; then
    echo \
"
usage: $0 <MATTYPE> <PREC> <OP> <N_LIST>
"
    exit 1
fi

MATTYPE="$1"
PREC="$2"
OP="$3"
N_LIST="$4"

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

for n_test in ${N_LIST} ; do
    rm -f tmp-${n_test}
    env min_nnz=`expr ${n_test} \* ${n_test}` \
	mattype=${MATTYPE} prec=${PREC} op=${OP} \
	xform="return ${MATTYPE}(InputMat)" numcalls=10 \
	outfile=tmp-${n_test} \
	${srcdir}/genprof-${KERNEL}.sh
    echo ${n_test} `cat tmp-${n_test}` >> "${OUTFILE}"
    rm -f tmp-${n_test}
done

# eof
