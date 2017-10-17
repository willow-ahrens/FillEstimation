/**
 *  \file src/BCSR/convert.c
 *  \brief  Conversion between CSR and SPARSITY-style BCSR (i.e.,
 *  register blocking) format.
 *  \ingroup MATTYPE_BCSR
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
#include <oski/CSR/module.h>
#include <oski/BCSR/format.h>
#include <oski/BCSR/module.h>

/**
 *  \brief Given an \f$m\times n\f$ CSR matrix \f$A\f$,
 *  convert the first \f$m-\mathrm{mod}(m,r)\f$ rows into
 *  \f$r\times c\f$ BCSR format.
 *
 *  If the converted matrix has \f$n_b\f$ blocks, this
 *  implementation executes in
 *  \f$O(\mbox{stored non-zeros}) = O(n_b\cdot r\cdot c)\f$
 *  time, but requires \f$O(n\cdot r)\f$ auxiliary storage
 *  space to store a dense copy of one block row.
 *
 *  This long procedure is divided into two phases:
 *     -# Count the exact number of new blocks to create.
 *     -# Convert to block format, one block row at a time.
 *
 *  This routine assumes the CSR matrix uses full storage,
 *  but otherwise is flexible with regard to the following
 *  variations:
 *    - 0 or 1-based indexing
 *    - 'ptr[0]' does not have to start at 'base' (i.e.,
 *    caller can pass a subset of rows of a larger matrix)
 *    - Column indices do not have to be sorted.
 *
 *  \param[in] ptr CSR row pointers.
 *  \param[in] ind CSR column indices.
 *  \param[in] val CSR explicitly stored values.
 *  \param[in] base Index base (0-based or 1-based)
 *  \param[in] m Logical number of matrix rows
 *  \param[in] n Logical number of matrix columns
 *  \param[in] r Desired row block size
 *  \param[in] c Desired column block size
 *  \param[in,out] p_bptr Used to return the BCSR block
 *  row pointers. Must not equal NULL.
 *  \param[in,out] p_bind Used to return the BCSR block
 *  column indices. Must not equal NULL.
 *  \param[in,out] p_bval Used to return the explicitly
 *  stored blocks. Must not equal NULL.
 *
 *  \note Block column indices are unsorted on output.
 *
 *  \returns On success, returns 0 and sets *p_bptr,
 *  *p_bind, and *p_bval to the BCSR representation.
 *  Otherwise, returns an error code and leaves
 *  p_bptr, p_bind, and p_bval unchanged.
 *  or an error code otherwise.
 */
