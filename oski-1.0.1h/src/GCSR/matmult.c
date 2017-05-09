/**
 *  \file src/GCSR/matmult.c
 *  \ingroup MATTYPE_GCSR
 *  \brief Sparse matrix-vector multiply implementation for a
 *  compressed sparse row (GCSR) matrix.
 */

#include <assert.h>
#include <oski/config.h>	/* for 'restrict' keyword */
#include <oski/common.h>
#include <oski/blas.h>
#include <oski/matmult.h>
#include <oski/GCSR/format.h>
#include <oski/GCSR/module.h>

/**
 *  \brief
 *
 *  \pre All arguments have legal values and meet basic dimensionality
 *  compatibility requirements.
 */
int
oski_MatReprMult (const void *pA, const oski_matcommon_t * props,
		  oski_matop_t opA,
		  oski_value_t alpha, const oski_vecview_t x_view,
		  oski_value_t beta, oski_vecview_t y_view)
{
  const oski_matGCSR_t *A = (const oski_matGCSR_t *) pA;

  assert (A != NULL);
  assert (props != NULL);

  oski_ScaleVecView (y_view, beta);
  if (IS_VAL_ZERO (alpha))
    return 0;
  else
    return GCSR_MatReprMult_1x1 (A, opA, alpha, x_view, y_view);
}

/* eof */
