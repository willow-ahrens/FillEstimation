/**
 *  \file src/BCSR/ata.c
 *  \brief BCSR implementation of the A^T*A*x kernel.
 *  \ingroup MATTYPE_BCSR
 */

#include <oski/config.h>
#include <oski/common.h>
#include <oski/blas.h>
#include <oski/blas_names.h>
#include <oski/modloader.h>
#include <oski/matrix.h>
#include <oski/BCSR/format.h>
#include <oski/BCSR/module.h>

#include <oski/matmodexport.h>

/**
 *  \brief
 *
 *  \pre All arguments have legal values and meet basic dimensionality
 *  compatibility requirements.
 *
 *  NOTE: The BCSR implementation does not contain cache-optimized
 *  implementations of \f$AA^Tx\f$ or \f$AA^Hx\f$ kernels, and
 *  returns ERR_NOT_IMPLEMENTED in these cases, and the caller should
 *  invoke the fall-back routine, #MatTransMatMultDefault(), if
 *  necessary.
 */
int
oski_MatReprTransMatReprMult (const void *A,
			      const oski_matcommon_t * props, oski_ataop_t op,
			      oski_value_t alpha, const oski_vecview_t x_view,
			      oski_value_t beta, oski_vecview_t y_view,
			      oski_vecview_t t_view)
{
  const char *routine = "";
  oski_BCSR_MatTransMatMult_funcpt func;
  oski_BCSR_MatTransMatMult_funcpt func_leftover;
  int err;
  const oski_matBCSR_t *B = (const oski_matBCSR_t *) A;

  switch (op)
    {
    case OP_AT_A:
      routine = "MatReprTransMatReprMult";
      break;
    case OP_AH_A:
      routine = "MatReprHermMatReprMult";
      break;
    default:
      return ERR_NOT_IMPLEMENTED;
    }

  func = (oski_BCSR_MatTransMatMult_funcpt) BCSR_GetKernel (B, routine);
  if (func == NULL)
    return ERR_NOT_IMPLEMENTED;

  func_leftover = NULL;
  if (B->num_rows_leftover && B->leftover != NULL)
    {
      func_leftover =
	(oski_BCSR_MatTransMatMult_funcpt) BCSR_GetKernel (B->leftover,
							   routine);
      if (func_leftover == NULL)
	return ERR_NOT_IMPLEMENTED;
    }

  /* y <-- beta * y */
  oski_ScaleVecView (y_view, beta);
  if (IS_VAL_ZERO (alpha))
    return 0;			/* quick return */

  /* Perform 'core' multiply */
  err = (*func) (B, alpha, x_view, y_view, t_view);

  /* Multiply by any leftover rows. */
  if (!err && func_leftover != NULL)
    {
      const oski_matBCSR_t *L = B->leftover;
      if (t_view != INVALID_VEC)
	{
	  oski_vecstruct_t t_left;
	  oski_CopyMem (&t_left, t_view, oski_vecstruct_t, 1);
	  t_left.num_rows = B->num_rows_leftover;
	  t_left.val += t_left.rowinc * B->num_block_rows * B->row_block_size;
	  err = (*func_leftover) (L, alpha, x_view, y_view, &t_left);
	}
      else			/* t_view == INVALID_VEC */
	err = (*func_leftover) (L, alpha, x_view, y_view, INVALID_VEC);
    }

  return err;
}

/* eof */