static int
ConvertFullBlocksFromCSR (const oski_index_t * ptr,
			  const oski_index_t * ind, const oski_value_t * val,
			  oski_index_t base,
			  oski_index_t m, oski_index_t n,
			  oski_index_t r, oski_index_t c,
			  oski_index_t ** p_bptr, oski_index_t ** p_bind,
			  oski_value_t ** p_bval)
{
  oski_index_t M;
  oski_index_t N;

  /* stores total number of non-zero blocks */
  oski_index_t num_blocks;

  /* auxiliary storage: reused for each block-row */
  oski_index_t *block_count;	/* size N */
  oski_value_t *blocks;		/* size N*r*c */
  oski_index_t *bptr_next;	/* size M+1 */


  /* return values */
  oski_index_t *bptr;
  oski_index_t *bind;
  oski_value_t *bval;

  oski_index_t I;		/* block-row iteration variable */
  int err;

  if (p_bptr == NULL || p_bind == NULL || p_bval == NULL)
    return ERR_BAD_ARG;

  M = m / r;			/* # of full block-rows */
  N = (n + c - 1) / c;		/* # of block columns */

  /* ----------------------------------------------------
   *  Allocate temporary space.
   */
  bptr = oski_Malloc (oski_index_t, M + 1);
  if (bptr == NULL)
    return ERR_OUT_OF_MEMORY;

  if (N == 0)
    {
      *p_bptr = bptr;
      *p_bind = NULL;
      *p_bval = NULL;
      return 0;			/* Quick return */
    }

  assert (N >= 1);
  err = oski_MultiMalloc (__FILE__, __LINE__, 2,
			  (size_t) (sizeof (oski_index_t) * N), &block_count,
			  (size_t) (sizeof (oski_index_t) * (M + 1)),
			  &bptr_next);
  if (err)
    {
      oski_Free (bptr);
      return ERR_OUT_OF_MEMORY;
    }
  oski_ZeroMem (block_count, sizeof (oski_index_t) * N);

  /* ----------------------------------------------------
   *  Phase I: Count the exact number of new blocks to
   *  create.
   */
  num_blocks = 0;
  for (I = 0; I < M; I++)	/* loop over block rows */
    {
      oski_index_t i;
      oski_index_t di;

      /* Count the number of blocks within block-row I, and
       * remember in 'block_count' which of the possible blocks
       * have been 'visited' (i.e., contain at least 1 non-zero).
       */
      for (i = I * r, di = 0; di < r; di++, i++)
	{
	  oski_index_t k;

	  /* Invariant: block_count[J] == # of non-zeros
	   * encountered in rows I*r .. I*r+di that should
	   * be stored in column-block J (i.e., that have
	   * column indices J*c <= j < (J+1)*c).
	   */

	  /* Count the number of additional logical blocks
	   * needed to store non-zeros in row i, and mark
	   * the blocks in block row I that have been visited.
	   */
	  for (k = ptr[i] - base; k < ptr[i + 1] - base; k++)
	    {
	      oski_index_t j = ind[k] - base;	/* column index */
	      oski_index_t J = j / c;	/* block column index */

	      if (block_count[J] == 0)	/* "create" (count) new block */
		{
		  num_blocks++;
		  block_count[J]++;
		}
	    }
	}
      /* POST: num_blocks == total # of new blocks in rows 0..i */
      /* POST: block_count[J] == # of non-zeros in block J of block-row I */

      bptr[I + 1] = num_blocks;

      /* Reset block_count for next iteration, I+1. This second loop
       * is needed to keep the complexity of phase I to O(nnz).
       */
      for (i = I * r, di = 0; di < r; di++, i++)
	{
	  oski_index_t k;

	  for (k = ptr[i] - base; k < ptr[i + 1] - base; k++)
	    {
	      oski_index_t j = ind[k] - base;	/* column index */
	      oski_index_t J = j / c;	/* block column index */
	      block_count[J] = 0;
	    }
	}
    }
  /* POST: num_blocks == total # of blocks. */

  /* Allocate space for the new blocks.
   *
   * Need additional temporary space to store the new blocks
   * of the current block row. Note the 'r*c' term, which is
   * a slightly generous extra fudge for one additional block
   * in case c does not divide n.
   */
  err = oski_MultiMalloc (__FILE__, __LINE__, 3,
			  (size_t) (sizeof (oski_index_t) * num_blocks),
			  &bind,
			  (size_t) (sizeof (oski_value_t) * num_blocks * r *
				    c), &bval,
			  (size_t) (sizeof (oski_value_t) * r * (n + c)),
			  &blocks);
  if (err)
    {
      oski_FreeAll (3, bptr, block_count, bptr_next);
      return ERR_OUT_OF_MEMORY;
    }

  /* ----------------------------------------------------
   *  Phase II: Copy all blocks.
   */

  /* Assign: bptr_next[i] = bptr[i] && blocks[k] = 0 */
  oski_CopyMem (bptr_next, bptr, oski_index_t, M + 1);
  oski_ZeroMem (blocks, sizeof (oski_value_t) * (n * r + r * c));

  for (I = 0; I < M; I++)
    {
      oski_index_t i0 = I * r;	/* starting row of block-row i */
      oski_index_t di;		/* iterates over rows in block-row i */
      oski_index_t i;		/* current row, i0 <= i < i0+r */

      /* Copy all non-zeros in block-row I to temp block storage */
      for (di = 0, i = i0; di < r; di++, i++)
	{
	  oski_index_t k;
	  for (k = ptr[i] - base; k < ptr[i + 1] - base; k++)
	    {
	      oski_index_t j = ind[k] - base;	/* col index */
	      oski_value_t a_ij;

	      oski_index_t J = j / c;	/* block-col index */
	      oski_index_t j0;

	      /* Compute starting column index, j0, of this block */
	      if ((J + 1) * c > n)
		j0 = n - c;	/* so block doesn't overrun edge */
	      else
		j0 = J * c;

	      /* Copy element (i, j) to block J.
	       * Each block is stored in row-major order.
	       */
	      VAL_ASSIGN (a_ij, val[k]);
	      VAL_INC (blocks[J * r * c + di * c + j - j0], a_ij);

	      /* Mark block J as 'visited' */
	      block_count[J]++;
	    }
	}
      /* POST: block_count[J] > 0  ==> blocks[J*r*c .. (J+1)*r*c-1]
       * stores non-zero block J in row-major order.
       */

      for (di = 0, i = i0; di < r; di++, i++)
	{
	  oski_index_t k;

	  for (k = ptr[i] - base; k < ptr[i + 1] - base; k++)
	    {
	      oski_index_t j = ind[k] - base;
	      oski_index_t J = j / c;
	      oski_index_t j0 = (J * c + c > n) ? n - c : J * c;

	      /* If block J visited, copy it to bval. */
	      if (block_count[J] > 0)
		{
		  oski_index_t k_next = bptr_next[I];

		  /* record column index */
		  bind[k_next] = j0;

		  /* record non-zero rxc block */
		  oski_CopyMem (bval + k_next * r * c, blocks + J * r * c,
				oski_value_t, r * c);

		  /* next column index/block location */
		  bptr_next[I]++;
		  assert (bptr_next[I] <= bptr[I + 1]);

		  block_count[J] = 0;	/* unmark this block */
		  oski_ZeroMem (blocks + J * r * c,
				sizeof (oski_value_t) * r * c);
		}
	    }
	}

      /* POST: Block-row I converted */
    }

  oski_FreeAll (3, block_count, bptr_next, blocks);

  *p_bptr = bptr;
  *p_bind = bind;
  *p_bval = bval;
  return 0;
}

