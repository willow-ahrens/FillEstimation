/**
 * \file include/poski/poski_loadmat.h
 * \brief
 *
 */

#ifndef _POSKI_LOADMAT_H
#define _POSKI_LOADMAT_H

#include "poski_loadmattype.h"
#include "poski_matrixtype.h"

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

/* User Interface */
// TODO: add poski_LoadMatrix_to_CSR
poski_sparse_matrix_t *poski_LoadMatrixFile_to_CSR(char *matfile, poski_MatFormat_t MatfileFormat);

/* Test */
int test_main_poski_LoadMatrixFileHandler(char *matfile);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_LOADMAT_H
