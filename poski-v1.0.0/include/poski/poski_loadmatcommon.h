#ifndef _POSKI_LOADMATCOMMON_H
#define _POSKI_LOADMATCOMMON_H

#include "poski_loadmattype.h"
#include "poski_matrixtype.h"

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

/* Internal */
void poski_InitSparseMatrix(poski_sparse_matrix_t *SpA);

/* HB format */
poski_sparse_matrix_t *poski_LoadMatrix_HBF_to_CSR(char *fname, poski_index_t MakeUnsymmetric);
int test_main_poski_LoadMatrix_HBF_to_CSR(char *fname);

/* MM format */
poski_sparse_matrix_t *poski_LoadMatrix_MM_to_CSR(char *fname, poski_index_t MakeUnsymmetric);
int test_main_poski_LoadMatrix_MM_to_CSR(char *fname);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_LOADMATCOMMON_H
