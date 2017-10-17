/**
 *  \file src/CB/convert.c
 *  \brief CSR-to-CB format conversion routines.
 *  \ingroup MATTYPES_CB
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/matcreate.h>
#include <oski/modloader.h>
#include <oski/matrix.h>
#include <oski/tune.h>

#include <oski/CSR/format.h>
#include <oski/CB/format.h>
#include <oski/CB/module.h>

static int
ExpandSymm (const oski_matCSR_t * mat, const oski_matcommon_t * props,
	    oski_matCSR_t ** p_mat_full)
{
  oski_ConditionallyExpandCSRToFull_funcpt func_Expand =
    OSKI_MATTYPE_METHOD ("CSR", ConditionallyExpandCSRToFull);
  assert (func_Expand != NULL);
  return (*func_Expand) (mat, props, p_mat_full);
}

/**
 *  \brief Returns the number of rows of the CSR matrix A that
 *  are structurally zero.
 *
 *  \todo This routine duplicates the functionality of
 *  oski_CountZeroRowsCSR(), and could be eliminated.
 */
static oski_index_t
CountZeroRows (oski_index_t m, const oski_index_t * ptr)
{
  oski_index_t i;
  oski_index_t num_zero_rows = 0;
  for (i = 0; i < m; i++)
    num_zero_rows += (ptr[i + 1] == ptr[i]);
  return num_zero_rows;
}

/**
 *  \brief Returns the index of the first row \f$i \geq i_0\f$
 *  containing at least one structural non-zero, and returns
 *  \f$i \geq m\f$ if no such row can be found.
 *
 *  \param[in] m Total number of rows \f$m\f$ in the matrix.
 *  \param[in] ptr Row pointers.
 *  \param[in] has_unit_diag Non-zero <==> the diagonal is
 *  all ones and not stored explicitly.
 *  \param[in] i_min Minimum desired row index, \f$i_0\f$.
 */
static oski_index_t
FindFirstNonzeroRow (oski_index_t m, const oski_index_t * ptr,
		     int has_unit_diag, oski_index_t i_min)
{
  oski_index_t i;
  if (has_unit_diag)
    i = i_min;
  else
    {
      for (i = i_min; i < m; i++)
	{
	  if (ptr[i + 1] > ptr[i])
	    break;
	}			/* i */
    }				/* if( has_unit_diag ) */
  return i;
}

/** Return the minimum of two values. */
#define MIN(a, b)  ((a) < (b) ? (a) : (b))

/**
 *  \brief Returns the smallest column \f$j \geq j_0\f$ of any
 *  structural non-zero in the \f$r\f$ consecutive rows beginning
 *  at row \f$i_0\f$, \f$j \geq n\f$ if no such non-zero can be
 *  found.
 */
static oski_index_t
FindMinColIndex (oski_index_t m, oski_index_t n,
		 const oski_index_t * ptr, const oski_index_t * ind,
		 oski_index_t b, oski_index_t has_unit_diag,
		 oski_index_t i_start, oski_index_t i_max, oski_index_t j_min)
{
  oski_index_t max_rows = i_max - i_start;
  oski_index_t i_end = i_max - 1;
  oski_index_t i;
  oski_index_t j_cur_min;

  /* First, make a 'best guess' as to minimum column index */
  j_cur_min = n;		/* initial value: no idea */
  if (has_unit_diag)
    {
      if (j_min <= i_end)
	{
	  if (i_start <= j_min)
	    return j_min;	/* implicit diag elem _must_ be min col index */
	  else
	    j_cur_min = j_min;	/* implicit diag elem _may_ be min col index */
	}
    }

  for (i = i_start; i <= i_end; i++)
    {
      oski_index_t k;
      for (k = ptr[i] - b; k < ptr[i + 1] - b; k++)
	{
	  oski_index_t j = ind[k] - b;
	  if (j >= j_min && j < j_cur_min)
	    j_cur_min = j;
	}
    }

  return j_cur_min;
}

