/**
 *  \file src/VBR/convert.c
 *  \brief  Conversion between CSR and VBR format.
 *  \ingroup MATTYPE_VBR
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
#include <oski/VBR/format.h>
#include <oski/VBR/module.h>

/** Return max of two side-effect free values */
#define MAX(a, b)  ((a) > (b) ? (a) : (b))

/** Flag type (takes the values 0 or 1) */
typedef unsigned char flag_t;

static int
ExpandSymm (const oski_matCSR_t * mat, const oski_matcommon_t * props,
            oski_matCSR_t ** p_mat_full)
{
  oski_ConditionallyExpandCSRToFull_funcpt func_Expand =
    OSKI_MATTYPE_METHOD ("CSR", ConditionallyExpandCSRToFull);
  assert (func_Expand != NULL);
  return (*func_Expand) (mat, props, p_mat_full);
}

static int
TransposeSymm (const oski_matCSR_t* mat, const oski_matcommon_t* props,
	       oski_matCSR_t **p_mat_trans)
{
  oski_TransposeCSR_funcpt func_Transpose;
  if (props->pattern.is_symm || props->pattern.is_herm)
    {
      *p_mat_trans = (oski_matCSR_t *)mat;
      return 0;
    }
  func_Transpose = OSKI_MATTYPE_METHOD ("CSR", TransposeCSR);
  if (func_Transpose == NULL)
    {
      *p_mat_trans = NULL;
      return 0;
    }
  *p_mat_trans = (*func_Transpose) (mat, props);
  return 1;
}

static void
DestroyCSR (oski_matCSR_t* mat)
{
  oski_DestroyMatRepr_funcpt func_Destroy =
    OSKI_MATTYPE_METHOD ("CSR", DestroyMatRepr);
  if (func_Destroy == NULL) return;
  (*func_Destroy) (mat);
}

/**
 *  \brief Scatters a specified value to all elements of a dense flag
 *  vector \f$f\f$ corresponding to the structurally non-zero elements
 *  of a sparse vector \f$x\f$.
 *
 *  More specifically, sets \f$f_j \leftarrow v\f$ for all \f$j =
 *  x_k\f$ such that \f$0 \leq k < n_x\f$.
 *
 *  \param[in] n_x Number \f$n_x\f$ of structurally non-zero elements
 *  in \f$x\f$.
 *  \param[in] x  Sparse vector indices of \f$x\f$.
 *  \param[in] value  Value \f$v\f$ to assign.
 *  \param[in] n  Maximum length of \f$f\f$.
 *  \param[out] f  Flag vector \f$f\f$.
 *  \returns Only elements of \f$f\f$ corresponding to structurally
 *  non-zero elements of \f$x\f$ are modified.
 */
static void
SetFlags (oski_index_t n_x, const oski_index_t* restrict x, flag_t value,
	  oski_index_t n, flag_t* restrict f)
{
  oski_index_t k;
  for (k = 0; k < n_x; k++)
    {
      oski_index_t j = x[k];
      assert (0 <= j && j <= n);
      f[j] = value;
    }
}

/**
 *  \brief Counts the number of unique structurally non-zero elements
 *  in a sparse vector \f$x\f$.
 *
 *  \param[in] n_x Number \f$n_x\f$ of structurally non-zero elements
 *  in \f$x\f$.
 *  \param[in] x  Sparse vector indices of \f$x\f$.
 *  \param[in] n  Value >= maximum index in \f$x\f$.
 *  \param[out] workspace  Buffer of size at least n+1, initialized to
 *  all zeros on entry.
 *  \returns The number of unique structurally non-zero elements in a
 *  sparse vector \f$x\f$. Resets any modified elements of the
 *  workspace back to 0 on return.
 */
static oski_index_t
CountUnique (oski_index_t n_x, const oski_index_t* restrict x,
	     oski_index_t n, flag_t* restrict workspace)
{
  oski_index_t num_unique = 0;
  oski_index_t k;

  SetFlags (n_x, x, 1, n, workspace);

  for (k = 0; k < n_x; k++)
    {
      oski_index_t j = x[k];
      if (workspace[j])
	{
	  num_unique++;
	  workspace[j] = 0; /* reset, so only unique elems counted */
	}
    }
  return num_unique;
}

