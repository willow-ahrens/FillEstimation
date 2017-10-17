/**
 *  \file src/CSC/getset.c
 *  \ingroup MATTYPE_CSC
 *  \brief CSC get/set value routines.
 */

#include <assert.h>
#include <stdio.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/getset.h>
#include <oski/CSR/format.h>
#include <oski/CSR/module.h>
#include <oski/CSC/format.h>
#include <oski/CSC/module.h>

int
oski_GetMatReprEntry (const void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t * p_value)
{
  /* matrix */
  const oski_matCSC_t *A = (const oski_matCSC_t *) mat;
  const oski_matCSR_t *AT = A->mat_trans;
  oski_matcommon_t props_AT;

  /* methods */
  oski_GetMatReprEntry_funcpt func_GetCSR;

  oski_CopyMem (&props_AT, props, oski_matcommon_t, 1);
  oski_TransposeProps (&props_AT);

  func_GetCSR = OSKI_MATTYPE_METHOD ("CSR", GetMatReprEntry);
  if (func_GetCSR == NULL)
    {
      OSKI_ERR_MATTYPE_MEMBER (oski_GetMatReprEntry, "CSR", GetMatReprEntry);
      return ERR_NOT_IMPLEMENTED;
    }

  return (*func_GetCSR) (AT, &props_AT, col, row, p_value);
}

int
oski_SetMatReprEntry (void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t new_val)
{
  /* matrix */
  oski_matCSC_t *A = (oski_matCSC_t *) mat;
  oski_matCSR_t *AT = A->mat_trans;
  oski_matcommon_t props_AT;

  /* methods */
  oski_SetMatReprEntry_funcpt func_SetCSR;

  oski_CopyMem (&props_AT, props, oski_matcommon_t, 1);
  oski_TransposeProps (&props_AT);

  func_SetCSR = OSKI_MATTYPE_METHOD ("CSR", SetMatReprEntry);
  if (func_SetCSR == NULL)
    {
      OSKI_ERR_MATTYPE_MEMBER (oski_SetMatReprEntry, "CSR", SetMatReprEntry);
      return ERR_NOT_IMPLEMENTED;
    }

  return (*func_SetCSR) (AT, &props_AT, col, row, new_val);
}

/* eof */
