/**
 *  \file expand.c
 *  \brief Routine to expand a symmetric/Hermitian matrix to full
 *  storage.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/CSR/format.h>
#include <oski/CSR/module.h>

/**
 *  \brief Temporary array element structure containing an integer index
 *  and its corresponding scalar non-zero value.
 */
typedef struct tagindvalpair_t
{
  oski_index_t index;  /**< Integer index */
  oski_value_t value;  /**< Corresponding non-zero value */
} indvalpair_t;


/**
 *  \brief Compare two index-value pairs (see #indvalpair_t), a and b,
 *  and returns 0 if index(a) == index(b), 1 if index(a) > index(b),
 *  and -1 if index(a) < index(b).
 *
 *  Used as a callback element comparison function for qsort().
 */
static int
CompareIndValPairs (const void *pa, const void *pb)
{
  const indvalpair_t *a = (const indvalpair_t *) pa;
  const indvalpair_t *b = (const indvalpair_t *) pb;

  if (a->index > b->index)
    return 1;
  else if (a->index < b->index)
    return -1;
  else
    return 0;
}

/**
 *  \brief Copies all of the indices ('base'-based) and values from row i
 *  (0-based) of A to the array row of maximum length max_nnz.
 */
static void
CopyRowToPairs (const oski_matCSR_t * A, oski_index_t i,
		indvalpair_t * row, oski_index_t max_nnz)
{
  oski_index_t base = A->base_index;
  oski_index_t a = A->ptr[i] - base;
  oski_index_t b = A->ptr[i + 1] - base;
  oski_index_t nnz = b - a;

  int k;

  assert (nnz <= max_nnz);

  for (k = 0; k < nnz; k++)
    {
      assert ((a + k) < b);
      row[k].index = A->ind[a + k];
      VAL_ASSIGN (row[k].value, A->val[a + k]);
    }
}

/**
 *  \brief Copies all of the indices and values in
 *  the array row (of maximum length max_nnz) to row i
 *  (0-based) of A to the array row of maximum length max_nnz.
 */
static void
CopyPairsToRow (const indvalpair_t * row, oski_index_t max_nnz,
		oski_matCSR_t * A, oski_index_t i)
{
  oski_index_t base = A->base_index;
  oski_index_t a = A->ptr[i] - base;
  oski_index_t b = A->ptr[i + 1] - base;
  oski_index_t nnz = b - a;

  int k;

  assert (nnz <= max_nnz);

  for (k = 0; k < nnz; k++)
    {
      assert ((a + k) < b);

      A->ind[a + k] = row[k].index;
      VAL_ASSIGN (A->val[a + k], row[k].value);
    }
}

/**
 *  \brief Sorts the column indices within each row of the sparse
 *  matrix A in ascending order.
 *
 *  \param[in,out] A Matrix whose indices are to be sorted.
 *  \param[in] m Number of rows in A.
 *
 *  \returns 0 on success, and an error code on error.
 *  Also sets the 'has_sorted_indices' flag of A to 1 on successful
 *  execution. If this flag was already set to 1 on entry, then
 *  no action is taken and 0 returned.
 */
int
oski_SortIndices (oski_matCSR_t * A, oski_index_t m)
{
  oski_index_t i;
  oski_index_t max_nnz;

  indvalpair_t *row;

  if (m <= 0)
    return 0;

  if (A == NULL)
    return ERR_BAD_ARG;

  if (A->has_sorted_indices)
    return 0;

  max_nnz = A->ptr[1] - A->ptr[0];

  for (i = 1; i < m; i++)
    {
      int nnz = A->ptr[i + 1] - A->ptr[i];
      if (nnz > max_nnz)
	max_nnz = nnz;
    }

  row = oski_Malloc (indvalpair_t, max_nnz);
  if (row == NULL && max_nnz > 0)
    return ERR_OUT_OF_MEMORY;

  for (i = 0; i < m; i++)
    {
      oski_index_t nnz = A->ptr[i + 1] - A->ptr[i];

      CopyRowToPairs (A, i, row, max_nnz);
      qsort (row, nnz, sizeof (indvalpair_t), CompareIndValPairs);
      CopyPairsToRow (row, max_nnz, A, i);
    }

  oski_Free (row);

  A->has_sorted_indices = 1;
  return 0;
}

/**
 *  \brief Allocates the basic CSR data structure for an \f$m\times n\f$
 *  matrix with \f$k\f$ explicitly stored elements (e.g., non-zeros),
 *  initializes all storage arrays to 0.
 *
 *  Sets the following default properties:
 *    - 0-based indices
 *    - No implicit entries
 *    - Unsorted indices
 *    - Full storage
 *
 *  \returns A pointer to the newly allocated structure, or NULL
 *  on error.
 */
