/**
 *  \file src/VBR/getset.c
 *  \ingroup MATTYPE_VBR
 *  \brief VBR get/set value routines.
 */

#include <assert.h>
#include <stdio.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/getset.h>
#include <oski/VBR/module.h>

/** Find block row containing a given row using binary search. */
static oski_index_t
FindRowVBR (const oski_matVBR_t* A, oski_index_t I_min, oski_index_t I_max,
	    oski_index_t i0)
{
  oski_index_t I_candidate;
  oski_index_t i_min, i_max;
  if (I_min > I_max)
    return I_max; /* not found */
  I_candidate = (I_min + I_max) / 2;
  i_min = A->brow[I_candidate];
  i_max = A->brow[I_candidate+1];
  if (i0 < i_min)
    return FindRowVBR (A, I_min, I_candidate-1, i0);
  else if (i0 >= i_max)
    return FindRowVBR (A, I_candidate+1, I_max, i0);
  /* else, i_min <= i0 < i_max */
  return I_candidate;
}

int
oski_GetMatReprEntry (const void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t * p_value)
{
  const oski_matVBR_t *A = (const oski_matVBR_t *) mat;

  oski_value_t aij;
  oski_index_t i0, j0;
  oski_index_t I;

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

  i0 = row - 1;	/* 0-based */
  j0 = col - 1;	/* 0-based */

  I = FindRowVBR (A, 0, A->mb, i0);
  if (I >= 0 && I < A->mb) /* found */
    {
      oski_index_t i_start = A->brow[I];
      oski_index_t di = i0 - i_start;
      oski_index_t K;
      for (K = A->ptr[I]; K < A->ptr[I+1]; K++)
	{
	  oski_index_t J = A->ind[K];
	  oski_index_t j_start = A->bcol[J];
	  oski_index_t j_max = A->bcol[J+1];
	  oski_index_t C = j_max - j_start;
	  if (j_start <= j0 && j0 < j_max)
	    {
	      oski_index_t dj = j0 - j_start;
	      const oski_value_t* vp = A->val + A->valptr[K] + di*C + dj;
	      VAL_INC (aij, vp[0]);
	    }
	} /* K */
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
SetEntry (oski_matVBR_t * A, oski_index_t i0, oski_index_t j0,
	  oski_value_t new_val)
{
  oski_index_t I = FindRowVBR (A, 0, A->mb, i0);
  int modified = 0;
  if (I >= 0 && I < A->mb)
    {
      oski_index_t i_start = A->brow[I];
      oski_index_t di = i0 - i_start;
      oski_index_t K;
      for (K = A->ptr[I]; K < A->ptr[I+1]; K++)
	{
	  oski_index_t J = A->ind[K];
	  oski_index_t j_start = A->bcol[J];
	  oski_index_t j_max = A->bcol[J+1];
	  oski_index_t C = j_max - j_start;
	  if (j_start <= j0 && j0 < j_max)
	    {
	      oski_index_t dj = j0 - j_start;
	      oski_value_t* vp = A->val + A->valptr[K] + di*C + dj;
	      if (!modified)
		{
		  VAL_ASSIGN (vp[0], new_val);
		  modified = 1;
		}
	      else /* modified */
		{
		  VAL_SET_ZERO (vp[0]);
		}
	    }
	} /* K */
    }
  return modified;
}

int
oski_SetMatReprEntry (void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t new_val)
{
  oski_matVBR_t *A = (oski_matVBR_t *) mat;

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