static oski_index_t
FindFirstNzRowInSubmat (oski_index_t m,
			const oski_index_t * ptr, const oski_index_t * ind,
			oski_index_t b, int has_unit_diag,
			oski_index_t i0, oski_index_t j0,
			oski_index_t i_max, oski_index_t j_max)
{
  oski_index_t i;
  for (i = i0; i < i_max; i++)
    {
      oski_index_t nnz_i;
      oski_index_t k;
      for (k = ptr[i] - b, nnz_i = 0; k < ptr[i + 1] - b; k++)
	{
	  oski_index_t j = ind[k] - b;
	  if (j0 <= j && j < j_max)
	    return i;
	}
      if (has_unit_diag && (j0 <= i) && (i < j_max))
	return i;
    }
  /* No non-zero rows left in this submatrix */
  return m;
}

static oski_index_t
FindNextZeroRowInSubmat (oski_index_t m,
			 const oski_index_t * ptr, const oski_index_t * ind,
			 oski_index_t b, int has_unit_diag,
			 oski_index_t i0, oski_index_t j0,
			 oski_index_t i_max, oski_index_t j_max)
{
  oski_index_t i;
  for (i = i0; i < i_max; i++)
    {
      if (has_unit_diag && (j0 <= i) && (i < j_max))
	continue;
      else
	{
	  int has_nz = 0;
	  oski_index_t k;
	  for (k = ptr[i] - b; k < ptr[i + 1] - b && !has_nz; k++)
	    {
	      oski_index_t j = ind[k] - b;
	      has_nz = (j0 <= j) && (j < j_max);
	    }
	  if (!has_nz)		/* Row is empty */
	    return i;
	}
    }
  return i_max;
}

static int
FindSubblock (oski_index_t m, const oski_index_t * ptr,
	      const oski_index_t * ind,
	      oski_index_t b, int has_unit_diag,
	      oski_index_t i_min, oski_index_t j_min,
	      oski_index_t i_max, oski_index_t j_max,
	      oski_index_t * p_i_start, oski_index_t * p_i_max)
{
  oski_index_t i_start = FindFirstNzRowInSubmat (m, ptr, ind,
						 b, has_unit_diag,
						 i_min, j_min,
						 i_max, j_max);
  oski_index_t i_end = i_start;

  oski_index_t max_zero_rows = (i_max - i_start) / 2;
  oski_index_t num_zero_rows = 0;

  if (i_start == m)		/* no more subblocks */
    return 0;

  while (i_end < i_max && num_zero_rows < max_zero_rows)
    {
      i_end = FindNextZeroRowInSubmat (m, ptr, ind, b, has_unit_diag,
				       i_end + 1, j_min, i_max, j_max);
      num_zero_rows += (i_end < i_max);
    }

  if (p_i_start != NULL)
    *p_i_start = i_start;
  if (p_i_max != NULL)
    *p_i_max = i_end + (i_end < i_max);

  return 1;
}

static void
SetupBlockRowPtrs (const oski_index_t * ptr, const oski_index_t * ind,
		   oski_index_t b, int has_unit_diag,
		   oski_index_t i_start, oski_index_t j_min,
		   oski_index_t R, oski_index_t j_max, oski_index_t * bptr)
{
  oski_index_t di;

  /* First, count number of non-zeros in each row of the submatrix */
  for (di = 0, bptr[0] = 0; di < R; di++)
    {
      oski_index_t i = i_start + di;
      oski_index_t k;
      oski_index_t nnz_i;
      for (k = ptr[i] - b, nnz_i = 0; k < ptr[i + 1] - b; k++)
	{
	  oski_index_t j = ind[k] - b;
	  nnz_i += (j_min <= j && j < j_max);
	}
      bptr[di + 1] = nnz_i;
    }

  /* Account for implicit diagonal entries if necessary */
  if (has_unit_diag)
    {
      for (di = 0; di < R; di++)
	{
	  oski_index_t i = i_start + di;
	  bptr[di + 1] += (j_min <= i && i < j_max);
	}
    }

  for (di = 1; di <= R; di++)
    bptr[di] += bptr[di - 1];
}

