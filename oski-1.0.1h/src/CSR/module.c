/**
 *  \file src/CSR/module.c
 *  \ingroup MATTYPE_CSR
 *  \brief Compressed sparse row (CSR) module.
 */

#include <math.h>
#include <string.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/CSR/module.h>

const char *
oski_GetShortDesc (void)
{
  return "Compressed sparse row (CSR) module";
}

const char *
oski_GetLongDesc (void)
{
  return "This module implements compressed sparse row (CSR) format.";
}

void
oski_InitModule (void)
{
  oski_PrintDebugMessage (2, "Loading CSR(ind_t=%s, val_t=%s)",
			  MACRO_TO_STRING (IND_T), MACRO_TO_STRING (VAL_T));
  oski_PrintDebugMessage (3, oski_GetShortDesc ());
}

void
oski_CloseModule (void)
{
  oski_PrintDebugMessage (2, "Shutting down CSR(%s, %s) module",
			  MACRO_TO_STRING (IND_T), MACRO_TO_STRING (VAL_T));
}

/**
 *  \brief Copy raw CSR representation to a concrete CSR matrix type
 *  representation.
 *
 *  \param[out] A Concrete CSR object.
 *  \param[in] mode Copy mode.
 *  \param[in] m Number of rows in the raw representation.
 *  \param[in] ptr Row pointers of the raw representation.
 *  \param[in] ind Column indices of the raw representation.
 *  \param[in] val Non-zero values of the raw representation.
 *  \param[in] index_base Base index for interpreting the values
 *  in ptr and ind.
 *
 *  \returns 1 if successful, 0 otherwise.
 *
 *  \pre All pointers are non-NULL.
 *  \pre The raw representation is valid.
 *  \pre m is valid (non-negative).
 */
static int
CopyPointers (oski_matCSR_t * A, oski_copymode_t mode,
	      oski_index_t * ptr, oski_index_t * ind, oski_value_t * val,
	      oski_index_t m, oski_index_t index_base)
{
  oski_index_t nnz;

  assert (A != NULL);
  assert (ptr != NULL);
  assert (m >= 0);

  switch (mode)
    {
    case COPY_INPUTMAT:
      A->is_shared = 0;

      A->ptr = oski_Malloc (oski_index_t, m + 1);
      if (A->ptr == NULL)
	return 0;

      oski_CopyMem (A->ptr, ptr, oski_index_t, m + 1);

      nnz = ptr[m] - ptr[0];
      if (nnz > index_base)
	{
	  A->ind = oski_Malloc (oski_index_t, nnz);
	  A->val = oski_Malloc (oski_value_t, nnz);

	  if ((A->ind == NULL || A->val == NULL) && (nnz > 0))
	    {
	      oski_Free (A->ptr);
	      oski_Free (A->ind);
	      oski_Free (A->val);
	      return 0;
	    }

	  oski_CopyMem (A->ind + ptr[0] - index_base,
			ind + ptr[0] - index_base, oski_index_t, nnz);
	  oski_CopyMem (A->val + ptr[0] - index_base,
			val + ptr[0] - index_base, oski_value_t, nnz);
	}
      else			/* No stored non-zeros, so no need to allocate */
	{
	  A->ind = NULL;
	  A->val = NULL;
	}
      break;

    case SHARE_INPUTMAT:
      A->is_shared = 1;
      A->ptr = ptr;
      A->ind = ind;
      A->val = val;
      break;
    }

  return 1;
}

/**
 *  \brief Returns the number of explicitly stored diagonal elements.
 */
static oski_index_t
CountDiagElems (const oski_index_t * ptr, const oski_index_t * ind,
		oski_index_t m, oski_index_t b)
{
  oski_index_t i, count;

  count = 0;
  for (i = 0; i < m; i++)
    {
      oski_index_t k;

      for (k = (ptr[i] - b); k < (ptr[i + 1] - b); k++)
	{
	  oski_index_t j0 = ind[k] - b;	/* zero-based col index */
	  if (i == j0)
	    count++;
	}
    }
  return count;
}

