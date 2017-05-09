/**
 *  \file CSR/check.c
 *  \ingroup MATTYPE_CSR
 *  \brief Check CSR properties.
 *
 *  This module implements "efficient" (i.e., O(nnz)) checks of
 *  various asserted input matrix properties.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/CSR/module.h>

/**
 *  \brief Returns 1 <==> pointers are non-decreasing.
 *
 *  \param[in] ptr Row pointers.
 *  \param[in] m Number of rows.
 *  \returns 1 <==> ptr[i] <= ptr[i+1], for all 0 <= i < m.
 *
 *  Calls the error handler and returns 0 if the non-decreasing
 *  index property cannot be verified.
 */
static int
oski_CheckIndNonDecreasing (const oski_index_t * ptr, oski_index_t m)
{
  oski_index_t i;

  for (i = 0; i < m; i++)
    {
      if (ptr[i] > ptr[i + 1])
	{
	  oski_HandleError (ERR_FALSE_INMATPROP,
			    "CSR data structure has an error", __FILE__,
			    __LINE__,
			    "Row pointer at logical row %d equals %d, "
			    "which is greater than the next row pointer (%d)",
			    (int) i + 1, (int) ptr[i], (int) ptr[i + 1]);
	  return 0;
	}
    }
  return 1;
}

/**
 *  \brief Returns 1 <==> indices match the specified base.
 *
 *  \pre base_index is 0 or 1
 *  \pre All pointer arguments are non-NULL.
 */
static int
oski_CheckIndexBase (const oski_index_t * ptr, const oski_index_t * ind,
		     oski_index_t m, oski_index_t n, oski_index_t base_index)
{
  oski_index_t i;

  /* Check row pointers */
  for (i = 0; i < m; i++)
    {
      if ((ptr[i] - base_index) < 0)
	{
	  oski_HandleError (ERR_FALSE_INMATPROP,
			    "CSR data structure has an error", __FILE__,
			    __LINE__,
			    "Row pointer at logical row %d has an illegal value of %d. "
			    "(%d-based indexing assumed.)", (int) i + 1,
			    (int) ptr[i], (int) base_index);
	  return 0;
	}
    }

  /* Check column indices */
  for (i = 0; i < m; i++)
    {
      oski_index_t k;

      for (k = ptr[i] - base_index; k < (ptr[i + 1] - base_index); k++)
	{
	  oski_index_t j = ind[k];

	  if (((j - base_index) < 0) || ((j - base_index) >= n))
	    {
	      oski_HandleError (ERR_FALSE_INMATPROP,
				"CSR data structure has an error", __FILE__,
				__LINE__,
				"%d-th column index (in logical row %d) equals %d, "
				"and is not between %d and %d, inclusive. "
				"(%d-based indexing assumed.)", (int) k,
				(int) i + 1, (int) j, (int) base_index,
				(int) n + base_index - 1, (int) base_index);
	      return 0;
	    }
	}
    }

  return 1;
}

/**
 *  \brief Returns 1 <==> column indices are, within each row, sorted.
 *
 *  \param[in] ptr Row pointers.
 *  \param[in] ind Column indices.
 *  \param[in] m Number of rows.
 *  \param[in] b Base index.
 *  \param[in,out] p_has_sorted_indices Pointer to an integer which,
 *  on input, indicates the ordering property being asserted by the
 *  caller: 1 if the indices are sorted, and 0 otherwise.
 *  If equal to 0 on input but the indices are sorted,
 *  then the value pointed to is changed to 1.
 *
 *  \returns Returns 1 if the column indices are sorted or if
 *  the caller didn't think they were sorted but they are. In the
 *  latter case, also sets *p_has_sorted_indices to 1.
 *
 *  \pre All pointer arguments are non-NULL.
 */