static void
CopyValues (const oski_index_t * ptr, const oski_index_t * ind,
	    const oski_value_t * val, oski_index_t b, int has_unit_diag,
	    oski_index_t i_start, oski_index_t j_min,
	    oski_index_t R, oski_index_t j_max,
	    const oski_index_t * bptr, oski_index_t * bind,
	    oski_value_t * bval)
{
  oski_index_t di;
  for (di = 0; di < R; di++)
    {
      oski_index_t i = i_start + di;
      oski_index_t k;
      oski_index_t kb;
      for (k = ptr[i] - b, kb = bptr[di]; k < ptr[i + 1] - b; k++)
	{
	  oski_index_t j = ind[k] - b;
	  if (j_min <= j && j < j_max)
	    {
	      assert (kb < bptr[di + 1]);
	      bind[kb] = j - j_min;
	      VAL_ASSIGN (bval[kb], val[k]);
	      kb++;
	    }
	}			/* k */
    }				/* di */
  if (has_unit_diag)
    {
      for (di = 0; di < R; di++)
	{
	  oski_index_t i = i_start + di;
	  if (j_min <= i && i < j_max)
	    {
	      oski_index_t kb = bptr[di + 1] - 1;
	      bind[kb] = i - j_min;
	      VAL_SET_ONE (bval[kb]);
	    }
	}
    }
}

static int
ConvertSubblock (const oski_index_t * ptr, const oski_index_t * ind,
		 const oski_value_t * val, oski_index_t b, int has_unit_diag,
		 oski_index_t i_start, oski_index_t j_min,
		 oski_index_t i_max, oski_index_t j_max,
		 simplelist_t * cache_blocks)
{
  oski_index_t *bptr;
  oski_index_t *bind;
  oski_value_t *bval;
  oski_submatCB_t *block;
  oski_index_t R = i_max - i_start;
  int err;
  size_t nnz_block;

  /* Set up row pointers */
  bptr = oski_Malloc (oski_index_t, R + 1);
  if (bptr == NULL)
    return ERR_OUT_OF_MEMORY;
  SetupBlockRowPtrs (ptr, ind, b, has_unit_diag,
		     i_start, j_min, R, j_max, bptr);
  nnz_block = (size_t) (bptr[R]);

  /* Copy indices and values */
  err = oski_MultiMalloc (__FILE__, __LINE__, 2,
			  nnz_block * sizeof (oski_index_t), &bind,
			  nnz_block * sizeof (oski_value_t), &bval);
  if (err)
    {
      oski_Free (bptr);
      return ERR_OUT_OF_MEMORY;
    }
  CopyValues (ptr, ind, val, b, has_unit_diag,
	      i_start, j_min, R, j_max, bptr, bind, bval);

  /* Package block */
  block = oski_Malloc (oski_submatCB_t, 1);
  if (block == NULL)
    {
      oski_FreeAll (3, bptr, bind, bval);
      return ERR_OUT_OF_MEMORY;
    }

  block->row = i_start;
  block->col = j_min;
  block->num_rows = R;
  block->num_cols = j_max - j_min;
  block->nnz = bptr[R];
  block->num_zero_rows = CountZeroRows (R, bptr);
  block->mat = oski_CreateMatCSR (bptr, bind, bval,
				  block->num_rows, block->num_cols,
				  COPY_INPUTMAT,
				  3,
				  MAT_GENERAL,
				  INDEX_ZERO_BASED, MAT_DIAG_EXPLICIT);
  oski_FreeAll (3, bptr, bind, bval);
  if (block->mat == NULL)
    {
      oski_Free (block);
      return ERR_OUT_OF_MEMORY;
    }
  /* Add to current list of blocks */
  if (!simplelist_Append (cache_blocks, block))
    {
      oski_Free (block);
      return ERR_OUT_OF_MEMORY;
    }
  return 0;
}

/**
 *  \brief Recursive procedure to convert a specified submatrix of
 *  a CSR matrix into a sequence of CSR subblocks consisting of
 *  consecutive non-zero rows.
 */
