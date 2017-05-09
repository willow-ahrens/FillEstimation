/**
 *  \file src/DENSE/matmult.c
 *  \ingroup MATTYPE_DENSE
 *  \brief Calls xGEMV / xGEMM.
 */

#include <assert.h>
#include <oski/config.h>	/* for 'restrict' keyword */
#include <oski/common.h>
#include <oski/blas.h>
#include <oski/blas_names.h>
#include <oski/matmult.h>
#include <oski/DENSE/format.h>
#include <oski/DENSE/module.h>

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
  const oski_matDENSE_t *A = (const oski_matDENSE_t *) pA;
  oski_index_t m, n, k;
  char op;

  assert (A != NULL);
  assert (props != NULL);

  oski_ScaleVecView (y_view, beta);
  if (IS_VAL_ZERO (alpha))
    return 0;

  m = props->num_rows;
  n = props->num_cols;
  k = x_view->num_cols;
  assert (k == y_view->num_cols);

  switch (opA)
    {
    case OP_NORMAL:
      op = 'N';
      break;
    case OP_TRANS:
      op = 'T';
      break;
    case OP_CONJ_TRANS:
      op = 'C';
      break;
    default:
      assert (0);     /** \todo Could theoretically encounter OP_CONJ */
    }

  /** \todo Call xGEMM in multiple vector case */
  {
    oski_index_t k0;
    for (k0 = 0; k0 < k; k0++)
      {
	const oski_value_t *xp = x_view->val + k0 * x_view->colinc;
	oski_value_t *yp = y_view->val + k0 * y_view->colinc;

	BLAS_xGEMV (&op, &m, &n, &alpha, A->val, &(A->lead_dim),
		    xp, &(x_view->rowinc), &TVAL_ZERO, yp, &(y_view->rowinc));
      }
  }
  return 0;
}

/* eof */
