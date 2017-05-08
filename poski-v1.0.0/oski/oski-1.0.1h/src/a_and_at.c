/**
 *  \file src/a_and_at.c
 *  \brief Implementation of simultaneously multiplication by sparse
 *  \f$A\f$ and \f$\mathrm{op}(A) \in \{A, A^T, A^H=\bar{A}^T\}\f$.
 *
 *  This module implements the matrix type-independent version,
 *  which performs error-checking first and then calls the
 *  the appropriate kernel implementation.
 */

#include <assert.h>
#include <oski/common.h>
#include <oski/matrix.h>

#if defined(DO_NAME_MANGLING)
/**
 *  \brief Mangled name for default simultaneous multiplication by
 *  sparse \f$A\f$ and \f$\mathrm{op}(A)\f$.
 */
#define MatMultAndMatTransMultDefault \
	MANGLE_(MatMultAndMatTransMultDefault)
#endif

/**
 *  \brief A default implementation of simultaneous multiplication by
 *  sparse \f$A\f$ and \f$\mathrm{op}(A)\f$ based on multiple calls
 *  to sparse matrix-vector multiply.
 *
 *  This implementation provides a fall-back code in the event that
 *  no tuned implementation exists.
 *
 *  \pre A_tunable != INVALID_VEC
 */
static int
MatMultAndMatTransMultDefault (const oski_matrix_t A_tunable,
			       oski_value_t alpha, const oski_vecview_t x,
			       oski_value_t beta, oski_vecview_t y,
			       oski_matop_t op, oski_value_t omega,
			       const oski_vecview_t w, oski_value_t zeta,
			       oski_vecview_t z)
{
  int err = oski_MatMult (A_tunable, OP_NORMAL, alpha, x, beta, y);
  if (err)
    return err;
  return oski_MatMult (A_tunable, op, omega, w, zeta, z);
}

/**
 *  \brief Verifies arguments to \ref oski_MatMultAndMatTransMult.
 *
 *  \returns 0 if all arguments are OK, or an error code (plus a
 *  call to the error handler) otherwise.
 */
int
oski_CheckArgsMatMultAndMatTransMult (const oski_matrix_t A_tunable,
				      oski_value_t alpha,
				      const oski_vecview_t x,
				      oski_value_t beta, oski_vecview_t y,
				      oski_matop_t op, oski_value_t omega,
				      const oski_vecview_t w,
				      oski_value_t zeta, oski_vecview_t z,
				      const char *caller)
{
  int err = 0;
  if (caller == NULL)
    caller = MACRO_TO_STRING (oski_CheckArgsMatMultAndMatTransMult);
  if (A_tunable == INVALID_MAT)
    {
      OSKI_ERR_BAD_MAT_MS (caller, 1);
      return ERR_BAD_MAT;
    }
  err = oski_CheckArgsMatMult (A_tunable, OP_NORMAL, alpha, x, beta, y,
			       caller);
  if (err)
    return err;
  return oski_CheckArgsMatMult (A_tunable, op, omega, w, zeta, z, caller);
}

/**
 *  \brief
 *
 *  \param[in] A_tunable Sparse matrix object, \f$A\f$.
 *  \param[in] op Specifies the transpose operation, \f$\mathrm{op}(A)\f$.
 *  \param[in] alpha Scalar multiplier, \f$\alpha\f$.
 *  \param[in] x_view Vector view object, \f$x\f$.
 *  \param[in] beta Scalar multiplier, \f$\beta\f$.
 *  \param[in] y_view Vector view object, \f$y\f$.
 *  \param[in] omega Scalar multiplier, \f$\omega\f$.
 *  \param[in] w_view Vector view object, \f$w\f$.
 *  \param[in] zeta Scalar multiplier, \f$\zeta\f$.
 *  \param[in] z_view Vector view object, \f$z\f$.
 *  \returns 0 on success, or an error code.
 */