/**
 *  \brief Returns the number of unique elements that two sparse
 *  vectors, \f$x_1\f$ and \f$x_2\f$, have in common. These vectors
 *  are assumed to be row (or column) vectors of a larger matrix,
 *  located at positions row (column) \f$i_1\f$ and \f$i_2\f$,
 *  respectively.
 *
 *  \param[in] i1  Index of vector \f$x_1\f$.
 *  \param[in] n1  No. of elements stored with \f$x_1\f$.
 *  \param[in] ind1  Non-zero indices associated with \f$x_1\f$.
 *  \param[in] i2  Index of vector \f$x_2\f$.
 *  \param[in] n2  No. of elements stored with \f$x_2\f$.
 *  \param[in] ind2  Non-zero indices associated with \f$x_2\f$.
 *  \param[in] b  Index base (0- or 1-based) for i1, i2, and all
 *  values in ind1 and ind2.
 *  \param[in] n  A value greater than or equal to the maximum value
 *  in either ind1 or ind2. That is, for all 0 <= k1 < n1,
 *  n >= ind1[k1] and for all 0 <= k2 < n2, n >= ind2[k2].
 *  \param[in,out] workspace  A buffer of size 2*(n+1), all of whose
 *  elements must be zero.
 *  \returns Returns the number of unique non-zero entries the two
 *  sparse vectors have in common. All entries of the workspace will
 *  also be reset to zero, provided they were all initially zero.
 *
 *  \note The algorithm below returns the number of unique elements, in the
 *  event that either ind1 or ind2 contain duplicate entries.
 */
static oski_index_t
CountNumCommonSpVec (oski_index_t i1, oski_index_t n1,
		     const oski_index_t* restrict ind1,
		     oski_index_t i2, oski_index_t n2,
		     const oski_index_t* restrict ind2,
		     oski_index_t b, int has_unit_diag,
		     oski_index_t n, flag_t* restrict workspace)
{
  flag_t* flags1 = workspace;
  flag_t* flags2 = workspace + n + 1;
  oski_index_t num_common = 0;
  oski_index_t k;

  if (n1 == 0 || n2 == 0 || ind1 == NULL || ind2 == NULL || n == 0)
    return 0;
  assert (flags1 != NULL && flags2 != NULL);

  /* Scatter values */
  SetFlags (n1, ind1, 1, n+1, flags1);
  if (has_unit_diag && (i1-b) < n)
    flags1[i1] = 1;

  SetFlags (n2, ind2, 1, n+1, flags2);
  if (has_unit_diag && (i2-b) < n)
    flags2[i2] = 1;

  /* Count common unique elements */
  for (k = 0; k < n2; k++)
    {
      oski_index_t j2 = ind2[k];
      if (flags2[j2])
	{
	  if (flags1[j2])
	    num_common++;
	  flags2[j2] = 0; /* So that we count unique common elements
			     */
	}
    }
  if (has_unit_diag && (i2-b) < n)
    {
      if (flags2[i2])
	{
	  if (flags1[i2])
	      num_common++;
	  flags2[i2] = 0;
	}
    }

  /* Reset modified workspace values */
  SetFlags (n1, ind1, 0, n+1, flags1);
  if (has_unit_diag && (i1-b) < n)
    flags1[i1] = 0;
  
  return num_common;
}

/**
 *  \brief Determine a partitioning of a CSR structure into groups of
 *  consecutive rows that have similar non-zero patterns.
 *
 *  This routine finds a block row partitioning, defined as a sequence
 *  of \f$M\f$ block row starting positions \f$0 = i_0 < i_1 < \cdots
 *  < i_{M} = m\f$, where the \f$K^{\mathrm{th}}\f$ block row consists
 *  of all rows starting at \f$i_K\f$ and ending at
 *  \f$i_{K+1}-1\f$. The block rows satisfy the following property.
 *  - Let \f$x_i\f$ be a pattern row vector corresponding to the
 *  non-zero pattern of row \f$i\f$ of the matrix, and let \f$k_i\f$
 *  be the number of unique structural non-zeros in that row.
 *  - Let \f$0 \leq \rho \leq 1\f$ be a user-defined partitioning
 *  threshold.
 *  - (Similarity property) Then, each row \f$i_K \leq i < i_{K+1}\f$
 *  satisfies \f$\frac{x_{i_K}\cdot x_{i}^T}{\max\{k_{i_K},k_i\}} \geq
 *  \rho\f$.
 *
 *  \param[in] m Number of rows, \f$m\f$.
 *  \param[in] ptr Row pointers, of size \f$m+1\f$.
 *  \param[in] ind Column indices, of size ptr[m].
 *  \param[in] b Index base (0- or 1-based)
 *  \param[in] has_unit_diag Non-zero if matrix has an implicit unit diagonal.
 *  \param[in] threshold The similarity threshold, \f$\rho\f$.
 *  \param[out] b_start Buffer in which to store the block row starts,
 *  of size at least \f$M+1\f$. If b_start is NULL, then these values
 *  are not returned.
 *  \param[in] n_max Maximum value of any column index in ind.
 *  \param[in,out] workspace A buffer of size at least 2*(n_max+1) to
 *  be used as temporary storage. The workspace must be all zero
 *  initially, and if so, will be returned in an all-zero state.
 *  \returns The number of block rows \f$M\f$. If b_start is not NULL,
 *  returns the block row partitioning as well.
 *
 *  The algorithm is greedy, starting at row 0 and growing each block
 *  row until the similarity property no longer holds.
 *
 *  \note The array b_start may be NULL, in which case this routine
 *  just returns \f$M\f$. Thus, this routine can be called once to
 *  determine \f$M\f$, allocate a buffer b_start of size \f$M+1\f$,
 *  and then called again to determine b_start.
 */
