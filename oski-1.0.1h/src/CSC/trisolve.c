/**
 *  \file src/CSC/trisolve.c
 *  \brief CSC sparse triangular solve implementation.
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
oski_MatReprTrisolve (const void *pT, const oski_matcommon_t * props,
		      oski_matop_t opT, oski_value_t alpha,
		      oski_vecview_t x_view)
{
  const oski_matCSC_t *T = (const oski_matCSC_t *) pT;

  const oski_matCSR_t *TT;
  oski_matcommon_t props_TT;

  oski_MatReprTrisolve_funcpt func_CSR;
  int err;

  func_CSR = OSKI_MATTYPE_METHOD ("CSR", MatReprTrisolve);
  if (func_CSR == NULL)
    {
      OSKI_ERR_MATTYPE_MEMBER (oski_MatReprTrisolve, "CSR", MatReprTrisolve);
      return ERR_NOT_IMPLEMENTED;
    }

  /* Extract transposed representation */
  TT = T->mat_trans;
  oski_CopyMem (&props_TT, props, oski_matcommon_t, 1);
  oski_TransposeProps (&props_TT);

  switch (opT)
    {
    case OP_NORMAL:
      err = (*func_CSR) (TT, &props_TT, OP_TRANS, alpha, x_view);
      break;

    case OP_TRANS:
      err = (*func_CSR) (TT, &props_TT, OP_NORMAL, alpha, x_view);
      break;

    case OP_CONJ_TRANS:
      err = (*func_CSR) (TT, &props_TT, OP_CONJ, alpha, x_view);
      break;

    default:
      assert (0);
      return ERR_NOT_IMPLEMENTED;
    }

  return err;
}

/* eof */
