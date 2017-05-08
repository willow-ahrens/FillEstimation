/**
 *  \file src/trisolve.c
 *  \brief Sparse triangular solve implementation.
 *
 *  This module implements the matrix type-independent version
 *  which performs error-checking of the input arguments first,
 *  and then calls the appropriate kernel implementation.
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
oski_CheckArgsMatTrisolve (const oski_matrix_t T_tunable, oski_matop_t opT,
			   oski_value_t alpha, oski_vecview_t x_view,
			   const char *caller)
{
  /* Valid triangular matrix? */
  if (T_tunable == INVALID_MAT)
    {
      OSKI_ERR_BAD_MAT_MS (caller, 1);
      return ERR_BAD_MAT;
    }

  if (!OSKI_MATPROP_IS_SQUARE (&(T_tunable->props)))
    {
      OSKI_ERR_MAT_NOTSQUARE_MS (caller, 1);
      return ERR_BAD_MAT;
    }

  if (!OSKI_MATPROP_IS_TRI (&(T_tunable->props)))
    {
      OSKI_ERR_MAT_NOTTRI_MS (caller, 1);
      return ERR_BAD_MAT;
    }

  if (!OSKI_CHECK_MATOP (opT))
    {
      OSKI_ERR_BAD_MATOP_MS (caller, 2, opT);
      return ERR_BAD_ARG;
    }

  if (x_view == INVALID_VEC)
    {
      OSKI_ERR_BAD_VEC_MS (caller, 4);
      return ERR_BAD_VECVIEW;
    }

  /* Ensure compatible dimensions */
  if (opT == OP_NORMAL
      && !OSKI_MATMULT_COMPAT_OBJ_DIMS (&(T_tunable->props), x_view, x_view))
    {
      OSKI_ERR_DIM_MISMATCH_OBJ_MS (caller, &(T_tunable->props),
				    x_view, x_view);
      return ERR_DIM_MISMATCH;
    }
  else if ((opT == OP_TRANS || opT == OP_CONJ_TRANS)
	   && !OSKI_MATTRANSMULT_COMPAT_OBJ_DIMS (&(T_tunable->props),
						  x_view, x_view))
    {
      OSKI_ERR_DIM_MISMATCH_TRANS_OBJ_MS (caller, &(T_tunable->props),
					  x_view, x_view);
      return ERR_DIM_MISMATCH;
    }

  return 0;
}

/**
 *  \brief
 *
 *  \param[in] T_tunable Triangular sparse matrix object, \f$T\f$.
 *  \param[in] opT Specifies the transpose operation, \f$op(T)\f$.
 *  \param[in] alpha Scalar multiplier, \f$\alpha\f$.
 *  \param[in,out] x_view Vector view object, \f$x\f$.
 *  \returns 0 on success, or an error code.
 *
 *  \todo For efficiency, this routine does not attempt to pre-scan
 *  the matrix data structure and ensure there are no zero diagonals.
 *  At least for CSR and CSC input matrices, we should add some kind
 *  of check somewhere (e.g., at matrix handle creation time). A
 *  similar to-do appears elsewhere in this source.
 */
int
oski_MatTrisolve (const oski_matrix_t T, oski_matop_t opT,
		  oski_value_t alpha, oski_vecview_t x)
{
  oski_MatReprTrisolve_funcpt func = NULL;
  oski_id_t id = INVALID_ID;
  void *repr = NULL;
  int err;

  /* Error checking */
  err = oski_CheckArgsMatTrisolve (T, opT, alpha, x,
				   MACRO_TO_STRING (oski_MatTrisolve));
  if (err)
    return err;

  if (x == SYMBOLIC_VEC)
    {
      OSKI_ERR_BAD_VEC (oski_MatTrisolve, 4);
      return ERR_BAD_VECVIEW;
    }

  /* Check for a tuned implementation */
  id = T->tuned_mat.type_id;
  if (id != INVALID_ID)
    {
      func = OSKI_MATTYPEID_METHOD (id, MatReprTrisolve);
      repr = T->tuned_mat.repr;
    }

  /* Check for the input matrix implementation */
  if (func == NULL)		/* no tuned implementation */
    {
      id = T->input_mat.type_id;
      func = OSKI_MATTYPEID_METHOD (id, MatReprTrisolve);
      repr = T->input_mat.repr;
    }

  if (func == NULL)		/* no implementation available */
    {
      OSKI_ERR_NOT_IMPLEMENTED (oski_MatTrisolve, "Sparse triangular solve");
      return ERR_NOT_IMPLEMENTED;
    }

  oski_RestartTimer (T->timer);
  err = (*func) (repr, &(T->props), opT, alpha, x);
  oski_StopTimer (T->timer);

  /* Record in the trace */
  if (!err)
    {
      oski_traceargs_MatTrisolve_t args;
      oski_MakeArglistMatTrisolve (opT, alpha, x, &args);
      oski_RecordCall (T->trace, OSKI_KERNEL_MatTrisolve,
		       &args, sizeof (args), NULL,
		       oski_ReadElapsedTime (T->timer));
    }
  else
    OSKI_ERR (oski_MatTrisolve, err);
  return err;
}

void
oski_MakeArglistMatTrisolve (oski_matop_t op,
			     oski_value_t alpha, const oski_vecview_t x_view,
			     oski_traceargs_MatTrisolve_t * args)
{
  assert (args != NULL);
  assert (x_view != INVALID_VEC);

  args->opT = op;
  args->num_vecs = x_view->num_cols;
  VAL_SET_ONE (args->alpha);
  args->x_orient = x_view->orient;
}

/* eof */