static int
MakeSparseSubblocks (oski_index_t m, oski_index_t n,
		     const oski_index_t * ptr,
		     const oski_index_t * ind, const oski_value_t * val,
		     oski_index_t b, int has_unit_diag,
		     oski_index_t i0, oski_index_t j0,
		     oski_index_t i_max, oski_index_t j_max,
		     simplelist_t * cache_blocks)
{
  oski_index_t i_start, i_submax;
  int found_subblock;

  found_subblock = FindSubblock (m, ptr, ind, b, has_unit_diag,
				 i0, j0, i_max, j_max, &i_start, &i_submax);
  while (found_subblock)
    {
      int err = ConvertSubblock (ptr, ind, val, b, has_unit_diag,
				 i_start, j0, i_submax, j_max,
				 cache_blocks);
      if (err)
	return err;
      found_subblock = FindSubblock (m, ptr, ind, b, has_unit_diag,
				     i_submax, j0, i_max, j_max,
				     &i_start, &i_submax);
    }
  return 0;
}

/**
 *  \brief Convert a specified submatrix of a CSR matrix into a
 *  single cache block.
 */
static int
MakeBlock (oski_index_t m, oski_index_t n, const oski_index_t * ptr,
	   const oski_index_t * ind, const oski_value_t * val,
	   oski_index_t b, int has_unit_diag,
	   oski_index_t i0, oski_index_t j0,
	   oski_index_t i_max, oski_index_t j_max,
	   simplelist_t * cache_blocks)
{
  int err = ConvertSubblock (ptr, ind, val, b, has_unit_diag,
			     i0, j0, i_max, j_max, cache_blocks);
  oski_submatCB_t *block =
    (oski_submatCB_t *) simplelist_GetLastElem (cache_blocks);
  if (err)
    return err;
  if (block != NULL
      && block->mat != NULL && block->num_zero_rows >= (block->num_rows / 2))
    oski_ApplyMatTransforms (block->mat, "return GCSR(InputMat)");
  return 0;
}

/**
 *  \brief Recursive procedure to compute a cache block partitioning
 *  of an \f$m\times n\f$ matrix \f$A\f$, stored in CSR format.
 *
 *  \param[in] m Total number of rows in the matrix.
 *  \param[in] n Total number of columns in the matrix.
 *  \param[in] ptr Row pointers for the CSR input matrix.
 *  \param[in] ind Column indices for the CSR input matrix.
 *  \param[in] val Non-zero values for the CSR input matrix.
 *  \param[in] b Index base of the input matrix (0 or 1).
 *  \param[in] has_unit_diag Zero only if the input matrix stores
 *  the diagonal elements explicitly.
 *  \param[in] min_row Minimum row index \f$i_0\f$ to consider for
 *  the current cache block (0-based index).
 *  \param[in] min_col Minimum column index \f$j_0\f$ to consider
 *  for the current cache block (0-based index).
 *  \param[in] R Maximum row block size.
 *  \param[in] C Maximum column block size.
 *  \param[in,out] cache_blocks Current list of cache blocks.
 *
 *  \returns This routine scans \f$A\f$, beginning at position
 *  \f$(i_0, j_0)\f$, for a non-zero cache block of maximum
 *  size \f$R\times C\f$. If such a cache block is found, it
 *  is appended to cache_blocks, and this procedure calls itself
 *  recursively to process the rest of the matrix. When no such
 *  cache blocks remain, this routine returns 0. This routine
 *  returns a non-zero error code on error.
 */
static int
ConvertToCSR (oski_index_t m, oski_index_t n,
	      const oski_index_t * ptr, const oski_index_t * ind,
	      const oski_value_t * val, oski_index_t b, int has_unit_diag,
	      oski_index_t min_row, oski_index_t min_col,
	      oski_index_t R, oski_index_t C, simplelist_t * cache_blocks)
{
  oski_index_t i_start, i_max;
  oski_index_t j_start;

  i_start = FindFirstNonzeroRow (m, ptr, has_unit_diag, min_row);
  if (i_start >= m)
    return 0;
  i_max = MIN (i_start + R, m);

  j_start = FindMinColIndex (m, n, ptr, ind, b, has_unit_diag,
			     i_start, i_max, min_col);
  while (j_start < n)
    {
      oski_index_t j_max = MIN (j_start + C, n);
#if defined(CB_USE_SPARSE_SUBBLOCKS)
      int err = MakeSparseSubblocks (m, n, ptr, ind, val, b, has_unit_diag,
				     i_start, j_start, i_max, j_max,
				     cache_blocks);
#else
      int err = MakeBlock (m, n, ptr, ind, val, b, has_unit_diag,
			   i_start, j_start, i_max, j_max, cache_blocks);
#endif
      if (err)
	return err;
      j_start = FindMinColIndex (m, n, ptr, ind, b, has_unit_diag,
				 i_start, i_max, j_max);
    }
  return ConvertToCSR (m, n, ptr, ind, val, b, has_unit_diag,
		       i_start + R, 0, R, C, cache_blocks);
}

