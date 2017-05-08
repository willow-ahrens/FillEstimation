/**
 *  \file src/ata.c
 *  \brief Sparse \f$A^TA\cdot x\f$ implementation.
 *
 *  This module implements the matrix type-independent version,
 *  which performs error-checking first and then calls the
 *  the appropriate kernel implementation.
 */

#include <assert.h>
#include <oski/common.h>
#include <oski/matrix.h>

#if defined(DO_NAME_MANGLING)
/** Mangled name for default sparse \f$A^TA\cdot x\f$ implementation. */
#define MatTransMatMultDefault MANGLE_(MatTransMatMultDefault)
#endif

/**
 *  \brief A default implementation of the sparse \f$A^TA\cdot x\f$
 *  kernel based on multiple calls to sparse matrix-vector multiply.
 *
 *  This implementation provides a fall-back code in the event that
 *  no tuned implementation exists.
 *
 *  \pre A_tunable != INVALID_VEC
 */
static int
MatTransMatMultDefault (const oski_matrix_t A_tunable,
			oski_ataop_t op, oski_value_t alpha,
			const oski_vecview_t x, oski_value_t beta,
			oski_vecview_t y, oski_vecview_t t0)
{
  /* Temporary vector, in case the user did not provide one. */
  oski_value_t *t_val = NULL;
  oski_vecview_t t = INVALID_VEC;
  oski_index_t t_num_rows = 0;
  oski_index_t t_num_cols = 0;
  int err;

  switch (op)
    {
    case OP_AT_A:
    case OP_AH_A:
      t_num_rows = A_tunable->props.num_rows;
      t_num_cols = x->num_cols;
      break;
    case OP_A_AT:
    case OP_A_AH:
      t_num_rows = A_tunable->props.num_cols;
      t_num_cols = x->num_cols;
      break;

    default:
      OSKI_ERR_BAD_ATAOP (MatTransMatMultDefault, 2, op);
      return ERR_BAD_ARG;
    }

  /*
   *  If user did not provide a temporary vector for the intermediate
   *  multiply, then create one.
   */
  if (t0 == INVALID_VEC)
    {
      t_val = oski_Malloc (oski_value_t, t_num_rows * t_num_cols);
      if (t_val == NULL)
	{
	  OSKI_ERR (MatTransMatMultDefault, ERR_OUT_OF_MEMORY);
	  return ERR_OUT_OF_MEMORY;
	}

      t = oski_CreateMultiVecView (t_val, t_num_rows, t_num_cols,
				   LAYOUT_COLMAJ, t_num_rows);
      if (!t)
	{
	  oski_Free (t_val);
	  return ERR_OUT_OF_MEMORY;
	}
    }
  else				/* Use user-supplied vector view */
    t = t0;

  /*
   *  Execute the default kernel.
   */
  switch (op)
    {
    case OP_AT_A:
      err = oski_MatMult (A_tunable, OP_NORMAL, TVAL_ONE, x, TVAL_ZERO, t);
      if (err)
	break;
      err = oski_MatMult (A_tunable, OP_TRANS, alpha, t, beta, y);
      if (err)
	break;
      break;

    case OP_AH_A:
      err = oski_MatMult (A_tunable, OP_NORMAL, TVAL_ONE, x, TVAL_ZERO, t);
      if (err)
	break;
      err = oski_MatMult (A_tunable, OP_CONJ_TRANS, alpha, t, beta, y);
      if (err)
	break;
      break;

    case OP_A_AT:
      err = oski_MatMult (A_tunable, OP_TRANS, TVAL_ONE, x, TVAL_ZERO, t);
      if (err)
	break;
      err = oski_MatMult (A_tunable, OP_NORMAL, alpha, t, beta, y);
      if (err)
	break;
      break;

    case OP_A_AH:
      err = oski_MatMult (A_tunable, OP_CONJ_TRANS,
			  TVAL_ONE, x, TVAL_ZERO, t);
      if (err)
	break;
      err = oski_MatMult (A_tunable, OP_NORMAL, alpha, t, beta, y);
      if (err)
	break;
      break;

    default:
      assert (0);
    }

  /* Clean-up temporary vector if necessary */
  if (t0 == INVALID_VEC)
    {
      oski_DestroyVecView (t);
      oski_Free (t_val);
    }

  return err;
}

/**
 *  \brief
 *
 *  \returns 0 if all arguments are OK, or an error code (plus a
 *  call to the error handler) otherwise.
 */
