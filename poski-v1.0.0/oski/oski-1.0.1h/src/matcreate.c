/**
 *  \file src/matcreate.c
 *  \brief Matrix creation routines.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/matcreate.h>

/**
 *  \brief
 *
 *  \param[in,out] A_tunable A valid matrix handle.
 *  \returns 0 if the object was successfully freed, or an
 *  error code otherwise.
 */
int
oski_DestroyMat (oski_matrix_t A_tunable)
{
  oski_PrintDebugMessage (1, "Freeing matrix handle %p", A_tunable);
  oski_DestroyMatStruct (A_tunable);
  return 0;
}

/**
 *  \brief Duplicate a #oski_matspecific_t data structure.
 *
 *  This implementation loads the matrix type-specific copy
 *  routine and executes it.
 *
 *  \returns 1 on success, or 0 on error.
 */
static int
CopyMatSpecific (const oski_matcommon_t * props,
		 const oski_matspecific_t * src, oski_matspecific_t * dest)
{
  int success = 0;

  if (src == NULL || dest == NULL || props == NULL)
    return 0;

  dest->type_id = src->type_id;

  if (src->type_id != INVALID_ID && src->repr != NULL)
    {
      /* Non-trivial copy; need type-specific copy method */
      oski_CopyMatRepr_funcpt func_Copy =
	OSKI_MATTYPEID_METHOD (src->type_id, CopyMatRepr);

      if (func_Copy == NULL)
	{
	  OSKI_ERR_MATTYPEID_MEMBER (CopyMatSpecific, src->type_id,
				     CopyMatRepr);
	  success = 0;
	}
      else
	{
	  dest->repr = (*func_Copy) (src->repr, props);
	  success = (dest->repr != NULL);
	}
    }
  else
    {
      dest->repr = NULL;
      success = 1;
    }

  return success;
}

/**
 *  \brief
 *
 *  This routine is logically equivalent to the matrix object the
 *  caller would obtain if she performed the following sequence
 *  of steps:
 *  -  Re-execute the original call to \ref oski_CreateMatCSR()
 *  or \ref oski_CreateMatCSC() to create a handle A_copy in
 *  \ref COPY_INPUTMAT mode. Thus, A_copy exists independently
 *  of A_tunable and any data upon which A_tunable might depend.
 *  -  Apply the same structural tuning hints to A_copy that
 *  were originally applied to A_tunable.
 *  - Execute \ref oski_GetMatTransforms() on A_tunable and
 *  apply these to A_copy.
 *
 *  According to these steps, A_copy does not have any of
 *  A_tunable's workload hints or implicit workload gathered
 *  from implicit profiling.
 *
 *  \param[in] A_tunable Handle to a valid matrix \f$A\f$.
 *  \returns A new, separate physical copy of \f$A\f$, or
 *  \ref INVALID_MAT on error.
 */
oski_matrix_t
oski_CopyMat (const oski_matrix_t A_tunable)
{
  oski_matrix_t A_copy;

  oski_PrintDebugMessage (1, "Duplicating matrix handle %p", A_tunable);

  if (A_tunable == INVALID_MAT)
    return INVALID_MAT;

  /* Quick return */
  A_copy = oski_CreateMatStruct ();
  if (A_copy == INVALID_MAT)
    {
      OSKI_ERR (oski_CopyMat, ERR_OUT_OF_MEMORY);
      return INVALID_MAT;
    }

  if (!CopyMatSpecific (&(A_tunable->props), &(A_tunable->input_mat),
			&(A_copy->input_mat))
      || !CopyMatSpecific (&(A_tunable->props), &(A_tunable->tuned_mat),
			   &(A_copy->tuned_mat)))
    {
      oski_DestroyMatStruct (A_copy);
      return INVALID_MAT;
    }

  oski_CopyMem (&(A_copy->props), &(A_tunable->props), oski_matcommon_t, 1);
  A_copy->time_stream = A_tunable->time_stream;
  oski_CopyMem (&(A_copy->structhints), &(A_tunable->structhints),
		oski_structhint_t, 1);

  A_copy->tuned_xforms = oski_DuplicateString (A_tunable->tuned_xforms);

  return A_copy;
}

/* ----------------------------------------------------------------
 *  CSR
 */
#include <oski/CSR/module.h>