static oski_index_t
FindBlockPart (oski_index_t m, const oski_index_t* restrict ptr,
	       const oski_index_t* restrict ind,
	       oski_index_t b, int has_unit_diag,
	       double threshold,
	       oski_index_t* b_start,
	       oski_index_t n_max, flag_t* workspace)
{
  oski_index_t i_start = 0;
  oski_index_t num_blocks = 0;
  while (i_start < m)
    {
      oski_index_t k_start = ptr[i_start]-b;
      oski_index_t nnz_start = ptr[i_start+1]-b - k_start;
      const oski_index_t* ind_start = ind + k_start;
      oski_index_t n_unique_start = CountUnique (nnz_start, ind_start,
						 n_max, workspace);
      oski_index_t i_next = i_start + 1;

      if (has_unit_diag && (i_start < n_max))
	n_unique_start++;

      while (i_next < m)
	{
	  oski_index_t k_next = ptr[i_next]-b;
	  oski_index_t nnz_next = ptr[i_next+1]-b - k_next;
	  const oski_index_t* ind_next = ind + k_next;
	  oski_index_t n_unique_next = CountUnique (nnz_next, ind_next,
						    n_max, workspace);
	  oski_index_t num_common;
          oski_index_t max_unique;
          double frac_common;

	  if (has_unit_diag && (i_next < n_max))
	    n_unique_next++;

	  num_common = CountNumCommonSpVec (i_start+b, nnz_start, ind_start,
					    i_next+b, nnz_next, ind_next,
					    b, has_unit_diag,
					    n_max, workspace);
	  max_unique = MAX (n_unique_start, n_unique_next);
	  frac_common = (double)num_common / max_unique;
	  if (frac_common < threshold)
	    break;
	  i_next++;
	}
      if (b_start != NULL)
	b_start[num_blocks] = i_start;
      num_blocks++;
      i_start = i_next;
    }
  if (b_start != NULL)
    b_start[num_blocks] = m;
  return num_blocks;
}

/**
 *  \brief Computes a row-to-block-row lookup table.
 *
 *  \param[in] I_starts Start of each row of a consecutive block row
 *  partitioning.
 *  \param[in] M Number of block rows.
 *  \param[in,out] map An array of size at least I_starts[M] to store
 *  the lookup table.
 *
 *  \returns An initialized row-to-block-row map such that map[i] == K
 *  where I_starts[K] <= i < I_starts[K+1].
 */
static void
MakeBlockMap (const oski_index_t* restrict I_starts, oski_index_t M,
	      oski_index_t* restrict map)
{
  oski_index_t ib;
  for (ib = 0; ib < M; ib++)
    {
      oski_index_t i;
      for (i = I_starts[ib]; i < I_starts[ib+1]; i++)
	map[i] = ib;
    }
}

static int
GetPart (oski_index_t m, oski_index_t n,
	 const oski_index_t* ptr, const oski_index_t* ind,
	 oski_index_t b, int has_unit_diag, int is_symm,
	 const oski_index_t* Tptr, const oski_index_t* Tind,
	 oski_index_t b_T, int has_unit_diag_T,
	 double threshold_r, double threshold_c,
	 oski_index_t* p_M, oski_index_t *p_N,
	 oski_index_t** p_brow, oski_index_t** p_bcol,
	 flag_t* work_f)
{
  int same_part; /* non-zero <==> brow and bcol are the same */
  oski_index_t M, N; /* block dimensions */
  oski_index_t* brow; /* row partition */
  oski_index_t* bcol = NULL; /* column partition */
  int err;

  /* Compute partition sizes */
  M = FindBlockPart (m, ptr, ind, b, has_unit_diag,
		     threshold_r, NULL, n, work_f);
  if (is_symm && IS_REAL_ZERO(threshold_r-threshold_c))
    {
      same_part = 1;
      N = M;
    }
  else
    {
      same_part = 0;
      N = FindBlockPart (n, Tptr, Tind, b_T, has_unit_diag_T,
			 threshold_c, NULL, m, work_f);
    }

  if (p_M != NULL) *p_M = M;
  if (p_N != NULL) *p_N = N;

  if (p_brow == NULL && p_bcol == NULL)
    return same_part;

  /* Allocate space for row/column partitions */
  err = oski_MultiMalloc (__FILE__, __LINE__, 2,
			  (size_t)(sizeof(oski_index_t)*(M+1)), &brow,
			  (size_t)(sizeof(oski_index_t)*(N+1)), &bcol);
  if (err)
    {
      if (p_brow != NULL) *p_brow = NULL;
      if (p_bcol != NULL) *p_bcol = NULL;
      return 0;
    }

  /* Compute and store partitions */
  FindBlockPart (m, ptr, ind, b, has_unit_diag,
		 threshold_r, brow, n, work_f);
  if (same_part)
    {
      assert (M == N);
      oski_CopyMem (bcol, brow, oski_index_t, M+1);
    }
  else
    FindBlockPart (n, Tptr, Tind, b_T, has_unit_diag_T,
		   threshold_c, bcol, m, work_f);

  if (p_brow != NULL)
    *p_brow = brow;
  else
    oski_Free (brow);
  if (p_bcol != NULL)
    *p_bcol = bcol;
  else
    oski_Free (bcol);
  return same_part;
}

