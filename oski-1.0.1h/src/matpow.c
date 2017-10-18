/**
 *  \file src/matpow.c
 *  \brief Sparse matrix-power-vector multiply implementation.
 *
 *  This module implements the matrix type-independent version,
 *  which performs error-checking first and then calls the
 *  the appropriate kernel implementation.
 */

#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/blas.h>

/**
 *  \brief
 *
 *  \returns 0 if all arguments are OK, or an error code (plus a
 *  call to the error handler) otherwise.
 *
 *  \todo Need more verbose error messages in the event that the
 *  vectors have improper dimensions.
 */
int
oski_CheckArgsMatPowMult (const oski_matrix_t A_tunable,
			  oski_matop_t opA, int power,
			  oski_value_t alpha, const oski_vecview_t x_view,
			  oski_value_t beta, const oski_vecview_t y_view,
			  const oski_vecview_t T_view, const char *caller)
{
  if (power == 1)
    return oski_CheckArgsMatMult (A_tunable, opA,
				  alpha, x_view, beta, y_view, caller);
  else if (power < 0)
    {
      OSKI_ERR_BAD_POWER_MS (caller, 3, power);
      return ERR_BAD_ARG;
    }

  assert (power == 0 || power >= 2);

  if (caller == NULL)
    caller = MACRO_TO_STRING (oski_CheckArgsMatPowMult);

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
      OSKI_ERR_BAD_VEC_MS (caller, 5);
      return ERR_BAD_VECVIEW;
    }
  if (y_view == INVALID_VEC)
    {
      OSKI_ERR_BAD_VEC_MS (caller, 7);
      return ERR_BAD_VECVIEW;
    }
  /* Check for compatible multiply dimensions */
  if (!OSKI_MATPROP_IS_SQUARE (&(A_tunable->props)))
    {
      OSKI_ERR_MAT_NOTSQUARE_MS (caller, 1);
      return ERR_BAD_MAT;
    }
  if (x_view != SYMBOLIC_VEC)
    {
      if (x_view->num_rows != A_tunable->props.num_rows)
	{
	  OSKI_ERR_BAD_VEC_MS (caller, 5);
	  return ERR_BAD_VECVIEW;
	}
      if (x_view->num_cols != 1)
	{
	  OSKI_ERR_VEC1_ONLY (caller, 5, x_view->num_cols);
	  return ERR_BAD_VECVIEW;
	}
    }
  if (y_view != SYMBOLIC_VEC)
    {
      if (y_view->num_rows != A_tunable->props.num_rows)
	{
	  OSKI_ERR_BAD_VEC_MS (caller, 7);
	  return ERR_BAD_VECVIEW;
	}
      if (y_view->num_cols != 1)
	{
	  OSKI_ERR_VEC1_ONLY (caller, 7, y_view->num_cols);
	  return ERR_BAD_VECVIEW;
	}
    }
  if (T_view != INVALID_VEC && T_view != SYMBOLIC_VEC)
    {
      if (T_view->num_rows != A_tunable->props.num_rows)
	{
	  OSKI_ERR_BAD_VEC_MS (caller, 8);
	  return ERR_BAD_VECVIEW;
	}
      if (T_view->num_cols < power - 1)
	{
	  OSKI_ERR_VEC_FEW_MS (caller, 8, T_view->num_cols, power - 1);
	  return ERR_BAD_VECVIEW;
	}
    }

  return 0;
}

/**
 *  \brief Logically partition the columns of a multivector object
 *  into three.
 */
static void
SplitVecsColwise (oski_vecview_t T,
		  oski_index_t na, oski_vecview_t A,
		  oski_vecview_t x, oski_index_t nb, oski_vecview_t B)
{
  assert (T != NULL);
  assert (x != NULL);
  assert (T->num_cols >= (na + 1 + nb));

  if (A != INVALID_VEC)
    {
      A->val = T->val;
      A->num_rows = T->num_rows;
      A->num_cols = na;
      A->orient = T->orient;
      A->stride = T->stride;
      A->rowinc = T->rowinc;
      A->colinc = T->colinc;
    }

  x->val = T->val + na * T->colinc;
  x->num_rows = T->num_rows;
  x->num_cols = 1;
  x->orient = T->orient;
  x->rowinc = T->rowinc;
  x->colinc = T->colinc;

  if (B != INVALID_VEC)
    {
      B->val = T->val + (na + 1) * T->colinc;
      B->num_rows = T->num_rows;
      B->num_cols = nb;
      B->orient = T->orient;
      B->rowinc = T->rowinc;
      B->colinc = T->colinc;
    }
}