static oski_index_t
FindMinColsBegin (oski_index_t n, const oski_index_t * ptr,
		  const oski_index_t * ind, oski_index_t b, int has_unit_diag,
		  oski_index_t i_start, oski_index_t i_max,
		  oski_index_t j_min, oski_index_t * k_begin)
{
  oski_index_t j_start = n;
  oski_index_t R = i_max - i_start;
  oski_index_t di;

  for (di = 0; di < R; di++)
    {
      oski_index_t i = i_start + di;
      oski_index_t k;
      k_begin[di] = ptr[i + 1] - b;	/* Assume no nz in row i initially */
      for (k = ptr[i] - b; k < ptr[i + 1] - b; k++)
	{
	  oski_index_t j0 = ind[k] - b;
	  if (j0 >= j_min)
	    {			/* Found a non-zero meeting constraint */
	      if (j0 < j_start)
		j_start = j0;
	      k_begin[di] = k;
	      break;
	    }
	}			/* k */
      if (has_unit_diag && i >= j_min && i < j_start)
	j_start = i;
    }				/* di */

  return j_start;
}

static void
FindMaxCols (const oski_index_t * ptr, const oski_index_t * ind,
	     oski_index_t b, int has_unit_diag,
	     oski_index_t i_start, oski_index_t i_max,
	     oski_index_t j_start, oski_index_t j_max,
	     const oski_index_t * k_begin, oski_index_t * k_end)
{
  oski_index_t R = i_max - i_start;
  oski_index_t C = j_max - j_start;
  oski_index_t di;
  for (di = 0; di < R; di++)
    {
      oski_index_t i = i_start + di;
      oski_index_t k;
      k_end[di] = ptr[i + 1] - b;
      for (k = k_begin[di]; k < ptr[i + 1] - b; k++)
	{
	  oski_index_t j = ind[k] - b;
	  if (j >= j_max)
	    {
	      k_end[di] = k;
	      break;
	    }
	}			/* k */
    }				/* di */
}

static oski_index_t
FindMinColsContinue (oski_index_t n,
		     const oski_index_t * ptr, const oski_index_t * ind,
		     oski_index_t b, int has_unit_diag,
		     oski_index_t i_start, oski_index_t i_max,
		     oski_index_t j_min, oski_index_t * k_begin)
{
  oski_index_t j_start = n;
  oski_index_t R = i_max - i_start;
  oski_index_t di;

  for (di = 0; di < R; di++)
    {
      oski_index_t i = i_start + di;
      oski_index_t k = k_begin[di];	/* pick up where we left off */
      k_begin[di] = ptr[i + 1] - b;	/* Assume no nz in row i initially */
      while (k < ptr[i + 1] - b)
	{
	  oski_index_t j0 = ind[k] - b;
	  if (j0 >= j_min)
	    {			/* Found a non-zero meeting constraint */
	      if (j0 < j_start)
		j_start = j0;
	      k_begin[di] = k;
	      break;
	    }
	  k++;
	}			/* k */
      if (has_unit_diag && i >= j_min && i < j_start)
	j_start = i;
    }				/* di */

  return j_start;
}