static oski_matCSR_t *
AllocAndZero (oski_index_t m, oski_index_t nnz)
{
  oski_matCSR_t *A;

  A = oski_Malloc (oski_matCSR_t, 1);
  if (A == NULL)
    return NULL;

  A->ptr = oski_Malloc (oski_index_t, m + 1);
  if (A->ptr == NULL)
    {
      oski_Free (A);
      return NULL;
    }
  A->ind = oski_Malloc (oski_index_t, nnz);
  if (A->ind == NULL && nnz > 0)
    {
      oski_Free (A->ptr);
      oski_Free (A);
      return NULL;
    }
  A->val = oski_Malloc (oski_value_t, nnz);
  if (A->val == NULL && nnz > 0)
    {
      oski_Free (A->ind);
      oski_Free (A->ptr);
      oski_Free (A);
      return NULL;
    }

  oski_ZeroMem (A->ptr, sizeof (oski_index_t) * (m + 1));
  oski_ZeroMem (A->ind, sizeof (oski_index_t) * nnz);
  oski_ZeroMem (A->val, sizeof (oski_value_t) * nnz);

  A->base_index = 0;
  A->has_unit_diag_implicit = 0;
  A->has_sorted_indices = 0;
  A->stored.is_upper = A->stored.is_lower = 1;
  A->is_shared = 0;

  return A;
}

/** \brief Print CSR arrays.
 *
 * \todo Move this to another module.
 */
static void
DUMP (const oski_matCSR_t * A, oski_index_t m)
{
  oski_index_t base = A->base_index;
  oski_index_t i;

  fprintf (stderr,
	   "**** DUMPING matrix with %d rows, %d-based indices, and %d explicit zeros. ***\n",
	   (int) m, (int) base, (int) (A->ptr[m] - A->ptr[0]));

  for (i = 0; i < m; i++)
    {
      oski_index_t k;
      for (k = A->ptr[i] - base; k < A->ptr[i + 1] - base; k++)
	{
	  oski_index_t j;
	  oski_value_t a_ij;

	  j = A->ind[k];
	  VAL_ASSIGN (a_ij, A->val[k]);

#if IS_VAL_COMPLEX
	  fprintf (stderr, "  A(%d, %d) == %g + %gi\n", i + 1, j + 1 - base,
		   _RE (a_ij), _IM (a_ij));
#else
	  fprintf (stderr, "  A(%d, %d) == %g\n", i + 1, j + 1 - base, a_ij);
#endif
	}
    }
}


/**
 *  \brief
 *
 *  \param[in] S Symmetric/Hermitian matrix S.
 *  \param[in] props General properties of S.
 *
 *  \returns A newly allocated CSR matrix corresponding to the full
 *  storage version of S, or NULL when S is not symmetric/Hermitian
 *  or is already using full storage.
 */
