/**
 *  \file src/MBCSR/convert.c
 *  \brief  Conversion between CSR and MBCSR format.
 *  \ingroup MATTYPE_MBCSR
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/modloader.h>
#include <oski/matrix.h>

#include <oski/CSR/format.h>
#include <oski/MBCSR/format.h>
#include <oski/MBCSR/module.h>

/** \brief Returns a non-zero value <==> j is in [j_min, j_max] */
#define IS_BOUNDED(j, j_min, j_max)  ((j) >= (j_min) && (j) <= (j_max))

/**
 *  \brief Scatter...
 *
 *  \param[in] ptr  CSR row pointers, in the base specified by 'indbase'.
 *  \param[in] ind  CSR column indices, in the base specified by 'indbase'.
 *  \param[in] val  CSR explicitly stored values.
 *  \param[in] n  Maximum number of columns.
 *  \param[in] indbase  Index base (0 or 1).
 *  \param[in] i0  Index of the first row in the block row to scatter,
 *  in the base specified by 'indbase'.
 *  \param[in] r  Row block size (i.e., number of rows in the block row
 *  starting at 'i0' to consider).
 *  \param[in] c  Column block size.
 *  \param[in] d0  Index of the first column of the 'r x r' diagonal
 *  block, in the base specified by 'indbase'.
 *  \param[in,out] has_block_col  Array of flags indicating whether
 *  a given block exists in the block row being scattered.
 *  \param[in,out] blocks  Values of each block in the block row.
 *  \param[in,out] diag  Values of the diagonal block.
 *
 *  \pre ptr != NULL && has_block_col != NULL
 *
 *  \todo Update documentation for this routine.
 */
static oski_index_t
ScatterBlockRow (const oski_index_t * ptr, const oski_index_t * ind,
		 const oski_value_t * val /* optional */ ,
		 oski_index_t n, oski_index_t indbase,
		 oski_index_t i0, oski_index_t r, oski_index_t c,
		 oski_index_t d0,
		 /* Required return parameters: */
		 oski_index_t * has_block_col,
		 /* Optional return parameters: */
		 oski_index_t * inds, oski_value_t * blocks,
		 oski_value_t * diag)
{
  oski_index_t di;		/* row offset from the base row, i0 */
  oski_index_t num_blocks = 0;	/* # of off-diagonal blocks */

  assert (ptr != NULL);
  assert (has_block_col != NULL);
  assert (IS_BOUNDED (d0 + r - indbase, r, n));

  /* Loop over rows within block of r rows beginning at i0 */
  for (di = 0; di < r; di++)
    {
      oski_index_t i = i0 - indbase + di;	/* row index (0-based) */
      oski_index_t k;		/* offset of non-zero (0-based) */

      /* Loop over non-zeros in this row */
      for (k = ptr[i] - indbase; k < ptr[i + 1] - indbase; k++)
	{
	  oski_index_t j = ind[k] - indbase;	/* column index (0-based) */

	  /* if not in diagonal block... */
	  if (!IS_BOUNDED (j, d0 - indbase, d0 - indbase + r - 1))
	    {
	      oski_index_t J = j / c;	/* block col index (0-based) */
	      oski_index_t j0 = J * c;	/* 1st col of block col J (0-based) */

	      /* Adjust block's starting column j0 if needed */
	      if (j0 + c > n)
		j0 = n - c;

	      if (has_block_col[J] == 0)
		{
		  if (inds != NULL)
		    inds[num_blocks] = j0;
		  has_block_col[J] = ++num_blocks;	/* 1-based! */
		}
	      if (blocks != NULL && val != NULL)
		{
		  oski_index_t db = has_block_col[J] - 1;	/* block offset */
		  VAL_INC (blocks[db * r * c + di * c + j - j0], val[k]);
		}
	    }
	  else if (diag != NULL)
	    {			/* is in diagonal block */
	      oski_index_t dj = j - (d0 - indbase);
	      VAL_INC (diag[di * r + dj], val[k]);
	    }
	}			/* k */
    }				/* di */
  return num_blocks;
}

/**
 *  \brief Unscatter...
 *
 *  \param[in] scatter_op  SCATTER_RESET or SCATTER_SET.
 *  \param[in] indbase  Index base (0 or 1).
 *  \param[in] ptr  CSR row pointers, in the base specified by 'indbase'.
 *  \param[in] ind  CSR column indices, in the base specified by 'indbase'.
 *  \param[in] val  CSR explicitly stored values.
 *  \param[in] i0  Index of the first row in the block row to scatter,
 *  in the base specified by 'indbase'.
 *  \param[in] r  Row block size (i.e., number of rows in the block row
 *  starting at 'i0' to consider).
 *  \param[in] c  Column block size.
 *  \param[in] d0  Index of the first column of the 'r x r' diagonal
 *  block, in the base specified by 'indbase'.
 *  \param[in,out] has_block_col  Array of flags indicating whether
 *  a given block exists in the block row being scattered.
 *  \param[in,out] blocks  Values of each block in the block row.
 *  \param[in,out] diag  Values of the diagonal block.
 *
 *  \pre ptr != NULL && has_block_col != NULL
 */