/**
 *  \brief Translate asserted properties of the input matrix into
 *  common properties of the output matrix.
 *
 *  \param[out] out_props Output matrix common properties.
 *  \param[in] ptr Row pointers.
 *  \param[in] ind Column indices.
 *  \param[in] m Number of logical rows.
 *  \param[in] n Number of logical columns.
 *  \param[in] props Asserted input matrix properties.
 *
 *  \pre All input matrix properties are valid and pointers are
 *  non-NULL.
 *  \post Sets all fields of out_props.
 */
static void
SetCommonProps (oski_matcommon_t * out_props,
		const oski_index_t * ptr, const oski_index_t * ind,
		oski_index_t m, oski_index_t n,
		const oski_inmatpropset_t * props)
{
  oski_index_t num_diag_elems;
  oski_index_t nnz_stored_offdiag;

  out_props->num_rows = m;
  out_props->num_cols = n;

  /* Compute properties of the diagonal */
  out_props->has_unit_diag = props->has_unit_diag_implicit;
  if (props->has_unit_diag_implicit)
    {
      num_diag_elems = OSKI_MIN (m, n);
      nnz_stored_offdiag = ptr[m] - props->index_base;
    }
  else
    {
      num_diag_elems = CountDiagElems (ptr, ind, m, props->index_base);
      nnz_stored_offdiag = ptr[m] - props->index_base - num_diag_elems;
    }

  /* Set shape and number of logical non-zeros properties */
  out_props->num_nonzeros = num_diag_elems + nnz_stored_offdiag;
  switch (props->pattern)
    {
    case MAT_TRI_UPPER:
      out_props->pattern.is_tri_upper = 1;
      break;

    case MAT_TRI_LOWER:
      out_props->pattern.is_tri_lower = 1;
      break;

    case MAT_SYMM_UPPER:
    case MAT_SYMM_LOWER:
      out_props->pattern.is_symm = 1;
      out_props->num_nonzeros += nnz_stored_offdiag;
      break;

    case MAT_HERM_UPPER:
    case MAT_HERM_LOWER:
      out_props->pattern.is_herm = 1;
      out_props->num_nonzeros += nnz_stored_offdiag;
      break;

    case MAT_SYMM_FULL:
      out_props->pattern.is_symm = 1;
      break;

    case MAT_HERM_FULL:
      out_props->pattern.is_herm = 1;
      break;

    case MAT_GENERAL:
    default:
      /* do nothing */
      break;
    }
}

/**
 *  \brief Set CSR properties based on common properties and
 *  asserted input matrix properties.
 *
 *  \param[in,out] A_csr A CSR representation whose properties
 *  have not yet been initialized.
 *  \param[in] out_props Common properties of the final output
 *  matrix which A_csr will represent.
 *  \param[in] props Asserted properties of the final output
 *  matrix which A_csr will represent.
 *
 *  This implementation does not set A_csr->is_shared, which
 *  is instead set in CopyPointers().
 *
 *  \see CopyPointers()
 */
static void
SetMatTypeProps (oski_matCSR_t * A_csr, const oski_matcommon_t * out_props,
		 const oski_inmatpropset_t * props)
{
  A_csr->base_index = props->index_base;
  A_csr->has_unit_diag_implicit = props->has_unit_diag_implicit;
  A_csr->has_sorted_indices = props->has_sorted_indices;

  switch (props->pattern)
    {
    case MAT_TRI_LOWER:
    case MAT_SYMM_LOWER:
    case MAT_HERM_LOWER:
      A_csr->stored.is_upper = 0;
      A_csr->stored.is_lower = 1;
      break;

    case MAT_TRI_UPPER:
    case MAT_SYMM_UPPER:
    case MAT_HERM_UPPER:
      A_csr->stored.is_upper = 1;
      A_csr->stored.is_lower = 0;
      break;

    case MAT_GENERAL:
    case MAT_SYMM_FULL:
    case MAT_HERM_FULL:
    default:
      A_csr->stored.is_upper = 1;
      A_csr->stored.is_lower = 1;
      break;
    }
}

