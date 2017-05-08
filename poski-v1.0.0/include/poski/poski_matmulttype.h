#ifndef _POSKI_MATMULTTYPE_H
#define _POSKI_MATMULTTYPE_H

#include "poski_commontype.h"
#include "poski_kerneltype.h"
#include "poski_matrixtype.h"
#include "poski_vectortype.h"

/* Kernel matmult data structures */
typedef struct
{
	int tid;
	int num_partitions;
	poski_submat_t *submat;
	poski_operation_t op;
	poski_value_t alpha;
	poski_value_t beta;
	poski_subvec_t *x;
	poski_subvec_t *y;
}poski_matmult_t;	// for y = alpha*op(A)*x + beta*y per thread

/* matmult reduction data structure */
typedef struct
{
	int tid;
	poski_vec_t vec;
	poski_mat_t A;
	poski_value_t beta;
	poski_operation_t op;
	poski_int_t nthreads;	
}poski_reduction_t;

#endif // _POSKI_MATMULTTYPE_H