static void
UnscatterBlockRow (const oski_index_t * ptr, const oski_index_t * ind,
		   oski_index_t indbase,
		   oski_index_t i0, oski_index_t r, oski_index_t c,
		   oski_index_t d0,
		   /* Return parameters: */
		   oski_index_t * has_block_col)
{
  oski_index_t di;		/* row offset from the base row, i0 */

  assert (has_block_col != NULL);

  /* Loop over rows within block of r rows beginning at i0 */
  for (di = 0; di < r; di++)
    {
      oski_index_t i = i0 - indbase + di;	/* row index (0-based) */
      oski_index_t k;		/* offset of non-zero (0-based) */
      for (k = ptr[i] - indbase; k < ptr[i + 1] - indbase; k++)
	{
	  oski_index_t j = ind[k] - indbase;	/* column index (0-based) */
	  oski_index_t J = j / c;	/* block column index (0-based) */

	  /* if not in diagonal block... */
	  if (!IS_BOUNDED (j, d0 - indbase, d0 - indbase + r - 1))
	    has_block_col[J] = 0;
	}			/* k */
    }				/* di */
}

/**
 *  \brief Allocate space for a given number of blocks, including their
 *  indices and diagonal blocks. All returned arrays are initialized
 *  to 0.
 *
 *  \returns A non-zero value and sets the output pointers
 *  <==> the allocation was successful. Otherwise, returns 0 and
 *  leaves the output pointers unchanged.
 */
static int
AllocateBlocks (oski_index_t num_block_rows, oski_index_t num_blocks,
		oski_index_t r, oski_index_t c,
		oski_index_t ** p_ind, oski_value_t ** p_blocks,
		oski_value_t ** p_diagblocks)
{
  oski_index_t *ind = NULL;
  oski_value_t *blocks = NULL;
  oski_value_t *diagblocks = NULL;

  if (num_blocks > 0)
    {
      ind = oski_Malloc (oski_index_t, num_blocks);
      if (ind == NULL)
	return 0;

      blocks = oski_Malloc (oski_value_t, num_blocks * r * c);
      if (blocks == NULL)
	{
	  oski_Free (ind);
	  return 0;
	}
    }

  if (num_block_rows > 0)
    {
      diagblocks = oski_Malloc (oski_value_t, num_block_rows * r * r);
      if (diagblocks == NULL)
	{
	  oski_Free (blocks);
	  oski_Free (ind);
	  return 0;
	}
    }

  oski_ZeroMem (ind, sizeof (oski_index_t) * num_blocks);
  oski_ZeroMem (blocks, sizeof (oski_value_t) * num_blocks * r * c);
  oski_ZeroMem (diagblocks, sizeof (oski_value_t) * num_block_rows);

  if (p_ind != NULL || !num_blocks)
    *p_ind = ind;
  else
    oski_Free (ind);
  if (p_blocks != NULL || !num_blocks)
    *p_blocks = blocks;
  else
    oski_Free (blocks);
  if (p_diagblocks != NULL || !num_block_rows)
    *p_diagblocks = diagblocks;
  else
    oski_Free (diagblocks);
  return 1;
}

/**
 *  \brief Given an input matrix \f$A\f$ stored in CSR format, convert a
 *  submatrix \f$A(I,:)\f$ to \f$r\times c\f$ MBCSR format.
 *
 *  The caller must also select an arbitrary starting column of
 *  \f$A(I,:)\f$ to be the starting point for the block diagonal of
 *  \f$r\times r\f$ blocks.
 *
 *  \param[in] M Number of block rows in the subset \f$I\f$, i.e.,
 *  \f$I\f$ consists of \f$M\cdot r\f$ rows.
 *  \param[in] n Number of columns of \f$A\f$.
 *  \param[in] i0 Starting row in the index range \f$I\f$.
 *  \param[in] d0 First column index of the block diagonal.
 *  \param[in] ptr Row pointers for \f$A\f$.
 *  \param[in] ind Column indices for \f$A\f$.
 *  \param[in] val Explicitly stored values for \f$A\f$.
 *  \param[in] indbase Index base (0 or 1) for: i0, d0, ptr, ind.
 *  \param[in] r Off-diagonal row block size, \f$r\f$.
 *  \param[in] c Off-diagonal column block size, \f$c\f$.
 *  \param[in,out] p_bptr Pointer to a pointer in which to store
 *  a newly allocated array of block-row pointers. The caller may
 *  set this pointer to NULL if none are desired.
 *  \param[in,out] p_bind Pointer to a pointer in which to store
 *  a newly allocated array of block column indices. The caller may
 *  set this pointer to NULL if none are desired.
 *  \param[in,out] p_bval Pointer to a pointer in which to store
 *  a newly allocated array of stored-value off-diagonal blocks.
 *  The caller may set this pointer to NULL if none are desired.
 *  \param[in,out] p_bdiag Pointer to a pointer in which to store
 *  a newly allocated array of block diagonal values. The caller
 *  may set this pointer to NULL if none are desired.
 *
 *  \returns 0 if any error occurs (typically on memory allocation
 *  failure) and leaves all output pointers unchanged, or 1 if
 *  successful with all non-NULL output pointers initialized.
 *
 *  \pre \f$r\f$ divides \f$m\f$.
 */
