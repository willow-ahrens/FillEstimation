#ifndef _POSKI_VECTORTYPE_H
#define _POSKI_VECTORTYPE_H

#include "poski_commontype.h"
#include "poski_matrixtype.h"
#include "poski_kerneltype.h"

#define poski_vecview_t oski_vecview_t
#define poski_storage_t oski_storage_t

typedef enum
{
	INPUTVEC = 301,
	OUTPUTVEC= 302
}poski_vecprop_t;

/* vector partition arguments */
typedef struct 
{
	poski_int_t pid;
	poski_mat_t A;
	poski_kernel_t kernel;
	poski_operation_t op;
	poski_vecprop_t vecprop;
	poski_index_t npartitions;	
}poski_partitionvec_struct;
typedef poski_partitionvec_struct poski_partitionvec_t;

typedef struct
{
	poski_int_t pid;
	poski_vecview_t subvec;
}poski_subvec_t;

typedef struct
{
	poski_value_t *vec;
	poski_index_t length;
	poski_index_t inc;

	poski_int_t npartitions;

	poski_partitionvec_t *partitionvec;

	poski_subvec_t *subvector;
}poski_vecview_struct_t;
typedef poski_vecview_struct_t *poski_vec_t;

typedef poski_subvec_t *(*poski_partitionvec_funcpt) (poski_value_t *, poski_index_t, poski_index_t, poski_mat_t );

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_VECTORTYPE_H
