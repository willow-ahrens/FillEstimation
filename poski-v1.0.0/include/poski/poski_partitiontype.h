#ifndef _POSKI_PARTITIONTYPE_H
#define _POSKI_PARTITIONTYPE_H

#include "poski_commontype.h"
#include "poski_kerneltype.h"

typedef enum
{
	OneD,
	SemiOneD,
	TwoD,
	Mondriaan,
	PaToH,
	/* add other threading models */
	PARTITION_LAST
}poski_partitiontype_t;

/* matrix partition arguments */
typedef struct
{
	poski_partitiontype_t ptype;
	poski_int_t npartitions;
	poski_kernel_t kernel;
	poski_operation_t op;
}poski_partitionarg_struct;
typedef poski_partitionarg_struct poski_partitionarg_t;


#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_PARTITIONTYPE_H