/**
 *  \brief Convert from full CSR to BCSR.
 *
 *  \pre None of A, mat, props are NULL
 *  \pre c <= n
 */
static int
ConvertFromCSR (oski_matBCSR_t * A, oski_index_t r, oski_index_t c,
		const oski_index_t * ptr, const oski_index_t * ind,
		const oski_value_t * val, oski_index_t m, oski_index_t n,
		oski_index_t base)
{
  int err;
  oski_index_t num_blockable_rows;

  assert (A != NULL);
  assert (c <= n);

  A->num_block_rows = m / r;
  A->num_block_cols = INT_CEIL_DIV (n, c);
  A->row_block_size = r;
  A->col_block_size = c;

  num_blockable_rows = A->num_block_rows * r;
  A->num_rows_leftover = m % r;

  if (A->num_rows_leftover > 0)
    {
      A->leftover = oski_Malloc (oski_matBCSR_t, 1);
      if (A->leftover == NULL)
	{
	  OSKI_ERR (ConvertFromCSR, ERR_OUT_OF_MEMORY);
	  return ERR_OUT_OF_MEMORY;
	}
    }
  else
    A->leftover = NULL;

  /* Block the top floor(m/r) rows */
  err = ConvertFullBlocksFromCSR (ptr, ind, val, base, m, n,
				  r, c, &(A->bptr), &(A->bind), &(A->bval));
  if (err)
    {
      OSKI_ERR (ConvertFromCSR, err);
      if (A->leftover != NULL)
	oski_Free (A->leftover);
      return ERR_OUT_OF_MEMORY;
    }

  /* Use smaller row block size for remaining rows. */
  if (A->num_rows_leftover > 0)
    {
      return ConvertFromCSR (A->leftover, A->num_rows_leftover, c,
			     ptr + num_blockable_rows, ind, val,
			     A->num_rows_leftover, n, base);
    }

  return 0;
}