/** Number of times to stream when estimating the matrix streaming time. */
#define NUM_STREAMS 2

/**
 *  \brief Measure the time to stream a CSR data structure, for use in
 *  future cost estimates.
 */
static double
MeasureStreamTimeCSR (const void *repr, const oski_matcommon_t * props,
		      oski_timer_t timer)
{
  oski_StreamMatRepr_funcpt func_StreamMatRepr;
  double time_stream;
  int i;

  if (props == NULL)
    return TIME_NOT_AVAIL;

  func_StreamMatRepr = OSKI_MATTYPE_METHOD ("CSR", StreamMatRepr);
  if (!func_StreamMatRepr)
    {
      OSKI_ERR_NOT_IMPLEMENTED (oski_StreamMatRepr,
				"Can't find matrix streaming");
      return TIME_NOT_AVAIL;
    }

	/** Stream NUM_STREAMS times and choose the fastest. */
  oski_RestartTimer (timer);
  (*func_StreamMatRepr) (repr, props);
  oski_StopTimer (timer);
  time_stream = oski_ReadElapsedTime (timer);

  for (i = 1; i < NUM_STREAMS; i++)
    {
      double t;

      oski_RestartTimer (timer);
      (*func_StreamMatRepr) (repr, props);
      oski_StopTimer (timer);

      t = oski_ReadElapsedTime (timer);
      if (t < time_stream)
	time_stream = t;
    }

  oski_PrintDebugMessage (2, "Time to stream input matrix: %g seconds",
			  time_stream);

  return time_stream;
}

oski_matrix_t
oski_CreateMatCSR_va (oski_index_t* Aptr, oski_index_t* Aind,
                      oski_value_t* Aval,
                      oski_index_t num_rows, oski_index_t num_cols,
                      oski_copymode_t mode, int k, va_list ap)
{
  oski_inmatprop_t* in_props;
  oski_matrix_t A_tunable;
  int cur_arg;

  /* Temporary storage to collect variable argument list. */
  in_props = oski_Malloc (oski_inmatprop_t, k);
  if (!in_props)
    return INVALID_MAT;

  for (cur_arg = 0; cur_arg < k; ++cur_arg)
    in_props[cur_arg] = va_arg (ap, oski_inmatprop_t);

  A_tunable = oski_CreateMatCSR_arr (Aptr, Aind, Aval, num_rows, num_cols,
				     mode, k, in_props);

  /* Clean-up and return. */
  oski_Free (in_props);
  return A_tunable;
}

oski_matrix_t
oski_CreateMatCSR_arr (oski_index_t* Aptr, oski_index_t* Aind,
		       oski_value_t* Aval,
		       oski_index_t num_rows, oski_index_t num_cols,
		       oski_copymode_t mode,
		       int k, const oski_inmatprop_t* in_props)
{
  /* The new matrix */
  oski_matstruct_t *A_tunable = INVALID_MAT;

  /* Stores parsed matrix properties */
  oski_inmatpropset_t Props;
  int err;

  /* CSR wrapping */
  oski_WrapCSR_funcpt func_WrapCSR;

  oski_PrintDebugMessage (1,
			  "Creating matrix handle for a %d x %d CSR matrix",
			  num_rows, num_cols);

  /**
   *  Check input arguments.
   */
  if (!OSKI_CHECK_MAT_DIMS (num_rows, num_cols))
    {
      OSKI_ERR_BAD_DIMS (oski_CreateMatCSR, num_rows, num_cols);
      return INVALID_MAT;
    }

  if (!OSKI_CHECK_COPYMODE (mode))
    {
      OSKI_ERR_BAD_COPYMODE (oski_CreateMatCSR, mode);
      return INVALID_MAT;
    }

  if (k < 0)
    {
      OSKI_ERR_SMALL_ARG (oski_CreateMatCSR, 7, "number of properties", 0);
      return INVALID_MAT;
    }

  /**
   *  Collect asserted input matrix properties.
   */
  err = oski_CollectInMatProps_arr (&Props, k, in_props);
  if (err)
    return INVALID_MAT;

  if (Aptr == NULL)
    {
      OSKI_ERR_NULL_ARG (oski_CreateMatCSR, 1, "row pointers");
      return INVALID_MAT;
    }

  if ((Aind == NULL) && (Aptr[num_rows] > Props.index_base))
    {
      OSKI_ERR_NULL_ARG (oski_CreateMatCSR, 2, "column indices");
      return INVALID_MAT;
    }

  if ((Aval == NULL) && (Aptr[num_rows] > Props.index_base))
    {
      OSKI_ERR_NULL_ARG (oski_CreateMatCSR, 3, "non-zero values");
      return INVALID_MAT;
    }

  /**
   *  Load CSR wrapping routine.
   */
  func_WrapCSR = OSKI_MATTYPE_METHOD ("CSR", WrapCSR);
  if (!func_WrapCSR)
    {
      OSKI_ERR_NOT_IMPLEMENTED (oski_WrapCSR, "Can't find CSR wrapper");
      return INVALID_MAT;
    }

  /**
   *  Create handle by wrapping input matrix in a matCSR_t
   *  data structure.
   */
  A_tunable = oski_CreateMatStruct ();
  if (A_tunable == INVALID_MAT)
    return INVALID_MAT;

  A_tunable->input_mat.repr =
    (void *) (*func_WrapCSR) (&A_tunable->props, Aptr, Aind, Aval, num_rows,
			      num_cols, &Props, mode);

  if (A_tunable->input_mat.repr == NULL)
    {
      oski_DestroyMatStruct (A_tunable);
      return INVALID_MAT;
    }

  A_tunable->input_mat.type_id = OSKI_MATTYPE_ID ("CSR");
  A_tunable->is_shared = (mode == SHARE_INPUTMAT);

  /**
   *  Cache streaming time for future cost estimates.
   */
  A_tunable->time_stream =
    MeasureStreamTimeCSR (A_tunable->input_mat.repr, &(A_tunable->props),
			  A_tunable->timer);

  return A_tunable;
}