int
oski_CheckArgsMatTransMatMult (const oski_matrix_t A_tunable,
			       oski_ataop_t op,
			       oski_value_t alpha,
			       const oski_vecview_t x_view, oski_value_t beta,
			       const oski_vecview_t y_view,
			       const oski_vecview_t t_view,
			       const char *caller)
{
  if (caller == NULL)
    caller = MACRO_TO_STRING (oski_CheckArgsMatTransMatMult);

  if (A_tunable == INVALID_MAT)
    {
      OSKI_ERR_BAD_MAT_MS (caller, 1);
      return ERR_BAD_MAT;
    }

  if (!OSKI_CHECK_ATAOP (op))
    {
      OSKI_ERR_BAD_ATAOP_MS (caller, 2, op);
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
  switch (op)
    {
    case OP_AT_A:
    case OP_AH_A:
      if (!OSKI_MATMULT_COMPAT_OBJ_DIMS (&(A_tunable->props), x_view, t_view))
	{
	  OSKI_ERR_DIM_MISMATCH_OBJ_MS (caller,
					&(A_tunable->props), x_view, t_view);
	  return ERR_DIM_MISMATCH;
	}

      if (!OSKI_MATTRANSMULT_COMPAT_OBJ_DIMS (&(A_tunable->props),
					      t_view, y_view))
	{
	  OSKI_ERR_DIM_MISMATCH_TRANS_OBJ_MS (caller,
					      &(A_tunable->props), t_view,
					      y_view);
	  return ERR_DIM_MISMATCH;
	}
      break;

    case OP_A_AT:
    case OP_A_AH:
      if (!OSKI_MATTRANSMULT_COMPAT_OBJ_DIMS (&(A_tunable->props),
					      x_view, t_view))
	{
	  OSKI_ERR_DIM_MISMATCH_TRANS_OBJ_MS (caller,
					      &(A_tunable->props), x_view,
					      t_view);
	  return ERR_DIM_MISMATCH;
	}

      if (!OSKI_MATMULT_COMPAT_OBJ_DIMS (&(A_tunable->props), t_view, y_view))
	{
	  OSKI_ERR_DIM_MISMATCH_OBJ_MS (caller,
					&(A_tunable->props), t_view, y_view);
	  return ERR_DIM_MISMATCH;
	}
      break;

    default:
      assert (0);
    }

  return 0;
}

/**
 *  \brief
 *
 *  \param[in] A_tunable Sparse matrix object, \f$A\f$.
 *  \param[in] op Specifies the transpose operation, \f$op(A)\f$.
 *  \param[in] alpha Scalar multiplier, \f$\alpha\f$.
 *  \param[in] x_view Vector view object, \f$x\f$.
 *  \param[in] beta Scalar multiplier, \f$\beta\f$.
 *  \param[in] y_view Vector view object, \f$y\f$.
 *  \param[in] t_view Vector view object, \f$t\f$. The caller may set
 *  t_view == INVALID_VEC if no intermediate result is desired.
 *  \returns 0 on success, or an error code.
 */
int
oski_MatTransMatMult (const oski_matrix_t A_tunable, oski_ataop_t op,
		      oski_value_t alpha, const oski_vecview_t x_view,
		      oski_value_t beta, oski_vecview_t y_view,
		      oski_vecview_t t_view)
{
  int err;
  oski_id_t id = INVALID_ID;

  /* Check arguments */
  err = oski_CheckArgsMatTransMatMult (A_tunable, op, alpha, x_view,
				       beta, y_view, t_view,
				       MACRO_TO_STRING
				       (oski_MatTransMatMult));
  if (err)
    return err;

  if ((x_view == SYMBOLIC_VEC) || (x_view == SYMBOLIC_MULTIVEC))
    {
      OSKI_ERR_BAD_VEC (oski_MatTransMatMult, 4);
      return ERR_BAD_VECVIEW;
    }

  if ((y_view == SYMBOLIC_VEC) || (y_view == SYMBOLIC_MULTIVEC))
    {
      OSKI_ERR_BAD_VEC (oski_MatTransMatMult, 4);
      return ERR_BAD_VECVIEW;
    }

  if (t_view != NULL)
    {
      if ((t_view == SYMBOLIC_VEC) || (t_view == SYMBOLIC_MULTIVEC))
	{
	  OSKI_ERR_BAD_VEC (oski_MatTransMatMult, 4);
	  return ERR_BAD_VECVIEW;
	}
    }

  /* Call underlying kernel implementation */
  err = 0;

  /* Check for a tuned implementation */
  err = ERR_NOT_IMPLEMENTED;
  id = A_tunable->tuned_mat.type_id;
  if (id != INVALID_ID)
    {
      void *repr = A_tunable->tuned_mat.repr;
      oski_MatReprTransMatReprMult_funcpt func_MatReprTransMatReprMult
	= OSKI_MATTYPEID_METHOD (id, MatReprTransMatReprMult);

      if (func_MatReprTransMatReprMult != NULL)	/* has tuned code */
	{
	  oski_RestartTimer (A_tunable->timer);
	  err = (*func_MatReprTransMatReprMult) (repr, &(A_tunable->props),
						 op, alpha, x_view, beta,
						 y_view, t_view);
	  oski_StopTimer (A_tunable->timer);

	  /* Record in the trace */
	  if (!err)
	    {
	      oski_traceargs_MatTransMatMult_t args;
	      oski_MakeArglistMatTransMatMult (op,
					       alpha, x_view, beta, y_view,
					       t_view, &args);
	      oski_RecordCall (A_tunable->trace, OSKI_KERNEL_MatTransMatMult,
			       &args, sizeof (args), NULL,
			       oski_ReadElapsedTime (A_tunable->timer));
	    }
	}
    }

  /* If no tuned implementation ran successfully, try default */
  if (err)
    {
      err = MatTransMatMultDefault (A_tunable, op, alpha, x_view,
				    beta, y_view, t_view);
      /* This call will lead to separate traces, so don't trace */
    }

  if (err)			/* Fatal error */
    OSKI_ERR (oski_MatTransMatMult, err);

  return err;
}

void
oski_MakeArglistMatTransMatMult (oski_ataop_t op,
				 oski_value_t alpha,
				 const oski_vecview_t x_view,
				 oski_value_t beta,
				 const oski_vecview_t y_view,
				 const oski_vecview_t t_view,
				 oski_traceargs_MatTransMatMult_t * args)
{
  assert (args != NULL);
  assert (x_view != INVALID_VEC);
  assert (y_view != INVALID_VEC);

  args->op = op;
  VAL_ASSIGN (args->alpha, TVAL_ONE);
  args->num_vecs = x_view->num_cols;
  VAL_ASSIGN (args->beta, TVAL_ONE);
  args->x_orient = x_view->orient;
  args->y_orient = y_view->orient;
  args->t_orient = (t_view == INVALID_VEC) ? LAYOUT_COLMAJ : t_view->orient;
}

/* eof */
