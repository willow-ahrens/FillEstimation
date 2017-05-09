#!/bin/sh

kernel=MatPowMult

if test x"${mattype}" = x \
    || test x"${prec}" = x \
    || test x"${op}" = x \
    || test x"${power}" = x \
    || test x"${xform}" = x \
    || test x"${outfile}" = x \
    ; then
    echo \
"
usage: $0

Benchmarks non-symmetric sparse matrix-vector multiply.
The following environment variables must be set upon
executing this script:

   mattype
   prec
   op
   power  (an integer >= 1)
   xform
   outfile

The following are optional environmental variables:
   row_multiple
   col_multiple
   min_nnz   (Default: 7000000)
   testmat   (Default: dense)
   numcalls  (Default: 25)
"
exit 1
fi

. ${srcdir}/genprof-begin.sh
. ${srcdir}/genprof-common.sh

echo ${row_multiple} ${col_multiple} 1 \
    `env ${ENV_VARS} ./oskibench_${prec} \
       --matrix ${TESTMAT} ${ROWMULT} ${COLMULT} \
       "${xform}" \
       ${kernel} --num-calls ${NUMCALLS} --op ${op} --power ${power} \
                 --num-vecs 1 \
                 --x-layout col --y-layout col --t-layout col \
                 --alpha 1 --beta 0` \
    % `echo "${xform}"` \
    | tee -a "${outfile}"

. ${srcdir}/genprof-end.sh

# eof