/**
 *  \brief Default matrix-power-vector multiply routine.
 *
 *  This routine does not perform any argument error checking.
 *  If T_view is NULL, this routine may quietly attempt to
 *  allocate memory for one temporary vector, deleted before
 *  returning.
 */
static int
DefaultMatPowMult (const oski_matrix_t A_tunable, oski_matop_t opA, int power,
		   oski_value_t alpha, const oski_vecview_t x_view,
		   oski_value_t beta, oski_vecview_t y_view,
		   oski_vecview_t T_view)
{
  int do_free_t = 0;
  oski_value_t *tp = NULL;
  oski_vecstruct_t Tp, t, Tq;
  oski_vecview_t Tp_view = INVALID_VEC;
  oski_vecview_t t_view = INVALID_VEC;
  oski_vecview_t Tq_view = INVALID_VEC;

  oski_index_t p, q;
  int err;

  if (power < 0)
    return ERR_BAD_ARG;
  if (power == 0)
    {
      oski_ScaleVecView (y_view, beta);
      oski_AXPYVecView (x_view, alpha, y_view);
      return 0;
    }
  if (power == 1)
    return oski_MatMult (A_tunable, opA, alpha, x_view, beta, y_view);

  p = (power + 1) / 2;		/* == ceil(power/2) */
  q = power - p;		/* >= 1 */
  assert (power >= 2 && p >= 1 && q >= 1);

  assert (x_view->num_cols == 1);
  assert (y_view->num_cols == 1);

  /* Allocate temporary vector, t_view, if needed */
  if (T_view == INVALID_VEC)
    {
      tp = oski_Malloc (oski_value_t, y_view->num_rows);
      if (tp == NULL)
	return ERR_OUT_OF_MEMORY;
      do_free_t = 1;
      Tp_view = INVALID_VEC;
      Tq_view = INVALID_VEC;
      t_view->val = tp;
      t_view->num_rows = y_view->num_rows;
      t_view->num_cols = 1;
      t_view->orient = LAYOUT_COLMAJ;
      t_view->stride = t_view->num_rows;
      t_view->rowinc = 1;
      t_view->colinc = t_view->stride;
    }
  else
    {
      Tp_view = (p == 1) ? INVALID_VEC : &Tp;
      Tq_view = (q == 1) ? INVALID_VEC : &Tq;
      t_view = &t;
      SplitVecsColwise (T_view, p - 1, Tp_view, t_view, q, Tq_view);
    }

  /* Compute:
     t <-- A^p * x
     y <-- beta*y + A^q * t
   */
  err = DefaultMatPowMult (A_tunable, opA, p, TVAL_ONE, x_view,
			   TVAL_ZERO, t_view, Tp_view);
  if (err)
    {
      if (do_free_t)
	oski_Free (tp);
      return err;
    }
  err = DefaultMatPowMult (A_tunable, opA, q, alpha, t_view,
			   beta, y_view, Tq_view);
  if (err)
    {
      if (do_free_t)
	oski_Free (tp);
      return err;
    }

  if (do_free_t)
    oski_Free (tp);
  return 0;
}

/**
 *  \brief
 *
 *  \param[in] A_tunable The \f$m\times n\f$ matrix object \f$A\f$.
 *  \param[in] opA Transpose option, \f$op(A)\f$.
 *  \param[in] power Non-negative matrix power \f$k\f$.
 *  \param[in] alpha Scalar multiplier \f$\alpha\f$.
 *  \param[in] x_view Single vector object \f$x\f$.
 *  \param[in] beta Scalar multiplier \f$\alpha\f$.
 *  \param[in,out] y_view Single vector object \f$y\f$.
 *  \param[in,out] T_view Multivector object \f$T\f$ consisting of
 *  \f$\max\{k-1,0\}\f$ column vectors in which to store intermediate
 *  results.
 *  \returns Computes \f$y, T\f$ and returns 0 on success, or an
 *  error code otherwise.
 */
