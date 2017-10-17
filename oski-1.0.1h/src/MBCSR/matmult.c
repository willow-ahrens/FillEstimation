/**
 *  \file src/MBCSR/matmult.c
 *  \brief MBCSR implementation of SpMV.
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
SymmMatMult (const oski_matMBCSR_t * A, int is_herm,
	     oski_matop_t opA, oski_value_t alpha,
	     const oski_vecview_t x_view, oski_value_t beta,
	     oski_vecview_t y_view)
{
  MBCSR_SymmSubmatReprMult_funcpt func1 = NULL;
  MBCSR_SymmSubmatReprMult_funcpt func2 = NULL;
  int is_alpha_zero = IS_VAL_ZERO (alpha);

  assert (A != NULL);
  if (A->p_A3 != NULL)
    return ERR_BAD_MAT;

  if (!is_alpha_zero)
    {
      /* Preload core-kernel implementations */
      if (A->A1.num_block_rows > 0)
	{
	  func1 =
	    (MBCSR_SymmSubmatReprMult_funcpt) MBCSR_GetSubmatKernel (&(A->A1),
								     "SymmSubmatReprMult");
	  if (func1 == NULL)
	    return ERR_NOT_IMPLEMENTED;
	}
      if (A->A2.num_block_rows > 0)
	{
	  func2 =
	    (MBCSR_SymmSubmatReprMult_funcpt) MBCSR_GetSubmatKernel (&(A->A2),
								     "SymmSubmatReprMult");
	  if (func2 == NULL)
	    return ERR_NOT_IMPLEMENTED;
	}
    }

  oski_ScaleVecView (y_view, beta);
  if (is_alpha_zero)
    return 0;			/* quick return */

  if (func1 != NULL)
    (*func1) (&(A->A1), is_herm, opA, alpha, x_view, y_view);
  if (func2 != NULL)
    (*func2) (&(A->A2), is_herm, opA, alpha, x_view, y_view);

  return 0;
}

/**
 *  \brief
 *
 *  \pre All arguments have legal values and meet basic dimensionality
 *  compatibility requirements.
 *
 *  \note 'props' is not required unless the matrix is stored using
 *  symmetric/Hermitian half-storage. Otherwise, the caller may set
 *  props to NULL.
 */
int
oski_MatReprMult (const void *pA, const oski_matcommon_t * props,
		  oski_matop_t opA,
		  oski_value_t alpha, const oski_vecview_t x_view,
		  oski_value_t beta, oski_vecview_t y_view)
{
  const oski_matMBCSR_t *A = (const oski_matMBCSR_t *) pA;

  MBCSR_SubmatReprMult_funcpt func_Mult1 = NULL;
  MBCSR_SubmatReprMult_funcpt func_Mult2 = NULL;
  oski_MatReprMult_funcpt func_Mult3 = NULL;

  int err = 0;

  assert (A != NULL);

  /* Special case: symmetric/Hermitian */
  if (props != NULL
      && (props->pattern.is_symm || props->pattern.is_herm)
      && A->stored.is_upper != A->stored.is_lower)
    return SymmMatMult (A, props->pattern.is_herm,
			opA, alpha, x_view, beta, y_view);

  if (!IS_VAL_ZERO (alpha))
    {
      /* Preload core-kernel implementations */
      if (A->A1.num_block_rows > 0)
	{
	  func_Mult1 =
	    (MBCSR_SubmatReprMult_funcpt) MBCSR_GetSubmatKernel (&(A->A1),
								 "SubmatReprMult");
	  if (func_Mult1 == NULL)
	    return ERR_NOT_IMPLEMENTED;
	}
      if (A->A2.num_block_rows > 0)
	{
	  func_Mult2 =
	    (MBCSR_SubmatReprMult_funcpt) MBCSR_GetSubmatKernel (&(A->A2),
								 "SubmatReprMult");
	  if (func_Mult2 == NULL)
	    return ERR_NOT_IMPLEMENTED;
	}
      if (A->p_A3 != NULL)
	{
	  func_Mult3 = OSKI_MATTYPE_METHOD ("BCSR", MatReprMult);
	  if (func_Mult3 == NULL)
	    return ERR_NOT_IMPLEMENTED;
	}
    }

  /* y <-- beta * y */
  oski_ScaleVecView (y_view, beta);
  if (IS_VAL_ZERO (alpha))
    return 0;			/* quick return */

  if (func_Mult1 != NULL)
    (*func_Mult1) (&(A->A1), opA, alpha, x_view, y_view);
  if (func_Mult2 != NULL)
    (*func_Mult2) (&(A->A2), opA, alpha, x_view, y_view);
  if (func_Mult3 != NULL)
    {
      oski_index_t row_start = A->A1.num_block_rows * A->A1.r
	+ A->A2.num_block_rows * A->A2.r;
      oski_vecstruct_t x3, y3;
      if (opA == OP_NORMAL || opA == OP_CONJ)
	{
	  oski_index_t num_rows = y_view->num_rows - row_start;
	  MBCSR_InitSubVecView (y_view, row_start, num_rows, &y3);
	  MBCSR_InitSubVecView (x_view, 0, x_view->num_rows, &x3);
	}
      else
	{			/* opA == OP_TRANS || opA == OP_CONJ_TRANS */
	  oski_index_t num_rows = x_view->num_rows - row_start;
	  MBCSR_InitSubVecView (x_view, row_start, num_rows, &x3);
	  MBCSR_InitSubVecView (y_view, 0, y_view->num_rows, &y3);
	}
      err = (*func_Mult3) (A->p_A3, props, opA, alpha, &x3, TVAL_ONE, &y3);
    }

  return err;
}

/* eof */