/**
 *  \brief Given a CSR matrix and a candidate block row/column
 *  partitioning, counts the number of non-zero blocks and
 *  stored non-zero values required to represent a VBR version of the
 *  matrix.
 *
 *  \param[in] m No. of rows.
 *  \param[in] n No. of columns.
 *  \param[in] ptr Row pointers, of size m+1.
 *  \param[in] ind Column indices, of size equal to the number of
 *  stored structural non-zero entries.
 *  \param[in] b Index base of CSR representation (0- or 1-based).
 *  \param[in] has_unit_diag Non-zero if the CSR representation
 *  assumes an implicit unit diagonal.
 *  \param[in] M No. of block rows in the VBR representation.
 *  \param[in] N No. of block columns in the VBR representation.
 *  \param[in] brow Starting row of each of the M block rows in the
 *  VBR representation. brow is of size M+1, and 0 = brow[0] < brow[1]
 *  < ... < brow[M] = m.
 *  \param[in] bcol Starting column of each of the M block columns in
 *  the VBR representation. bcol is of size M+1, and
 *  0 = bcol[0] < bcol[1] < ... < bcol[N] = n.
 *  \param[out] p_nb Pointer to the number of non-zero blocks needed
 *  to store the CSR matrix in VBR format. If p_nb == NULL, then this
 *  value is not returned.
 *  \param[out] p_nnz Pointer to the number of non-zero elements
 *  needed to store the CSR matrix in VBR format. If p_nnz == NULL,
 *  then this value is not returned.
 *  \param[in,out] f_workspace Temporary workspace, of size at least
 *  N. This workspace must be initialized to zero on entry, and will
 *  be returned as all zero on exit.
 *  \param[in,out] i_workspace Additional temporary workspace, of size
 *  at least n. The values on entry are not used. On exit, the values
 *  are undefined.
 *
 *  \returns The number of structurally non-zero blocks in *p_nb, and
 *  the number of structural non-zeros in *p_nnz.
 */
