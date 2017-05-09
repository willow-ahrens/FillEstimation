#!/bin/sh

kernel=MatMultAndMatTransMult

if test x"${mattype}" = x \
    || test x"${prec}" = x \
    || test x"${op}" = x \
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
     ${kernel} --num-calls ${NUMCALLS} --op ${op} --num-vecs 1 \
               --x-layout col --y-layout col --alpha 1 --beta 0 \
               --w-layout col --z-layout col --omega 1 --zeta 0` \
    % `echo "${xform}"` \
    | tee -a "${outfile}"

. ${srcdir}/genprof-end.sh

# eof