static int
ExpandSymm (const oski_matCSR_t * mat, const oski_matcommon_t * props,
	    oski_matCSR_t ** p_mat_full)
{
  oski_ConditionallyExpandCSRToFull_funcpt func_Expand =
    OSKI_MATTYPE_METHOD ("CSR", ConditionallyExpandCSRToFull);
  assert (func_Expand != NULL);
  return (*func_Expand) (mat, props, p_mat_full);
}


void *
oski_CreateMatReprFromCSR (const oski_matCSR_t * mat,
			   const oski_matcommon_t * props, ...)
{
  oski_matBCSR_t *A;

  va_list ap;
  oski_index_t r;
  oski_index_t c;

  oski_matCSR_t *mat_copy;
  int did_copy;

  if (mat == NULL || props == NULL)
    return NULL;

  va_start (ap, props);
  r = va_arg (ap, oski_index_t);
  c = va_arg (ap, oski_index_t);

  oski_PrintDebugMessage (1, "Creating BCSR(r=%d, c=%d) ...", r, c);

  if (r <= 0)
    return NULL;
  if (c <= 0)
    return NULL;

  /* In order to reuse old BCSR conversion routine, make a
   * fully expanded copy.
   */
  did_copy = ExpandSymm (mat, props, &mat_copy);
  if (mat_copy == NULL)
    {
      OSKI_ERR (oski_CreateMatReprFromCSR, ERR_OUT_OF_MEMORY);
      return NULL;
    }

  /* Convert! */
  A = oski_Malloc (oski_matBCSR_t, 1);
  if (A != NULL)
    {
      assert (!
	      ((props->pattern.is_symm || props->pattern.is_herm)
	       && (mat_copy->stored.is_upper != mat_copy->stored.is_lower)));

      if (c > props->num_cols)
	c = props->num_cols;

      oski_PrintDebugMessage (2, "Converting ...");

      ConvertFromCSR (A, r, c, mat_copy->ptr, mat_copy->ind,
		      mat_copy->val, props->num_rows, props->num_cols,
		      mat_copy->base_index);

      oski_PrintDebugMessage (3, "Block dimensions: %d x %d",
			      A->num_block_rows, A->num_block_cols);
      oski_PrintDebugMessage (3, "Number of blocks: %d",
			      A->bptr[A->num_block_rows]);

      A->has_unit_diag_implicit = mat_copy->has_unit_diag_implicit;

      A->mod_name = oski_MakeModuleName ("mat", "BCSR",
					 OSKI_IND_ID, OSKI_VAL_ID);
      A->mod_cached = NULL;

      if (A->num_rows_leftover && A->leftover)
	{
	  A->leftover->has_unit_diag_implicit = A->has_unit_diag_implicit;
	  A->leftover->mod_name = oski_MakeModuleName ("mat", "BCSR",
						       OSKI_IND_ID,
						       OSKI_VAL_ID);
	  A->leftover->mod_cached = NULL;
	}

      oski_PrintDebugMessage (2, "Cached module name: %s", A->mod_name);
		/** \todo This fill ratio is wrong for symmetric matrices. */
      oski_PrintDebugMessage (2, "Fill ratio: %d*%d*%d/%d == %g",
			      A->bptr[A->num_block_rows], A->row_block_size,
			      A->col_block_size, props->num_nonzeros,
			      (double) A->bptr[A->num_block_rows] *
			      A->row_block_size * A->col_block_size /
			      props->num_nonzeros);
    }

  /*
   * Destroy copy if any.
   */
  if (did_copy && mat_copy != NULL)
    {
      oski_DestroyMatRepr_funcpt func_Destroy =
	OSKI_MATTYPE_METHOD ("CSR", DestroyMatRepr);

      oski_PrintDebugMessage (2, "Destroying copy ...");

      if (func_Destroy != NULL)
	(*func_Destroy) (mat_copy);
    }

  oski_PrintDebugMessage (2, "Returning %p.", (void *) A);

  return (void *) A;
}

