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
 *  dynamically loadable subroutines exist to execute sparse triangular
 *  solve in MBCSR format.
 *
 *  \note 'props' is only required if the matrix A is stored using
 *  symmetric or Hermitian half-storage; otherwise, the caller may
 *  set props to NULL.
 */
static int
CanBeExecuted (const oski_matMBCSR_t * T, const oski_matcommon_t * props,
	       MBCSR_SubmatReprTrisolve_funcpt * p_func_T1,
	       MBCSR_SubmatReprTrisolve_funcpt * p_func_T2)
{
  const char *kernel_name = "SubmatReprTrisolve";

  /* Bad matrix */
  if (T == NULL)
    return 0;
  if (T->stored.is_lower == T->stored.is_upper)
    return 0;

  /* Symmetric storage is not supported */
  if (props != NULL && (props->pattern.is_symm || props->pattern.is_herm)
      && (T->stored.is_upper != T->stored.is_lower))
    return 0;

  /* Must have core kernel */
  if (T->A1.num_block_rows > 0 && p_func_T1 != NULL)
    {
      *p_func_T1 =
	(MBCSR_SubmatReprTrisolve_funcpt) MBCSR_GetSubmatKernel (&(T->A1),
								 kernel_name);
      if (*p_func_T1 == NULL)
	return 0;
    }
  if (T->A2.num_block_rows > 0 && p_func_T2 != NULL)
    {
      *p_func_T2 =
	(MBCSR_SubmatReprTrisolve_funcpt) MBCSR_GetSubmatKernel (&(T->A2),
								 kernel_name);
      if (*p_func_T2 == NULL)
	return 0;
    }
  if (T->p_A3 != NULL)
    return 0;

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
oski_MatReprTrisolve (const void *pT, const oski_matcommon_t * props,
		      oski_matop_t opT, oski_value_t alpha,
		      const oski_vecview_t x_view)
{
  /* Perform multiply */
  if (IS_VAL_ZERO (alpha))
    oski_ZeroVecView (x_view);
  else
    {
      const oski_matMBCSR_t *T = (const oski_matMBCSR_t *) pT;
      MBCSR_SubmatReprTrisolve_funcpt func1 = NULL;
      MBCSR_SubmatReprTrisolve_funcpt func2 = NULL;
      int do_order_A1_A2 = 0;

      if (!CanBeExecuted (T, props, &func1, &func2))
	return ERR_NOT_IMPLEMENTED;

      oski_ScaleVecView (x_view, alpha);

      do_order_A1_A2 =
	(T->stored.is_upper && (opT == OP_TRANS || opT == OP_CONJ_TRANS))
	|| (T->stored.is_lower && (opT == OP_NORMAL || opT == OP_CONJ));

      if (do_order_A1_A2)
	{
	  if (func1)
	    (*func1) (&(T->A1), T->stored.is_lower, opT, TVAL_ONE, x_view);
	  if (func2)
	    (*func2) (&(T->A2), T->stored.is_lower, opT, TVAL_ONE, x_view);
	}
      else
	{			/* Do A2 first, then A1 */
	  if (func2)
	    (*func2) (&(T->A2), T->stored.is_lower, opT, TVAL_ONE, x_view);
	  if (func1)
	    (*func1) (&(T->A1), T->stored.is_lower, opT, TVAL_ONE, x_view);
	}
    }
  return 0;
}

/* eof */
