#ifndef _POSKI_MATRIXCOMMON_H
#define _POSKI_MATRIXCOMMON_H

#include "poski_matrixtype.h"

#define poski_CreateSubMatCSR oski_CreateMatCSR_arr

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

/* Internal */
int poski_Init_GlobalMatrix (poski_mat_t A, poski_index_t *Aptr, poski_index_t *Aind, poski_value_t *Aval, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz);
poski_mat_t poski_InitMat(poski_threadarg_t *threadargs, poski_partitionarg_t *partitionargs, poski_index_t *ptr, poski_index_t *ind, poski_value_t *val, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz, poski_copymode_t mode);
int poski_Init_LocalSubMat(poski_submat_t *submatrix);

poski_inmatprop_t *poski_Collect_valist(int k, va_list ap);

int poski_Check_matCSR (poski_index_t *ptr, poski_index_t *ind, poski_value_t *val, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz);
int poski_Check_matcopymode(poski_copymode_t mode);
int poski_Check_CreateMatCSRInput(poski_threadarg_t *threadargs, poski_index_t *ptr, poski_index_t *ind, poski_value_t *val, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz, poski_copymode_t mode);
int poski_Check_Matrix(poski_mat_t A_tunable);
int poski_Check_Matrix_location(poski_mat_t A, poski_int_t row, poski_int_t col);

int poski_report_sparse_CSR_va(poski_index_t *Aptr, poski_index_t *Aind, poski_value_t *Aval, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz);
int poski_report_sparse_CSR(poski_sparse_matrix_t *A);
int poski_report_submat(poski_submat_t *submat, poski_int_t npartitions);
int poski_report_tunable_matrix_va(poski_mat_t A);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_MATRIXCOMMON_H
