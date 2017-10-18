/**
 *  \file src/MBCSR/a_and_at.c
 *  \brief MBCSR implementation of the simultaneous multiplication
 *  by sparse \f$A\f$ and \f$op(A) \in \{A, A^T, \bar{A}^T=A^H\}\f$.
 *  \ingroup MATTYPE_MBCSR
 */

#include <assert.h>
#include <oski/config.h>
#include <oski/common.h>
#include <oski/blas.h>
#include <oski/blas_names.h>
#include <oski/modloader.h>
#include <oski/matrix.h>

#include <oski/MBCSR/module.h>
#include <oski/matmodexport.h>

static int
CanBeExecuted (const oski_matMBCSR_t * A, const oski_matcommon_t * props,
	       oski_matop_t op,
	       MBCSR_SubmatReprMultAndSubmatReprTransMult_funcpt * p_func1,
	       MBCSR_SubmatReprMultAndSubmatReprTransMult_funcpt * p_func2,
	       oski_MatReprMultAndMatReprTransMult_funcpt * p_func3,
	       oski_MatReprMult_funcpt * p_func3_alt)
{
  const char *kernel_name = "SubmatReprMultAndSubmatReprTransMult";

  if (A == NULL)
    return 0;

  if (props != NULL
      && (props->pattern.is_symm || props->pattern.is_herm)
      && A->stored.is_upper != A->stored.is_lower)
    {
      return 0;
    }
  if (op != OP_NORMAL && op != OP_CONJ && op != OP_TRANS
      && op != OP_CONJ_TRANS)
    {
      return 0;
    }

  if (((op == OP_NORMAL || op == OP_CONJ)
       && (!A->enabled.MatMultAndMatMult))
      || ((op == OP_TRANS || op == OP_CONJ_TRANS)
	  && (!A->enabled.MatMultAndMatTransMult)))
    {
      return 0;
    }

  if (p_func1 != NULL && A->A1.num_block_rows > 0)
    {
      *p_func1 = MBCSR_GetSubmatKernel (&(A->A1), kernel_name);
      if (*p_func1 == NULL)
	return 0;
    }
  if (p_func2 != NULL && A->A2.num_block_rows > 0)
    {
      *p_func2 = MBCSR_GetSubmatKernel (&(A->A2), kernel_name);
      if (*p_func2 == NULL)
	return 0;
    }
  if (A->p_A3 != NULL)
    {
      if (p_func3 != NULL)
	{
	  *p_func3 = OSKI_MATTYPE_METHOD ("BCSR",
					  MatReprMultAndMatReprTransMult);
	}
      if (p_func3_alt != NULL)
	{
	  *p_func3_alt = OSKI_MATTYPE_METHOD ("BCSR", MatReprMult);
	  if (*p_func3_alt == NULL && p_func3 != NULL && *p_func3 == NULL)
	    {
	      return 0;
	    }
	}
    }

  /* Seems to meet all preconditions... */
  return 1;
}

/**
 *  \brief
 *
 *  \note 'props' is not required unless the matrix is stored using
 *  symmetric/Hermitian half-storage. That is, the caller may set
 *  props to NULL.
 */
int
oski_MatReprMultAndMatReprTransMult (const void *pA,
				     const oski_matcommon_t * props,
				     oski_value_t alpha,
				     const oski_vecview_t x_view,
				     oski_value_t beta, oski_vecview_t y_view,
				     oski_matop_t opA, oski_value_t omega,
				     const oski_vecview_t w_view,
				     oski_value_t zeta, oski_vecview_t z_view)
{
  const oski_matMBCSR_t *A = (const oski_matMBCSR_t *) pA;

  MBCSR_SubmatReprMultAndSubmatReprTransMult_funcpt func1 = NULL;
  MBCSR_SubmatReprMultAndSubmatReprTransMult_funcpt func2 = NULL;
  oski_MatReprMultAndMatReprTransMult_funcpt func3 = NULL;
  oski_MatReprMult_funcpt func3_alt = NULL;

  if (!CanBeExecuted (A, props, opA, &func1, &func2, &func3, &func3_alt))
    {
      return ERR_NOT_IMPLEMENTED;
    }

  /* Perform multiply */
  oski_ScaleVecView (y_view, beta);
  oski_ScaleVecView (z_view, zeta);

  if (func1)
    (*func1) (&(A->A1), alpha, x_view, y_view, opA, omega, w_view, z_view);
  if (func2)
    (*func2) (&(A->A2), alpha, x_view, y_view, opA, omega, w_view, z_view);
  if (A->p_A3 != NULL)
    {
      int err = 0;

      oski_index_t row_start3 = A->A1.num_block_rows * A->A1.r
	+ A->A2.num_block_rows * A->A2.r;
      oski_index_t num_rows =
	A->p_A3->num_block_rows * A->p_A3->row_block_size
	+ A->p_A3->num_rows_leftover;

      oski_vecstruct_t y3, w3, z3;

      MBCSR_InitSubVecView (y_view, row_start3, num_rows, &y3);
      if (opA == OP_NORMAL || opA == OP_CONJ)
	{
	  MBCSR_InitSubVecView (w_view, 0, w_view->num_rows, &w3);
	  MBCSR_InitSubVecView (z_view, row_start3, num_rows, &z3);
	}
      else
	{			/* opA == OP_TRANS || opA == OP_CONJ_TRANS */
	  MBCSR_InitSubVecView (w_view, row_start3, num_rows, &w3);
	  MBCSR_InitSubVecView (z_view, 0, z_view->num_rows, &z3);
	}

      err = ERR_NOT_IMPLEMENTED;
      if (func3 != NULL)
	{
	  err = (*func3) (A->p_A3, NULL, alpha, x_view, TVAL_ONE, y_view,
			  opA, omega, w_view, TVAL_ONE, z_view);
	}
      if (err)
	{			/* call alternate */
			/** \todo What to do here if either of these calls fails? */
	  assert (func3_alt != NULL);
	  (*func3_alt) (A->p_A3, NULL, OP_NORMAL,
			alpha, x_view, TVAL_ONE, &y3);
	  (*func3_alt) (A->p_A3, NULL, opA, omega, &w3, TVAL_ONE, &z3);
	}
    }

  return 0;
}

/* eof */