static void
CopyOne (const oski_matBCSR_t * A, oski_matCSR_t * B, oski_index_t i0)
{
  oski_index_t I;
  oski_index_t M, r, c;
  oski_index_t nnz_stored;
  int is_lower = B->stored.is_lower;
  int is_upper = B->stored.is_upper;

  if (A == NULL || B == NULL)
    return;

  M = A->num_block_rows;
  r = A->row_block_size;
  c = A->col_block_size;

  /* assert( B->ptr[i0] initialized ); */
  nnz_stored = B->ptr[i0];
  for (I = 0; I < M; I++)
    {
      oski_index_t nb = A->bptr[I + 1] - A->bptr[I];
      oski_index_t di;
      for (di = 0; di < r; di++)
	{
	  oski_index_t i_dest = i0 + I * r + di;
	  oski_index_t K;

	  B->ptr[i_dest + 1] = B->ptr[i_dest] + nb * c;

	  for (K = A->bptr[I]; K < A->bptr[I + 1]; K++)
	    {
	      oski_index_t j0 = A->bind[K];
	      oski_index_t dj;
	      for (dj = 0; dj < c; dj++)
		{
		  oski_index_t j_dest = j0 + dj;
		  oski_value_t a_ij;

		  /* If matrix is triangular, check whether
		   * j_dest needs to be "hacked".
		   */
		  if (is_lower && !is_upper && j_dest > i_dest)
		    {
		      j_dest = i_dest;
		      VAL_SET_ZERO (a_ij);
		    }
		  else if (is_upper && !is_lower)
		    {
		      j_dest = i_dest;
		      VAL_SET_ZERO (a_ij);
		    }
		  else
		    {
		      VAL_ASSIGN (a_ij, A->bval[K * r * c + di * c + dj]);
		    }

		  assert (nnz_stored >= B->ptr[i_dest]);
		  assert (nnz_stored < B->ptr[i_dest + 1]);

		  B->ind[nnz_stored] = j_dest;
		  VAL_ASSIGN (B->val[nnz_stored], a_ij);
		  nnz_stored++;
		}		/* dj */
	    }			/* K */
	}			/* di */
    }				/* I */
}

static int
ConvertToCSR (const oski_matBCSR_t * A, const oski_matcommon_t * props,
	      oski_matCSR_t * B)
{
  oski_index_t M, r, c, nnz;
  oski_index_t M2, r2, c2, nnz2;

  if (A == NULL || B == NULL)
    return 0;

  M = A->num_block_rows;
  r = A->row_block_size;
  c = A->col_block_size;
  nnz = A->bptr[M] * r * c;

  if (A->leftover != NULL)
    {
      M2 = A->leftover->num_block_rows;
      r2 = A->leftover->row_block_size;
      c2 = A->leftover->col_block_size;
      nnz2 = A->leftover->bptr[M2] * r2 * c2;
    }
  else
    {
      M2 = 0;
      r2 = 0;
      c2 = 0;
      nnz2 = 0;
    }

  B->ptr = oski_Malloc (oski_index_t, M * r + M2 * r2 + 1);
  if (B->ptr == NULL)
    return 0;
  B->ind = oski_Malloc (oski_index_t, nnz + nnz2);
  if (B->ind == NULL)
    {
      oski_Free (B->ptr);
      return 0;
    }
  B->val = oski_Malloc (oski_value_t, nnz + nnz2);
  if (B->val == NULL)
    {
      oski_Free (B->ptr);
      oski_Free (B->ind);
      return 0;
    }

  B->base_index = 0;
  B->has_unit_diag_implicit = A->has_unit_diag_implicit;
  B->has_sorted_indices = 1;
  B->is_shared = 0;
  if (props != NULL)
    {
      assert (props->num_rows == M * r + M2 * r2);
      B->stored.is_upper = props->pattern.is_tri_upper;
      B->stored.is_lower = props->pattern.is_tri_lower;
    }
  else
    {				/* assume full storage */
      B->stored.is_upper = 0;
      B->stored.is_lower = 0;
    }

  B->ptr[0] = 0;
  CopyOne (A, B, 0);
  CopyOne (A->leftover, B, M * r);
  return 1;
}