/**
 *  \brief
 *
 *  This routine wraps a #oski_matCSR_t data structure around an
 *  existing, pre-assembled raw CSR 3-array representation.
 *
 *  If any of the asserted properties can be strengthened, then
 *  the parameter props (below) is changed accordingly. See
 *  #oski_CheckCSR() for details.
 *
 *  \param[out] out_props Matrix type-independent properties, derived
 *  from the input properties props.
 *  \param[in] Aptr Row pointers.
 *  \param[in] Aind Column indices.
 *  \param[in] Aval Non-zero values.
 *  \param[in,out] props Semantic properties of the matrix represented by
 *  \param[in] num_rows Number of rows.
 *  \param[in] num_cols Number of columns.
 *  Aptr, Aind, and Aval.
 *  \param[in] mode Specify the copy mode for the returned representation.
 *
 *  \pre All pointer arguments are non-NULL
 *  \pre num_rows, num_cols are non-negative
 *  \pre mode is a legal value.
 *
 *  \see oski_wrapCSR_funcpt
 *
 *  \returns A matrix type-specific representation of the input matrix,
 *  or NULL on error.
 *
 *  \todo The output properties data structure actually defines a
 *  more general property about the diagonal, namely, that it is
 *  all ones. However, the available input matrix properties only
 *  allow the user to specify whether or not there is an implicit
 *  unit diagonal. Thus, it is possible that the user could create
 *  an input matrix with an explicit unit diagonal, but this
 *  condition is not checked when wrapping the data structure. It
 *  might be desirable to do this to make optimized triangular
 *  solve for the unit diagonal case more efficient.
 *
 *  \todo Similarly, the oski_matCSR_t data structure has
 *  "is_upper" and "is_lower" flags, which could be set even
 *  if the user asserts that the matrix has a "general" pattern.
 */
oski_matCSR_t *
oski_WrapCSR (oski_matcommon_t * out_props,
	      oski_index_t * Aptr, oski_index_t * Aind, oski_value_t * Aval,
	      oski_index_t num_rows, oski_index_t num_cols,
	      oski_inmatpropset_t * props, oski_copymode_t mode)
{
  oski_matCSR_t *A_csr;

  oski_PrintDebugMessage (2, "Storing: %d x %d matrix with %d nz",
			  num_rows, num_cols,
			  Aptr[num_rows] - props->index_base);

	/**
	 * Basic steps:
	 *   -# Check asserted properties.
	 *   -# Create a CSR handle.
	 *   -# Copy buffers (deep or shallow copy, depending on mode)
	 *   -# Set common properties.
	 *   -# Set matrix-specific properties.
	 *   -# Return.
	 */

  /* Check asserted properties of the input matrix. */
  if (!oski_CheckCSR (Aptr, Aind, num_rows, num_cols, props))
    return NULL;

  A_csr = oski_Malloc (oski_matCSR_t, 1);
  if (!A_csr)
    return NULL;

  if (!CopyPointers (A_csr, mode, Aptr, Aind, Aval, num_rows,
		     props->index_base))
    return NULL;

  SetCommonProps (out_props, Aptr, Aind, num_rows, num_cols, props);
  SetMatTypeProps (A_csr, out_props, props);

  if (!A_csr->is_shared && !A_csr->has_sorted_indices)
    {
      oski_PrintDebugMessage (3, "Unshared and unsorted; sorting...");
      oski_SortIndices (A_csr, out_props->num_rows);
    }

  return A_csr;
}

/**
 *  \brief
 *
 *  Frees the given oski_matCSR_t object. If the underlying
 *  array representation is not shared, they are also freed.
 *
 *  \param[in,out] mat A oski_matCSR_t object.
 */
void
oski_DestroyMatRepr (void *mat)
{
  oski_matCSR_t *A_csr = (oski_matCSR_t *) mat;

  if (!A_csr)
    return;

  if (!A_csr->is_shared)
    {
      oski_Free (A_csr->ptr);
      oski_Free (A_csr->ind);
      oski_Free (A_csr->val);
    }

  oski_Free (A_csr);
}

