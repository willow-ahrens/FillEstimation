/**
 * \file include/poski/poski_partition.h
 * \brief
 *
 */

#ifndef _POSKI_PARTITION_H
#define _POSKI_PARTITION_H

#include "poski_partitiontype.h"
#include "poski_matrixtype.h"
#include "poski_vectortype.h"
#include "poski_kerneltype.h"

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

/* User Interface */
poski_partitionarg_t *poski_partitionMatHints(poski_partitiontype_t ptype, poski_int_t npartitions, poski_kernel_t kernel, poski_operation_t op);
poski_partitionvec_t *poski_PartitionVecHints(poski_mat_t A_tunable, poski_kernel_t kernel, poski_operation_t op, poski_vecprop_t vecprop);

int poski_DestroyPartitionMat(poski_partitionarg_t *args);
int poski_DestroyPartitionVec(poski_partitionvec_t *args);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_PARTITION_H