static void
SetupBlockRowPtrs_Sorted (const oski_index_t * ptr, const oski_index_t * ind,
			  oski_index_t b, int has_unit_diag,
			  oski_index_t i_start, oski_index_t j_min,
			  oski_index_t R, oski_index_t j_max,
			  const oski_index_t * k_begin,
			  const oski_index_t * k_end, oski_index_t * bptr)
{
  oski_index_t di;

  /* First, count number of non-zeros in each row of the submatrix */
  for (di = 0; di < R; di++)
    {
      oski_index_t i = i_start + di;
      if (k_begin[di] < k_end[di] && k_begin[di] >= ptr[i] - b
	  && k_end[di] <= ptr[i + 1] - b)
	{
	  bptr[di + 1] = k_end[di] - k_begin[di];
	}
    }

  /* Account for implicit diagonal entries if necessary */
  if (has_unit_diag)
    {
      for (di = 0; di < R; di++)
	{
	  oski_index_t i = i_start + di;
	  bptr[di + 1] += (j_min <= i && i < j_max);
	}
    }

  for (di = 1; di <= R; di++)
    bptr[di] += bptr[di - 1];
}

static void
CopyValues_Sorted (const oski_index_t * ptr, const oski_index_t * ind,
		   const oski_value_t * val, oski_index_t b,
		   int has_unit_diag, oski_index_t i_start,
		   oski_index_t j_min, oski_index_t R, oski_index_t j_max,
		   const oski_index_t * k_begin, const oski_index_t * k_end,
		   const oski_index_t * bptr, oski_index_t * bind,
		   oski_value_t * bval)
{
  oski_index_t di;
  for (di = 0; di < R; di++)
    {
      oski_index_t i = i_start + di;
      oski_index_t k_start = k_begin[di];
      oski_index_t k_max = k_end[di];
      if (k_start >= ptr[i] - b && k_start < k_max && k_max <= ptr[i + 1] - b)
	{
	  oski_index_t nnz_i = k_max - k_start;
	  oski_index_t kb = bptr[di];
	  oski_index_t dk;
	  /* Copy column indices to local block coordinates */
	  for (dk = 0; dk < nnz_i; dk++)
	    bind[kb + dk] = ind[k_start + dk] - b - j_min;
	  /* Copy non-zero values */
	  oski_CopyMem (bval + kb, val + k_start, oski_value_t, nnz_i);
	}
    }				/* di */
  if (has_unit_diag)
    {
      for (di = 0; di < R; di++)
	{
	  oski_index_t i = i_start + di;
	  if (j_min <= i && i < j_max)
	    {
	      oski_index_t kb = bptr[di + 1] - 1;
	      bind[kb] = i - j_min;
	      VAL_SET_ONE (bval[kb]);
	    }
	}
    }
}

static int
ConvertSubblock_Sorted (const oski_index_t * ptr, const oski_index_t * ind,
			const oski_value_t * val,
			oski_index_t b, int has_unit_diag,
			oski_index_t i_start, oski_index_t j_min,
			oski_index_t i_max, oski_index_t j_max,
			const oski_index_t * k_begin,
			const oski_index_t * k_end,
			simplelist_t * cache_blocks)
{
  oski_index_t *bptr;
  oski_index_t *bind;
  oski_value_t *bval;
  oski_submatCB_t *block;
  oski_index_t R = i_max - i_start;
  int err;
  size_t nnz_block;

  /* Set up row pointers */
  bptr = oski_Malloc (oski_index_t, R + 1);
  if (bptr == NULL)
    return ERR_OUT_OF_MEMORY;
  SetupBlockRowPtrs_Sorted (ptr, ind, b, has_unit_diag,
			    i_start, j_min, R, j_max, k_begin, k_end, bptr);
  nnz_block = (size_t) (bptr[R]);

  /* Copy indices and values */
  err = oski_MultiMalloc (__FILE__, __LINE__, 2,
			  nnz_block * sizeof (oski_index_t), &bind,
			  nnz_block * sizeof (oski_value_t), &bval);
  if (err)
    {
      oski_Free (bptr);
      return ERR_OUT_OF_MEMORY;
    }
  CopyValues_Sorted (ptr, ind, val, b, has_unit_diag,
		     i_start, j_min, R, j_max, k_begin, k_end,
		     bptr, bind, bval);

  /* Package block */
  block = oski_Malloc (oski_submatCB_t, 1);
  if (block == NULL)
    {
      oski_FreeAll (3, bptr, bind, bval);
      return ERR_OUT_OF_MEMORY;
    }

  block->row = i_start;
  block->col = j_min;
  block->num_rows = R;
  block->num_cols = j_max - j_min;
  block->nnz = bptr[R];
  block->num_zero_rows = CountZeroRows (R, bptr);
  block->mat = oski_CreateMatCSR (bptr, bind, bval,
				  block->num_rows, block->num_cols,
				  COPY_INPUTMAT,
				  3,
				  MAT_GENERAL,
				  INDEX_ZERO_BASED, MAT_DIAG_EXPLICIT);
  oski_FreeAll (3, bptr, bind, bval);
  if (block->mat == NULL)
    {
      oski_Free (block);
      return ERR_OUT_OF_MEMORY;
    }
  /* Add to current list of blocks */
  if (!simplelist_Append (cache_blocks, block))
    {
      oski_Free (block);
      return ERR_OUT_OF_MEMORY;
    }
  return 0;
}

