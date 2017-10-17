/**
 * \file include/poski/poski_matrix.h
 * \brief
 *
 */

#ifndef _POSKI_MATRIX_H
#define _POSKI_MATRIX_H

#include "poski_matrixtype.h"
#include "poski_kerneltype.h"
#include "poski_threadtype.h"
#include "poski_partitiontype.h"

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

/* User Interface */
poski_mat_t poski_CreateMatCSR(poski_index_t *ptr, poski_index_t *ind, poski_value_t *val, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz, poski_copymode_t mode, poski_threadarg_t *threadargs, poski_partitionarg_t *partitionargs, int k, ...);
poski_mat_t poski_CreateMatCSRFile(poski_sparse_matrix_t *SpA, poski_copymode_t mode, poski_threadarg_t *threadargs, poski_partitionarg_t *partitionargs, int k, ...);

int poski_DestroyMat(poski_mat_t A_tunable);

int poski_SetMatEntry(poski_mat_t A_tunable, poski_int_t row, poski_int_t col, poski_value_t val);	
poski_value_t poski_GetMatEntry(poski_mat_t A_tunable, poski_int_t row, poski_int_t col);

int poski_SetMatSubset (poski_mat_t A_tunable, const int numRows, const int numCols, const int rowStride, const int colStride, const poski_int_t* rows, const poski_int_t* cols, const poski_value_t* vals, poski_subsettype_t subsettype);
int poski_GetMatSubset (poski_mat_t A_tunable, const int numRows, const int numCols, const int rowStride, const int colStride, const poski_int_t* rows, const poski_int_t* cols, poski_value_t* vals, poski_subsettype_t subsettype);

int poski_report_tunable_matrix(poski_mat_t mat);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_MATRIX_H