static int
oski_CheckHasSortedInds (const oski_index_t * ptr,
			 const oski_index_t * ind, oski_index_t m,
			 oski_index_t b, int *p_has_sorted_indices)
{
  int assert_is_sorted;		/* Caller's assertion about ordering ... */
  int is_sorted;		/* vs. reality */

  oski_index_t i;

  assert_is_sorted = *p_has_sorted_indices;
  is_sorted = 1;		/* Assume true to start */

  for (i = 0; i < m; i++)
    {
      oski_index_t k;

      for (k = (ptr[i] - b); (k + 1) < (ptr[i + 1] - b); k++)
	{
	  if ((ind[k] > ind[k + 1]))
	    {
	      is_sorted = 0;	/* Not sorted */

	      if (assert_is_sorted)	/* But caller claimed it was... */
		{
		  oski_HandleError (ERR_FALSE_INMATPROP,
				    "CSR data structure has an error",
				    __FILE__, __LINE__,
				    "The indices in logical row %d are not sorted.",
				    (int) i + 1);
		  return 0;
		}
	      else		/* Caller was right---indices aren't sorted */
		return 1;
	    }
	}
    }

  if (is_sorted)		/* In reality, they are sorted */
    *p_has_sorted_indices = 1;

  return 1;
}

/**
 *  \brief Returns 1 <==> column indices are, within each row, unique.
 *
 *  \param[in] ptr Row pointers.
 *  \param[in] ind Column indices.
 *  \param[in] m Number of rows.
 *  \param[in] n Number of columns.
 *  \param[in] b Base index.
 *  \param[in,out] p_has_unique_indices Pointer to an integer which,
 *  on input, indicates the uniqueness property being asserted by the
 *  caller: 1 if the indices are unique, and 0 otherwise.
 *  If equal to 0 on input but the indices are unique,
 *  then the value pointed to is changed to 1.
 *
 *  \returns Returns 1 if the column indices are unique or if
 *  the caller didn't think they were unique but they are. In the
 *  latter case, also sets *p_has_unique_indices to 1.
 *
 *  \note This routine requires a temporary storage space of
 *  n elements. If the routine cannot malloc this space itself,
 *  it simply returns 2 without calling the error handler.
 *
 *  \pre All pointer arguments are non-NULL.
 *  \pre Matrix is either 0-based or 1-based.
 *  \pre Column indices are legal.
 */
static int
oski_CheckHasUniqueInds (const oski_index_t * ptr, const oski_index_t * ind,
			 oski_index_t m, oski_index_t n, oski_index_t b,
			 int *p_has_unique_indices)
{
  int assert_is_unique;		/* Caller's assertion about uniqueness ... */
  int is_unique;		/* vs. reality */

  /*
   *  num_occur[j] == number of occurrences of column index j in
   *  the current row, where 0 <= j <= n to allow for either 0 or
   *  1 based indexing.
   */
  oski_index_t *num_occur;

  /* temp iteration var */
  oski_index_t i;

  assert_is_unique = *p_has_unique_indices;

  num_occur = oski_MallocNoError (oski_index_t, n + 1);
  if (!num_occur)
    {
      if (!assert_is_unique)
	return 1;		/* Caller asserts nothing about uniqueness */
      else			/* Caller asserted uniqueness but we can't check */
	return 2;
    }

  is_unique = 1;		/* Assume true to start */

  for (i = 0; i < m; i++)
    {
      oski_index_t k;

      /* First pass: Count occurrences */
      for (k = (ptr[i] - b); k < (ptr[i + 1] - b); k++)
	num_occur[ind[k]]++;

      /* Second pass: Check for uniqueness */
      for (k = (ptr[i] - b); k < (ptr[i + 1] - b); k++)
	{
	  if (num_occur[ind[k]] > 1)
	    {
	      is_unique = 0;
	      break;
	    }
	}

      /* Did the caller lie? */
      if (assert_is_unique && !is_unique)
	{
	  oski_HandleError (ERR_FALSE_INMATPROP,
			    "CSR data structure has an error", __FILE__,
			    __LINE__,
			    "Column indices in logical row %d are not unique",
			    (int) i + 1);
	  break;
	}

      /* Third pass: Reset num_occur array.
       *
       * This pass is needed to keep the overall complexity
       * of this check to O(nnz).
       */
      for (k = (ptr[i] - b); k < (ptr[i + 1] - b); k++)
	num_occur[ind[k]] = 0;
    }

  if (is_unique)		/* In reality, they are unique */
    *p_has_unique_indices = 1;

  oski_Free (num_occur);
  return is_unique;
}

/**
 *  \brief Returns 0 <==> Pattern is upper triangular, and
 *  returns the offending logical row number (1-based) otherwise.
 *
 *  This implementation does not call the error handler if the
 *  pattern is not upper triangular.
 *
 *  \pre All pointer arguments are not NULL.
 */