static void
CountVBRSize (oski_index_t m, oski_index_t n,
	      const oski_index_t* restrict ptr,
	      const oski_index_t* restrict ind,
	      oski_index_t b, int has_unit_diag,
	      oski_index_t M, oski_index_t N,
	      oski_index_t* restrict brow, oski_index_t* restrict bcol,
	      oski_index_t* p_nb, oski_index_t* p_nnz,
	      flag_t* restrict f_workspace,
	      oski_index_t* restrict i_workspace)
{
  oski_index_t nb = 0; /* No. of blocks in the VBR representation */
  oski_index_t nnz = 0; /* No. of non-zeros in the VBR representation */
  oski_index_t I;
  oski_index_t* J_map = i_workspace;
  flag_t* is_marked = f_workspace;

  if (p_nb == NULL && p_nnz == NULL) return;

  MakeBlockMap (bcol, N, J_map);
  for (I = 0; I < M; I++)
    {
      oski_index_t R = brow[I+1] - brow[I]; /* row block size */
      oski_index_t nb_I = 0; /* no. of non-zero blocks in block row I */
      oski_index_t nnz_I = 0; /* no. of non-zeros in block row I */
      oski_index_t i;
      assert (0 <= brow[I] && brow[I] <= brow[I+1] && brow[I+1] <= m);
      for (i = brow[I]; i < brow[I+1]; i++)
	{
	  oski_index_t k;
	  for (k = ptr[i]-b; k < ptr[i+1]-b; k++)
	    {
	      oski_index_t j; /* column index */
	      oski_index_t K; /* equivalent block col index */

	      j = ind[k] - b;   assert (0 <= j && j < n);
	      K = J_map[j];     assert (0 <= K && K < N);

	      if (!is_marked[K])
		{
		  oski_index_t C = bcol[K+1] - bcol[K]; /* col block
							   size */
		  /* "create" a new R x C block */
		  is_marked[K] = 1;
		  nb_I++;
		  nnz_I += R*C;
		}
	    } /* k */

	  if (has_unit_diag && (i < n))
	    {
	      oski_index_t K = J_map[i];
	      if (!is_marked[K])
		{
		  oski_index_t C = bcol[K+1] - bcol[K]; /* col block
							   size */
		  /* "create" a new R x C block */
		  is_marked[K] = 1;
		  nb_I++;
		  nnz_I += R*C;
		}
	    }
	} /* i */
      nb += nb_I;
      nnz += nnz_I;

      /* Reset flag workspace */
      for (i = brow[I]; i < brow[I+1]; i++)
	{
	  oski_index_t k;
	  for (k = ptr[i]-b; k < ptr[i+1]-b; k++)
	    {
	      oski_index_t j = ind[k] - b; /* column index */
	      oski_index_t K = J_map[j]; /* equivalent block col */
	      is_marked[K] = 0;
	    } /* k */

	  if (has_unit_diag && (i < n))
	    {
	      oski_index_t K = J_map[i];
	      is_marked[K] = 0;
	    }
	} /* i */
    } /* I */

  if (p_nb != NULL) *p_nb = nb;
  if (p_nnz != NULL) *p_nnz = nnz;
}


/**
 *  \brief Given a CSR matrix and a candidate block row/column
 *  partitioning, counts the number of non-zero blocks and
 *  stored non-zero values required to represent a VBR version of the
 *  matrix.
 *
 *  \param[in] m No. of rows.
 *  \param[in] n No. of columns.
 *  \param[in] ptr Row pointers, of size m+1.
 *  \param[in] ind Column indices, of size equal to the number of
 *  stored structural non-zero entries.
 *  \param[in] b Index base of CSR representation (0- or 1-based).
 *  \param[in] has_unit_diag Non-zero if the CSR representation
 *  assumes an implicit unit diagonal.
 *  \param[in] M No. of block rows in the VBR representation.
 *  \param[in] N No. of block columns in the VBR representation.
 *  \param[in] brow Starting row of each of the M block rows in the
 *  VBR representation. brow is of size M+1, and 0 = brow[0] < brow[1]
 *  < ... < brow[M] = m.
 *  \param[in] bcol Starting column of each of the M block columns in
 *  the VBR representation. bcol is of size M+1, and
 *  0 = bcol[0] < bcol[1] < ... < bcol[N] = n.
 *  \param[out] nb Number of blocks needed to store the CSR matrix in
 *  VBR format.
 *  \param[out] nnz Number of non-zero elements needed to store the
 *  CSR matrix in VBR format.
 *  \param[in,out] f_workspace Temporary workspace, of size at least
 *  N. This workspace must be initialized to zero on entry, and will
 *  be returned as all zero on exit.
 *  \param[in,out] i_workspace Additional temporary workspace, of size
 *  at least 2*n. The values on entry are not used. On exit, the values
 *  are undefined.
 *
 *  \returns The number of structurally non-zero blocks in *p_nb, and
 *  the number of structural non-zeros in *p_nnz.
 */