static int
ConvertSubmatFromCSR (
		       /* Input matrix */
		       oski_index_t M, oski_index_t n, oski_index_t i0,
		       oski_index_t d0, const oski_index_t * ptr,
		       const oski_index_t * ind, const oski_value_t * val,
		       oski_index_t indbase,
		       /* MBCSR submat output block size */
		       oski_index_t r, oski_index_t c,
		       /* Output submatrix */
		       oski_index_t ** p_bptr,
		       oski_index_t ** p_bind,
		       oski_value_t ** p_bval, oski_value_t ** p_bdiag)
{
  oski_index_t N = c ? ((n + c) / c) : 0;	/* max # of block columns */
  oski_index_t *bptr = NULL;
  oski_index_t *bind = NULL;
  oski_value_t *bval = NULL;
  oski_value_t *bdiag = NULL;
  oski_index_t *has_block_col = NULL;	/* Auxiliary storage */
  oski_index_t I0 = 0;

  bptr = oski_Malloc (oski_index_t, M + 1);
  if (bptr == NULL)
    {
      oski_Free (has_block_col);
      return 0;
    }
  bptr[0] = 0;

  /* Quick return */
  if (M == 0)
    {
      if (p_bptr != NULL)
	*p_bptr = bptr;
      else
	oski_Free (bptr);
      if (p_bind != NULL)
	*p_bind = NULL;
      if (p_bval != NULL)
	*p_bval = NULL;
      if (p_bdiag != NULL)
	*p_bdiag = NULL;
      return 1;
    }

  /* has_block_col[J] != 0 <==> block column J exists */
  has_block_col = oski_Malloc (oski_index_t, N);
  if (N && has_block_col == NULL)
    return 0;
  oski_ZeroMem (has_block_col, sizeof (oski_index_t) * N);

  /* Setup block row pointers, i.e., count the number of
   * off-diagonal blocks needed in each block row.
   */
  for (I0 = 0; I0 < M; I0++)
    {
      oski_index_t row_start = i0 + I0 * r;
      oski_index_t diag_start = d0 + I0 * r;
      oski_index_t num_blocks = ScatterBlockRow (ptr, ind, NULL, n, indbase,
						 row_start, r, c, diag_start,
						 has_block_col, NULL, NULL,
						 NULL);
      bptr[I0 + 1] = bptr[I0] + num_blocks;
      UnscatterBlockRow (ptr, ind, indbase,
			 row_start, r, c, diag_start, has_block_col);
    }

  /* Allocate blocks */
  if (!AllocateBlocks (M, bptr[M], r, c, &bind, &bval, &bdiag))
    {
      oski_Free (bptr);
      oski_Free (has_block_col);
      return 0;
    }

  /* Copy non-zero values in submatrix to blocked format */
  for (I0 = 0; I0 < M; I0++)
    {
      oski_index_t *inds = bind + bptr[I0];
      oski_value_t *blocks = bval + bptr[I0] * r * c;
      oski_value_t *diagblock = bdiag + I0 * r * r;
      oski_index_t row_start = i0 + I0 * r;
      oski_index_t diag_start = d0 + I0 * r;
      oski_index_t num_blocks = ScatterBlockRow (ptr, ind, val, n, indbase,
						 row_start, r, c, diag_start,
						 has_block_col, inds, blocks,
						 diagblock);
      assert (bptr[I0 + 1] - bptr[I0] == num_blocks);
      UnscatterBlockRow (ptr, ind, indbase,
			 row_start, r, c, diag_start, has_block_col);
    }

  /* Clean-up and return */
  if (p_bdiag != NULL)
    *p_bdiag = bdiag;
  else
    oski_Free (bdiag);
  if (p_bval != NULL)
    *p_bval = bval;
  else
    oski_Free (bval);
  if (p_bind != NULL)
    *p_bind = bind;
  else
    oski_Free (bind);
  if (p_bptr != NULL)
    *p_bptr = bptr;
  else
    oski_Free (bptr);
  oski_Free (has_block_col);
  return 1;
}

/**
 *  \brief Given a packed array of \f$M\f$ \f$b\times b\f$ blocks,
 *  sets the diagonal entries of each block to 1.
 *
 *  \pre If num_blocks > 0, then blocks != NULL.
 */
static void
SetUnitDiag (oski_index_t num_blocks, oski_index_t block_size,
	     oski_value_t * blocks)
{
  oski_index_t I;
  assert (num_blocks <= 0 || blocks != NULL);
  for (I = 0; I < num_blocks; I++)
    {
      oski_index_t i;
      for (i = 0; i < block_size; i++)
	VAL_SET_ONE (blocks
		     [I * block_size * block_size + i * block_size + i]);
    }				/* I */
}

