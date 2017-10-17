#ifndef _POSKI_PARTITIONCOMMON_H
#define _POSKI_PARTITIONCOMMON_H

#include "poski_common.h"
#include "poski_partitiontype.h"
#include "poski_threadtype.h"
#include "poski_matrixtype.h"
#include "poski_vectortype.h"
#include "poski_kerneltype.h"

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

/* Internal */
int poski_Init_partitionargs(poski_partitionarg_t *dst, poski_partitionarg_t *src, poski_int_t nthreads);
int poski_Initpartitionmat(poski_partitionmat_t *partitionmat);

int poski_Adjust_args(poski_partitiontype_t ptype, poski_int_t *npartitions, poski_int_t *nthreads, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz);
poski_subvec_t *poski_PartitionVec(poski_value_t *vec, poski_index_t length, poski_int_t inc, poski_partitionvec_t *partitionvec);

poski_partitionmat_funcpt poski_find_partitionmat_funcpt(poski_threadtype_t ttype, poski_partitiontype_t ptype);
poski_partitionvec_funcpt poski_find_partitionvec_funcpt(poski_threadtype_t ttype, poski_partitiontype_t ptype, poski_kernel_t kernel, poski_operation_t op, poski_vecprop_t vecprop);

char *poski_findpartitiontype(poski_partitiontype_t ptype);

int poski_Check_partitionargs(poski_partitionarg_t *partitionargs);
int poski_Check_numpartitions(poski_int_t npartitions);
int poski_Check_partitioningmodeltype(poski_partitiontype_t ptype);
int poski_Check_partitionvec(poski_partitionvec_t *partitionvec);
int poski_Check_partitionedVec(poski_mat_t A, poski_vec_t vec, poski_kernel_t kernel, poski_operation_t op, poski_vecprop_t vecprop);

int poski_report_partitionmodel(poski_partitionarg_t *partitionargs, poski_mat_t A);
void poski_report_partitioned(poski_partitionmat_t *partitionmat);
void poski_report_allpartitioned(poski_partitionmat_t *partitionmat);
void poski_report_partitionvec(poski_partitionvec_t *partitionvec);
int poski_report_partitionmat_va(poski_partitionmat_t *partitionmat, poski_int_t pid);
int poski_report_partitionmat(poski_submat_t *submat, poski_int_t npartitions);

/// OneD
poski_sparse_matrix_t *poski_CreateSubMatCSR_OneD(poski_partitionmat_t *partitionmat, poski_index_t *Aptr, poski_index_t *Aind, poski_value_t *Aval, poski_index_t Anrows, poski_index_t Ancols, poski_index_t Annz, poski_copymode_t mode);

poski_partitionmat_t *poski_PartitionMatCSR_OneD(poski_int_t npartitions, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz, poski_index_t *Aptr);

int poski_Set_pthreadarg(poski_thread_t *thread, int tid, int num_partitions, poski_copymode_t mode, int k, poski_inmatprop_t *in_props, poski_mat_t A, poski_partitionmat_t *partitionmat, poski_submat_t *submat);

void *PartitionMat_PthreadOneD(void *poski_thread);
void PartitionMat_OpenmpOneD(void *poski_thread);

poski_submat_t *poski_PartitionMat_PthreadOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props);
poski_submat_t *poski_PartitionMat_OpenmpOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props);
poski_submat_t *poski_PartitionMat_ThreadpoolOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props);

poski_subvec_t *poski_PartitionVec_PthreadOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_PthreadOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_OpenmpOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_OpenmpOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_ThreadpoolOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_ThreadpoolOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );

poski_subvec_t *poski_PartitionVec_PthreadOneDMatMultTransInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_PthreadOneDMatMultTransOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_OpenmpOneDMatMultTransInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_OpenmpOneDMatMultTransOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_ThreadpoolOneDMatMultTransInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_ThreadpoolOneDMatMultTransOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );

/// SemiOneD
poski_sparse_matrix_t *poski_CreateSubMatCSR_SemiOneD(poski_partitionmat_t *partitionmat, poski_index_t *Aptr, poski_index_t *Aind, poski_value_t *Aval, poski_index_t Anrows, poski_index_t Ancols, poski_index_t Annz, poski_copymode_t mode);

poski_partitionmat_t *poski_PartitionMatCSR_SemiOneD(poski_int_t npartitions, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz, poski_index_t *Aptr);

void *PartitionMat_PthreadSemiOneD(void *poski_thread);
void PartitionMat_OpenmpSemiOneD(void *poski_thread);

poski_submat_t *poski_PartitionMat_PthreadSemiOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props);
poski_submat_t *poski_PartitionMat_OpenmpSemiOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props);
poski_submat_t *poski_PartitionMat_ThreadpoolSemiOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props);

poski_subvec_t *poski_PartitionVec_PthreadSemiOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_PthreadSemiOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_OpenmpSemiOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_OpenmpSemiOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_ThreadpoolSemiOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_ThreadpoolSemiOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );

poski_subvec_t *poski_PartitionVec_PthreadSemiOneDMatMultTransInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_PthreadSemiOneDMatMultTransOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_OpenmpSemiOneDMatMultTransInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_OpenmpSemiOneDMatMultTransOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_ThreadpoolSemiOneDMatMultTransInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );
poski_subvec_t *poski_PartitionVec_ThreadpoolSemiOneDMatMultTransOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A );

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_PARTITIONCOMMON_H