int
oski_MatMultAndMatTransMult (const oski_matrix_t A_tunable,
			     oski_value_t alpha, const oski_vecview_t x_view,
			     oski_value_t beta, oski_vecview_t y_view,
			     oski_matop_t op, oski_value_t omega,
			     const oski_vecview_t w_view, oski_value_t zeta,
			     oski_vecview_t z_view)
{
  int err = 0;
  oski_MatReprMultAndMatReprTransMult_funcpt func_Mult = NULL;
  oski_id_t id = INVALID_ID;
  void *repr = NULL;

  /* Check arguments */
  err = oski_CheckArgsMatMultAndMatTransMult (A_tunable,
					      alpha, x_view, beta, y_view, op,
					      omega, w_view, zeta, z_view,
					      "oski_MatMultAndMatTransMult");
  if (err)
    return err;
  if (x_view == SYMBOLIC_VEC || x_view == SYMBOLIC_MULTIVEC)
    {
      OSKI_ERR_BAD_VEC (oski_MatTransMatMult, 3);
      return ERR_BAD_VECVIEW;
    }
  if (y_view == SYMBOLIC_VEC || y_view == SYMBOLIC_MULTIVEC)
    {
      OSKI_ERR_BAD_VEC (oski_MatTransMatMult, 5);
      return ERR_BAD_VECVIEW;
    }
  if (w_view == SYMBOLIC_VEC || w_view == SYMBOLIC_MULTIVEC)
    {
      OSKI_ERR_BAD_VEC (oski_MatTransMatMult, 8);
      return ERR_BAD_VECVIEW;
    }
  if (z_view == SYMBOLIC_VEC || z_view == SYMBOLIC_MULTIVEC)
    {
      OSKI_ERR_BAD_VEC (oski_MatTransMatMult, 10);
      return ERR_BAD_VECVIEW;
    }

  /* Check for a tuned implementation */
  id = A_tunable->tuned_mat.type_id;
  if (id != INVALID_ID)
    {
      repr = A_tunable->tuned_mat.repr;
      func_Mult = OSKI_MATTYPEID_METHOD (id, MatReprMultAndMatReprTransMult);
    }

  if (func_Mult != NULL)
    {				/* execute the tuned implementation */
      err = (*func_Mult) (repr, &(A_tunable->props),
			  alpha, x_view, beta, y_view, op, omega, w_view,
			  zeta, z_view);
    }
  if (func_Mult == NULL || err)
    {				/* Try default */
      err = MatMultAndMatTransMultDefault (A_tunable,
					   alpha, x_view, beta, y_view, op,
					   omega, w_view, zeta, z_view);
    }

  /* Record in the trace */
  if (err)
    {
      OSKI_ERR (oski_MatMultAndMatTransMult, err);
    }
  else
    {
      oski_traceargs_MatMultAndMatTransMult_t args;
      oski_MakeArglistMatMultAndMatTransMult (alpha, x_view, beta, y_view,
					      op, omega, w_view, zeta, z_view,
					      &args);
      oski_RecordCall (A_tunable->trace, OSKI_KERNEL_MatMultAndMatTransMult,
		       &args, sizeof (args), NULL,
		       oski_ReadElapsedTime (A_tunable->timer));
    }

  return err;
}

void
oski_MakeArglistMatMultAndMatTransMult (oski_value_t alpha,
					const oski_vecview_t x_view,
					oski_value_t beta,
					const oski_vecview_t y_view,
					oski_matop_t opA, oski_value_t omega,
					const oski_vecview_t w_view,
					oski_value_t zeta,
					const oski_vecview_t z_view,
					oski_traceargs_MatMultAndMatTransMult_t
					* args)
{
  assert (args != NULL);
  assert (x_view != INVALID_VEC);
  assert (y_view != INVALID_VEC);
  assert (w_view != INVALID_VEC);
  assert (z_view != INVALID_VEC);

  VAL_ASSIGN (args->alpha, TVAL_ONE);
  VAL_ASSIGN (args->beta, TVAL_ONE);
  VAL_ASSIGN (args->omega, TVAL_ONE);
  VAL_ASSIGN (args->zeta, TVAL_ONE);
  args->num_vecs = x_view->num_cols;
  args->opA = opA;
  args->x_orient = x_view->orient;
  args->y_orient = y_view->orient;
  args->w_orient = w_view->orient;
  args->z_orient = z_view->orient;
}

/* eof */
