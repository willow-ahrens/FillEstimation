#ifndef _POSKI_KERNELTYPE_H
#define _POSKI_KERNELTYPE_H

#include "poski_commontype.h"

#define poski_operation_t oski_matop_t

typedef enum
{
	KERNEL_NULL=101,
	KERNEL_MatMult=102,
	KERNEL_MatTrisolve=103,
	KERNEL_MatTransMatMult=104,
	KERNEL_MatMultAndMatTransMult=105,
	KERNEL_MatPowMult=106,
	KERNEL_Last=107
}poski_kernel_type_t;
typedef poski_kernel_type_t poski_kernel_t;

typedef enum
{
	CREATEMAT_ONED=201,
	CREATEMAT_SEMIONED=202,
	CREATEVEC_NORMAL=203,
	CREATEVEC_PARTITION=204,
	TUNEMAT=205,
	REDUCTION=206

}poski_job_type_t;
typedef poski_job_type_t poski_job_t;

#endif // _POSKI_KERNELTYPE_H