/**
 *  \brief Convert the portions of the CSR matrix that correspond
 *  to partitions \f$A_1\f$ and \f$A_2\f$ in MBCSR format.
 *
 *  \returns The starting row index (0-based) in the input
 *  CSR data structure where partition \f$A_3\f$ should
 *  begin. The caller should call the BCSR routine to
 *  convert this part of the matrix.
 */
static int
ConvertFromCSR (
		 /* Input matrix */
		 oski_index_t m, oski_index_t n,
		 const oski_index_t * ptr, const oski_index_t * ind,
		 const oski_value_t * val,
		 oski_index_t indbase, oski_index_t has_unit_diag_implicit,
		 /* MBCSR output block size */
		 oski_index_t r, oski_index_t c,
		 /* Output partition \f$A_1\f$ */
		 oski_index_t * p_M1,
		 oski_index_t ** p_ptr1,
		 oski_index_t ** p_ind1,
		 oski_value_t ** p_val1, oski_value_t ** p_diag1,
		 /* Output partition \f$A_2\f$ */
		 oski_index_t * p_M2,
		 oski_index_t * p_r2,
		 oski_index_t ** p_ptr2,
		 oski_index_t ** p_ind2,
		 oski_value_t ** p_val2, oski_value_t ** p_diag2,
		 /* Output starting row index for \f$A_3\f$ */
		 oski_index_t * p_num_rows_left)
{
  oski_index_t min_dim = (m <= n) ? m : n;

  oski_index_t M1 = min_dim / r;
  oski_index_t *ptr1 = NULL;
  oski_index_t *ind1 = NULL;
  oski_value_t *val1 = NULL;
  oski_value_t *diag1 = NULL;

  oski_index_t r2 = min_dim % r;
  oski_index_t M2 = (r2 > 0);
  oski_index_t *ptr2 = NULL;
  oski_index_t *ind2 = NULL;
  oski_value_t *val2 = NULL;
  oski_value_t *diag2 = NULL;

  int is_ok = 0;

  is_ok = ConvertSubmatFromCSR (M1, n, indbase, indbase,
				ptr, ind, val, indbase, r, c, &ptr1, &ind1,
				&val1, &diag1);
  if (!is_ok)
    return 0;

  is_ok = ConvertSubmatFromCSR (M2, n, indbase + M1 * r, indbase + M1 * r,
				ptr, ind, val, indbase, r2, c, &ptr2, &ind2,
				&val2, &diag2);
  if (!is_ok)
    {
      oski_Free (diag1);
      oski_Free (val1);
      oski_Free (ind1);
      oski_Free (ptr1);
      return 0;
    }

  if (has_unit_diag_implicit)
    {
      SetUnitDiag (M1, r, diag1);
      SetUnitDiag (M2, r2, diag2);
    }

  /* Return */
  if (p_M1 != NULL)
    *p_M1 = M1;
  if (p_ptr1 != NULL)
    *p_ptr1 = ptr1;
  else
    oski_Free (ptr1);
  if (p_ind1 != NULL || !n)
    *p_ind1 = ind1;
  else
    oski_Free (ind1);
  if (p_val1 != NULL || !n)
    *p_val1 = val1;
  else
    oski_Free (val1);
  if (p_diag1 != NULL)
    *p_diag1 = diag1;
  else
    oski_Free (diag1);
  if (p_M2 != NULL)
    *p_M2 = M2;
  if (p_r2 != NULL)
    *p_r2 = r2;
  if (p_ptr2 != NULL)
    *p_ptr2 = ptr2;
  else
    oski_Free (ptr2);
  if (p_ind2 != NULL || !n)
    *p_ind2 = ind2;
  else
    oski_Free (ind2);
  if (p_val2 != NULL || !n)
    *p_val2 = val2;
  else
    oski_Free (val2);
  if (p_diag2 != NULL)
    *p_diag2 = diag2;
  else
    oski_Free (diag2);
  if (!n)
    *p_num_rows_left = 0;
  else if (p_num_rows_left != NULL)
    *p_num_rows_left = (m > n) ? (m - n) : 0;
  return 1;
}

/**
 *  \brief Initializes the cached module name for an MBCSR submatrix.
 */
static void
SetModuleName (oski_submatMBCSR_t * B)
{
  if (B == NULL)
    return;

  B->mod_cached = NULL;
  B->mod_name = oski_MakeModuleName ("mat", "MBCSR",
				     OSKI_IND_ID, OSKI_VAL_ID);
  oski_PrintDebugMessage (2, "Cached MBCSR module name: %s",
			  B->mod_name !=
			  NULL ? B->mod_name : "(ERROR: unknown?)");
}

/**
 *  \brief Given a CSR matrix \f$A\f$, compute a shallow copy
 *  corresponding to a subset of the rows of \f$A\f$.
 *
 *  \pre A != NULL && A_sub != NULL
 */
