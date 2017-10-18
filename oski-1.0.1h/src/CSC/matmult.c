/**
 *  \file src/CSC/matmult.c
 *  \brief CSC SpMV implementation.
 *  \ingroup MATTYPE_CSC
 *
 *  These kernels simply call the appropriate CSR kernel,
 *  switching the transpose operation as needed.
 */

#include <oski/config.h>
#include <oski/common.h>
#include <oski/blas.h>
#include <oski/matrix.h>
#include <oski/CSC/format.h>
#include <oski/CSC/module.h>

int
oski_MatReprMult (const void *pA, const oski_matcommon_t * props,
		  oski_matop_t opA,
		  oski_value_t alpha, const oski_vecview_t x_view,
		  oski_value_t beta, oski_vecview_t y_view)
{
  const oski_matCSC_t *A = (const oski_matCSC_t *) pA;

  const oski_matCSR_t *AT;
  oski_matcommon_t props_CSR;

  oski_MatReprMult_funcpt func_CSRMult;
  int err;

  func_CSRMult = OSKI_MATTYPE_METHOD ("CSR", MatReprMult);
  if (func_CSRMult == NULL)
    {
      OSKI_ERR_MATTYPE_MEMBER (oski_MatReprMult, "CSR", MatReprMult);
      return ERR_NOT_IMPLEMENTED;
    }

  AT = A->mat_trans;
  oski_CopyMem (&props_CSR, props, oski_matcommon_t, 1);
  oski_TransposeProps (&props_CSR);
	/** \todo Delete the following assignment statements,
	 *  made obsolete by above call to \ref oski_TransposeProps(). */
  /*
     props_CSR.num_rows = props->num_cols;
     props_CSR.num_cols = props->num_rows;
   */

  switch (opA)
    {
    case OP_NORMAL:
      err = (*func_CSRMult) (AT, &props_CSR, OP_TRANS,
			     alpha, x_view, beta, y_view);
      break;

    case OP_TRANS:
      err = (*func_CSRMult) (AT, &props_CSR, OP_NORMAL,
			     alpha, x_view, beta, y_view);
      break;

    case OP_CONJ_TRANS:
      err = (*func_CSRMult) (AT, &props_CSR, OP_CONJ,
			     alpha, x_view, beta, y_view);
      break;

    default:
      assert (0);
      return ERR_NOT_IMPLEMENTED;
    }

  return err;
}

/* eof */