/**
 *  \brief Convert a specified submatrix of a CSR matrix into a
 *  single cache block, assuming sorted column indices.
 */
static int
MakeBlock_Sorted (const oski_index_t * ptr, const oski_index_t * ind,
		  const oski_value_t * val, oski_index_t b, int has_unit_diag,
		  oski_index_t i0, oski_index_t j0,
		  oski_index_t i_max, oski_index_t j_max,
		  const oski_index_t * k_begin, const oski_index_t * k_end,
		  simplelist_t * cache_blocks)
{
  int err = ConvertSubblock_Sorted (ptr, ind, val, b, has_unit_diag,
				    i0, j0, i_max, j_max,
				    k_begin, k_end, cache_blocks);
  oski_submatCB_t *block =
    (oski_submatCB_t *) simplelist_GetLastElem (cache_blocks);
  if (err)
    return err;
  if (block != NULL
      && block->mat != NULL && block->num_zero_rows >= (block->num_rows / 2))
    oski_ApplyMatTransforms (block->mat, "return GCSR(InputMat)");
  return 0;
}

/**
 *  \brief Faster but less memory-efficient implementation of
 *  \ref ConvertToCSR() which assumes sorted column indices.
 *
 *  This algorithm reverts to ConvertToCSR() on error.
 */
static int
ConvertToCSR_Sorted (oski_index_t m, oski_index_t n,
		     const oski_index_t * ptr, const oski_index_t * ind,
		     const oski_value_t * val,
		     oski_index_t b, int has_unit_diag,
		     oski_index_t min_row, oski_index_t min_col,
		     oski_index_t R, oski_index_t C,
		     simplelist_t * cache_blocks)
{
  oski_index_t *k_begin;
  oski_index_t *k_end;
  oski_index_t i_start, i_max;
  oski_index_t i;
  int err;

  err = oski_MultiMalloc (__FILE__, __LINE__, 2,
			  (size_t) (sizeof (oski_index_t) * R), &k_begin,
			  (size_t) (sizeof (oski_index_t) * R), &k_end);
  if (err)
    return ERR_OUT_OF_MEMORY;
  oski_ZeroMem (k_begin, sizeof (oski_index_t) * R);

  i_start = FindFirstNonzeroRow (m, ptr, has_unit_diag, min_row);
  i_max = MIN (i_start + R, m);

  /* loop over cache blocks */
  while (i_start < i_max && i_start < m)
    {
      oski_index_t j_start = FindMinColsBegin (n, ptr, ind, b, has_unit_diag,
					       i_start, i_max, min_col,
					       k_begin);
      while (j_start < n)
	{
	  oski_index_t j_max = MIN (j_start + C, n);
	  FindMaxCols (ptr, ind, b, has_unit_diag, i_start, i_max, j_start,
		       j_max, k_begin, k_end);
	  err =
	    MakeBlock_Sorted (ptr, ind, val, b, has_unit_diag, i_start,
			      j_start, i_max, j_max, k_begin, k_end,
			      cache_blocks);
	  if (err)
	    return err;

	  /* Next block column */
	  oski_CopyMem (k_begin, k_end, oski_index_t, R);
	  j_start = FindMinColsContinue (n, ptr, ind, b, has_unit_diag,
					 i_start, i_max, j_max, k_begin);
	}			/* j_start */

      i_start = FindFirstNonzeroRow (m, ptr, has_unit_diag, i_max);
      i_max = MIN (i_start + R, m);
    }				/* i_start */

  oski_FreeAll (2, k_begin, k_end);
  return 0;
}