static void
InitSubmatCSR (const oski_matCSR_t * A, const oski_matcommon_t * props,
	       oski_index_t row_start, oski_index_t num_rows,
	       oski_matCSR_t * A_sub, oski_matcommon_t * props_sub)
{
  oski_index_t row_offset = 0;

  assert (A != NULL);
  assert (A_sub != NULL);
  assert (props != NULL);
  assert (props_sub != NULL);

  row_offset = row_start - A->base_index;
  oski_CopyMem (A_sub, A, oski_matCSR_t, 1);
  A_sub->is_shared = 1;
  A_sub->ptr += row_offset;
  A_sub->stored.is_upper = A_sub->stored.is_lower = 0;
  A_sub->has_unit_diag_implicit = 0;

  oski_ZeroMem (props_sub, sizeof (oski_matcommon_t));
  props_sub->num_rows = num_rows;
  props_sub->num_cols = props->num_cols;
  props_sub->num_nonzeros = A->ptr[props->num_rows] - A->ptr[row_offset];
}

/**
 *  \brief Given a CSR matrix \f$A\f$, convert all the rows
 *  beginning at some row \f$i\f$ to \f$r\times c\f$ BCSR format.
 */
static int
ConvertCSRLeftoverRows (const oski_matCSR_t * A,
			const oski_matcommon_t * props,
			oski_index_t row_start, oski_index_t num_rows,
			oski_index_t r, oski_index_t c, oski_matBCSR_t ** p_B)
{
  /* Stores a subset of the rows of A */
  oski_matCSR_t A_sub;
  oski_matcommon_t props_sub;

  /* Stores the new BCSR representation */
  oski_matBCSR_t *B = NULL;
  oski_CreateMatReprFromCSR_funcpt func_Convert;

  if (p_B == NULL)
    return 1;
  if (num_rows == 0)
    {
      p_B = NULL;
      return 1;
    }
  assert (A != NULL && props != NULL);

  func_Convert = OSKI_MATTYPE_METHOD ("BCSR", CreateMatReprFromCSR);
  if (func_Convert == NULL)
    return 0;

  InitSubmatCSR (A, props, row_start, num_rows, &A_sub, &props_sub);
  B = (*func_Convert) (&A_sub, &props_sub, r, c);

  if (B == NULL)
    return 0;
  *p_B = B;

  return 1;
}

/** \brief Direction option for \ref MirrorDiagBlocks(). */
typedef enum
{
  NO_MIRROR = 0,
  LOWER_TO_UPPER = 1,
  LOWER_TO_UPPER_CONJ = 2,
  UPPER_TO_LOWER = 3,
  UPPER_TO_LOWER_CONJ = 4
} mirrordir_t;

/**
 *  \brief For each diagonal block \f$B\f$ of a matrix which is either
 *  symmetric or Hermitian, store \f$B\f$ using full-storage such
 *  that \f$B = \mathrm{op}(B)\f$ where \f$\mathrm{op}(B) \in \{B^T, B^H\}\f$,
 *  as appropriate.
 *
 *  \param[in] blocks Array of r x r blocks.
 *  \param[in] num_blocks Number of blocks in 'blocks'.
 *  \param[in] b Block size.
 *  \param[in] op Desired transpose operation (OP_TRANS, OP_CONJ_TRANS).
 *  \param[in] dir Direction in which to mirror the block.
 *
 *  \pre The input parameter dir is valid if A is not NULL.
 *  If A is NULL or has no diagonal, or if op is not one of
 *  {OP_TRANS, OP_CONJ_TRANS}, returns taking no action.
 */
static void
MirrorDiagBlocks (mirrordir_t dir,
		  oski_value_t * blocks, oski_index_t num_blocks,
		  oski_index_t b)
{
  oski_index_t I;
  oski_value_t *block;

  int do_L2U = 0;
  int do_conj = 0;

  if (blocks == NULL || num_blocks <= 0)
    return;

  switch (dir)
    {
    case LOWER_TO_UPPER:
      do_L2U = 1;
      do_conj = 0;
      break;
    case LOWER_TO_UPPER_CONJ:
      do_L2U = 1;
      do_conj = 1;
      break;
    case UPPER_TO_LOWER:
      do_L2U = 0;
      do_conj = 0;
      break;
    case UPPER_TO_LOWER_CONJ:
      do_L2U = 0;
      do_conj = 1;
      break;
    default:
      return;
    }

  if (do_conj)
    {
      for (I = 0, block = blocks; I < num_blocks; I++, block += b * b)
	{
	  oski_index_t i;
	  for (i = 0; i < b; i++)
	    {
	      oski_index_t j_start = do_L2U ? 0 : (i + 1);
	      oski_index_t j_stop = do_L2U ? i : b;
	      oski_index_t j;
	      for (j = j_start; j != j_stop; j++)
		{
		  VAL_ASSIGN_CONJ (block[j * b + i], block[i * b + j]);
		}		/* j */
	    }			/* i */
	}			/* I */
    }
  else				/* !do_conj */
    for (I = 0, block = blocks; I < num_blocks; I++, block += b * b)
      {
	oski_index_t i;
	for (i = 0; i < b; i++)
	  {
	    oski_index_t j_start = do_L2U ? 0 : (i + 1);
	    oski_index_t j_stop = do_L2U ? i : b;
	    oski_index_t j;
	    for (j = j_start; j != j_stop; j++)
	      {
		VAL_ASSIGN (block[j * b + i], block[i * b + j]);
	      }			/* j */
	  }			/* i */
      }				/* I */
}

