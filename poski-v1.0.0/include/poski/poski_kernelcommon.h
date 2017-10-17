#ifndef _POSKI_KERNELCOMMON_H
#define _POSKI_KERNELCOMMON_H

#include "poski_threadtype.h"
#include "poski_matrixtype.h"
#include "poski_vectortype.h"
#include "poski_kerneltype.h"
#include "poski_matmulttype.h"

#define poski_SubMatMult oski_MatMult

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

/* Internal functions */
int poski_MatMult_run(poski_mat_t A, poski_operation_t op, poski_value_t alpha, poski_vec_t x, poski_value_t beta, poski_vec_t y);
int poski_Check_PartitionVecMatMult(poski_mat_t A, poski_operation_t op, poski_kernel_t ktype, poski_vec_t x, poski_vec_t y);

int poski_reductionMatMult(poski_vec_t vec, poski_mat_t A, poski_value_t beta, poski_operation_t op);
void reductionMatMult(poski_int_t tid, poski_vec_t vec, poski_mat_t A, poski_value_t beta, poski_operation_t op, poski_int_t nthreads);

char *poski_findkerneltype(poski_kernel_t ktype);
char *poski_findoperation(poski_operation_t op);

int poski_Check_kerneltype(poski_kernel_t ktype);
int poski_Check_operations(poski_operation_t op);

int poski_report_MatMultCSR(poski_sparse_matrix_t *A, poski_value_t alpha, poski_value_t *x, poski_value_t beta, poski_value_t *y);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_KERNELCOMMON_H