static void
CopyToVBR (oski_index_t m, oski_index_t n,
	   const oski_index_t* restrict ptr,
	   const oski_index_t* restrict ind,
	   const oski_value_t* restrict val,
	   oski_index_t b, int has_unit_diag,
	   oski_index_t M, oski_index_t N,
	   oski_index_t* restrict brow, oski_index_t* restrict bcol,
	   oski_index_t nb, oski_index_t nnz,
	   oski_index_t* V_ptr, oski_index_t* V_ind,
	   oski_index_t* V_valptr, oski_value_t* V_val,
	   flag_t* restrict f_workspace,
	   oski_index_t* restrict i_workspace)
{
  oski_index_t nb_cur = 0; /* No. of blocks in the VBR representation */
  oski_index_t nnz_cur = 0; /* No. of non-zeros in the VBR representation */
  oski_index_t I;
  oski_index_t* J_map = i_workspace;
  oski_index_t* block_offset = i_workspace + n;
  flag_t* is_marked = f_workspace;

  MakeBlockMap (bcol, N, J_map);
  V_ptr[0] = 0;
  for (I = 0; I < M; I++)
    {
      oski_index_t R = brow[I+1] - brow[I]; /* row block size */
      oski_index_t i;
      assert (0 <= brow[I] && brow[I] <= brow[I+1] && brow[I+1] <= m);
      for (i = brow[I]; i < brow[I+1]; i++)
	{
	  oski_index_t dr = i - brow[I];
	  oski_index_t k;
	  for (k = ptr[i]-b; k < ptr[i+1]-b; k++)
	    {
	      oski_index_t j = ind[k]-b; /* column index */
	      oski_index_t K = J_map[j]; /* equivalent block col index */
	      oski_index_t C = bcol[K+1] - bcol[K]; /* col block size */
	      oski_value_t* v_start; /* Start of block in V_val */
	      oski_index_t dc;

	      if (!is_marked[K])
		{
		  /* "create" a new R x C block */
		  is_marked[K] = 1;
		  block_offset[K] = nnz_cur;

		  V_valptr[nb_cur] = nnz_cur;
		  V_ind[nb_cur] = K;
		  oski_ZeroMem (V_val + nnz_cur,
				sizeof(oski_value_t) * R * C);

		  nb_cur++;
		  nnz_cur += R*C;
		}

	      /* Accumulate non-zero value into the right place */
	      dc = j - bcol[K];
	      assert (0 <= dr && dr < R);
	      assert (0 <= dc && dc < C);
	      v_start = V_val + block_offset[K];
	      VAL_INC (v_start[dr*C + dc], val[k]);
	    } /* k */

	  if (has_unit_diag && (i < n))
	    {
	      oski_index_t K = J_map[i];
	      oski_index_t C = bcol[K+1] - bcol[K]; /* col block size */
	      oski_index_t dc;
	      oski_value_t* v_start; /* Start of block in V_val */

	      if (!is_marked[K])
		{
		  /* "create" a new R x C block */
		  is_marked[K] = 1;
		  block_offset[K] = nnz_cur;

		  V_valptr[nb_cur] = nnz_cur;
		  V_ind[nb_cur] = K;
		  oski_ZeroMem (V_val + nnz_cur,
				sizeof(oski_value_t) * R * C);

		  nb_cur++;
		  nnz_cur += R*C;
		}
	      /* Set diagonal entry */
	      dc = i - bcol[K];
	      assert (0 <= dr && dr < R);
	      assert (0 <= dc && dc < C);
	      v_start = V_val + block_offset[K];
	      VAL_SET_ONE (v_start[dr*C + dc]);
	    }
	} /* i */

      V_ptr[I+1] = nb_cur;

      /* Reset flag workspace */
      for (i = brow[I]; i < brow[I+1]; i++)
	{
	  oski_index_t k;
	  for (k = ptr[i]-b; k < ptr[i+1]-b; k++)
	    {
	      oski_index_t j = ind[k] - b; /* column index */
	      oski_index_t K = J_map[j]; /* equivalent block col */
	      is_marked[K] = 0;
	    } /* k */

	  if (has_unit_diag && (i < n))
	    {
	      oski_index_t K = J_map[i];
	      is_marked[K] = 0;
	    }
	} /* i */
    } /* I */

  /* Sanity tests */
  assert (nb_cur == nb);
  assert (nnz_cur == nnz);
  V_valptr[nb] = nnz;
}

