/**
 *  \file src/GCSR/module.c
 *  \ingroup MATTYPE_GCSR
 *  \brief Generalized compressed sparse row (GCSR) implementation.
 */

#include <oski/config.h>

#include <stdarg.h>

#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/GCSR/format.h>
#include <oski/GCSR/module.h>
#include <oski/CSR/module.h>

const char *
oski_GetShortDesc (void)
{
  return "Generalized compressed sparse row (GCSR) implementation";
}

const char *
oski_GetLongDesc (void)
{
  return oski_GetShortDesc ();
}

void
oski_InitModule (void)
{
  oski_PrintDebugMessage (2, "Loading GCSR(ind_t=%s, val_t=%s)",
			  MACRO_TO_STRING (IND_T), MACRO_TO_STRING (VAL_T));
  oski_PrintDebugMessage (3, oski_GetShortDesc ());
}

void
oski_CloseModule (void)
{
  oski_PrintDebugMessage (2, "Shutting down GCSR(%s, %s) module",
			  MACRO_TO_STRING (IND_T), MACRO_TO_STRING (VAL_T));
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

/** \brief Returns 0 on success, or an error code on failure. */
static int
ConvertToCSR (oski_index_t m_orig,
	      const oski_index_t * ptr_orig, const oski_index_t * ind_orig,
	      const oski_value_t * val_orig,
	      oski_index_t base, int has_unit_diag,
	      oski_index_t * p_m, oski_index_t ** p_ptr,
	      oski_index_t ** p_rind, oski_index_t ** p_cind,
	      oski_value_t ** p_val)
{
  oski_index_t m;		/* # of non-zero rows in GCSR matrix */
  oski_index_t nnz;		/* # of stored non-zeros in GCSR matrix */
  oski_index_t nnz_orig;	/* # of stored non-zeros in original CSR matrix */

  /* GCSR data structure */
  oski_index_t *ptr;
  oski_index_t *rind;
  oski_index_t *cind;
  oski_value_t *val;

  int err;

  oski_index_t i_orig;
  oski_index_t i;

  assert (ptr_orig != NULL);
  assert (ptr_orig[m_orig] == 0 || (ind_orig != NULL && val_orig != NULL));

  nnz_orig = ptr_orig[m_orig] - base;
  if (has_unit_diag)
    {
      m = m_orig;
      nnz = nnz_orig + m;
    }
  else
    {
      m = m_orig - CountZeroRows (m_orig, ptr_orig);
      nnz = nnz_orig;
    }

  err = oski_MultiMalloc (__FILE__, __LINE__, 4,
			  (size_t) (sizeof (oski_index_t) * (m + 1)), &ptr,
			  (size_t) (sizeof (oski_index_t) * m), &rind,
			  (size_t) (sizeof (oski_index_t) * nnz), &cind,
			  (size_t) (sizeof (oski_value_t) * nnz), &val);
  if (err)
    return ERR_OUT_OF_MEMORY;

  /* Copy row structure */
  for (i_orig = 0, i = 0, ptr[0] = 0; i_orig < m_orig; i_orig++)
    {
      oski_index_t nnz_i =
	ptr_orig[i_orig + 1] - ptr_orig[i_orig] + (has_unit_diag != 0);
      if (nnz_i > 0)
	{
	  assert (i >= 0 && i < m);
	  ptr[i + 1] = ptr[i] + nnz_i;
	  rind[i] = i_orig;
	  i++;
	}
    }
  assert (ptr[m] == nnz);

  /* Copy column structure and non-zero values */
  for (i_orig = 0, i = 0; i_orig < m_orig; i_orig++)
    {
      oski_index_t nnz_i_orig = ptr_orig[i_orig + 1] - ptr_orig[i_orig];
      oski_index_t k = ptr[i];

      if (has_unit_diag)
	{
	  cind[k] = i_orig;
	  VAL_SET_ONE (val[k]);
	  k++;
	}
      if (nnz_i_orig > 0)
	{
	  oski_index_t k_orig = ptr_orig[i_orig] - base;
	  oski_CopyMem (val + k, val_orig + k_orig, oski_value_t, nnz_i_orig);
	  for (; k_orig < (ptr_orig[i_orig + 1] - base); k_orig++, k++)
	    cind[k] = ind_orig[k_orig] - base;
	}

      if (has_unit_diag || nnz_i_orig)
	{
	  i++;
	  assert (k == ptr[i]);
	}
    }

  if (p_m != NULL)
    *p_m = m;
  if (p_ptr != NULL)
    *p_ptr = ptr;
  else
    oski_Free (ptr);
  if (p_rind != NULL)
    *p_rind = rind;
  else
    oski_Free (rind);
  if (p_cind != NULL)
    *p_cind = cind;
  else
    oski_Free (cind);
  if (p_val != NULL)
    *p_val = val;
  else
    oski_Free (val);
  return 0;
}

void *
oski_CreateMatReprFromCSR (const oski_matCSR_t * mat,
			   const oski_matcommon_t * props, ...)
{
  /* Store a full copy of the matrix if it uses half-storage */
  const oski_matCSR_t *A_full = NULL;
  int did_copy = 0;
  int err;

  oski_matGCSR_t *A = NULL;

  if (mat == NULL || props == NULL)
    return NULL;

  A = oski_Malloc (oski_matGCSR_t, 1);
  if (A == NULL)
    {
      OSKI_ERR (oski_CreateMatReprFromCSR, ERR_OUT_OF_MEMORY);
      return NULL;
    }

  oski_PrintDebugMessage (1, "Creating GCSR() matrix...");

  did_copy = ExpandSymm (mat, props, (oski_matCSR_t **) (&A_full));
  if (A_full == NULL)
    {
      OSKI_ERR (oski_CreateMatReprFromCSR, ERR_OUT_OF_MEMORY);
      return NULL;
    }

  err = ConvertToCSR (props->num_rows,
		      A_full->ptr, A_full->ind, A_full->val,
		      A_full->base_index, A_full->has_unit_diag_implicit,
		      &(A->num_stored_rows),
		      &(A->ptr), &(A->rind), &(A->cind), &(A->val));
  if (err)
    {
      OSKI_ERR (oski_CreateMatReprFromCSR, err);
      oski_Free (A);
      A = NULL;
    }

  if (did_copy)
    {
      oski_DestroyMatRepr_funcpt func_Destroy =
	OSKI_MATTYPE_METHOD ("CSR", DestroyMatRepr);
      oski_PrintDebugMessage (2, "Destroying full-storage coppy");
      if (func_Destroy != NULL)
	(*func_Destroy) ((void *) A_full);
      /* \todo Should return an error if CSR's destroy not found */
    }

  oski_PrintDebugMessage (2, "Done.");
  return A;
}

void
oski_DestroyMatRepr (void *mat)
{
  oski_matGCSR_t *A_gcsr = (oski_matGCSR_t *) mat;
  if (A_gcsr == NULL)
    return;
  if (A_gcsr->num_stored_rows > 0)
    {
      if (A_gcsr->ptr != NULL)
	oski_Free (A_gcsr->ptr);
      if (A_gcsr->rind != NULL)
	oski_Free (A_gcsr->rind);
      if (A_gcsr->cind != NULL)
	oski_Free (A_gcsr->cind);
      if (A_gcsr->val != NULL)
	oski_Free (A_gcsr->val);
    }
  oski_Free (A_gcsr);
}

void *
oski_CopyMatRepr (const void *mat, const oski_matcommon_t * props)
{
  const oski_matGCSR_t *A_gcsr = (const oski_matGCSR_t *) mat;
  oski_matGCSR_t *A_copy = NULL;
  oski_index_t m;
  oski_index_t nnz;
  int err;

  if (mat == NULL)
    return NULL;

  A_copy = oski_Malloc (oski_matGCSR_t, 1);
  if (A_copy == NULL)
    return NULL;

  m = A_gcsr->num_stored_rows;
  nnz = A_gcsr->ptr[m];
  err = oski_MultiMalloc (__FILE__, __LINE__, 4,
			  (size_t) (sizeof (oski_index_t) * (m + 1)),
			  &(A_copy->ptr),
			  (size_t) (sizeof (oski_index_t) * m),
			  &(A_copy->rind),
			  (size_t) (sizeof (oski_index_t) * nnz),
			  &(A_copy->cind),
			  (size_t) (sizeof (oski_value_t) * nnz),
			  &(A_copy->val));
  if (!err)
    {
      A_copy->num_stored_rows = m;
      oski_CopyMem (A_copy->ptr, A_gcsr->ptr, oski_index_t, m + 1);
      oski_CopyMem (A_copy->rind, A_gcsr->rind, oski_index_t, m);
      oski_CopyMem (A_copy->cind, A_gcsr->cind, oski_index_t, nnz);
      oski_CopyMem (A_copy->val, A_gcsr->val, oski_value_t, nnz);
    }
  else
    {				/* Out of memory */
      oski_Free (A_copy);
      A_copy = NULL;
    }

  return A_copy;
}

oski_matCSR_t *
oski_ConvertMatReprToCSR (const void *mat, const oski_matcommon_t * props)
{
  OSKI_ERR (oski_ConvertMatReprToCSR, ERR_NOT_IMPLEMENTED);
  return NULL;
}

oski_index_t
oski_FindRowGCSR (const oski_matGCSR_t * A, oski_index_t i0)
{
  if (A != NULL)
    {
      oski_index_t k;
      for (k = 0; k < A->num_stored_rows; k++)
	{
	  if (A->rind[k] == i0)
	    {
	      return k;
	    }
	}
    }
  return -1;
}

/* -----------------------------------------------------------------
 *  Lua transformations.
 */

#include <oski/xforms_internal.h>

int
oski_CreateLuaMatReprFromCSR (lua_State * L)
{
  return oski_CreateLuaMatReprGenericFromCSR (L, "GCSR");
}

/* eof */
