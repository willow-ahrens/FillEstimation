#ifndef _POSKI_MATRIXTYPE_H
#define _POSKI_MATRIXTYPE_H

#include "poski_commontype.h"
#include "poski_threadtype.h"
#include "poski_partitiontype.h"

#define poski_copymode_t oski_copymode_t
#define poski_inmatprop_t oski_inmatprop_t
#define poski_matstruct_t oski_matrix_t

typedef enum
{
	POSKI_BLOCKALL,
	POSKI_BLOCKENTRIES,
	POSKI_ARRAYALL,
	POSKI_ARRAYENTRIES
}poski_subsettype_t;

/* data structure for a sparse matrix in CSR format */
typedef struct 
{
	poski_index_t *Aptr;	/* row pointer in CSR */
	poski_index_t *Aind;	/* index in CSR */
	poski_value_t *Aval;	/* value in CSR */
	poski_index_t nrows;	/* number of rows */
	poski_index_t ncols;	/* number of cols */
	poski_index_t nnz;	/* number of nnz */
}poski_sparse_matrix_struct;
typedef poski_sparse_matrix_struct poski_sparse_matrix_t;

/* data structure for submatrix in CSR */
typedef struct
{
	poski_int_t npartitions;	/* total number of partitions */
	poski_int_t pid;		/* specified id of a partition */
	poski_index_t row_start;	/* start row of <pid> partition */
	poski_index_t row_end;		/* end row of <pid> partition */
	poski_index_t col_start;	/* start col of <pid> partition */
	poski_index_t nnz;		/* number of nonzeros for <pid> partition */
	poski_index_t nrows;		/* number of rows for <pid> partition */
	poski_index_t ncols;		/* number of cols for <pid> partition */
	poski_index_t *permuteRows;	/* row permute information of <pid> partition */
	poski_index_t *permuteCols;	/* column permute information of <pid> partition */
	poski_sparse_matrix_t submat;	/* submatrix in CSR format */
}poski_partitionmat_struct;
typedef poski_partitionmat_struct poski_partitionmat_t;

/* data structure for a tunable submatrix */
typedef struct
{
	poski_int_t npartitions;		/* total number of partitions */
	poski_int_t pid;			/* specified id of a partition */
	poski_partitionmat_t *partitionmat;	/* partitioned submatrix information in CSR */
	poski_matstruct_t submat;		/* tunable submatrix */
}poski_submat_t;

/* data structure for a tunable matrix */
typedef struct
{
	/* global threading arguments */
	poski_threadarg_t threadargs;	

	/* global partitioning arguments */
	poski_partitionarg_t partitionargs;

	/* global input matrix information */
	poski_index_t nrows;
	poski_index_t ncols;
	poski_index_t nnz;
	poski_index_t *Aptr;
	poski_index_t *Aind;
	poski_value_t *Aval;
	poski_copymode_t mode;

	/* global permute information */
	poski_index_t *permuteRows;	
	poski_index_t *permuteCols;

	/* tunable submatrices */
	poski_submat_t *submatrix;

	/* kernel */
	void *kernel;
	
	/* reduction */
	int require;
}poski_mat_struct;
typedef poski_mat_struct *poski_mat_t;

typedef poski_submat_t *(*poski_partitionmat_funcpt) (poski_mat_t, poski_copymode_t, int, poski_inmatprop_t *);

typedef struct
{
	poski_int_t tid;
	poski_int_t num_partitions;
	poski_copymode_t mode;
	int k;
	poski_inmatprop_t *in_props;
	poski_mat_t A;
	poski_partitionmat_t *partitionmat;
	
	poski_submat_t *submat;
}poski_thread_t;

#endif // _POSKI_MATRIXTYPE_H