static oski_index_t
CalcNnzStored_CSR (oski_index_t m, const oski_matCSR_t * A)
{
  return (A != NULL) ? A->ptr[m] : 0;
}

static oski_index_t
CalcNnzStored_BCSR (const oski_matBCSR_t * A)
{
  if (A == NULL)
    return 0;
  else
    {
      oski_index_t M = A->num_block_rows;
      oski_index_t r = A->row_block_size;
      oski_index_t c = A->col_block_size;
      return (A->bptr[M] * r * c) + CalcNnzStored_BCSR (A->leftover);
    }
}

static oski_index_t
CalcNnzStored_submatMBCSR (const oski_submatMBCSR_t * A)
{
  if (A == NULL)
    return 0;
  return (A->num_block_rows * A->r * A->r) +
    (A->bptr[A->num_block_rows] * A->r * A->c);
}

static oski_index_t
CalcNnzStored_MBCSR (const oski_matMBCSR_t * A)
{
  if (A == NULL)
    return 0;
  else
    return CalcNnzStored_submatMBCSR (&(A->A1))
      + CalcNnzStored_submatMBCSR (&(A->A2)) + CalcNnzStored_BCSR (A->p_A3);
}

static double
CalcFillRatio (const oski_matMBCSR_t * A, const oski_matCSR_t * A_orig,
	       const oski_matcommon_t * props)
{
  oski_index_t nnz_stored;
  oski_index_t nnz_stored_orig;

  if (props == NULL)
    return 1.0;

  nnz_stored = CalcNnzStored_MBCSR (A);
  nnz_stored_orig = CalcNnzStored_CSR (props->num_rows, A_orig);
  return (double) nnz_stored / nnz_stored_orig;
}

/**
 *  \brief
 *
 *  The variable argument list must contain the following
 *  parameters, in the order listed:
 *    -# \f$r\f$: The desired row block size.
 *    -# \f$c\f$: The desired column block size.
 *
 *  \returns A pointer to a new oski_matMBCSR_t object
 *  containing the input matrix in MBCSR format. If an
 *  error occurs, returns NULL.
 *
 *  \pre The variable part of the argument list has been
 *  specified correctly, and \f$1 \leq r, c \leq 12\f$.
 */
