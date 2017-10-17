/**
 *  \file src/DENSE/trisolve.c
 *  \ingroup MATTYPE_DENSE
 *  \brief Calls xTRSV / xTRSM.
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
oski_MatReprTrisolve (const void *pT, const oski_matcommon_t * props,
		      oski_matop_t opT, oski_value_t alpha,
		      oski_vecview_t x_view)
{
  const oski_matDENSE_t *T = (const oski_matDENSE_t *) pT;
  oski_index_t m, n, k;
  char op, shape, diag;

  assert (T != NULL);
  assert (props != NULL);

  if (IS_VAL_ZERO (alpha))
    {
      oski_ZeroVecView (x_view);
      return 0;
    }
  else if (!IS_VAL_ONE (alpha))
    oski_ScaleVecView (x_view, alpha);

  m = props->num_rows;
  n = props->num_cols;
  assert (n == m);
  k = x_view->num_cols;

  switch (opT)
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
      assert (0);	/** \todo Could theoretically encounter OP_CONJ */
    }

  if (props->pattern.is_tri_upper)
    shape = 'U';
  else if (props->pattern.is_tri_lower)
    shape = 'L';
  else
    return ERR_BAD_MAT;

  diag = 'N';

  /** \todo Call xTRSM in multiple vector case */
  {
    oski_index_t k0;
    for (k0 = 0; k0 < k; k0++)
      {
	oski_value_t *xp = x_view->val + k0 * x_view->colinc;

	BLAS_xTRSV (&shape, &op, &diag, &m, T->val, &(T->lead_dim),
		    xp, &(x_view->rowinc));
      }
  }
  return 0;
}

/* eof */