static int
ConvertToVBR (oski_index_t m, oski_index_t n,
	      const oski_index_t* ptr, const oski_index_t* ind,
	      const oski_value_t* val,
	      oski_index_t b, int has_unit_diag, int is_symm,
	      const oski_index_t* Tptr, const oski_index_t* Tind,
	      oski_index_t b_T, int has_unit_diag_T,
	      double threshold_r, double threshold_c,
	      oski_index_t* p_M, oski_index_t* p_N,
	      oski_index_t** p_brow, oski_index_t** p_bcol,
	      oski_index_t** p_V_ptr, oski_index_t** p_V_ind,
	      oski_index_t** p_V_valptr, oski_value_t** p_V_val)
{
  /* VBR data structure */
  oski_index_t M, N; /* Block dimensions */
  oski_index_t* brow;
  int same_part;
  oski_index_t* bcol;
  oski_index_t* V_ptr;
  size_t bytes_V_ptr;
  oski_index_t* V_ind;
  size_t bytes_V_ind;
  oski_index_t* V_valptr;
  size_t bytes_V_valptr;
  oski_value_t* V_val;
  size_t bytes_V_val;

  flag_t* work_f;
  size_t len_f;
  oski_index_t* work_i;
  size_t len_i;

  oski_index_t nb; /* no. of blocks in the VBR representation */
  oski_index_t nnz; /* no. of non-zeros in the VBR representation */

  int err;

  len_f = 2 * (MAX(m,n)+1);
  len_i = MAX(m, n);
  err = oski_MultiMalloc (__FILE__, __LINE__, 2,
			  len_f * sizeof(oski_index_t), &work_f,
			  2 * len_i * sizeof(oski_index_t), &work_i);
  if (err)
    return err;

  same_part = GetPart (m, n, ptr, ind, b, has_unit_diag, is_symm,
		       Tptr, Tind, b_T, has_unit_diag_T,
		       threshold_r, threshold_c,
		       &M, &N, &brow, &bcol, work_f);
  if (brow == NULL || bcol == NULL)
    {
      oski_Free (work_f);
      oski_Free (work_i);
      return ERR_OUT_OF_MEMORY;
    }

  /* Determine data structure size */
  CountVBRSize (m, n, ptr, ind, b, has_unit_diag, M, N, brow, bcol,
		&nb, &nnz, work_f, work_i);
  bytes_V_val = nnz * sizeof(oski_value_t);
  bytes_V_valptr = (nb+1) * sizeof(oski_index_t);
  bytes_V_ind = nb * sizeof(oski_index_t);
  bytes_V_ptr = (M+1) * sizeof(oski_index_t);

  err = oski_MultiMalloc (__FILE__, __LINE__, 4,
			  bytes_V_val, &V_val,
			  bytes_V_valptr, &V_valptr,
			  bytes_V_ind, &V_ind,
			  bytes_V_ptr, &V_ptr);
  if (err)
      oski_FreeAll (4, work_f, work_i, brow, bcol);
  else
    CopyToVBR (m, n, ptr, ind, val, b, has_unit_diag,
	       M, N, brow, bcol, nb, nnz,
	       V_ptr, V_ind, V_valptr, V_val,
	       work_f, work_i);

  oski_FreeAll (2, work_f, work_i);

  if (p_M != NULL) *p_M = M;
  if (p_N != NULL) *p_N = N;
  if (p_brow != NULL) *p_brow = brow; else oski_Free (brow);
  if (p_bcol != NULL) *p_bcol = bcol; else oski_Free (bcol);
  if (p_V_ptr != NULL) *p_V_ptr = V_ptr; else oski_Free (V_ptr);
  if (p_V_ind != NULL) *p_V_ind = V_ind; else oski_Free (V_ind);
  if (p_V_val != NULL) *p_V_val = V_val; else oski_Free (V_val);
  if (p_V_valptr != NULL) *p_V_valptr = V_valptr; else oski_Free (V_valptr);

  return 0;
}

/**
 *  \brief
 *
 *  The variable argument list must contain the following parameters
 *  in the order listed:
 *    -# \f$\rho_r\f$: Block-row partitioning threshold.
 *    -# \f$\rho_c\f$: Block-column partitioning threshold.
 *
 *  \returns A pointer to a new oski_matVBR_t object containing the
 *  input matrix in VBR format, or NULL on error.
 */
void *
oski_CreateMatReprFromCSR (const oski_matCSR_t * mat,
			   const oski_matcommon_t * props, ...)
{
  oski_matVBR_t* A = NULL;

  va_list ap;
  double threshold_r = 1.0;
  double threshold_c = 1.0;

  int did_copy = 0;
  oski_matCSR_t* mat_full = NULL;
  oski_index_t b_full; /* index base for mat_full */
  int has_unit_diag_full;

  int did_trans = 0;
  oski_matCSR_t* mat_trans = NULL;
  oski_index_t b_trans; /* index base for mat_trans */
  int has_unit_diag_trans;

  int err;

  if (mat == NULL || props == NULL)
    return NULL;

#if 0
  va_start (ap, props);
  threshold_r = va_arg (ap, double);
  threshold_c = va_arg (ap, double);
  va_end (ap);
#else
  threshold_r = 1.0;
  threshold_c = 1.0;
#endif
  if (threshold_r < 0.0 || threshold_r > 1.0) return NULL;
  if (threshold_c < 0.0 || threshold_c > 1.0) return NULL;
  oski_PrintDebugMessage (1, "Creating VBR(rho_r=%f, rho_c=%f) ...",
			  threshold_r, threshold_c);

  A = oski_Malloc (oski_matVBR_t, 1);
  if (A == NULL) return NULL;

  did_copy = ExpandSymm (mat, props, &mat_full);
  if (mat_full == NULL)
    {
      OSKI_ERR (oski_CreateMatReprFromCSR, ERR_OUT_OF_MEMORY);
      oski_Free (A);
      return NULL;
    }
  if (did_copy)
    {
      b_full = 0;
      has_unit_diag_full = 0;
    }
  else
    {
      b_full = mat->base_index;
      has_unit_diag_full = mat->has_unit_diag_implicit;
    }

  did_trans = TransposeSymm (mat, props, &mat_trans);
  if (mat_trans == NULL)
    {
      if (did_copy)
	DestroyCSR (mat_full);
      oski_Free (A);
      return NULL;
    }
  if (did_trans)
    {
      b_trans = 0;
      has_unit_diag_trans = 0;
    }
  else
    {
      b_trans = mat->base_index;
      has_unit_diag_full = mat->has_unit_diag_implicit;
    }

  err = ConvertToVBR (props->num_rows, props->num_cols,
		      mat_full->ptr, mat_full->ind, mat_full->val,
		      b_full, has_unit_diag_full, did_copy,
		      mat_trans->ptr, mat_trans->ind,
		      b_trans, has_unit_diag_trans,
		      threshold_r, threshold_c,
		      &(A->mb), &(A->nb),
		      &(A->brow), &(A->bcol),
		      &(A->ptr), &(A->ind), &(A->valptr), &(A->val));

  if (did_copy)
    DestroyCSR (mat_full);
  if (did_trans)
    DestroyCSR (mat_trans);

  if (err)
    {
      oski_Free (A);
      return NULL;
    }
  return A;
}