static int
oski_CheckIsUpperTriangular (const oski_index_t * ptr,
			     const oski_index_t * ind, oski_index_t m,
			     oski_index_t b)
{
  oski_index_t i;
  for (i = 0; i < m; i++)
    {
      oski_index_t k;
      for (k = (ptr[i] - b); k < (ptr[i + 1] - b); k++)
	if ((ind[k] - b) < i)
	  return i + 1;
    }
  return 0;
}

/**
 *  \brief Returns 0 <==> Pattern is lower triangular, and
 *  returns the offending logical row number (1-based) otherwise.
 *
 *  This implementation does not call the error handler if the
 *  pattern is not lower triangular.
 *
 *  \pre All pointer arguments are not NULL.
 */
static int
oski_CheckIsLowerTriangular (const oski_index_t * ptr,
			     const oski_index_t * ind, oski_index_t m,
			     oski_index_t b)
{
  oski_index_t i;
  for (i = 0; i < m; i++)
    {
      oski_index_t k;
      for (k = (ptr[i] - b); k < (ptr[i + 1] - b); k++)
	if ((ind[k] - b) > i)
	  return i + 1;
    }
  return 0;
}

/**
 *  \brief Verify that the stored part of the CSR matrix matches
 *  the specified pattern.
 *
 *  \param[in] ptr Row pointers.
 *  \param[in] ind Column indices.
 *  \param[in] m Number of rows.
 *  \param[in] n Number of cols.
 *  \param[in] b Base index.
 *  \param[in] pattern Asserted pattern type.
 *
 *  \returns 1 <==> The non-zero pattern is "consistent" with the
 *  specified pattern.
*
*  \todo Check symmetric and Hermitian full storage cases.
 */
static int
oski_CheckPattern (const oski_index_t * ptr, const oski_index_t * ind,
		   oski_index_t m, oski_index_t n, oski_index_t b,
		   oski_inmatprop_t pattern)
{
  int offending_row;

  switch (pattern)
    {
    case MAT_SYMM_UPPER:
    case MAT_SYMM_LOWER:
    case MAT_SYMM_FULL:
    case MAT_HERM_UPPER:
    case MAT_HERM_LOWER:
    case MAT_HERM_FULL:
      if (m != n)
	{
	  oski_HandleError (ERR_FALSE_INMATPROP,
			    "CSR data structure has an error", __FILE__,
			    __LINE__,
			    "Input matrix asserted to be symmetric or Hermitian"
			    " but is not square (it has dimensions %d x %d)",
			    (int) m, (int) n);
	  return 0;
	}

    default:
      /* do nothing */
      break;
    }

  switch (pattern)
    {
    case MAT_TRI_UPPER:
    case MAT_SYMM_UPPER:
    case MAT_HERM_UPPER:
      offending_row = oski_CheckIsUpperTriangular (ptr, ind, m, b);
      if (offending_row >= 1)
	{
	  oski_HandleError (ERR_FALSE_INMATPROP,
			    "CSR data structure has an error", __FILE__,
			    __LINE__,
			    "Input matrix asserted to have upper triangular "
			    "storage has an element in the lower triangle "
			    "in logical row %d", offending_row);
	  return 0;
	}
      break;

    case MAT_TRI_LOWER:
    case MAT_SYMM_LOWER:
    case MAT_HERM_LOWER:
      offending_row = oski_CheckIsLowerTriangular (ptr, ind, m, b);
      if (offending_row >= 1)
	{
	  oski_HandleError (ERR_FALSE_INMATPROP,
			    "CSR data structure has an error", __FILE__,
			    __LINE__,
			    "Input matrix asserted to have lower triangular "
			    "storage has an element in the upper triangle "
			    "in logical row %d", offending_row);
	  return 0;
	}
      break;

    case MAT_SYMM_FULL:
    case MAT_HERM_FULL:
			/** \todo Add symmetric/Hermitian pattern check */
      break;

    default:
      break;
    }
  return 1;
}

/**
 *  \brief Verify whether or not matrix has an implicit unit diagonal.
 *
 *  \param[in] ptr Row pointers.
 *  \param[in] ind Column indices.
 *  \param[in] m Number of rows.
 *  \param[in] b Base index (0 or 1).
 *  \param[in,out] has_unit_diag_implicit An integer specifying whether
 *  the caller believes the pattern has an implicit unit diagonal
 *  (=1) or not (=0).
 *
 *  \returns 1 if the pattern is consistent with the asserted
 *  pattern, or 0 otherwise.
 */