int
oski_MatPowMult (const oski_matrix_t A_tunable, oski_matop_t opA, int power,
		 oski_value_t alpha, const oski_vecview_t x_view,
		 oski_value_t beta, oski_vecview_t y_view,
		 oski_vecview_t T_view)
{
  oski_MatReprPowMult_funcpt func = NULL;
  oski_id_t id = INVALID_ID;
  void *repr = NULL;
  int err;

  /* Check input parameters */
  err = oski_CheckArgsMatPowMult (A_tunable, opA, power,
				  alpha, x_view, beta, y_view, T_view,
				  MACRO_TO_STRING (oski_MatPowMult));
  if (err)
    return err;

  if ((x_view == SYMBOLIC_VEC) || (x_view == SYMBOLIC_MULTIVEC))
    {
      OSKI_ERR_BAD_VEC (oski_MatPowMult, 5);
      return ERR_BAD_VECVIEW;
    }
  if ((y_view == SYMBOLIC_VEC) || (y_view == SYMBOLIC_MULTIVEC))
    {
      OSKI_ERR_BAD_VEC (oski_MatPowMult, 7);
      return ERR_BAD_VECVIEW;
    }
  if (T_view == SYMBOLIC_VEC)
    {
      OSKI_ERR_BAD_VEC (oski_MatPowMult, 8);
      return ERR_BAD_VECVIEW;
    }

  /* Check for a tuned implementation */
  id = A_tunable->tuned_mat.type_id;
  if (id != INVALID_ID)
    {
      func = OSKI_MATTYPEID_METHOD (id, MatReprPowMult);
      repr = A_tunable->tuned_mat.repr;
    }

  /* Check for the input matrix implementation */
  if (func == NULL)
    {				/* no tuned implementation */
      id = A_tunable->input_mat.type_id;
      func = OSKI_MATTYPEID_METHOD (id, MatReprPowMult);
      repr = A_tunable->input_mat.repr;
    }

  oski_RestartTimer (A_tunable->timer);
  if (func == NULL)
    err = DefaultMatPowMult (A_tunable, opA, power,
			     alpha, x_view, beta, y_view, T_view);
  else				/* func != NULL */
    err = (*func) (repr, &(A_tunable->props), opA, power,
		   alpha, x_view, beta, y_view, T_view);
  oski_StopTimer (A_tunable->timer);

  /* Record in the trace */
  if (err)
    {
      OSKI_ERR (oski_MatPowMult, err);
    }
  else
    {				/* !err */
      oski_traceargs_MatPowMult_t args;
      oski_MakeArglistMatPowMult (opA, power,
				  alpha, x_view, beta, y_view, T_view, &args);
      oski_RecordCall (A_tunable->trace, OSKI_KERNEL_MatPowMult,
		       &args, sizeof (args), NULL,
		       oski_ReadElapsedTime (A_tunable->timer));
    }

  return err;
}

void
oski_MakeArglistMatPowMult (oski_matop_t opA, int power,
			    oski_value_t alpha, const oski_vecview_t x_view,
			    oski_value_t beta, const oski_vecview_t y_view,
			    const oski_vecview_t T_view,
			    oski_traceargs_MatPowMult_t * args)
{
  assert (args != NULL);
  assert (x_view != INVALID_VEC);
  assert (y_view != INVALID_VEC);

  args->opA = opA;
  args->power = power;
  args->num_vecs = x_view->num_cols;
  VAL_ASSIGN (args->alpha, TVAL_ONE);
  args->x_orient = x_view->orient;
  VAL_ASSIGN (args->beta, TVAL_ONE);
  args->y_orient = y_view->orient;
  args->t_orient = (T_view == INVALID_VEC) ? LAYOUT_COLMAJ : T_view->orient;
}

/* eof */
