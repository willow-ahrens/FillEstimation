/**
 *  \file src/CSR/getset.c
 *  \ingroup MATTYPE_CSR
 *  \brief CSR get/set value routines.
 */

#include <assert.h>
#include <stdio.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/getset.h>
#include <oski/CSR/module.h>

int
oski_GetMatReprEntry (const void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t * p_value)
{
  const oski_matCSR_t *A = (const oski_matCSR_t *) mat;
  oski_index_t b = A->base_index;

  oski_value_t aij;
  oski_index_t i0, j0, k;
  int transposed;

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
  if (row == col && A->has_unit_diag_implicit)
    {
      VAL_SET_ONE (aij);
      VAL_ASSIGN (*p_value, aij);
      return 0;
    }

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

  /* Normalize row/column index to match storage if symmetric */
  if ((props->pattern.is_symm || props->pattern.is_herm) && (A->stored.is_upper != A->stored.is_lower)	/* half-storage */
      && ((A->stored.is_upper && col < row)
	  || (A->stored.is_lower && col > row)))
    {
      transposed = 1;
      i0 = col;
      j0 = row;
    }
  else
    {
      transposed = 0;
      i0 = row;
      j0 = col;
    }

  for (k = A->ptr[i0 - 1] - b; k < A->ptr[i0] - b; k++)
    {
      oski_index_t j = A->ind[k] + 1 - b;	/* 1-based */
      if (j == j0)
	VAL_INC (aij, A->val[k]);
    }

  if (transposed && props->pattern.is_herm && (i0 != j0))
    VAL_CONJ (aij);

  VAL_ASSIGN (*p_value, aij);
  return 0;
}

/** Returns 1 on success, or 0 on error. */
static int
SetEntry (oski_matCSR_t * A, oski_index_t i0, oski_index_t j0,
	  oski_index_t b, oski_value_t new_val)
{
  oski_index_t k;
  int modified = 0;

  for (k = A->ptr[i0 - 1] - b; k < A->ptr[i0] - b; k++)
    {
      oski_index_t j = A->ind[k] + 1 - b;	/* 1-based */
      if (j == j0)
	{
	  if (!modified)	/* first occurrence */
	    {
	      VAL_ASSIGN (A->val[k], new_val);
	      modified = 1;
	    }
	  else			/* modified == 1 */
	    VAL_SET_ZERO (A->val[k]);
	}
    }

  return modified;
}

int
oski_SetMatReprEntry (void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t new_val)
{
  oski_matCSR_t *A = (oski_matCSR_t *) mat;
  oski_index_t b = A->base_index;

  oski_index_t i0, j0;
  int symm_pattern;
  int symm_pattern_half;
  int symm_pattern_full;
  int transposed;

  assert (A != NULL);

  /* Quick return cases */
  if (row == col && A->has_unit_diag_implicit)
    {
      if (IS_VAL_ONE (new_val))
	return 0;
#if IS_VAL_COMPLEX
      OSKI_ERR_MOD_UNIT_DIAG_COMPLEX (oski_SetMatReprEntry,
				      row, col, new_val);
#else
      OSKI_ERR_MOD_UNIT_DIAG_REAL (oski_SetMatReprEntry, row, col, new_val);
#endif
      return ERR_INMATPROP_CONFLICT;
    }

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
  symm_pattern_full = symm_pattern
    && A->stored.is_upper == A->stored.is_lower;
  symm_pattern_half = symm_pattern
    && A->stored.is_upper != A->stored.is_lower;
  transposed = symm_pattern_half
    && ((A->stored.is_upper && col < row)
	|| (A->stored.is_lower && col > row));

  if (transposed)
    i0 = col, j0 = row;
  else
    i0 = row, j0 = col;

  if (props->pattern.is_herm && transposed && (i0 != j0))
    VAL_CONJ (new_val);

  if (SetEntry (A, i0, j0, b, new_val) == 0)
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
      SetEntry (A, j0, i0, b, new_val);
    }

  return 0;
}

/* eof */
