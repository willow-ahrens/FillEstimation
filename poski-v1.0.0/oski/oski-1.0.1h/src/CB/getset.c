/**
 *  \file src/CB/getset.c
 *  \ingroup MATTYPE_CB
 *  \brief CB get/set value routines.
 */

#include <assert.h>
#include <stdio.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/getset.h>
#include <oski/CB/format.h>
#include <oski/CB/module.h>

static const oski_submatCB_t *
FindSubblock (const simplelist_t * cache_blocks,
	      oski_index_t i0, oski_index_t j0)
{
  const oski_submatCB_t *block;
  simplelist_iter_t i;
  for (block = simplelist_BeginIter (cache_blocks, &i);
       block != NULL; block = simplelist_NextIter (&i))
    {
      if (block->row <= i0 && i0 < (block->row + block->num_rows)
	  && block->col <= j0 && j0 < (block->col + block->num_cols))
	{
	  return block;
	}
    }
  return NULL;
}

int
oski_GetMatReprEntry (const void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t * p_value)
{
  const oski_matCB_t *A = (const oski_matCB_t *) mat;
  const oski_submatCB_t *block;

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

  /* Otherwise, need to scan to compute. */
  block = FindSubblock (A->cache_blocks, row - 1, col - 1);
  if (block != NULL)
    {
      oski_value_t aij = oski_GetMatEntry (block->mat,
					   row - block->row,
					   col - block->col);
      VAL_ASSIGN (*p_value, aij);
    }
  else
    {
      VAL_SET_ZERO (*p_value);
    }
  return 0;
}

static int
SetEntry (oski_matCB_t * A, oski_index_t i0, oski_index_t j0,
	  oski_value_t aij)
{
  oski_submatCB_t *block = (oski_submatCB_t *) FindSubblock (A->cache_blocks,
							     i0, j0);
  if (block != NULL)
    {
      return oski_SetMatEntry (block->mat,
			       i0 - block->row + 1, j0 - block->col + 1, aij);
    }
  else
    return ERR_ZERO_ENTRY;
}

int
oski_SetMatReprEntry (void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t new_val)
{
  oski_matCB_t *A = (oski_matCB_t *) mat;
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

  if (SetEntry (A, i0 - 1, j0 - 1, new_val))
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
