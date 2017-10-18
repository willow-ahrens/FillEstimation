/**
 *  \file src/GCSR/getset.c
 *  \ingroup MATTYPE_GCSR
 *  \brief GCSR get/set value routines.
 */

#include <assert.h>
#include <stdio.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/getset.h>
#include <oski/GCSR/module.h>

int
oski_GetMatReprEntry (const void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t * p_value)
{
  const oski_matGCSR_t *A = (const oski_matGCSR_t *) mat;

  oski_value_t aij;
  oski_index_t i0, j0;
  oski_index_t i_loc;

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
  VAL_SET_ZERO (aij);
  if (props->pattern.is_tri_upper && col < row)
    {
      VAL_ASSIGN (*p_value, aij);
      return 0;
    }
  if (props->pattern.is_tri_lower && col > row)
    {
      VAL_ASSIGN (*p_value, aij);
      return 0;
    }

  /* Otherwise, need to scan to compute. */

  i0 = row - 1;			/* 0-based */
  j0 = col - 1;			/* 0-based */

  i_loc = oski_FindRowGCSR (A, i0);
  if (i_loc >= 0)
    {				/* found */
      oski_index_t k;
      for (k = A->ptr[i_loc]; k < A->ptr[i_loc + 1]; k++)
	{
	  oski_index_t j = A->cind[k];	/* 1-based */
	  if (j == j0)
	    VAL_INC (aij, A->val[k]);
	}
    }

  VAL_ASSIGN (*p_value, aij);
  return 0;
}

/**
 *  \brief Returns 1 on success, or 0 on error.
 *
 *  Input indices (i0, j0) are 0-based.
 */
static int
SetEntry (oski_matGCSR_t * A, oski_index_t i0, oski_index_t j0,
	  oski_value_t new_val)
{
  oski_index_t i_loc = oski_FindRowGCSR (A, i0);
  int modified = 0;
  if (i_loc >= 0)
    {
      oski_index_t k;
      for (k = A->ptr[i_loc]; k < A->ptr[i_loc + 1]; k++)
	{
	  oski_index_t j = A->cind[k];	/* 1-based */
	  if (j == j0)
	    {
	      if (!modified)
		{		/* first occurrence */
		  VAL_ASSIGN (A->val[k], new_val);
		  modified = 1;
		}
	      else
		{		/* modified == 1 */
		  VAL_SET_ZERO (A->val[k]);
		}
	    }
	}
    }
  return modified;
}

int
oski_SetMatReprEntry (void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t new_val)
{
  oski_matGCSR_t *A = (oski_matGCSR_t *) mat;

  oski_index_t i0, j0;
  int symm_pattern;
  int symm_pattern_half;
  int symm_pattern_full;
  int transposed;

  assert (A != NULL);

  /* Quick return cases */
  if (props->pattern.is_tri_upper && col < row)
    {
      if (IS_VAL_ZERO (new_val))
	return 0;
#if IS_VAL_COMPLEX
      OSKI_ERR_MOD_TRI_COMPLEX (oski_SetMatReprEntry, 1, row, col, new_val);
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
      OSKI_ERR_MOD_TRI_COMPLEX (oski_SetMatReprEntry, 0, row, col, new_val);
#else
      OSKI_ERR_MOD_TRI_REAL (oski_SetMatReprEntry, 0, row, col, new_val);
#endif
      return ERR_ZERO_ENTRY;
    }

  /* Set predicates */
  symm_pattern = props->pattern.is_symm || props->pattern.is_herm;
  symm_pattern_full = symm_pattern;
  symm_pattern_half = 0;
  transposed = 0;

  if (transposed)
    i0 = col, j0 = row;
  else
    i0 = row, j0 = col;

  if (props->pattern.is_herm && transposed && (i0 != j0))
    {
      VAL_CONJ (new_val);
    }

  if (SetEntry (A, i0 - 1, j0 - 1, new_val) == 0)
    {
#if IS_VAL_COMPLEX
      OSKI_ERR_ZERO_ENTRY_COMPLEX (oski_SetMatReprEntry, i0, j0, new_val);
#else
      OSKI_ERR_ZERO_ENTRY_REAL (oski_SetMatReprEntry, i0, j0, new_val);
#endif
      return ERR_ZERO_ENTRY;
    }

  if (symm_pattern_full && (i0 != j0))
    {
      if (props->pattern.is_herm)
	VAL_CONJ (new_val);
      SetEntry (A, j0 - 1, i0 - 1, new_val);
    }

  return 0;
}

/* eof */