/**
 *  \brief
 *
 *  \param[in] num_rows \f$m \geq 0\f$
 *  \param[in] num_cols \f$n \geq 0\f$
 *  \param[in] Aptr CSR row pointers.
 *  \param[in] Aind CSR column indices.
 *  \param[in] Aval CSR non-zero values.
 *  \param[in] k Number of explicitly specified semantic properties
 *  of (Aptr, Aind, Aval).
 *  \returns A valid, tunable matrix object, or \ref INVALID_MAT on
 *  error. Any kernel operations or tuning operations may be called
 *  using this object.
 */
oski_matrix_t
oski_CreateMatCSR (oski_index_t * Aptr, oski_index_t * Aind,
		   oski_value_t * Aval,
                   oski_index_t num_rows, oski_index_t num_cols,
                   oski_copymode_t mode, int k, ...)
{
  oski_matrix_t A_tunable;
  va_list ap;
  va_start (ap, k);
  A_tunable = oski_CreateMatCSR_va (Aptr, Aind, Aval,
                                    num_rows, num_cols,
                                    mode, k, ap);
  va_end (ap);
  return A_tunable;
}

/* ----------------------------------------------------------------
 *  CSC
 */
#include <oski/CSC/format.h>
#include <oski/CSC/module.h>

/**
 *  \brief Measure the time to stream a CSC data structure, for use
 *  in future cost estimates.
 */
static double
MeasureStreamTimeCSC (const void *repr, const oski_matcommon_t * props,
		      oski_timer_t timer)
{
  const oski_matCSC_t *A_csc = (const oski_matCSC_t *) repr;
  oski_matcommon_t props_csr;
  oski_CopyMem (&props_csr, props, oski_matcommon_t, 1);
  oski_TransposeProps (&props_csr);
  return MeasureStreamTimeCSR (A_csc->mat_trans, &props_csr, timer);
}

