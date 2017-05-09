/**
 *  \file src/DENSE/getset.c
 *  \ingroup MATTYPE_DENSE
 *  \brief DENSE get/set value routines.
 */

#include <assert.h>
#include <stdio.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/getset.h>
#include <oski/DENSE/module.h>

int
oski_GetMatReprEntry (const void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t * p_value)
{
  const oski_matDENSE_t *A = (const oski_matDENSE_t *) mat;

  assert (A != NULL);

  if (p_value == NULL)
    {
      oski_HandleError (ERR_BAD_ARG,
			"Nowhere to put return value", __FILE__, __LINE__,
			"Parameter #%d to parameter %s() is NULL",
			5, MACRO_TO_STRING (oski_GetMatReprEntry));
      return ERR_BAD_ARG;
    }

  /* Quick return cases */
  if (props != NULL)
    {
      if (props->pattern.is_tri_upper && col < row)
	{
	  VAL_SET_ZERO (*p_value);
	  return 0;
	}
      if (props->pattern.is_tri_lower && col > row)
	{
	  VAL_SET_ZERO (*p_value);
	  return 0;
	}
    }

  VAL_ASSIGN (*p_value, A->val[row - 1 + (col - 1) * (A->lead_dim)]);
  return 0;
}

int
oski_SetMatReprEntry (void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t new_val)
{
  oski_matDENSE_t *A = (oski_matDENSE_t *) mat;

  oski_index_t i0, j0;
  int is_herm = 0;
  int symm_pattern = 0;

  assert (A != NULL);

  /* Quick return cases */
  if (props != NULL)
    {
      if (props->pattern.is_tri_upper && col < row)
	{
	  if (IS_VAL_ZERO (new_val))
	    return 0;
#if IS_VAL_COMPLEX
	  OSKI_ERR_MOD_TRI_COMPLEX (oski_SetMatReprEntry, 1, row, col,
				    new_val);
#else
	  OSKI_ERR_MOD_TRI_REAL (oski_SetMatReprEntry, 1, row, col, new_val);
#endif
	  return ERR_ZERO_ENTRY;
	}
      if (props->pattern.is_tri_lower && col > row)
	{
	  if (IS_VAL_ZERO (new_val))
	    return 0;
#if IS_VAL_COMPLEX
	  OSKI_ERR_MOD_TRI_COMPLEX (oski_SetMatReprEntry, 0, row, col,
				    new_val);
#else
	  OSKI_ERR_MOD_TRI_REAL (oski_SetMatReprEntry, 0, row, col, new_val);
#endif
	  return ERR_ZERO_ENTRY;
	}

      is_herm = props->pattern.is_herm;
      symm_pattern = is_herm || props->pattern.is_symm;
    }

  i0 = row - 1;
  j0 = col - 1;
  VAL_ASSIGN (A->val[i0 + j0 * A->lead_dim], new_val);
  if (symm_pattern && (i0 != j0))
    {
      if (is_herm)
	{
	  VAL_CONJ (new_val);
	}
      VAL_ASSIGN (A->val[j0 + i0 * A->lead_dim], new_val);
    }

  return 0;
}

/* eof */
