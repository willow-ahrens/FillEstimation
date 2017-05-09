/**
 *  \file src/CSR/mattrisolve.c
 *  \ingroup MATTYPE_CSR
 *  \brief Sparse triangular solve implementation for a compressed
 *  sparse row (CSR) matrix.
 */

#include <assert.h>
#include <oski/config.h>	/* for 'restrict' keyword */
#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/trisolve.h>
#include <oski/CSR/format.h>
#include <oski/CSR/module.h>

static int
Normal (const oski_matCSR_t * T, const oski_matcommon_t * props,
	oski_value_t alpha, oski_vecview_t x_view)
{
  if (props->pattern.is_tri_lower)
    {
      CSR_MatTrisolveLower (props->num_rows, T->base_index,
			    T->has_unit_diag_implicit, T->has_sorted_indices,
			    T->ptr, T->ind, T->val,
			    alpha, x_view->val, x_view->num_cols,
			    x_view->rowinc, x_view->colinc);
    }
  else
    {
      assert (props->pattern.is_tri_upper);
      CSR_MatTrisolveUpper (props->num_rows, T->base_index,
			    T->has_unit_diag_implicit, T->has_sorted_indices,
			    T->ptr, T->ind, T->val,
			    alpha, x_view->val, x_view->num_cols,
			    x_view->rowinc, x_view->colinc);
    }
  return 0;
}

static int
Trans (const oski_matCSR_t * T, const oski_matcommon_t * props,
       oski_value_t alpha, oski_vecview_t x_view)
{
  if (props->pattern.is_tri_lower)
    {
      CSR_MatTransTrisolveLower (props->num_rows, T->base_index,
				 T->has_unit_diag_implicit,
				 T->has_sorted_indices, T->ptr, T->ind,
				 T->val, alpha, x_view->val, x_view->num_cols,
				 x_view->rowinc, x_view->colinc);
    }
  else
    {
      assert (props->pattern.is_tri_upper);
      CSR_MatTransTrisolveUpper (props->num_rows, T->base_index,
				 T->has_unit_diag_implicit,
				 T->has_sorted_indices, T->ptr, T->ind,
				 T->val, alpha, x_view->val, x_view->num_cols,
				 x_view->rowinc, x_view->colinc);
    }
  return 0;
}

static int
Conj (const oski_matCSR_t * T, const oski_matcommon_t * props,
      oski_value_t alpha, oski_vecview_t x_view)
{
  if (props->pattern.is_tri_lower)
    {
      CSR_MatConjTrisolveLower (props->num_rows, T->base_index,
				T->has_unit_diag_implicit,
				T->has_sorted_indices, T->ptr, T->ind, T->val,
				alpha, x_view->val, x_view->num_cols,
				x_view->rowinc, x_view->colinc);
    }
  else
    {
      assert (props->pattern.is_tri_upper);
      CSR_MatConjTrisolveUpper (props->num_rows, T->base_index,
				T->has_unit_diag_implicit,
				T->has_sorted_indices, T->ptr, T->ind, T->val,
				alpha, x_view->val, x_view->num_cols,
				x_view->rowinc, x_view->colinc);
    }
  return 0;
}

static int
ConjTrans (const oski_matCSR_t * T, const oski_matcommon_t * props,
	   oski_value_t alpha, oski_vecview_t x_view)
{
  if (props->pattern.is_tri_lower)
    {
      CSR_MatConjTransTrisolveLower (props->num_rows, T->base_index,
				     T->has_unit_diag_implicit,
				     T->has_sorted_indices, T->ptr, T->ind,
				     T->val, alpha, x_view->val,
				     x_view->num_cols, x_view->rowinc,
				     x_view->colinc);
    }
  else
    {
      assert (props->pattern.is_tri_upper);
      CSR_MatConjTransTrisolveUpper (props->num_rows, T->base_index,
				     T->has_unit_diag_implicit,
				     T->has_sorted_indices, T->ptr, T->ind,
				     T->val, alpha, x_view->val,
				     x_view->num_cols, x_view->rowinc,
				     x_view->colinc);
    }
  return 0;
}

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
  const oski_matCSR_t *T = (const oski_matCSR_t *) pT;
  int err = 0;

  assert (T != NULL);
  assert (props != NULL);

  switch (opT)
    {
    case OP_CONJ:
      err = Conj (T, props, alpha, x_view);
      break;
    case OP_NORMAL:
      err = Normal (T, props, alpha, x_view);
      break;
    case OP_CONJ_TRANS:
      err = ConjTrans (T, props, alpha, x_view);
      break;
    case OP_TRANS:
      err = Trans (T, props, alpha, x_view);
      break;
    default:
      assert (0);
    }

  return err;
}

/* eof */