oski_matrix_t
oski_CreateMatCSC_arr (oski_index_t * Aptr, oski_index_t * Aind,
		       oski_value_t * Aval, oski_index_t num_rows,
		       oski_index_t num_cols, oski_copymode_t mode,
		       int k, const oski_inmatprop_t* in_props)
{
  /* The new matrix */
  oski_matstruct_t *A_tunable = INVALID_MAT;

  /* Stores parsed matrix properties */
  oski_inmatpropset_t Props;
  int err;

  /* CSR wrapping */
  oski_WrapCSC_funcpt func_WrapCSC;

  oski_PrintDebugMessage (1,
			  "Creating matrix handle for a %d x %d CSC matrix",
			  num_rows, num_cols);

  /**
   *  Check input arguments.
   */
  if (!OSKI_CHECK_MAT_DIMS (num_rows, num_cols))
    {
      OSKI_ERR_BAD_DIMS (oski_CreateMatCSC, num_rows, num_cols);
      return INVALID_MAT;
    }

  if (!OSKI_CHECK_COPYMODE (mode))
    {
      OSKI_ERR_BAD_COPYMODE (oski_CreateMatCSC, mode);
      return INVALID_MAT;
    }

  if (k < 0)
    {
      OSKI_ERR_SMALL_ARG (oski_CreateMatCSC, 7, "number of properties", 0);
      return INVALID_MAT;
    }

  /**
   *  Collect asserted input matrix properties.
   */
  err = oski_CollectInMatProps_arr (&Props, k, in_props);
  if (err)
    return INVALID_MAT;

  if (Aptr == NULL)
    {
      OSKI_ERR_NULL_ARG (oski_CreateMatCSC, 1, "row pointers");
      return INVALID_MAT;
    }

  if ((Aind == NULL) && (Aptr[num_rows] > Props.index_base))
    {
      OSKI_ERR_NULL_ARG (oski_CreateMatCSC, 2, "column indices");
      return INVALID_MAT;
    }

  if ((Aval == NULL) && (Aptr[num_rows] > Props.index_base))
    {
      OSKI_ERR_NULL_ARG (oski_CreateMatCSC, 3, "non-zero values");
      return INVALID_MAT;
    }

  /**
   *  Load CSC wrapping routine.
   */
  func_WrapCSC = OSKI_MATTYPE_METHOD ("CSC", WrapCSC);
  if (!func_WrapCSC)
    {
      OSKI_ERR_NOT_IMPLEMENTED (oski_WrapCSC, "Can't find CSC wrapper");
      return INVALID_MAT;
    }

  /**
   *  Create handle by wrapping input matrix in a matCSC_t
   *  data structure.
   */
  A_tunable = oski_CreateMatStruct ();
  if (A_tunable == INVALID_MAT)
    return INVALID_MAT;

  A_tunable->input_mat.repr =
    (void *) (*func_WrapCSC) (&A_tunable->props, Aptr, Aind, Aval, num_rows,
			      num_cols, &Props, mode);

  if (A_tunable->input_mat.repr == NULL)
    {
      oski_DestroyMatStruct (A_tunable);
      return INVALID_MAT;
    }

  A_tunable->input_mat.type_id = OSKI_MATTYPE_ID ("CSC");
  A_tunable->is_shared = (mode == SHARE_INPUTMAT);

  /**
   *  Cache streaming time for future cost estimates.
   */
  A_tunable->time_stream =
    MeasureStreamTimeCSC (A_tunable->input_mat.repr, &(A_tunable->props),
			  A_tunable->timer);

  return A_tunable;
}

oski_matrix_t
oski_CreateMatCSC_va (oski_index_t * Aptr, oski_index_t * Aind,
                      oski_value_t * Aval, oski_index_t num_rows,
                      oski_index_t num_cols, oski_copymode_t mode,
                      int k, va_list ap)
{
  oski_inmatprop_t* in_props;
  oski_matrix_t A_tunable;
  int cur_arg;

  /* Temporary storage to collect variable argument list. */
  in_props = oski_Malloc (oski_inmatprop_t, k);
  if (!in_props)
    return INVALID_MAT;

  for (cur_arg = 0; cur_arg < k; ++cur_arg)
    in_props[cur_arg] = va_arg (ap, oski_inmatprop_t);

  A_tunable = oski_CreateMatCSC_arr (Aptr, Aind, Aval, num_rows, num_cols,
				     mode, k, in_props);

  /* Clean-up and return. */
  oski_Free (in_props);
  return A_tunable;
}

/**
 *  \brief
 *
 *  \param[in] num_rows \f$m \geq 0\f$
 *  \param[in] num_cols \f$n \geq 0\f$
 *  \param[in] Aptr CSC row pointers.
 *  \param[in] Aind CSC column indices.
 *  \param[in] Aval CSC non-zero values.
 *  \param[in] k Number of explicitly specified semantic properties
 *  of (Aptr, Aind, Aval).
 *  \returns A valid, tunable matrix object, or \ref INVALID_MAT on
 *  error. Any kernel operations or tuning operations may be called
 *  using this object.
 */