oski_matCSR_t *
oski_ConvertMatReprToCSR (const void *mat, const oski_matcommon_t * props)
{
  const oski_matBCSR_t *A = (const oski_matBCSR_t *) mat;
  oski_matCSR_t *B = oski_Malloc (oski_matCSR_t, 1);

  if (!ConvertToCSR (A, props, B))
    {
      oski_Free (B);
      return NULL;
    }
  else
    {
      return B;
    }
}

/**
 *  \brief
 *
 *  \note Ignores 'props'.
 */
void *
oski_CopyMatRepr (const void *mat, const oski_matcommon_t * props)
{
  oski_index_t r, c;
  oski_matBCSR_t *A_copy;

  if (mat == NULL)
    return NULL;

  r = ((const oski_matBCSR_t *) (mat))->num_block_rows;
  c = ((const oski_matBCSR_t *) (mat))->num_block_cols;

  A_copy = oski_Malloc (oski_matBCSR_t, 1);
  if (A_copy != NULL)
    {
      const oski_matBCSR_t *A_orig = (const oski_matBCSR_t *) mat;
      oski_index_t bm = A_orig->num_block_rows;
      A_copy->bptr = oski_Malloc (oski_index_t, bm + 1);
      if (A_copy->bptr != NULL)
	{
	  /* Number of blocks. */
	  oski_index_t nb = A_orig->bptr[bm] - A_orig->bptr[0];

	  A_copy->bind = oski_Malloc (oski_index_t, nb);
	  if (A_copy->bind != NULL || nb == 0)
	    {
	      A_copy->bval = oski_Malloc (oski_value_t, nb * r * c);
	      if (A_copy->bval != NULL || nb == 0)
		{
		  A_copy->has_unit_diag_implicit =
		    A_orig->has_unit_diag_implicit;
		  A_copy->row_block_size = A_orig->row_block_size;
		  A_copy->col_block_size = A_orig->col_block_size;

		  A_copy->num_rows_leftover = A_orig->num_rows_leftover;
		  if (A_orig->leftover != NULL)
		    {
		      A_copy->leftover =
			oski_CopyMatRepr (A_orig->leftover, NULL);

		      if (A_copy->leftover != NULL)
			return A_copy;
		    }
		  else		/* A_orig->leftover == NULL */
		    {
		      A_copy->leftover = NULL;
		      return A_copy;
		    }

		  oski_Free (A_copy->bval);
		}

	      oski_Free (A_copy->bind);
	    }

	  oski_Free (A_copy->bptr);
	}

      oski_Free (A_copy);
    }

  OSKI_ERR (oski_CopyMatRepr, ERR_OUT_OF_MEMORY);
  return NULL;
}

void
oski_DestroyMatRepr (void *mat)
{
  oski_matBCSR_t *A;

  if (mat == NULL)
    return;

  A = (oski_matBCSR_t *) mat;

  oski_DestroyMatRepr (A->leftover);
  if (A->bval != NULL)
    oski_Free (A->bval);
  if (A->bind != NULL)
    oski_Free (A->bind);
  if (A->bptr != NULL)
    oski_Free (A->bptr);
  if (A->mod_name != NULL)
    oski_Free (A->mod_name);

  oski_Free (A);
}

/* --------------------------------------------------------------------
 *  Lua transformations.
 */

#include <oski/xforms_internal.h>

/**
 *  \brief
 *
 *  The BCSR implementation expects the following arguments on
 *  the stack:
 *    -# Destination matrix type ('BCSR')
 *    -# Source matrix ('CSR')
 *    -# Row block size ('r')
 *    -# Column block size ('c')
 */
int
oski_CreateLuaMatReprFromCSR (lua_State * L)
{
  return oski_CreateLuaMatReprGeneric2IndexFromCSR (L, "BCSR");
}

/* eof */
