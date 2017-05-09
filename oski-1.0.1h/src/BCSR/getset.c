/**
 *  \file src/BCSR/getset.c
 *  \ingroup MATTYPE_BCSR
 *  \brief BCSR get/set value routines.
 */

#include <assert.h>
#include <stdio.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/getset.h>
#include <oski/CSR/format.h>
#include <oski/CSR/module.h>
#include <oski/BCSR/format.h>
#include <oski/BCSR/module.h>

/**
 *  \brief Reads a matrix entry.
 *
 *  \param[in,out] A BCSR matrix. Must be non-NULL.
 *  \param[in] i0 Row of element (0-based).
 *  \param[in] j0 Column of element (0-based).
 *  \param[in] p_value Pointer to buffer to store return value.
 *  Must be non-NULL.
 *
 *  \returns 0 on success, or an error code otherwise.
 *
 *  This routine operates on the raw explicit storage only
 *  (i.e., does not account for an implicit unit diagonal).
 *  The caller must ensure that the requested entry (i0, j0)
 *  is valid.
 */
static int
GetEntryRaw (const oski_matBCSR_t * A,
	     oski_index_t i0, oski_index_t j0, oski_value_t * p_value)
{
  const oski_index_t *bptr;
  const oski_index_t *bind;
  const oski_value_t *bval;

  oski_index_t I;
  oski_index_t K;
  oski_index_t r, c;
  oski_index_t dr;

  if (A == NULL)
    return ERR_BAD_MAT;

  r = A->row_block_size;
  I = i0 / r;
  dr = i0 - I * r;
  if (I >= A->num_block_rows)	/* in leftover row */
    return GetEntryRaw (A->leftover, dr, j0, p_value);

  /* Otherwise, need to scan to compute. */
  bptr = A->bptr;
  bind = A->bind;
  bval = A->bval;
  c = A->col_block_size;
  VAL_SET_ZERO (*p_value);
  for (K = bptr[I]; K < bptr[I + 1]; K++)
    {
      oski_index_t jb = bind[K];
      if (jb <= j0 && j0 < jb + c)
	{
	  oski_index_t dc = j0 - jb;
	  VAL_INC (*p_value, bval[K * r * c + dr * c + dc]);
	}
    }
  return 0;
}

/**
 *  \brief
 *
 *  \note As written, this routine ignores 'props.' This point is
 *  critical because this routine calls itself on the leftover
 *  rows if the requested entry lies there, and 'props' is set to
 *  NULL in that call.
 */
int
oski_GetMatReprEntry (const void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t * p_value)
{
  const oski_matBCSR_t *A = (const oski_matBCSR_t *) mat;

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
      VAL_SET_ONE (*p_value);
      return 0;
    }

  return GetEntryRaw (A, row - 1, col - 1, p_value);
}

#define STORE_SINGLE 0 /**< Store specified element only */
#define STORE_CONJ 1 /**< Store the conjugate only */
#define STORE_SYMM 2 /**< Store element and its symmetric image */
#define STORE_HERM 3 /**< Store element and its Hermitian image */

/**
 *  \brief Changes a matrix entry to a given value.
 *
 *  \param[in,out] A BCSR matrix. Must be non-NULL.
 *  \param[in] store_op What to store: STORE_SINGLE,
 *  STORE_CONJ, STORE_SYMM, and STORE_HERM.
 *  \param[in] i0 Row of element (0-based).
 *  \param[in] j0 Column of element (0-based).
 *  \param[in] new_val New value.
 *
 *  \returns 0 on success, or an error code otherwise.
 *
 *  This routine operates on the raw explicit storage only
 *  (i.e., does not account for an implicit unit diagonal).
 *  The caller must ensure that the requested entry (i0, j0)
 *  is valid.
 */
static int
SetEntryRaw (oski_matBCSR_t * A, int store_op,
	     oski_index_t i0, oski_index_t j0, oski_value_t new_val)
{
  const oski_index_t *bptr;
  const oski_index_t *bind;
  oski_value_t *bval;
  oski_index_t r;
  oski_index_t c;

  oski_index_t I;
  oski_index_t dr;
  oski_index_t K;

  oski_value_t val_store;
  int modified = 0;
  int err;

  if (A == NULL)
    return ERR_BAD_MAT;

  bptr = A->bptr;
  bind = A->bind;
  bval = A->bval;
  r = A->row_block_size;
  c = A->col_block_size;
  I = i0 / r;
  dr = i0 - I * r;

  if (I >= A->num_block_rows)	/* in leftover row */
    {
      if (store_op == STORE_CONJ)
	return SetEntryRaw (A->leftover, STORE_CONJ, dr, j0, new_val);

      err = SetEntryRaw (A->leftover, STORE_SINGLE, dr, j0, new_val);
      if (err)
	return err;

      if (store_op == STORE_SYMM)
	return SetEntryRaw (A, STORE_SINGLE, j0, i0, new_val);
      else if (store_op == STORE_HERM)
	return SetEntryRaw (A, STORE_CONJ, j0, i0, new_val);
      return err;
    }

  if (store_op == STORE_CONJ)
    VAL_ASSIGN_CONJ (val_store, new_val);
  else
    VAL_ASSIGN (val_store, new_val);

  modified = 0;
  for (K = bptr[I]; K < bptr[I + 1]; K++)
    {
      oski_index_t jb = bind[K];
      if (jb <= j0 && j0 < jb + c)
	{
	  oski_index_t dc = j0 - jb;
	  if (!modified)
	    {
	      VAL_ASSIGN (bval[K * r * c + dr * c + dc], val_store);
	      modified = 1;
	    }
	  else
	    {
	      VAL_SET_ZERO (bval[K * r * c + dr * c + dc]);
	    }
	}
    }

  if (!modified)
    return ERR_ZERO_ENTRY;

  err = 0;
  switch (store_op)
    {
    case STORE_SYMM:
      err = SetEntryRaw (A, STORE_SINGLE, j0, i0, val_store);
      break;
    case STORE_HERM:
      if (i0 == j0)
	err = SetEntryRaw (A, STORE_SINGLE, j0, i0, val_store);
      else
	err = SetEntryRaw (A, STORE_CONJ, j0, i0, val_store);
      break;
    default:
      break;
    }
  return err;
}

/**
 *  \brief
 *
 *  \note This routine only depends on 'props' if the matrix is
 *  symmetric/Hermitian/triangular. Otherwise, it is OK to pass
 *  'props' as NULL.
 */
int
oski_SetMatReprEntry (void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t new_val)
{
  oski_matBCSR_t *A = (oski_matBCSR_t *) mat;
  int store_op = STORE_SINGLE;
  int err;

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

  if (props != NULL && row != col)
    {
      if (props->pattern.is_symm)
	store_op = STORE_SYMM;
      else if (props->pattern.is_herm)
	store_op = STORE_HERM;
    }
  err = SetEntryRaw (A, store_op, row - 1, col - 1, new_val);

  if (err)
    {
#if IS_VAL_COMPLEX
      OSKI_ERR_ZERO_ENTRY_COMPLEX (oski_SetMatReprEntry, row, col, new_val);
#else
      OSKI_ERR_ZERO_ENTRY_REAL (oski_SetMatReprEntry, row, col, new_val);
#endif
    }

  return err;
}

/* eof */