oski_matrix_t
oski_CreateMatCSC (oski_index_t * Aptr, oski_index_t * Aind,
		   oski_value_t * Aval, oski_index_t num_rows,
		   oski_index_t num_cols, oski_copymode_t mode, int k, ...)
{
  oski_matrix_t A_tunable;
  va_list ap;

  va_start (ap, k);
  A_tunable = oski_CreateMatCSC_va (Aptr, Aind, Aval,
                                    num_rows, num_cols,
                                    mode, k, ap);
  va_end (ap);
  return A_tunable;
}

/* ----------------------------------------------------------------
 *  Miscellaneous
 */

/**
 *  \brief Compute the 1-norm of a matrix representation by first
 *  trying to convert to CSR format, since CSR always has a
 *  1-norm computation routine.
 */
static double
CalcMatRepr1NormViaCSR (const oski_matspecific_t * A,
			const oski_matcommon_t * props)
{
  /* Intermediate CSR representation */
  oski_ConvertMatReprToCSR_funcpt func_ToCSR;
  oski_matCSR_t *A_csr;

  assert (A != NULL);
  assert (props != NULL);

  func_ToCSR = OSKI_MATTYPEID_METHOD (A->type_id, ConvertMatReprToCSR);
  if (func_ToCSR != NULL)
    {
      A_csr = (*func_ToCSR) (A->repr, props);
      if (A_csr != NULL)
	{
	  oski_DestroyMatRepr_funcpt func_DestroyCSR;
	  oski_CalcMatRepr1Norm_funcpt func_1Norm;
	  double norm = -1;

	  func_1Norm = OSKI_MATTYPE_METHOD ("CSR", CalcMatRepr1Norm);
	  if (func_1Norm != NULL)
	    norm = (*func_1Norm) (A_csr, props);

	  /* clean-up intermediate representation */
	  func_DestroyCSR = OSKI_MATTYPE_METHOD ("CSR", DestroyMatRepr);
	  if (func_DestroyCSR)
	    (*func_DestroyCSR) (A_csr);

	  return norm;
	}
    }

  /* Couldn't find a 1-norm computation routine... */
  return -1;
}

/**
 *  \brief
 *
 *  \returns \f$||A||_1 = \max_{1 \leq i \leq m} \sum_{j=1}^n |a_{ij}|\f$,
 *  or -1 on error.
 */
double
oski_CalcMat1Norm (const oski_matrix_t A)
{
  double norm;

  /* Native conversion routine */
  oski_CalcMatRepr1Norm_funcpt func_1Norm;

  if (A == INVALID_MAT)
    return -1;

  func_1Norm = OSKI_MATTYPEID_METHOD (A->input_mat.type_id, CalcMatRepr1Norm);
  if (func_1Norm != NULL)
    return (*func_1Norm) (A->input_mat.repr, &A->props);

  func_1Norm = OSKI_MATTYPEID_METHOD (A->tuned_mat.type_id, CalcMatRepr1Norm);
  if (func_1Norm != NULL)
    return (*func_1Norm) (A->tuned_mat.repr, &A->props);

  /* No native types. Try converting to CSR first. */
  norm = CalcMatRepr1NormViaCSR (&A->input_mat, &A->props);
  if (norm >= 0)
    return norm;

  norm = CalcMatRepr1NormViaCSR (&A->tuned_mat, &A->props);
  if (norm >= 0)
    return norm;

  OSKI_ERR_NOT_IMPLEMENTED (oski_CalcMat1Norm,
			    "Matrix 1-norm calculation for the given matrix type");

  return -1;
}

/**
 *  \brief
 *
 *  \param[in] A A valid tunable matrix object.
 *  \returns A's type-independent properties (a read-only pointer to its
 *  \ref oski_matcommon_t structure, valid only for the lifetime of A),
 *  or NULL on error.
 */
const oski_matcommon_t *
oski_GetMatProps (const oski_matrix_t A)
{
  if (A == INVALID_MAT)
    {
      OSKI_ERR_BAD_MAT (oski_GetMatProps, 1);
      return NULL;
    }

  return &(A->props);
}

/* eof */