/**
 *  \brief
 *
 *  Example usage:
 *  \code
 *    oski_CreateMatReprFromCSR( mat, props, r, c );
 *  \endcode
 *  where \f$R\times C\f$ is the desired cache block size.
 */
void *
oski_CreateMatReprFromCSR (const oski_matCSR_t * mat,
			   const oski_matcommon_t * props, ...)
{
  /* Store a full copy of the matrix if it uses half-storage */
  const oski_matCSR_t *A_full = NULL;
  int did_copy = 0;
  int err;
  oski_matCB_t *A = NULL;
  oski_index_t row_block_size;
  oski_index_t col_block_size;
  va_list ap;

  oski_PrintDebugMessage (1, "Creating CB() matrix...");

  if (mat == NULL || props == NULL)
    return NULL;

  did_copy = ExpandSymm (mat, props, (oski_matCSR_t **) (&A_full));
  if (A_full == NULL)
    {
      OSKI_ERR (oski_CreateMatReprFromCSR, ERR_OUT_OF_MEMORY);
      return NULL;
    }

  va_start (ap, props);
  row_block_size = va_arg (ap, oski_index_t);
  col_block_size = va_arg (ap, oski_index_t);
  va_end (ap);

  A = oski_Malloc (oski_matCB_t, 1);
  if (A == NULL)
    return NULL;
  A->cache_blocks = simplelist_Create ();
  if (A->cache_blocks == NULL)
    {
      OSKI_ERR (oski_CreateMatReprFromCSR, ERR_OUT_OF_MEMORY);
      oski_DestroyMatRepr (A);
      return NULL;
    }

  if (A_full->has_sorted_indices)
    err = ConvertToCSR_Sorted (props->num_rows, props->num_cols,
			       A_full->ptr, A_full->ind, A_full->val,
			       A_full->base_index,
			       A_full->has_unit_diag_implicit,
			       0, 0, row_block_size, col_block_size,
			       A->cache_blocks);
  else
    err = -1;
  if (err)
    err = ConvertToCSR (props->num_rows, props->num_cols,
			A_full->ptr, A_full->ind, A_full->val,
			A_full->base_index, A_full->has_unit_diag_implicit,
			0, 0, row_block_size, col_block_size,
			A->cache_blocks);
  if (err)
    {
      OSKI_ERR (oski_CreateMatReprFromCSR, err);
      simplelist_Destroy (A->cache_blocks);
      oski_Free (A);
      A = NULL;
    }
  else
    {
      oski_PrintDebugMessage (3, "Created %d cache blocks.",
			      (int) simplelist_GetLength (A->cache_blocks));
    }

  if (did_copy)
    {
      oski_DestroyMatRepr_funcpt func_Destroy =
	OSKI_MATTYPE_METHOD ("CSR", DestroyMatRepr);
      oski_PrintDebugMessage (2, "Destroying full-storage copy");
      if (func_Destroy != NULL)
	(*func_Destroy) ((void *) A_full);
      /* \todo Should return an error if CSR's destroy not found */
    }

  oski_PrintDebugMessage (2, "Done.");
  return A;
}

oski_matCSR_t *
oski_ConvertMatReprToCSR (const void *mat, const oski_matcommon_t * props)
{
  OSKI_ERR (oski_ConvertMatReprToCSR, ERR_NOT_IMPLEMENTED);
  return NULL;
}

/* -----------------------------------------------------------------
 *  Lua transformations.
 */

#include <oski/xforms_internal.h>

/**
 *  \brief
 *
 *  The CB implementation expects the following arguments on
 *  the stack:
 *    -# Destination matrix type ('MBCSR')
 *    -# Source matrix ('CSR')
 *    -# Row block size ('r')
 *    -# Column block size ('c')
 */
int
oski_CreateLuaMatReprFromCSR (lua_State * L)
{
  return oski_CreateLuaMatReprGeneric2IndexFromCSR (L, "CB");
}

/* eof */