oski_matCSR_t *
oski_ConvertMatReprToCSR (const void *mat, const oski_matcommon_t * props)
{
  OSKI_ERR_NOT_IMPLEMENTED (oski_ConvertMatReprToCSR,
			    "Convert VBR matrix to CSR");
  return NULL;
}

void
oski_DestroyMatRepr (void *mat)
{
  oski_matVBR_t* A = (oski_matVBR_t *)mat;
  if (A == NULL) return;
  if (A->brow != NULL) oski_Free (A->brow);
  if (A->bcol != NULL) oski_Free (A->bcol);
  if (A->ptr != NULL) oski_Free (A->ptr);
  if (A->ind != NULL) oski_Free (A->ind);
  if (A->valptr != NULL) oski_Free (A->valptr);
  if (A->val != NULL) oski_Free (A->val);
  oski_Free (A);
}

void *
oski_CopyMatRepr (const void *mat, const oski_matcommon_t * props)
{
  const oski_matVBR_t* A_src = (const oski_matVBR_t *)mat;
  oski_matVBR_t* A_copy = NULL;
  oski_index_t M, N, nb, nnz;

  size_t bytes_brow;
  size_t bytes_bcol;
  size_t bytes_ptr;
  size_t bytes_ind;
  size_t bytes_valptr;
  size_t bytes_val;
  int err;

  if (A_src == NULL) return NULL;
  A_copy = oski_Malloc (oski_matVBR_t, 1);
  if (A_copy == NULL) return NULL;

  M = A_src->mb;
  N = A_src->nb;
  nb = A_src->ptr[M];
  nnz = A_src->valptr[nb];

  bytes_brow = sizeof(oski_index_t) * (M+1);
  bytes_bcol = sizeof(oski_index_t) * (N+1);
  bytes_ptr = sizeof(oski_index_t) * (M+1);
  bytes_ind = sizeof(oski_index_t) * nb;
  bytes_valptr = sizeof(oski_index_t) * (nb+1);
  bytes_val = sizeof(oski_value_t) * nnz;

  err = oski_MultiMalloc (__FILE__, __LINE__, 6,
			  bytes_brow, &(A_copy->brow),
			  bytes_bcol, &(A_copy->bcol),
			  bytes_ptr, &(A_copy->ptr),
			  bytes_ind, &(A_copy->ind),
			  bytes_valptr, &(A_copy->valptr),
			  bytes_val, &(A_copy->val));
  if (err)
    {
      oski_Free (A_copy);
      return NULL;
    }

  oski_CopyMem (A_copy->brow, A_src->brow, oski_index_t, M+1);
  oski_CopyMem (A_copy->bcol, A_src->bcol, oski_index_t, N+1);
  oski_CopyMem (A_copy->ptr, A_src->ptr, oski_index_t, M+1);
  oski_CopyMem (A_copy->ind, A_src->ind, oski_index_t, nb);
  oski_CopyMem (A_copy->valptr, A_src->valptr, oski_index_t, nb+1);
  oski_CopyMem (A_copy->val, A_src->val, oski_value_t, nnz);

  return A_copy;
}

/* --------------------------------------------------------------------
 *  Lua transformations.
 */

#include <oski/xforms_internal.h>

/**
 *  \brief
 *
 *  The VBR implementation expects the following arguments on
 *  the stack:
 *    -# Destination matrix type ('VBR')
 *    -# Source matrix ('CSR')
 *    -# @LIST-ADDITIONAL-OPTIONS-IF-ANY-HERE@
 */
int
oski_CreateLuaMatReprFromCSR (lua_State * L)
{
  return oski_CreateLuaMatReprGenericFromCSR (L, "VBR");
}

/* eof */