void *
oski_CreateMatReprFromCSR (const oski_matCSR_t * mat,
			   const oski_matcommon_t * props, ...)
{
  oski_matCSR_t *A = oski_Malloc (oski_matCSR_t, 1);

  if (!A)
    return NULL;

  /* make sure we copy all the CSR-specific properties */
  oski_CopyMem (A, mat, oski_matCSR_t, 1);

  /* physically duplicate array storage */
  if (!CopyPointers (A, COPY_INPUTMAT, mat->ptr, mat->ind, mat->val,
		     props->num_rows, mat->base_index))
    return NULL;

  return (void *) A;
}

oski_matCSR_t *
oski_ConvertMatReprToCSR (const void *mat, const oski_matcommon_t * props)
{
  return (oski_matCSR_t *) oski_CreateMatReprFromCSR ((const oski_matCSR_t *)
						      mat, props);
}

void *
oski_CopyMatRepr (const void *mat, const oski_matcommon_t * props)
{
  return oski_CreateMatReprFromCSR ((const oski_matCSR_t *) mat, props);
}

double
oski_StreamMatRepr (const void *mat, const oski_matcommon_t * props)
{
  const oski_matCSR_t *A = (const oski_matCSR_t *) mat;
  oski_index_t i;
  double ind_sum = 0;
  double val_sum = 0;

  for (i = 0; i < props->num_rows; i++)
    {
      oski_index_t k;
      for (k = A->ptr[i] - A->base_index;
	   k < (A->ptr[i + 1] - A->base_index); k++)
	{
	  oski_index_t j = A->ind[k];
	  oski_value_t a_ij = A->val[k];

	  /* Some dummy computations */
	  ind_sum += (double) j;
#if IS_VAL_COMPLEX
	  val_sum += (double) (_RE (a_ij)) + (double) (_IM (a_ij));
#else
	  val_sum += (double) a_ij;
#endif
	}
    }

  return (double) ind_sum + (double) val_sum;
}

void
oski_ChangeIndexBase (oski_matCSR_t * A,
		      const oski_matcommon_t * props,
		      const oski_index_t new_base)
{
  oski_index_t i;
  oski_index_t m;
  oski_index_t old_base;
  oski_index_t base_change;

  if (A == NULL || props == NULL || new_base < 0 || new_base > 1)
    return;

  old_base = A->base_index;
  if (new_base == old_base)
    return;

  base_change = new_base - old_base;
  m = props->num_rows;

  for (i = A->ptr[0] - old_base; i < (A->ptr[m] - old_base); i++)
    A->ind[i] += base_change;

  for (i = 0; i <= m; i++)
    A->ptr[i] += base_change;

  A->base_index = new_base;
}

double
oski_CalcMatRepr1Norm (const void *mat, const oski_matcommon_t * props)
{
  const oski_matCSR_t *A = (const oski_matCSR_t *) mat;
  double max_absrowsum = 0;
  oski_index_t i;
  oski_index_t base = A->base_index;

  for (i = 0; i < props->num_rows; i++)
    {
      double absrowsum = 0.0;
      oski_index_t k;

      for (k = A->ptr[i] - base; k < A->ptr[i + 1] - base; k++)
	{
	  oski_value_t a_ij;

	  VAL_ASSIGN (a_ij, A->val[k]);
#if IS_VAL_COMPLEX
	  absrowsum +=
	    sqrt (_RE (a_ij) * _RE (a_ij) + _IM (a_ij) * _IM (a_ij));
#else
	  absrowsum += fabs (a_ij);
#endif
	}

      if (absrowsum > max_absrowsum)
	max_absrowsum = absrowsum;
    }

	/** \todo Fix the symmetric case; this is only an estimate. */
  if ((props->pattern.is_symm || props->pattern.is_herm)
      && (A->stored.is_upper != A->stored.is_lower))
    max_absrowsum *= 2.0;

  return max_absrowsum;
}

oski_index_t
oski_CountZeroRowsCSR (const oski_matCSR_t * A,
		       const oski_matcommon_t * props)
{
  oski_index_t i;
  oski_index_t num_zero_rows;
  if (A == NULL || props == NULL)
    return 0;
  for (i = 0, num_zero_rows = 0; i < props->num_rows; i++)
    {
      num_zero_rows += (A->ptr[i + 1] == A->ptr[i]);
    }
  return num_zero_rows;
}

/* eof */
