/**
 * @file src/Partition/poski_PartitionHandler.c
 * @brief Handle partitioning model routines.
 *
 * This module implements the routines to handle the partitioning model.
 *
 * @attention Currently, only support OneD, and SemiOneD partitioning.
 *
 * Also, refer
 * @ref poski_Partition_common.c, 
 * @ref poski_Partition_OneD.c, 
 * @ref poski_Partition_SemiOneD.c
 */

#include <stdio.h>
#include <stdlib.h>

#include <poski/poski_partitioncommon.h>
#include <poski/poski_kernelcommon.h>
#include <poski/poski_matrixcommon.h>
#include <poski/poski_vectorcommon.h>
#include <poski/poski_malloc.h>
#include <poski/poski_print.h>

//-----------------------------------------------------------------------------------------
// User interface
/**
 * @brief Create a partition-matrix object by user's partitioning hint.
 * 
 * Note: Currently only take k=0 or 2 for the properties of kernel and operation {2, kernel, op}.
 * @param[in] ptype The partitioning model type.
 * @param[in] npartitions Number of partitions.
 * @param[in] kernel Specified kernel.
 * @param[in] op Specified transpose operation. 
 * 
 * @return A valid partition object if success, or an error message otherwise.
 */
poski_partitionarg_t *poski_partitionMatHints(poski_partitiontype_t ptype, poski_int_t npartitions, poski_kernel_t kernel, poski_operation_t op)
//poski_partitionarg_t *poski_partitionMatHints(poski_partitiontype_t ptype, poski_int_t npartitions, int k, ...)
{
	poski_printMessage(1, "Creating a valid partition-matrix object...\n");

	// (1) error-check for the input parameters.
	poski_Check_partitioningmodeltype(ptype);	/* @ Check partitioning model type */
	poski_Check_numpartitions(npartitions);		/* @ Check number of partitions */
	
/*	poski_kernel_t kernel;
	poski_operation_t op;
	if (k==0)
	{
		kernel = KERNEL_MatMult;
		op = OP_NORMAL;
	}
	else if (k==2)
	{
		va_list ap;
		va_start(ap, k);
		kernel = va_arg(ap,poski_kernel_t);
		op = va_arg(ap, poski_operation_t);	
		va_end(ap);
	}
	else 
	{
		poski_error("poski_partitioinMatHints", "Invalid number of input parameters (currently only take k=0 or 2)");
		return NULL;
	}
*/	
	if (poski_Check_kerneltype(kernel))	/* @ Check kernel type */
	{
		poski_error("poski_partitionMatHints", "Invalid Kernel");
		return NULL;
	}
	if (poski_Check_operations(op))		/* @ Check operations */
	{
		poski_error("poski_partitionMatHints", "Invalid kernel operation");
		return NULL;
	}

	// (2) create a new partition-matrix object.
	poski_partitionarg_t *partitionargs = poski_malloc(poski_partitionarg_t, 1, 0);
	if (partitionargs == NULL) 
	{
		poski_error("poski_PartitionMatHints", "Failed to create a vailde partition-matrix object");
		return NULL;
	}


	// (3) set user's hints into the partition-matrix object.
	partitionargs->ptype = ptype;
	partitionargs->npartitions = npartitions;
	partitionargs->kernel = kernel;
	partitionargs->op = op;

	// (4) return a valid partition-matrix object. 
	return partitionargs;	// success!
}

/**
 * @brief Destroy a partition-matrix object.
 *
 * @param[in] args Partition-matrix object.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_DestroyPartitionMat( poski_partitionarg_t *args)
{
	poski_printMessage(1, "Destroying the partition-matrix object...\n");

	// (1) destroy the partition-matrix object.
	poski_free(args);
	
	return 0;	// success!
}

/**
 * @brief Create a partition-vector object by user's partitioning hint.
 * 
 * @param[in] A_tunable Tunable matrix object.
 * @param[in] kernel Specified kernel.
 * @param[in] op Specified transpose operation.
 * @param[in] vecprop Specified vector property.
 *
 * @return A valid partition object if success, or an error message otherwise.
 */
poski_partitionvec_t *poski_PartitionVecHints(poski_mat_t A_tunable, poski_kernel_t kernel, poski_operation_t op, poski_vecprop_t vecprop)
{
	poski_printMessage(1, "Creating a partition-vector object...\n");

	// (1) error-check for the input parameters.
	if ( A_tunable == NULL ) 
	{
		poski_error("poski_PartitionVecHints", "Invalid tunable matrix");
		return NULL;
	}
	if (poski_Check_kerneltype(kernel))
	{
		poski_error("poski_PartitionVecHints", "Invalid kernel");
		return NULL;
	}
	if (poski_Check_operations(op))
	{
		poski_error("poski_PartitionVecHints", "Invalid kernel operation");
		return NULL;
	}
	if (poski_Check_vecprop(vecprop))
	{
		poski_error("poski_PartitionVecHints", "Invalid vector property");
		return NULL;
	}

	// (2) create a new partition-vector object. 
	poski_partitionvec_t *partitionVec = poski_malloc(poski_partitionvec_t,1,0);
	if (partitionVec == NULL) 
	{
		poski_error("poski_PartitionVecHints", "Failed to create a vailde partition-vector object");
		return NULL;
	}

	// (3) set user's hints into the partition-vector object.
	partitionVec->A = A_tunable;
	partitionVec->kernel = kernel;
	partitionVec->op = op;
	partitionVec->vecprop = vecprop;	

	// (4) return a valid partition-vector object.
	return partitionVec;
}

/**
 * @brief Destroy a partition-vector object.
 *
 * @param[in] args Partition-vector object.
 *
 * @return 1 if success, or an error message otherwise.
 */
int poski_DestroyPartitionVec(poski_partitionvec_t *args)
{
	poski_printMessage(1, "Destroying the partition-vector object...\n");

	// (1) destroy the partition-vector object.
	poski_free(args);

	return 0;	// success!
}
// User interface
//-----------------------------------------------------------------------------------------

