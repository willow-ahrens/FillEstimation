/**
 *  \file src/MBCSR/getset.c
 *  \ingroup MATTYPE_MBCSR
 *  \brief MBCSR get/set value routines.
 */

#include <assert.h>
#include <stdio.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/getset.h>
#include <oski/CSR/format.h>
#include <oski/MBCSR/module.h>

/**
 *  \brief Returns a pointer to an entry in the diagonal block.
 *
 *  \param[in] A MBCSR submatrix.
 *  \param[in] i Row index (0-based).
 *  \param[in] j Column index (0-based).
 *
 *  \returns If the specified entry lies in the block diagonal, returns
 *  a pointer to the entry. Otherwise, returns NULL.
 */
static const oski_value_t *
GetBlockDiagEntryRaw (const oski_submatMBCSR_t * A,
		      oski_index_t i, oski_index_t j)
{
  oski_index_t r;		/* row block size */
  oski_index_t I;		/* block row number */
  oski_index_t i0;		/* first row of i's block row */
  oski_index_t j0;		/* first column of i's block row */

  if (A == NULL)
    return NULL;
  if (i < 0 || j < 0)
    return NULL;
  r = A->r;
  if (i >= A->num_block_rows * r)
    return NULL;
  I = i / r;
  i0 = I * r;
  j0 = A->offset + i0;
  if (j >= j0 && j < (j0 + r))
    {
      return &(A->bdiag[I * r * r + (i - i0) * r + (j - j0)]);
    }
  else
    return NULL;
}

/**
 *  \brief Returns a pointer to an entry in an MBCSR submatrix,
 *  or NULL if no explicitly stored entry exists.
 *
 *  \param[in] A MBCSR submatrix.
 *  \param[in] i Row index (0-based).
 *  \param[in] j Row index (0-based).
 */
static const oski_value_t *
GetSubmatEntryRaw (const oski_submatMBCSR_t * A,
		   oski_index_t i, oski_index_t j)
{
  const oski_value_t *entry = GetBlockDiagEntryRaw (A, i, j);
  if (entry == NULL)
    {
      oski_index_t r = A->r;
      oski_index_t I0 = i / r;

      if (I0 < A->num_block_rows)
	{
	  oski_index_t i0 = I0 * r;
	  oski_index_t c = A->c;
	  oski_index_t any_match = 0, j0_last_match = 0;
	  oski_index_t k;
	  for (k = A->bptr[I0]; k < A->bptr[I0 + 1]; k++)
	    {
	      const oski_value_t *blocks = &(A->bval[k * r * c]);
	      oski_index_t j0 = A->bind[k];

	      if (j >= j0 && j < j0 + c)
		{
		  /* It is possible that there will be *two* blocks
		   * containing the desired entry, because the last
		   * block in each row may have been shifted on
		   * conversion so that it does not overshoot the
		   * rightmost column of the matrix. The following
		   * logic favors a block whose corresponding block
		   * column index is the smallest.
		   */
		  if (!any_match)
		    {
		      entry = &(blocks[(i - i0) * c + (j - j0)]);
		      any_match = 1;
		      j0_last_match = j0;
		    }
		  else if (j0 < j0_last_match)
		    {
		      entry = &(blocks[(i - i0) * c + (j - j0)]);
		      j0_last_match = j0;
		    }		/* else, ignore this match */
		}
	    }			/* k */
	}
    }
  return entry;
}

/**
 *  \brief Returns an element from a given BCSR submatrix.
 *
 *  \param[in] A BCSR submatrix.
 *  \param[in] i Row index (0-based).
 *  \param[in] j Row index (0-based).
 *  \param[in,out] p_value Pointer to buffer into which value should
 *  be returned.
 */
static int
GetBCSREntryRaw (const oski_matBCSR_t * A, oski_index_t i, oski_index_t j,
		 oski_value_t * p_value)
{
  oski_GetMatReprEntry_funcpt func_GetBCSR =
    OSKI_MATTYPE_METHOD ("BCSR", GetMatReprEntry);
  if (A == NULL || p_value == NULL)
    return ERR_BAD_ARG;
  if (func_GetBCSR == NULL)
    return ERR_NOT_IMPLEMENTED;
  return (*func_GetBCSR) (A, NULL, i + 1, j + 1, p_value);
}

/**
 *  \brief Sets an element from a given BCSR submatrix.
 *
 *  \param[in] A BCSR submatrix.
 *  \param[in] i Row index (0-based).
 *  \param[in] j Row index (0-based).
 *  \param[in,out] p_value Pointer to buffer into which value should
 *  be returned.
 */
static int
SetBCSREntryRaw (oski_matBCSR_t * A, oski_index_t i, oski_index_t j,
		 oski_value_t new_value)
{
  oski_SetMatReprEntry_funcpt func_SetBCSR =
    OSKI_MATTYPE_METHOD ("BCSR", SetMatReprEntry);
  if (A == NULL)
    return ERR_BAD_ARG;
  if (func_SetBCSR == NULL)
    return ERR_NOT_IMPLEMENTED;
  return (*func_SetBCSR) (A, NULL, i + 1, j + 1, new_value);
}

