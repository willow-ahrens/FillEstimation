#ifndef _POSKI_MATRIXGETSET_H
#define _POSKI_MATRIXGETSET_H

#include "poski_matrixtype.h"
typedef int (*oski_pOSKIGetMatReprEntry_funcpt) (void *mat,
                                             const oski_matcommon_t * props,
                                             oski_index_t row,
                                             oski_index_t col,
                                             oski_value_t * p_value);
#undef MOD_NAME
#define MOD_NAME poski_CSR
#define oski_pOSKIGetMatReprEntry \
                 MANGLE_MOD_(oski_pOSKIGetMatReprEntry)
int
oski_pOSKIGetMatReprEntry (const void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t * p_value);

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

/* Internal */
int poski_SetMatEntry_run(poski_mat_t A_tunable, poski_int_t row, poski_int_t col, poski_value_t val);
poski_value_t poski_GetMatEntry_run(poski_mat_t A_tunable, poski_int_t row, poski_int_t col);
int poski_SetMatSubset_run (poski_mat_t A_tunable, const int numRows, const int numCols, const poski_value_t* vals, const int rowStride, const int colStride, const poski_int_t* rows, const poski_int_t* cols, poski_subsettype_t subsettype);
int poski_GetMatSubset_run (poski_mat_t A_tunable, const int numRows, const int numCols, poski_value_t* vals, const int rowStride, const int colStride, const poski_int_t* rows, const poski_int_t* cols, poski_subsettype_t subsettype);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_MATRIXGETSET_H