void *
oski_CreateMatReprFromCSR (const oski_matCSR_t * mat,
			   const oski_matcommon_t * props, ...)
{
  va_list ap;
  oski_index_t r;
  oski_index_t c;
  oski_matMBCSR_t *A = NULL;
  oski_index_t num_rows_A3 = 0;
  int is_ok;

  if (mat == NULL || props == NULL)
    return NULL;

  /* Get block size, r x c */
  va_start (ap, props);
  r = va_arg (ap, oski_index_t);
  c = va_arg (ap, oski_index_t);
  if (r < 1 || r > 12)
    return NULL;
  if (c < 1 || c > 12)
    return NULL;
  if (r > props->num_rows)
    r = props->num_rows;
  if (c > props->num_cols)
    c = props->num_cols;
  oski_PrintDebugMessage (1, "Creating MBCSR(r=%d, c=%d) ...", r, c);

  /* Create space for this matrix */
  A = oski_Malloc (oski_matMBCSR_t, 1);
  if (A == NULL)
    return NULL;

  /* Copy the input matrix symmetric storage properties */
  A->stored.is_upper = mat->stored.is_upper;
  A->stored.is_lower = mat->stored.is_lower;

  /* Convert! */
  is_ok = ConvertFromCSR (props->num_rows, props->num_cols,
			  mat->ptr, mat->ind, mat->val, mat->base_index,
			  mat->has_unit_diag_implicit, r, c,
			  &(A->A1.num_block_rows),
			  &(A->A1.bptr), &(A->A1.bind), &(A->A1.bval),
			  &(A->A1.bdiag), &(A->A2.num_block_rows), &(A->A2.r),
			  &(A->A2.bptr), &(A->A2.bind), &(A->A2.bval),
			  &(A->A2.bdiag), &num_rows_A3);
  if (is_ok)
    {

      mirrordir_t op_mirror = NO_MIRROR;

      A->A1.offset = 0;
      A->A1.r = r;
      A->A1.c = c;
      A->A2.offset = A->A1.num_block_rows * r;
      A->A2.c = c;

      /* Mirror diagonal blocks if necessary */
      if (props->pattern.is_symm)
	{
	  if (A->stored.is_upper && !A->stored.is_lower)
	    op_mirror = UPPER_TO_LOWER;
	  else if (A->stored.is_lower && !A->stored.is_upper)
	    op_mirror = LOWER_TO_UPPER;
	}
      else if (props->pattern.is_herm)
	{
	  if (A->stored.is_upper && !A->stored.is_lower)
	    op_mirror = UPPER_TO_LOWER_CONJ;
	  else if (A->stored.is_lower && !A->stored.is_upper)
	    op_mirror = LOWER_TO_UPPER_CONJ;
	}
      MirrorDiagBlocks (op_mirror,
			A->A1.bdiag, A->A1.num_block_rows, A->A1.r);
      MirrorDiagBlocks (op_mirror,
			A->A2.bdiag, A->A2.num_block_rows, A->A2.r);

      /* Cache module names for subsequent kernel loads */
      SetModuleName (&(A->A1));
      SetModuleName (&(A->A2));

      /* Convert rest of the matrix, if any */
      is_ok = ConvertCSRLeftoverRows (mat, props,
				      props->num_rows - num_rows_A3 +
				      mat->base_index, num_rows_A3, r, c,
				      &(A->p_A3));
      if (!is_ok)
	{
	  oski_Free (A->A1.bptr);
	  oski_Free (A->A1.bind);
	  oski_Free (A->A1.bval);
	  oski_Free (A->A1.bdiag);
	  oski_Free (A->A2.bptr);
	  oski_Free (A->A2.bind);
	  oski_Free (A->A2.bval);
	  oski_Free (A->A2.bdiag);
	}
    }

  if (!is_ok)
    {
      oski_Free (A);
      A = NULL;
    }
  else
    {
      oski_PrintDebugMessage (2, "Matrix is %s",
			      props->pattern.is_symm ? "symmetric"
			      : props->pattern.
			      is_herm ? "Hermitian" : "non-symmetric");
      oski_PrintDebugMessage (2, "Stored pattern: %s",
			      A->stored.is_upper ==
			      A->stored.is_lower ? "Full" : A->stored.
			      is_upper ? "Upper triangular" :
			      "Lower triangular");
      oski_PrintDebugMessage (2, "Fill ratio: %.3f",
			      CalcFillRatio (A, mat, props));
      oski_PrintDebugMessage (2, "A_1: %d*%d x %d [offset=%d, c=%d]",
			      A->A1.num_block_rows, A->A1.r, props->num_cols,
			      A->A1.offset, A->A1.c);
      oski_PrintDebugMessage (3, "<%p, %p, %p, %p>", A->A1.bptr, A->A1.bind,
			      A->A1.bval, A->A1.bdiag);
      oski_PrintDebugMessage (2, "A_2: %d*%d x %d [offset=%d, c=%d]",
			      A->A2.num_block_rows, A->A2.r, props->num_cols,
			      A->A2.offset, A->A2.c);
      oski_PrintDebugMessage (3, "<%p, %p, %p, %p>", A->A2.bptr, A->A2.bind,
			      A->A2.bval, A->A2.bdiag);
      oski_PrintDebugMessage (2, "A_3: %p", A->p_A3);
    }

  /* Set default kernel-enable settings */
  oski_EnableMBCSRMatTransMatMult (A, 1);
  oski_EnableMBCSRMatMultAndMatMult (A, 1);
  oski_EnableMBCSRMatMultAndMatTransMult (A, 1);
  oski_EnableMBCSRMatPowMult (A, 1);
  oski_EnableMBCSRMatTransPowMult (A, 1);

  oski_PrintDebugMessage (2, "Returning %p.", (void *) A);
  return (void *) A;
}

oski_matCSR_t *
oski_ConvertMatReprToCSR (const void *mat, const oski_matcommon_t * props)
{
  OSKI_ERR_NOT_IMPLEMENTED (oski_ConvertMatReprToCSR,
			    "Convert MBCSR matrix to CSR");
  return NULL;
}

static int
CopySubmat (const oski_submatMBCSR_t * A, oski_submatMBCSR_t * B)
{
  oski_index_t M;
  int err = 0;

  if (A == NULL || B == NULL)
    return 0;

  M = A->num_block_rows;

  oski_CopyMem (B, A, oski_submatMBCSR_t, 1);
  B->bptr = NULL;
  B->bind = NULL;
  B->bval = NULL;
  B->bdiag = NULL;
  B->mod_name = NULL;

  B->bptr = oski_Malloc (oski_index_t, M + 1);
  if (B->bptr == NULL)
    err = 1;
  else
    oski_CopyMem (B->bptr, A->bptr, oski_index_t, M + 1);

  if (!err && A->bptr[M] > 0)
    {
      B->bind = oski_Malloc (oski_index_t, A->bptr[M]);
      if (B->bind == NULL)
	err = 1;
      else
	oski_CopyMem (B->bind, A->bind, oski_index_t, A->bptr[M]);
    }

  if (!err && A->bptr[M] > 0)
    {
      B->bval = oski_Malloc (oski_value_t, A->bptr[M] * A->r * A->c);
      if (B->bval == NULL)
	err = 1;
      else
	oski_CopyMem (B->bval, A->bval, oski_value_t,
		      A->bptr[M] * A->r * A->c);
    }

  if (!err && M > 0)
    {
      B->bdiag = oski_Malloc (oski_value_t, M * A->r * A->r);
      if (B->bdiag == NULL)
	err = 1;
      else
	oski_CopyMem (B->bdiag, A->bdiag, oski_value_t, M * A->r * A->r);
    }

  if (err)
    {
      if (B->bdiag)
	oski_Free (B->bdiag);
      if (B->bval)
	oski_Free (B->bval);
      if (B->bind)
	oski_Free (B->bind);
      if (B->bptr)
	oski_Free (B->bptr);
      return 0;
    }

  return 1;
}

