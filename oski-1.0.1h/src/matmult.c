/**
 *  \file src/matmult.c
 *  \brief Sparse matrix-vector multiply implementation.
 *
 *  This module implements the matrix type-independent version,
 *  which performs error-checking first and then calls the
 *  the appropriate kernel implementation.
 */

#include <oski/common.h>
#include <oski/matrix.h>

/**
 *  \brief
 *
 *  \returns 0 if all arguments are OK, or an error code (plus a
 *  call to the error handler) otherwise.
 */
int
oski_CheckArgsMatMult (const oski_matrix_t A_tunable,
		       oski_matop_t opA,
		       oski_value_t alpha, const oski_vecview_t x_view,
		       oski_value_t beta, const oski_vecview_t y_view,
		       const char *caller)
{
  if (caller == NULL)
    caller = MACRO_TO_STRING (oski_CheckArgsMatMult);

  /* Check arguments */
  if (A_tunable == INVALID_MAT)
    {
      OSKI_ERR_BAD_MAT_MS (caller, 1);
      return ERR_BAD_MAT;
    }

  if (!OSKI_CHECK_MATOP (opA))
    {
      OSKI_ERR_BAD_MATOP_MS (caller, 2, opA);
      return ERR_BAD_ARG;
    }

  if (x_view == INVALID_VEC)
    {
      OSKI_ERR_BAD_VEC_MS (caller, 4);
      return ERR_BAD_VECVIEW;
    }

  if (y_view == INVALID_VEC)
    {
      OSKI_ERR_BAD_VEC_MS (caller, 6);
      return ERR_BAD_VECVIEW;
    }

  /* Check for compatible multiply dimensions */
  if ((opA == OP_NORMAL)
      && !OSKI_MATMULT_COMPAT_OBJ_DIMS (&(A_tunable->props), x_view, y_view))
    {
      OSKI_ERR_DIM_MISMATCH_OBJ_MS (caller, &(A_tunable->props),
				    x_view, y_view);
      return ERR_DIM_MISMATCH;
    }
  else if (((opA == OP_TRANS) || (opA == OP_CONJ_TRANS))
	   && !OSKI_MATTRANSMULT_COMPAT_OBJ_DIMS (&(A_tunable->props),
						  x_view, y_view))
    {
      OSKI_ERR_DIM_MISMATCH_TRANS_OBJ_MS (caller, &(A_tunable->props),
					  x_view, y_view);
      return ERR_DIM_MISMATCH;
    }

  return 0;
}

/**
 *  \brief
 *
 *  \param[in] A_tunable Sparse matrix object, \f$A\f$.
 *  \param[in] opA Specifies the transpose operation, \f$op(A)\f$.
 *  \param[in] alpha Scalar multiplier, \f$\alpha\f$.
 *  \param[in] x_view Vector view object, \f$x\f$.
 *  \param[in] beta Scalar multiplier, \f$\beta\f$.
 *  \param[in,out] y_view Vector view object, \f$y\f$.
 *  \returns 0 on success, or an error code.
 */
int
oski_MatMult (const oski_matrix_t A_tunable, oski_matop_t opA,
	      oski_value_t alpha, const oski_vecview_t x_view,
	      oski_value_t beta, const oski_vecview_t y_view)
{
  oski_MatReprMult_funcpt func_MatReprMult = NULL;
  oski_id_t id = INVALID_ID;
  void *repr = NULL;
  int err;

  /* Check input parameters */
  err = oski_CheckArgsMatMult (A_tunable, opA,
			       alpha, x_view, beta, y_view,
			       MACRO_TO_STRING (oski_MatMult));
  if (err)
    return err;

  if ((x_view == SYMBOLIC_VEC) || (x_view == SYMBOLIC_MULTIVEC))
    {
      OSKI_ERR_BAD_VEC (oski_MatMult, 4);
      return ERR_BAD_VECVIEW;
    }

  if ((y_view == SYMBOLIC_VEC) || (y_view == SYMBOLIC_MULTIVEC))
    {
      OSKI_ERR_BAD_VEC (oski_MatMult, 6);
      return ERR_BAD_VECVIEW;
    }

  /* Check for a tuned implementation */
  id = A_tunable->tuned_mat.type_id;
  if (id != INVALID_ID)
    {
      func_MatReprMult = OSKI_MATTYPEID_METHOD (id, MatReprMult);
      repr = A_tunable->tuned_mat.repr;
    }

  /* Check for the input matrix implementation */
  if (func_MatReprMult == NULL)	/* no tuned implementation */
    {
      id = A_tunable->input_mat.type_id;
      func_MatReprMult = OSKI_MATTYPEID_METHOD (id, MatReprMult);
      repr = A_tunable->input_mat.repr;
    }

  if (func_MatReprMult == NULL)	/* no implementation at all! */
    {
      OSKI_ERR_NOT_IMPLEMENTED (oski_MatMult,
				"Sparse matrix-vector multiply");
      return ERR_NOT_IMPLEMENTED;
    }

  oski_RestartTimer (A_tunable->timer);
  err = (*func_MatReprMult) (repr, &(A_tunable->props),
			     opA, alpha, x_view, beta, y_view);
  oski_StopTimer (A_tunable->timer);

  /* Record in the trace */
  if (err)
    {
      OSKI_ERR (oski_MatMult, err);
    }
  else
    {
      oski_traceargs_MatMult_t args;
      oski_MakeArglistMatMult (opA, alpha, x_view, beta, y_view, &args);
      oski_RecordCall (A_tunable->trace, OSKI_KERNEL_MatMult,
		       &args, sizeof (args), NULL,
		       oski_ReadElapsedTime (A_tunable->timer));
    }

  return err;
}

void
oski_MakeArglistMatMult (oski_matop_t opA,
			 oski_value_t alpha, const oski_vecview_t x_view,
			 oski_value_t beta, const oski_vecview_t y_view,
			 oski_traceargs_MatMult_t * args)
{
  assert (args != NULL);
  assert (x_view != INVALID_VEC);
  assert (y_view != INVALID_VEC);

  args->opA = opA;
  args->num_vecs = x_view->num_cols;
  VAL_SET_ONE (args->alpha);
  args->x_orient = x_view->orient;
  VAL_SET_ONE (args->beta);
  args->y_orient = y_view->orient;
}

/* eof */