int
oski_GetMatReprEntry (const void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t * p_value)
{
  const oski_matMBCSR_t *A = (const oski_matMBCSR_t *) mat;
  oski_index_t row_start_A2 = 0, row_start_A3 = 0;
  int err = 0;

  assert (A != NULL && p_value != NULL);

  /* Quick return cases for triangular matrices */
  if ((props->pattern.is_tri_upper && col < row)
      || (props->pattern.is_tri_lower && col > row))
    {
      VAL_SET_ZERO (*p_value);
      return 0;
    }

  /* Transpose request if matrix is half-storage symmetric/Hermitian */
  if ((props->pattern.is_symm || props->pattern.is_herm)
      && (A->stored.is_upper != A->stored.is_lower)
      && ((A->stored.is_upper && col < row)
	  || (A->stored.is_lower && col > row)))
    {
      err = oski_GetMatReprEntry (mat, props, col, row, p_value);
      if (!err && props->pattern.is_herm)
        {
          VAL_CONJ (*p_value);
        }
      return err;
    }

  /* Starting row for partition \f$A_2\f$ (1-based) */
  row_start_A2 = 1 + A->A1.num_block_rows * A->A1.r;
  row_start_A3 = row_start_A2 + A->A2.num_block_rows * A->A2.r;

  if (row >= 1 && row < row_start_A2)
    {
      const oski_value_t *entry =
	GetSubmatEntryRaw (&(A->A1), row - 1, col - 1);
      if (entry == NULL)
	VAL_SET_ZERO (*p_value);
      else
	VAL_ASSIGN (*p_value, *entry);
    }
  else if (row >= row_start_A2 && row < row_start_A3)
    {
      const oski_value_t *entry =
	GetSubmatEntryRaw (&(A->A2), row - row_start_A2, col - 1);
      if (entry == NULL)
	VAL_SET_ZERO (*p_value);
      else
	VAL_ASSIGN (*p_value, *entry);
    }
  else if (A->p_A3 != NULL)
    {				/* row >= row_start_A3 */
      err = GetBCSREntryRaw (A->p_A3, row - row_start_A3, col - 1, p_value);
    }
  return err;
}

#define STORE_NORMAL 0
#define STORE_CONJ 1
#define STORE_SYMM 2
#define STORE_HERM 3

static int
SetEntry (oski_matMBCSR_t * A, oski_index_t i0, oski_index_t j0,
	  oski_value_t new_val, int store_mode)
{
  oski_index_t row_start_A2 = 0, row_start_A3 = 0;
  int err = 0;

  if (store_mode == STORE_CONJ)
    VAL_CONJ (new_val);

  /* Starting row for partitions \f$A_2, A_3\f$ (1-based) */
  row_start_A2 = A->A1.num_block_rows * A->A1.r;
  row_start_A3 = row_start_A2 + A->A2.num_block_rows * A->A2.r;

  if (i0 >= 0 && i0 < row_start_A2)
    {
      oski_value_t *entry =
	(oski_value_t *) GetSubmatEntryRaw (&(A->A1), i0, j0);
      if (entry == NULL)
	err = ERR_ZERO_ENTRY;
      else
        VAL_ASSIGN (*entry, new_val);
    }
  else if (i0 >= row_start_A2 && i0 < row_start_A3)
    {
      oski_value_t *entry = (oski_value_t *) GetSubmatEntryRaw (&(A->A2),
								i0 -
								row_start_A2,
								j0);
      if (entry == NULL)
	err = ERR_ZERO_ENTRY;
      else
        VAL_ASSIGN (*entry, new_val);
    }
  else if (A->p_A3 != NULL)
    {				/* row >= row_start_A3 */
      err = SetBCSREntryRaw (A->p_A3, i0 - row_start_A3, j0, new_val);
    }

  if (err)
    return err;

  /* Handle symmetric image if needed */
  if (j0 != i0)
    switch (store_mode)
      {
      case STORE_SYMM:
        err = SetEntry (A, j0, i0, new_val, STORE_NORMAL);
        break;
      case STORE_HERM:
        err = SetEntry (A, j0, i0, new_val, STORE_CONJ);
        break;
      default:			/* do nothing */
        break;
      }
  return err;
}

/**
 *  \brief
 *
 *  \note props may be set to NULL.
 */
int
oski_SetMatReprEntry (void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t new_val)
{
  oski_matMBCSR_t *A = (oski_matMBCSR_t *) mat;

  int has_symm_pattern = 0;
  int has_symm_pattern_half = 0;
  int has_symm_pattern_full = 0;
  int is_herm = 0;

  assert (A != NULL);

  if (props != NULL)
    {
      /* Quick return cases for triangular matrices */
      if ((props->pattern.is_tri_upper && col < row)
	  || (props->pattern.is_tri_lower && col > row))
	{
	  return ERR_ZERO_ENTRY;
	}

      is_herm = props->pattern.is_herm;
      has_symm_pattern = props->pattern.is_symm || is_herm;
      has_symm_pattern_half = has_symm_pattern
	&& (A->stored.is_upper != A->stored.is_lower);
      has_symm_pattern_full = has_symm_pattern
	&& (A->stored.is_upper == A->stored.is_lower);
    }

  /* Transpose request if matrix is half-storage symmetric/Hermitian */
  if (has_symm_pattern_half
      && ((A->stored.is_upper && col < row)
	  || (A->stored.is_lower && col > row)))
    {
      if (is_herm)
	VAL_CONJ (new_val);
      return oski_SetMatReprEntry (mat, props, col, row, new_val);
    }
  else
    {
      int store_mode = STORE_NORMAL;
      if (has_symm_pattern_full)
	store_mode = is_herm ? STORE_HERM : STORE_SYMM;
      return SetEntry (mat, row - 1, col - 1, new_val, store_mode);
    }
}

/* eof */
