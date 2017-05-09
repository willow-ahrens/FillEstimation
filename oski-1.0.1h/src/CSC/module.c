/**
 *  \file src/CSC/module.c
 *  \ingroup MATTYPE_CSC
 *  \brief Compressed sparse column (CSC) implementation.
 */

#include <oski/config.h>
#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/CSC/format.h>
#include <oski/CSC/module.h>
#include <oski/CSR/module.h>

const char *
oski_GetShortDesc (void)
{
  return "Compressed sparse column (CSC) implementation";
}

const char *
oski_GetLongDesc (void)
{
  return "This module implements compressed sparse column (CSC) format.";
}

void
oski_InitModule (void)
{
  oski_PrintDebugMessage (2, "Loading CSC(ind_t=%s, val_t=%s)",
			  MACRO_TO_STRING (IND_T), MACRO_TO_STRING (VAL_T));
  oski_PrintDebugMessage (3, oski_GetShortDesc ());
}

void
oski_CloseModule (void)
{
  oski_PrintDebugMessage (2, "Shutting down CSC(%s, %s) module",
			  MACRO_TO_STRING (IND_T), MACRO_TO_STRING (VAL_T));
}

void
oski_DestroyMatRepr (void *mat)
{
  oski_DestroyMatRepr_funcpt func_DestroyCSR;
  oski_matCSC_t *A_csc;

  if (mat == NULL)
    return;
  A_csc = (oski_matCSC_t *) mat;
  if (A_csc == NULL) return;

  if (A_csc->mat_trans != NULL)
    {
      func_DestroyCSR = OSKI_MATTYPE_METHOD ("CSR", DestroyMatRepr);
      if (func_DestroyCSR != NULL)
	(*func_DestroyCSR) (A_csc->mat_trans);
      else
	{
	  OSKI_ERR_MATTYPE_MEMBER (oski_DestroyMatRepr, "CSR", DestroyMatRepr);
	}
    }

  oski_Free (A_csc);
}

void *
oski_CopyMatRepr (const void *mat, const oski_matcommon_t * props)
{
  oski_matCSC_t *A_csc;
  oski_matCSC_t *A_copy;

  if (mat == NULL)
    return NULL;
  A_csc = (oski_matCSC_t *) mat;

  A_copy = oski_Malloc (oski_matCSC_t, 1);
  if (A_copy == NULL)
    return NULL;

  if (A_csc->mat_trans == NULL)
    A_copy->mat_trans = NULL;
  else
    {
      /* Copy internally stored CSR representation of \f$A^T\f$. */
      oski_CopyMatRepr_funcpt func_CopyCSR =
	OSKI_MATTYPE_METHOD ("CSR", CopyMatRepr);
      oski_matcommon_t props_csr;
      if (func_CopyCSR == NULL)
	{
	  OSKI_ERR_MATTYPE_MEMBER (oski_CopyMatRepr, "CSR", CopyMatRepr);
	  oski_Free (A_copy);
	  return NULL;
	}

      oski_CopyMem (&props_csr, props, oski_matcommon_t, 1);
      oski_TransposeProps (&props_csr);
      A_copy->mat_trans = (*func_CopyCSR) (A_csc->mat_trans, &props_csr);
      if (A_copy->mat_trans == NULL)
	{
	  oski_Free (A_copy);
	  return NULL;
	}
    }

  return A_copy;
}

/**
 *  \brief
 *
 *  This routine wraps a #oski_matCSC_t data structure around an
 *  existing, pre-assembled raw CSC 3-array representation.
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
 *  \see oski_WrapCSC_funcpt
 *
 *  \returns A matrix type-specific representation of the input matrix,
 *  or NULL on error.
 */
oski_matCSC_t *
oski_WrapCSC (oski_matcommon_t * out_props,
	      oski_index_t * Aptr, oski_index_t * Aind, oski_value_t * Aval,
	      oski_index_t num_rows, oski_index_t num_cols,
	      oski_inmatpropset_t * props, oski_copymode_t mode)
{
  oski_WrapCSR_funcpt func_WrapCSR;
  oski_matCSC_t *A_csc;

  func_WrapCSR = OSKI_MATTYPE_METHOD ("CSR", WrapCSR);
  if (!func_WrapCSR)
    {
      OSKI_ERR_NOT_IMPLEMENTED (oski_WrapCSR, "Can't find CSR wrapper");
      return NULL;
    }

  A_csc = oski_Malloc (oski_matCSC_t, 1);
  if (A_csc == NULL)
    return NULL;

  /* Fudge input properties so that they apply to the transposed
   * (CSR) matrix, and not the CSC matrix. */
  props->pattern = oski_TransposePatternProp (props->pattern);

  A_csc->mat_trans = (*func_WrapCSR) (out_props, Aptr, Aind, Aval,
				      num_cols, num_rows, props, mode);

  /* Fudge output properties so that they apply to the CSC matrix,
   * not the CSR matrix. */
  oski_TransposeProps (out_props);

  if (A_csc->mat_trans == NULL)
    {
      oski_DestroyMatRepr (A_csc);
      return NULL;
    }

  return A_csc;
}

void *
oski_CreateMatReprFromCSR (const oski_matCSR_t * mat,
			   const oski_matcommon_t * props, ...)
{
  oski_TransposeCSR_funcpt func_Trans;
  oski_matCSC_t *A;

  if (mat == NULL || props == NULL)
    return NULL;

  A = oski_Malloc (oski_matCSC_t, 1);
  if (A == NULL)
    {
      OSKI_ERR (oski_CreateMatReprFromCSR, ERR_OUT_OF_MEMORY);
      return NULL;
    }

  func_Trans = OSKI_MATTYPE_METHOD ("CSR", TransposeCSR);
  if (func_Trans == NULL)
    {
      OSKI_ERR_MATTYPE_MEMBER (oski_CreateMatReprFromCSR,
			       "CSR", TransposeCSR);
      return NULL;
    }

  A->mat_trans = (*func_Trans) (mat, props);
  if (A->mat_trans == NULL)
    {
      oski_Free (A);
      A = NULL;
    }

  return (void *) A;
}

oski_matCSR_t *
oski_ConvertMatReprToCSR (const void *mat, const oski_matcommon_t * props)
{
  oski_TransposeCSR_funcpt func_Trans;
  const oski_matCSC_t *A;
  oski_matcommon_t props_CSR;

  if (mat == NULL || props == NULL)
    return NULL;

  A = (const oski_matCSC_t *) mat;

  /* Transpose properties so they apply to the output CSR matrix,
   * not the input CSC matrix. */
  oski_CopyMem (&props_CSR, props, oski_matcommon_t, 1);
  oski_TransposeProps (&props_CSR);

  func_Trans = OSKI_MATTYPE_METHOD ("CSR", TransposeCSR);
  if (func_Trans == NULL)
    {
      OSKI_ERR_MATTYPE_MEMBER (oski_CreateMatReprFromCSR,
			       "CSR", TransposeCSR);
      return NULL;
    }

  return (*func_Trans) (A->mat_trans, &props_CSR);
}

/* -----------------------------------------------------------------
 *  Lua transformations.
 */

#include <oski/xforms_internal.h>

int
oski_CreateLuaMatReprFromCSR (lua_State * L)
{
  return oski_CreateLuaMatReprGenericFromCSR (L, "CSC");
}

/* eof */