oski_matCSR_t *
oski_ExpandCSRToFull (const oski_matCSR_t * S, const oski_matcommon_t * props)
{
  oski_index_t *cur_row_nnz;	/* # of non-zeros in each row of S */
  oski_index_t *new_row_nnz;	/* # of non-zeros in each row of A */
  oski_index_t new_nnz;		/* total # of non-zeros in A */

  oski_index_t i;		/* current row (0-based index) */
  oski_index_t b;		/* index base (0 or 1) */
  oski_index_t n;		/* dimension */

  oski_matCSR_t *A;

  if ((S == NULL) || (props == NULL)	/* must have valid pointers */
      || (!props->pattern.is_symm && !props->pattern.is_herm)
      /* S must be symmetric or Hermitian */
      || (S->stored.is_upper == S->stored.is_lower)
      /* Must not already be using full storage */
    )
    return NULL;

  if (props->num_rows != props->num_cols)	/* must be square */
    return NULL;

  n = props->num_rows;
  b = S->base_index;

  /* set-up */
  cur_row_nnz = oski_Malloc (oski_index_t, n);
  if (cur_row_nnz == NULL && n > 0)
    return NULL;
  oski_ZeroMem (cur_row_nnz, sizeof (oski_index_t) * n);

  new_row_nnz = oski_Malloc (oski_index_t, n);
  if (new_row_nnz == NULL && n > 0)
    {
      oski_Free (cur_row_nnz);
      return NULL;
    }
  oski_ZeroMem (new_row_nnz, sizeof (oski_index_t) * n);

  /*
   * Scan S and count how many new non-zeros we'll need to create.
   *
   * Post:
   *   cur_row_nnz[i] == # of non-zeros in row i of S.
   *   new_row_nnz[i] == # of non-zeros to be stored in row i of A.
   *   new_nnz == total # of non-zeros to be stored in A.
   */
  new_nnz = 0;
  for (i = 0; i < n; i++)
    {
      cur_row_nnz[i] = S->ptr[i + 1] - S->ptr[i];
      new_row_nnz[i] = cur_row_nnz[i];
      new_nnz += new_row_nnz[i];
    }
  for (i = 0; i < n; i++)
    {
      oski_index_t k;
      for (k = S->ptr[i] - b; k < S->ptr[i + 1] - b; k++)
	{
	  oski_index_t j = S->ind[k] - b;	/* column index, 0-based */
	  if (j != i)
	    {
	      new_row_nnz[j]++;
	      new_nnz++;
	    }
	}
    }
  if (S->has_unit_diag_implicit)
    for (i = 0; i < n; i++)
      {
	new_row_nnz[i]++;
	new_nnz++;
      }

  /* initialize new matrix */
  A = AllocAndZero (n, new_nnz);
  if (A == NULL)
    {
      oski_Free (cur_row_nnz);
      oski_Free (new_row_nnz);
      return NULL;
    }

  /*
   *  Initialize row pointers in A.
   *
   *  Post:
   *    A->ptr initialized to the correct, final values.
   *    new_row_nnz[i] reset to be equal to cur_row_nnz[i].
   */
  for (i = 1; i <= n; i++)
    {
      A->ptr[i] = A->ptr[i - 1] + new_row_nnz[i - 1];
      new_row_nnz[i - 1] = cur_row_nnz[i - 1];
    }

  /*
   *  Complete expansion of S to full storage in A.
   *
   *  Post:
   *    A is the full-storage equivalent of S.
   *    new_row_nnz[i] == # of non-zeros in row i of A.
   */
  for (i = 0; i < n; i++)
    {
      oski_index_t cur_nnz = cur_row_nnz[i];
      oski_index_t k_cur = S->ptr[i] - b;
      oski_index_t k_new = A->ptr[i];

      oski_index_t k;

      /* copy current non-zeros from S to A */
      for (k = 0; k < cur_nnz; k++)
	A->ind[k_new + k] = S->ind[k_cur + k] - b;	/* make 0-based */
      oski_CopyMem (A->val + k_new, S->val + k_cur, oski_value_t, cur_nnz);

      /* fill in the symmetric/Hermitian "missing" values */
      while (k_cur < (S->ptr[i + 1] - b))
	{
	  /* column index of non-zero of S, 0-based */
	  oski_index_t j = S->ind[k_cur] - b;

	  if (j != i)		/* if not a non-diagonal element ... */
	    {
	      oski_value_t a_ij;
	      oski_value_t a_ji;

	      VAL_ASSIGN (a_ij, S->val[k_cur]);

	      if (props->pattern.is_herm)
		VAL_ASSIGN_CONJ (a_ji, a_ij);
	      else		/* just symmetric */
		VAL_ASSIGN (a_ji, a_ij);

	      /* position of this transposed element in A */
	      k_new = A->ptr[j] + new_row_nnz[j];

	      /* store */
	      A->ind[k_new] = i;
	      VAL_ASSIGN (A->val[k_new], a_ji);

	      /*  update so next element stored at row j will appear
	       *  at the right place.
	       */
	      new_row_nnz[j]++;
	    }

	  k_cur++;
	}
    }

  /* Store implicit diagonal, if any. */
  if (S->has_unit_diag_implicit)
    for (i = 0; i < n; i++)
      {
	oski_index_t k = A->ptr[i] + new_row_nnz[i];

	assert (k < A->ptr[i + 1]);

	A->ind[k] = i;
	VAL_SET_ONE (A->val[k]);
      }


  /* clean-up */
  oski_Free (cur_row_nnz);
  oski_Free (new_row_nnz);

  /* Sort column indices */
  oski_SortIndices (A, props->num_rows);

  return A;
}

/**
 *  \brief Conditional full-storage expansion: if the specified matrix
 *  is stored in symmetric/Hermitian half-storage CSR, then this routine
 *  returns a newly allocated full-storage matrix, and otherwise returns
 *  the original matrix.
 *
 *  \param[in] mat Matrix to expand conditionally.
 *  \param[in] props Matrix properties.
 *  \param[in,out] p_mat_full Pointer to a pointer in which to store
 *  the result.
 *  \returns 0 if the matrix was not symmetric half-storage format
 *  and sets *p_mat_full to point to a full-storage copy. Otherwise,
 *  returns 1 and sets *p_mat_full to be mat. On error, sets
 *  *p_mat_full to NULL.
 */
int
oski_ConditionallyExpandCSRToFull (const oski_matCSR_t * mat,
				   const oski_matcommon_t * props,
				   oski_matCSR_t ** p_mat_full)
{
  oski_matCSR_t *A_full = NULL;
  int is_symm = 0;

  if (mat == NULL || props == NULL || p_mat_full == NULL)
    return 0;

  is_symm = (props->pattern.is_symm || props->pattern.is_herm)
    && (mat->stored.is_upper != mat->stored.is_lower);

  if (is_symm)
    {
      oski_ExpandCSRToFull_funcpt func_Expand;
      oski_PrintDebugMessage (2, "Expanding symmetric matrix...");
      func_Expand = OSKI_MATTYPE_METHOD ("CSR", ExpandCSRToFull);
      if (func_Expand != NULL)
	A_full = (*func_Expand) (mat, props);
    }
  else
    {				/* already have full storage */
      oski_PrintDebugMessage (2, "Full matrix already stored");
      A_full = (oski_matCSR_t *) mat;
    }

  *p_mat_full = A_full;

  return is_symm;
}

/* eof */
