/**
 *  \file CSR/transpose.c
 *  \ingroup MATTYPE_CSR
 *  \brief Implements a routine to transpose a CSR matrix.
 */

#include <oski/config.h>
#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/CSR/format.h>
#include <oski/CSR/module.h>

/**
 *  \brief Given an array \f$x\f$ of \f$n\f$ values, this routine
 *  computes the cumulative sum \f$y_i = \sum_{k=0}^{i-1} x_k\f$,
 *  where \f$0 \leq i \leq n\f$.
 *
 *  \param[in] x Array of values to sum.
 *  \param[in] n Length of \f$x\f$.
 *  \param[in,out] y An array of size \f$n+1\f$ used to store
 *  the cumulative sum.
 *
 *  \note This routine was "outlined" from its use in this module
 *  (TransposeFullCSR) to avoid an unexplained bug that occurred
 *  when using the Intel C compiler on IA-64.
 */
static void
cumulative_sum (const oski_index_t * x, oski_index_t n, oski_index_t * y)
{
  oski_index_t i;
  if (n == 0)
    return;
  y[0] = 0;
  for (i = 1; i <= n; i++)
    y[i] = y[i - 1] + x[i - 1];
}

/**
 *  \brief Transpose a full-storage \f$m\times n\f$ matrix \f$A\f$
 *  stored in CSR matrix (or equivalently, convert from CSR to CSC).
 *
 *  \param[in] ptr Row pointers.
 *  \param[in] ind Column indices.
 *  \param[in] val Non-zero values, or NULL if the caller only
 *  wishes to transpose the pattern.
 *  \param[in] m Number of rows.
 *  \param[in] n Number of columns.
 *  \param[in] base Index base used for ptr, ind.
 *  \param[out] p_Tptr *p_Tptr stores the \f$n+1\f$ row pointers of
 *  \f$A^T\f$.
 *  \param[out] p_Tind *p_Tind stores the column indices of \f$A^T\f$.
 *  \param[out] p_Tval *p_Tval stores the non-zero values of \f$A^T\f$.
 *
 *  \returns 0 on success, an error code otherwise. This routine
 *  does not call the error handler. If an error occurs, the
 *  output variables *p_Tptr, *p_Tind, and *p_Tval remain
 *  unchanged.
 */
static int
TransposeFullCSR (const oski_index_t * ptr, const oski_index_t * ind,
		  const oski_value_t * val, oski_index_t m, oski_index_t n,
		  oski_index_t base,
		  oski_index_t ** p_Tptr, oski_index_t ** p_Tind,
		  oski_value_t ** p_Tval)
{
  oski_index_t *count_nnz;	/* # of non-zeros in each row */
  oski_index_t i;

  /* Stores the transposed copy */
  oski_index_t nnz;
  oski_index_t *Tptr;
  oski_index_t *Tind;
  oski_value_t *Tval;

  if (ptr == NULL || m == 0 || n == 0)
    {
      *p_Tptr = *p_Tind = NULL;
      *p_Tval = NULL;
      return 0;
    }

  /* Allocate space for the transposed copy. */
  nnz = ptr[m] - base;

  Tptr = oski_Malloc (oski_index_t, n + 1);
  if (Tptr == NULL)
    return ERR_OUT_OF_MEMORY;
  if (nnz > 0)
    {
      int err = oski_MultiMalloc (__FILE__, __LINE__, 2,
				  (size_t)(nnz * sizeof(oski_index_t)),
				  &Tind,
				  (size_t)(nnz * sizeof(oski_value_t)),
				  &Tval);
      if (err)
	{
	  oski_Free (Tptr);
	  return ERR_OUT_OF_MEMORY;
	}
    }
  else /* nnz <= 0 */
    {
      Tind = NULL;
      Tval = NULL;
    }

  /* count # of non-zeros per column */
  count_nnz = oski_Malloc (oski_index_t, n);
  if (count_nnz == NULL)
    {
      oski_FreeAll (3, Tptr, Tind, Tval);
      return ERR_OUT_OF_MEMORY;
    }

  for (i = 0; i < n; i++)
    count_nnz[i] = 0;
  for (i = 0; i < nnz; i++)
    {
      oski_index_t k = ind[i] - base;
      count_nnz[k]++;
    }

  /* Initialize transpose row pointers using 0-based indices. */
  cumulative_sum (count_nnz, n, Tptr);

  /* Copy row pointers so that count_nnz[i] == location in
   * Tind, Tval where the next transposed element should be
   * stored.
   */
  for (i = 0; i < n; i++)
    count_nnz[i] = Tptr[i];

  /* Copy non-zeros into transpose positions */
  for (i = 0; i < m; i++)
    {
      oski_index_t k;
      for (k = ptr[i] - base; k < ptr[i + 1] - base; k++)
	{
	  /* (i, j) is the current non-zero, in 0-based units */
	  oski_index_t j = ind[k] - base;

	  /* location in Tind, Tval where A(i, j) should go */
	  oski_index_t l = count_nnz[j];

	  Tind[l] = i;
	  if (Tval != NULL)
	    VAL_ASSIGN (Tval[l], val[k]);
	  count_nnz[j]++;
	}
    }

  oski_Free (count_nnz);

  /* Perform some basic data structure consistency checks. */
  if (Tptr[n] != ptr[m] - base)
    {
      oski_HandleError (ERR_WRONG_ANS,
			"Error detected after CSR matrix transposition",
			__FILE__, __LINE__,
			"In call to %s(): Original stored non-zero count is %d,"
			" while transposed non-zero count is %d.",
			MACRO_TO_STRING (TransposeFullCSR),
			(int) (ptr[m] - base), (int) (Tptr[n]));

      oski_FreeAll (3, Tptr, Tind, Tval);

      return ERR_WRONG_ANS;
    }

  /* Probably OK; return. */
  *p_Tptr = Tptr;
  *p_Tind = Tind;
  *p_Tval = Tval;
  return 0;
}