static int
oski_CheckUnitDiagImplicit (const oski_index_t * ptr,
			    const oski_index_t * ind, oski_index_t m,
			    oski_index_t b, int has_unit_diag_implicit)
{
  oski_index_t i;

  if (!has_unit_diag_implicit)	/* no need to check */
    return 1;

  for (i = 0; i < m; i++)
    {
      oski_index_t k;

      for (k = (ptr[i] - b); k < (ptr[i + 1] - b); k++)
	{
	  oski_index_t j = ind[k] - b;	/* col index (0-based) */

	  if (i == j)		/* diagonal element! */
	    {
	      oski_HandleError (ERR_FALSE_INMATPROP,
				"CSR data structure has an error", __FILE__,
				__LINE__,
				"Input matrix asserted to have an implicit unit "
				"diagonal has an explicit diagonal element "
				"in logical row %d", (int) i + 1);
	      return 0;
	    }
	}
    }

  return 1;
}

/**
 *  \brief Check run-time environment variable, OSKI_BYPASS_CHECK,
 *  to see whether the user wants us to skip the checking of
 *  asserted input matrix properties.
 */
static int
BypassCheck (void)
{
  const char *env_val = getenv ("OSKI_BYPASS_CHECK");
  if (env_val != NULL)
    return strcmp (env_val, "yes") == 0;
  else
    return 0;
}

/**
 *  \brief Verify that a CSR matrix representation satisfies
 *  asserted properties.
 *
 *  This routine verifies that the following properties, if
 *  asserted, are true:
 *    - Row pointer indices are non-decreasing
 *    - 0- or 1-based indexing and indices bound by matrix dimensions.
 *    - Sorted column indices
 *    - Unique column indices.
 *    - Triangular storage
 *    - Implicit unit diagonal (i.e., if asserted, no diagonals
 *    elements must be present).
 *
 *  The implementation relies on O(nnz) algorithms to verify
 *  these properties.
 *
 *  If any of the properties can be strengthened to improve
 *  kernel performance, then the properties are modified
 *  to reflect the stronger assertion. There are two specific
 *  examples:
 *    - The caller asserts that column indices are not sorted
 *    but they are.
 *    - The caller asserts that indices are not unique within
 *    a row but they are.
 *
 *  \param[in] ptr Row pointers.
 *  \param[in] ind Column indices.
 *  \param[in] m Number of rows.
 *  \param[in] n Number of columns.
 *  \param[in,out] props Asserted properties.
 *
 *  \returns This routine performs a series of O(nnz) checks
 *  to verify asserted properties of the given CSR matrix pattern.
 *  If any false properties are detected, this routine returns 0.
 *  Otherwise, it returns 1. If any properties can be strengthened,
 *  the corresponding property is changed in props.
 *
 *  \pre Pointer arguments are not NULL and dimensions are non-negative.
 */
int
oski_CheckCSR (const oski_index_t * ptr, const oski_index_t * ind,
	       oski_index_t m, oski_index_t n, oski_inmatpropset_t * props)
{
  int is_ok;

  assert (m >= 0);
  assert (n >= 0);
  assert (props != NULL);
  assert (ptr != NULL);
  if (ptr[m] > (props->index_base))
    assert (ind != NULL);

  if (!BypassCheck ())
    {
      oski_PrintDebugMessage (2, "Checking asserted matrix properties...");

      is_ok = oski_CheckIndNonDecreasing (ptr, m)
	&& oski_CheckIndexBase (ptr, ind, m, n, props->index_base)
	&& oski_CheckHasSortedInds (ptr, ind, m, props->index_base,
				    &(props->has_sorted_indices))
	&& (oski_CheckHasUniqueInds (ptr, ind, m, n, props->index_base,
				     &(props->has_unique_indices))
	    || !(props->has_unique_indices))
	&& oski_CheckPattern (ptr, ind, m, n, props->index_base,
			      props->pattern)
	&& oski_CheckUnitDiagImplicit (ptr, ind, m, props->index_base,
				       props->has_unit_diag_implicit);

      if (is_ok)
	oski_PrintDebugMessage (3, "Input matrix is probably OK.");
      else
	oski_PrintDebugMessage (3, "An asserted property was false.");
    }
  else
    {				/* Bypass checks */
      oski_PrintDebugMessage (2, "WARNING: Bypassed matrix check.");
      is_ok = 1;
    }
  return is_ok;
}

/* eof */
