/**
 *  \file src/MBCSR/ata.c
 *  \brief MBCSR implementation of the sparse \f$A^TA\cdot x\f$ kernel.
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

/**
 *  \brief Returns a non-zero value if and only if all of the necessary
 *  dynamically loadable subroutines exist to execute a cache-optimized
 *  A'*A*x in MBCSR format.
 *
 *  \note 'props' is only required if the matrix A is stored using
 *  symmetric or Hermitian half-storage; otherwise, the caller may
 *  set props to NULL.
 */
static int
CanBeExecuted (const oski_matMBCSR_t * A, const oski_matcommon_t * props,
	       oski_ataop_t opA,
	       MBCSR_SubmatReprTransSubmatReprMult_funcpt * p_func_ATA_1,
	       MBCSR_SubmatReprTransSubmatReprMult_funcpt * p_func_ATA_2,
	       oski_MatReprTransMatReprMult_funcpt * p_func_ATA_3)
{
  const char *kernel_name = (opA == OP_AT_A)
    ? "SubmatRperTransSubmatReprMult" : "SubmatRperHermSubmatReprMult";

  /* Bad arguments */
  if (A == NULL)
    return 0;

  /* Cache-optimized A*A' is not supported for MBCSR storage */
  if (opA == OP_A_AT || opA == OP_A_AH)
    return 0;

  /* Explicitly disabled */
  if (!A->enabled.MatTransMatMult)
    return 0;

  /* Symmetric storage is not supported */
  if (props != NULL && (props->pattern.is_symm || props->pattern.is_herm)
      && (A->stored.is_upper != A->stored.is_lower))
    return 0;

  /* Must have core kernel */
  if (A->A1.num_block_rows > 0 && p_func_ATA_1 != NULL)
    {
      *p_func_ATA_1 = (MBCSR_SubmatReprTransSubmatReprMult_funcpt)
	MBCSR_GetSubmatKernel (&(A->A1), kernel_name);
      if (*p_func_ATA_1 == NULL)
	return 0;
    }
  if (A->A2.num_block_rows > 0 && p_func_ATA_2 != NULL)
    {
      *p_func_ATA_2 = (MBCSR_SubmatReprTransSubmatReprMult_funcpt)
	MBCSR_GetSubmatKernel (&(A->A2), kernel_name);
      if (*p_func_ATA_2 == NULL)
	return 0;
    }
  if (A->p_A3 != NULL && p_func_ATA_3 != NULL)
    {
      *p_func_ATA_3 = OSKI_MATTYPE_METHOD ("BCSR", MatReprTransMatReprMult);
      if (*p_func_ATA_3 == NULL)
	return 0;
    }

  /* Meets all known preconditions... */
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
oski_MatReprTransMatReprMult (const void *pA, const oski_matcommon_t * props,
			      oski_ataop_t opA, oski_value_t alpha,
			      const oski_vecview_t x_view, oski_value_t beta,
			      oski_vecview_t y_view, oski_vecview_t t_view)
{
  const oski_matMBCSR_t *A = (const oski_matMBCSR_t *) pA;
  int alpha_is_zero = IS_VAL_ZERO (alpha);
  const oski_value_t ONE = MAKE_VAL_COMPLEX (1.0, 0.0);

  MBCSR_SubmatReprTransSubmatReprMult_funcpt func1 = NULL;
  MBCSR_SubmatReprTransSubmatReprMult_funcpt func2 = NULL;
  oski_MatReprTransMatReprMult_funcpt func3 = NULL;

  if (!alpha_is_zero
      && !CanBeExecuted (A, props, opA, &func1, &func2, &func3))
    return ERR_NOT_IMPLEMENTED;

  /* Perform multiply */
  oski_ScaleVecView (y_view, beta);
  if (alpha_is_zero)
    return 0;

  if (func1)
    (*func1) (&(A->A1), alpha, x_view, y_view, t_view);
  if (func2)
    (*func2) (&(A->A2), alpha, x_view, y_view, t_view);
  if (A->p_A3 != NULL)
    {
      oski_vecstruct_t t3;
      oski_index_t row_start3 = A->A1.num_block_rows * A->A1.r
	+ A->A2.num_block_rows * A->A2.r;
      oski_index_t num_rows =
	A->p_A3->num_block_rows * A->p_A3->row_block_size
	+ A->p_A3->num_rows_leftover;
      MBCSR_InitSubVecView (t_view, row_start3, num_rows, &t3);
      (*func3) (A->p_A3, NULL, opA, alpha, x_view, ONE, y_view, &t3);
    }

  return 0;
}

/* eof */