/** \brief Destroys any memory allocated for the fields of a submatrix. */
static void
DestroySubmat (oski_submatMBCSR_t * const A)
{
  if (A == NULL)
    return;
  if (A->num_block_rows > 0 && A->bptr != NULL)
    {
      if (A->bptr[A->num_block_rows] > 0)
	{
	  if (A->bind != NULL)
	    oski_Free (A->bind);
	  if (A->bval != NULL)
	    oski_Free (A->bval);
	}
      if (A->bdiag != NULL)
	oski_Free (A->bdiag);
      oski_Free (A->bptr);
    }
  if (A->mod_name != NULL)
    oski_Free (A->mod_name);
}

/** \brief Free BCSR matrix representation. */
static void
DestroySubmatBCSR (oski_matBCSR_t * A)
{
  if (A != NULL)
    {
      oski_DestroyMatRepr_funcpt func_BCSR =
	OSKI_MATTYPE_METHOD ("BCSR", DestroyMatRepr);
      if (func_BCSR != NULL)
	(*func_BCSR) (A);
    }
}

void
oski_DestroyMatRepr (void *mat)
{
  if (mat != NULL)
    {
      oski_matMBCSR_t *A = (oski_matMBCSR_t *) mat;
      DestroySubmat (&(A->A1));
      DestroySubmat (&(A->A2));
      DestroySubmatBCSR (A->p_A3);
      oski_Free (A);
    }
}

void *
oski_CopyMatRepr (const void *mat, const oski_matcommon_t * props)
{
  const oski_matMBCSR_t *A = (const oski_matMBCSR_t *) mat;
  oski_matMBCSR_t *A_copy = NULL;
  oski_CopyMatRepr_funcpt func_CopyBCSR =
    OSKI_MATTYPE_METHOD ("BCSR", CopyMatRepr);

  if (A == NULL || func_CopyBCSR == NULL)
    return NULL;

  A_copy = oski_Malloc (oski_matMBCSR_t, 1);
  if (A_copy == NULL)
    {
      OSKI_ERR (oski_CopyMatRepr, ERR_OUT_OF_MEMORY);
      return NULL;
    }

  assert (A_copy != NULL);
  A_copy->stored.is_upper = A->stored.is_upper;
  A_copy->stored.is_lower = A->stored.is_lower;
  CopySubmat (&(A->A1), &(A_copy->A1));
  CopySubmat (&(A->A2), &(A_copy->A2));
  A_copy->p_A3 = NULL;

  if (A->p_A3 != NULL)
    {
      A_copy->p_A3 = (*func_CopyBCSR) (A->p_A3, NULL);
      if (A_copy->p_A3 == NULL)
	{
	  OSKI_ERR (oski_CopyMatRepr, ERR_OUT_OF_MEMORY);
	  DestroySubmat (&(A_copy->A1));
	  DestroySubmat (&(A_copy->A2));
	  oski_Free (A_copy);
	  return NULL;
	}
    }

  return (void *) A_copy;
}

/* --------------------------------------------------------------------
 *  Lua transformations.
 */

#include <oski/xforms_internal.h>

/**
 *  \brief
 *
 *  The MBCSR implementation expects the following arguments on
 *  the stack:
 *    -# Destination matrix type ('MBCSR')
 *    -# Source matrix ('CSR')
 *    -# Row block size ('r')
 *    -# Column block size ('c')
 */
int
oski_CreateLuaMatReprFromCSR (lua_State * L)
{
  return oski_CreateLuaMatReprGeneric2IndexFromCSR (L, "MBCSR");
}

void
oski_EnableMBCSRMatTransMatMult (oski_matMBCSR_t * A, int v)
{
  if (A != NULL)
    A->enabled.MatTransMatMult = v;
}

void
oski_EnableMBCSRMatMultAndMatMult (oski_matMBCSR_t * A, int v)
{
  if (A != NULL)
    A->enabled.MatMultAndMatMult = v;
}

void
oski_EnableMBCSRMatMultAndMatTransMult (oski_matMBCSR_t * A, int v)
{
  if (A != NULL)
    A->enabled.MatMultAndMatTransMult = v;
}

void
oski_EnableMBCSRMatPowMult (oski_matMBCSR_t * A, int v)
{
  if (A != NULL)
    A->enabled.MatPowMult = v;
}

void
oski_EnableMBCSRMatTransPowMult (oski_matMBCSR_t * A, int v)
{
  if (A != NULL)
    A->enabled.MatTransPowMult = v;
}

/* eof */
