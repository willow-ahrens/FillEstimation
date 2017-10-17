/**
 *  \file src/DENSE/module.c
 *  \ingroup MATTYPE_DENSE
 *  \brief Generalized compressed sparse row (DENSE) implementation.
 */

#include <oski/config.h>

#include <stdarg.h>

#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/DENSE/format.h>
#include <oski/DENSE/module.h>
#include <oski/CSR/module.h>

const char *
oski_GetShortDesc (void)
{
  return "Dense column major (DENSE) format";
}

const char *
oski_GetLongDesc (void)
{
  return oski_GetShortDesc ();
}

void
oski_InitModule (void)
{
  oski_PrintDebugMessage (2, "Loading DENSE(ind_t=%s, val_t=%s)",
			  MACRO_TO_STRING (IND_T), MACRO_TO_STRING (VAL_T));
  oski_PrintDebugMessage (3, oski_GetShortDesc ());
}

void
oski_CloseModule (void)
{
  oski_PrintDebugMessage (2, "Shutting down DENSE(%s, %s) module",
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

static int
Convert (oski_index_t m, oski_index_t n,
	 const oski_index_t * ptr, const oski_index_t * ind,
	 const oski_value_t * val,
	 oski_index_t b, int has_unit_diag, oski_value_t ** p_val)
{
  oski_index_t nnz = m * n;
  oski_value_t *vp = oski_Malloc (oski_value_t, nnz);
  oski_index_t i;

  if (val == NULL)
    if (nnz > 0)
      return ERR_OUT_OF_MEMORY;

  oski_ZeroMem (vp, sizeof (oski_value_t) * nnz);

  for (i = 0; i < m; i++)
    {
      oski_index_t k;
      for (k = ptr[i] - b; k < ptr[i + 1] - b; k++)
	{
	  oski_index_t j = ind[k] - b;
	  VAL_INC (vp[i + j * m], val[k]);
	}

      if (has_unit_diag)
	{
	  VAL_SET_ONE (vp[i + i * m]);
	}
    }

  *p_val = vp;
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

  oski_matDENSE_t *A = NULL;

  if (mat == NULL || props == NULL)
    return NULL;

  A = oski_Malloc (oski_matDENSE_t, 1);
  if (A == NULL)
    {
      OSKI_ERR (oski_CreateMatReprFromCSR, ERR_OUT_OF_MEMORY);
      return NULL;
    }

  oski_PrintDebugMessage (1, "Creating DENSE() matrix...");

  did_copy = ExpandSymm (mat, props, (oski_matCSR_t **) (&A_full));
  if (A_full == NULL)
    {
      OSKI_ERR (oski_CreateMatReprFromCSR, ERR_OUT_OF_MEMORY);
      return NULL;
    }

  err = Convert (props->num_rows, props->num_cols,
		 A_full->ptr, A_full->ind, A_full->val,
		 A_full->base_index, A_full->has_unit_diag_implicit,
		 &(A->val));
  if (err)
    {
      OSKI_ERR (oski_CreateMatReprFromCSR, err);
      oski_Free (A);
      A = NULL;
    }
  A->lead_dim = props->num_rows;

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
  oski_matDENSE_t *A = (oski_matDENSE_t *) mat;
  if (A == NULL)
    return;
  if (A->val != NULL)
    oski_Free (A->val);
  oski_Free (A);
}

void *
oski_CopyMatRepr (const void *mat, const oski_matcommon_t * props)
{
  const oski_matDENSE_t *A = (const oski_matDENSE_t *) mat;
  oski_matDENSE_t *A_copy = NULL;
  oski_index_t m;
  oski_index_t n;
  oski_index_t nnz;
  int err;

  if (mat == NULL || props == NULL)
    return NULL;

  A_copy = oski_Malloc (oski_matDENSE_t, 1);
  if (A_copy == NULL)
    return NULL;

  m = props->num_rows;
  n = props->num_cols;
  nnz = m * n;
  A_copy->val = oski_Malloc (oski_value_t, nnz);
  if (A_copy->val == NULL)
    {
      oski_Free (A_copy);
      A_copy = NULL;
    }
  else
    {				/* A_copy->val != NULL */
      oski_CopyMem (A_copy->val, A->val, oski_value_t, nnz);
    }
  return A_copy;
}

oski_matCSR_t *
oski_ConvertMatReprToCSR (const void *mat, const oski_matcommon_t * props)
{
  const oski_matDENSE_t *A_dense = (const oski_matDENSE_t *) mat;
  const oski_value_t *vp;
  oski_index_t m, n, nnz;
  oski_matCSR_t *A_csr;
  oski_index_t *ptr;
  oski_index_t *ind;
  oski_value_t *val;
  int err;
  oski_index_t i;

  if (mat == NULL || props == NULL)
    return NULL;

  m = props->num_rows;
  n = props->num_cols;
  nnz = m * n;
  err = oski_MultiMalloc (__FILE__, __LINE__, 4,
			  (size_t) (sizeof (oski_matCSR_t)), &A_csr,
			  (size_t) (sizeof (oski_index_t) * (m + 1)), &ptr,
			  (size_t) (sizeof (oski_index_t) * nnz), &ind,
			  (size_t) (sizeof (oski_value_t) * nnz), &val);
  if (err)
    return NULL;

  vp = A_dense->val;
  ptr[0] = 0;
  for (i = 0; i < m; i++)
    {
      oski_index_t j;
      for (j = 0; j < n; j++)
	{
	  ind[i * n + j] = j;
	  VAL_ASSIGN (val[i * n + j], vp[i + j * m]);
	}
      ptr[i + 1] = ptr[i] + n;
    }

  A_csr->ptr = ptr;
  A_csr->ind = ind;
  A_csr->val = val;
  A_csr->base_index = 0;
  A_csr->has_unit_diag_implicit = 0;
  A_csr->has_sorted_indices = 1;
  A_csr->stored.is_upper = 1;
  A_csr->stored.is_lower = 1;
  A_csr->is_shared = 0;

  return A_csr;
}

/* -----------------------------------------------------------------
 *  Lua transformations.
 */

#include <oski/xforms_internal.h>

int
oski_CreateLuaMatReprFromCSR (lua_State * L)
{
  return oski_CreateLuaMatReprGenericFromCSR (L, "DENSE");
}

/* eof */
