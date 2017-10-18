/**
 *  \file src/tconst.c
 *  \brief Typed constant declarations.
 */

#include <oski/common.h>
#include <oski/mangle.h>

const oski_value_t TVAL_ZERO = MAKE_VAL_COMPLEX (VAL_ZERO, VAL_ZERO);
const oski_value_t TVAL_ONE = MAKE_VAL_COMPLEX (VAL_ONE, VAL_ZERO);
const oski_value_t TVAL_NEG_ONE = MAKE_VAL_COMPLEX (VAL_NEG_ONE, VAL_ZERO);
const oski_value_t TVAL_IMAG = MAKE_VAL_COMPLEX (VAL_ZERO, VAL_ONE);
const oski_value_t TVAL_NEG_IMAG = MAKE_VAL_COMPLEX (VAL_ZERO, VAL_NEG_ONE);

/* eof */