/** \brief Conjugate all off-diagonal elements of a CSR representation. */
static void
ConjOffDiagElems (const oski_index_t * ptr, const oski_index_t * ind,
		  oski_value_t * val, oski_index_t m, oski_index_t b)
{
  oski_index_t i;
  for (i = 0; i < m; i++)
    {
      oski_index_t k;
      for (k = ptr[i] - b; k < ptr[i + 1] - b; k++)
	{
	  oski_index_t j = ind[k] - b;
	  if (j != i)
	    VAL_CONJ (val[k]);
	}
    }
}

/**
 *  \brief
 *
 *  \param[in] A Matrix \f$A\f$ to transpose.
 *  \param[in] props Common properties of \f$A\f$.
 *
 *  \returns A pointer to a new CSR matrix storing \f$A^T\f$,
 *  or NULL on memory error.
 *
 *  This implementation maintains symmetric/Hermitian properties
 *  if they apply to \f$A\f$.
 */
oski_matCSR_t *
oski_TransposeCSR (const oski_matCSR_t * A, const oski_matcommon_t * props)
{
  oski_matCSR_t *AT;
  int err;

  if (A == NULL || props == NULL)
    return NULL;

  AT = oski_Malloc (oski_matCSR_t, 1);
  if (AT == NULL)
    return NULL;

  oski_CopyMem (AT, A, oski_matCSR_t, 1);
  AT->ptr = AT->ind = NULL;
  AT->val = NULL;
  AT->is_shared = 0;

  /* Symmetric case is easy: just copy the data */
  if (props->pattern.is_symm || props->pattern.is_herm)
    {
      assert (props->num_rows == props->num_cols);

      /* Copy the array data */
      AT->ptr = oski_Malloc (oski_index_t, props->num_cols + 1);
      if (AT->ptr != NULL)
	{
	  oski_index_t nnz_stored = A->ptr[props->num_rows] - A->base_index;
	  AT->ind = oski_Malloc (oski_index_t, nnz_stored);
	  if (AT->ind != NULL)
	    {
	      AT->val = oski_Malloc (oski_value_t, nnz_stored);
	      if (AT->val != NULL)
		{
		  oski_CopyMem (AT->ptr, A->ptr, oski_index_t,
				props->num_rows + 1);
		  oski_CopyMem (AT->ind, A->ind, oski_index_t, nnz_stored);
		  oski_CopyMem (AT->val, A->val, oski_value_t, nnz_stored);

		  /* If Hermitian, need to conjugate non-zero values */
		  if (props->pattern.is_herm)
		    ConjOffDiagElems (AT->ptr, AT->ind, AT->val,
				      props->num_cols, AT->base_index);

		  return AT;
		}
	      oski_Free (AT->ind);
	    }
	  oski_Free (AT->ptr);
	}
      err = ERR_OUT_OF_MEMORY;
      OSKI_ERR (TransposeCSR, err);
    }
  else				/* Non-symmetric case */
    {
      err = TransposeFullCSR (A->ptr, A->ind, A->val,
			      props->num_rows, props->num_cols, A->base_index,
			      &(AT->ptr), &(AT->ind), &(AT->val));
      AT->base_index = 0;
    }

  if (err)
    {
      oski_Free (AT);
      AT = NULL;
    }

  return AT;
}

/* eof */
