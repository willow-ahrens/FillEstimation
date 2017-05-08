/**
 * @file /src/Matrix/poski_Matrix_common.c
 * @brief Commonly used routines in Matrix Handler's modules. 
 *
 *  This module implements the commonly used routines in matrix handler's modules.
 *
 * @attention These routines are usually for the library's internal use
 *  and should not normally be called directly by users.
 * @attention Called by
 * @ref poski_MatrixHandler.c
 *
 *  Also, refer
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <poski/poski_threadcommon.h>
#include <poski/poski_matrixcommon.h>
#include <poski/poski_partition.h>
#include <poski/poski_partitioncommon.h>
#include <poski/poski_malloc.h>
#include <poski/poski_print.h>
/**
 * @brief [library's internal use] Initialize a tunable matrix.
 *
 * @internal
 * @param[in,out] A tunable matrix object.
 * @param[in] Aptr CSR row pointers. 
 * @param[in] Aind CSR column indices.
 * @param[in] Aval CSR non-zero values.
 * @param[in] nrows Number of rows.
 * @param[in] ncols Number of columns.
 * @param[in] nnz Number of non-zeros.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_Init_GlobalMatrix (poski_mat_t A, poski_index_t *Aptr, poski_index_t *Aind, poski_value_t *Aval, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz)
{
	A->Aptr = Aptr;
	A->Aind = Aind;
	A->Aval = Aval;
	A->nrows = nrows;
	A->ncols = ncols;
	A->nnz = nnz;

	A->permuteRows = NULL;
	A->permuteCols = NULL;
	A->submatrix = NULL;
	
	return 0;
}

/**
 * @brief [library's internal use] Allocate space for a new tunable matrix object, and initailize it.
 *
 * @internal
 *
 * @param[in] threadargs Threading model information.
 * @param[in] partitionargs Partitioning model information.
 * @param[in] ptr CSR row pointers.
 * @param[in] ind CSR column indices.
 * @param[in] val CSR non-zero values.
 * @param[in] nrows Number of rows.
 * @param[in] ncols Number of columns.
 * @param[in] nnz Number of non-zeros.
 * @param[in] mode Copy mode in {SHARE_INPUTMAT, COPY_INPUTMAT} for matrix creation routines.
 *
 * @return A valid, tunable matrix object, or an error message otherwise.
 */
poski_mat_t poski_InitMat(poski_threadarg_t *threadargs, poski_partitionarg_t *partitionargs, poski_index_t *ptr, poski_index_t *ind, poski_value_t *val, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz, poski_copymode_t mode)
{

	/* NUMA affinity @ Create Global tunable matrix wrapper */
	poski_mat_t newA = (poski_mat_t)malloc(sizeof(poski_mat_struct)*1);
	if (newA == NULL) assert(0);

	/* Copy a threading model into a global matrix object */
	if ( poski_Copy_threadargs(&newA->threadargs, threadargs) )
	{
		poski_error("poski_InitMat", "Can't copy the <threadargs> into matrix object");
		assert(0);
	}

	/* Initalize a partitioning model into a global matrix object */
	poski_Init_partitionargs(&(newA->partitionargs), partitionargs, newA->threadargs.nthreads);
	poski_Adjust_args(newA->partitionargs.ptype, &(newA->partitionargs.npartitions), &(newA->threadargs.nthreads), nrows, ncols, nnz); 
	
	/* Initalize a matrix in CSR into a global matrix object */
	poski_Init_GlobalMatrix(newA, ptr, ind, val, nrows, ncols, nnz);

	newA->mode = mode;

	return newA;	// success!
}

/**
 * @brief [library's internal use] Initialize a submatrix object.
 *
 * @internal
 * @param[in,out] submatrix A submatrix object. 
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_Init_LocalSubMat(poski_submat_t *submatrix)
{
	submatrix = NULL;
	return 0;
}

/**
 * @brief [library's internal use] Collect specified semantic properies. 
 *
 * @internal
 *
 * @param[in] k Number of explicitly specified semantic properties of a sparse matrix (ptr, ind, val).
 * @param[in] ap The variable argument list of specified semantic properties. 
 *
 * @return A object of specified semantic properties if success, or an error message otherwise. 
 */ 
poski_inmatprop_t *poski_Collect_valist(int k, va_list ap)
{
	poski_inmatprop_t *in_props;
	in_props=oski_Malloc(poski_inmatprop_t, k);
	if (!in_props) assert(0);
	int i;
	for (i=0;i<k;++i)
		in_props[i] = va_arg(ap, poski_inmatprop_t);
	return in_props;
}


/**
 * @brief [library's internal use] Error-checking for a sparse matrix in CSR format.
 *
 * @internal
 * @param[in] ptr CSR row pointers. 
 * @param[in] ind CSR column indices.
 * @param[in] val CSR non-zero values.
 * @param[in] nrows Number of rows.
 * @param[in] ncols Number of columns.
 * @param[in] nnz Number of non-zeros.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_Check_matCSR (poski_index_t *ptr, poski_index_t *ind, poski_value_t *val, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz)
{
	// do we need to check matrix more specific? 
	if ( (ptr == NULL) || (ind == NULL) || (val == NULL) ) assert(0);
	if ( (nrows < 1) || (ncols < 1) ) assert(0);
	return 0;	// success!
}

/**
 * @brief [library's internal use] Error-checking for a matrix copy mode.
 *
 * @internal
 * @param[in] mode Copy mode in {SHARE_INPUTMAT, COPY_INPUTMAT} for matrix creation routines.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_Check_matcopymode(poski_copymode_t mode)
{
	if (mode != SHARE_INPUTMAT || mode != COPY_INPUTMAT) 
	{
		return -1;
	}
	return 0;
}

/**
 * @brief [library's internal use] Error-checking for input parameters of creating a tunable matrix object.
 * 
 * @internal
 *  This routine performs error-checking for the threading model, the sparse matrix, and the matrix copy mode. 
 *
 * @param[in] threadargs Threading model information.
 * @param[in] ptr CSR row pointers.
 * @param[in] ind CSR column indices.
 * @param[in] val CSR non-zero values.
 * @param[in] nrows Number of rows.
 * @param[in] ncols Number of columns.
 * @param[in] nnz Number of non-zeros.
 * @param[in] mode Copy mode in {SHARE_INPUTMAT, COPY_INPUTMAT} for matrix creation routines.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_Check_CreateMatCSRInput(poski_threadarg_t *threadargs, poski_index_t *ptr, poski_index_t *ind, poski_value_t *val, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz, poski_copymode_t mode)
{
	if ( poski_Check_threadargs(threadargs) )
	{
		poski_error("poski_Check_CreateMatCSRInput", "The <threadargs> is not properly set");
		assert(0);
	}
	poski_Check_matCSR(ptr, ind, val, nrows, ncols, nnz);
	poski_Check_matcopymode(mode);
	return 0;	// success!
}

/**
 * @brief [library's internal use] Error-checking for a tunable matrix.
 *
 * @internal
 * @param[in] A tunable matrix.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_Check_Matrix(poski_mat_t A)
{
	if (A==NULL) 
	{
		poski_error("poski_Check_Matrix", "The <tunable matrix> is not properly set");
		assert(0);	
	}
	return 0;	
}

/**
 * @brief [library's internal use] Error-checking for the location in the tunable matrix.
 *
 * @internal
 * @param[in] A The tunable matrix.
 * @param[in] row The specified row.
 * @param[in] col The specified column.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_Check_Matrix_location(poski_mat_t A, poski_int_t row, poski_int_t col)
{
	int nrows = A->nrows;
	int ncols = A->ncols;
	
	if (row < 0 || row >= nrows) return -1;
	if (col < 0 || col >= ncols) return -1;

	return 0;	
}

/**
 * @brief [library's internal use] Report information of a sparse matrix in CSR format.
 *
 * @internal
 *  This routine prints a sparse matrix A in CSR as 2D matrix format including all nonzeros & zeros.
 *
 * @param[in] Aptr CSR row pointers.
 * @param[in] Aind CSR column indices.
 * @param[in] Aval CSR non-zero values.
 * @param[in] nrows Number of rows.
 * @param[in] ncols Number of columns.
 * @param[in] nnz Number of non-zeros.
 * @return 0 if success, or an error message otherwise. 
 */ 
int poski_report_sparse_CSR_va(poski_index_t *Aptr, poski_index_t *Aind, poski_value_t *Aval, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz)
{
	// report a sparse matrix A in CSR
	poski_int_t i,j;
	int count = 0;

	// error check for matrix A in CSR
	if (Aptr == NULL || Aind == NULL || Aval == NULL) { printf("*! Warning: sparse matrix in CSR is not set!\n"); return 1;}
	if (nrows < 1 || ncols < 1 || nnz < 1) { printf("*! Warning: sparse matrix in CSR is not set!\n"); return 1;}

	if (nnz != Aptr[nrows])
	{	
		printf("*! Warning: the number of nnz and Aptr[last] is not matching (nnz = %d != Aptr = %d)\n", nnz, Aptr[nrows]);
		return 1;
	}

	// print a sparse matrix A in CSR as 2D matrix format including all nonzeros & zeros
	printf("\t+ matrix A : ( %d x %d ) with %d nnz\n", nrows, ncols, nnz);
	for (i=0;i<nrows;i++)
	{
		printf("\t\t");
		for (j=0;j<ncols;j++)
		{
			if (count <= Aptr[i+1] ) 
			{
				if (j == Aind[count] && count < nnz) 
				{
					printf("%5.4lf  ", Aval[count]);
					count++;
				}
				else printf("%5.4lf  ", (double)0); 
			}
			else printf("%5.4lf  ", (double)0); 

		}
		printf("\n");
	}
	
	return 0; 	// success!
}

/**
 * @brief [library's internal use] Report information of a sparse matrix in CSR format.
 *
 * @internal
 *  This routine prints a sparse matrix A in CSR as 2D matrix format including all nonzeros & zeros.
 *
 * @param[in] A Sparse matrix in CSR format.
 *
 * @return 0 if success, or an error message otherwise. 
 */ 
int poski_report_sparse_CSR(poski_sparse_matrix_t *A)
{
	// report a sparse matrix A in CSR
	if (A==NULL || A->Aptr==NULL) {printf("\t\t+ submat = %p\n",A); return 1;}
	poski_report_sparse_CSR_va(A->Aptr, A->Aind, A->Aval, A->nrows, A->ncols, A->nnz);
	
	return 0; 	// success!
}

static int poski_report_tunable_submatrix(int cont, poski_index_t *Aptr, poski_index_t *Aind, poski_value_t *Aval, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz)
{
	int i,j;
	int count;
	
	for (i=0;i<nrows;i++)
	{
		if (cont==1 || i>0) printf("\t\t");
		
		for (j=0;j<ncols;j++)
		{
			if (count <= Aptr[i+1] ) 
			{
				if (j == Aind[count] && count < nnz) 
				{
					printf("%5.4lf  ", Aval[count]);
					count++;
				}
				else printf("%5.4lf  ", (double)0); 
			}
			else printf("%5.4lf  ", (double)0); 

		}
		if (i < nrows-1)printf("\n");
	}
	return 0;
}

int poski_report_tunable_matrix_va(poski_mat_t A)
{
	int pid;
	int npartitions = A->partitionargs.npartitions;
	poski_index_t nnz=0;
	poski_value_t *temp = (poski_value_t *)malloc( (A->nnz)*sizeof(poski_value_t) );
	printf("\t\t+ matrix A : ( %d x %d ) with %d nnz\n", A->nrows, A->ncols, A->nnz);
	for (pid=0;pid<npartitions;pid++)
	{
		poski_sparse_matrix_t submat = A->submatrix[pid].partitionmat->submat;
		memcpy(&temp[nnz], &submat.Aval[0],sizeof(poski_value_t)*submat.nnz);
		nnz += submat.nnz;
	}
	poski_report_sparse_CSR_va(A->Aptr, A->Aind, temp, A->nrows, A->ncols, A->nnz);
	poski_free(temp);	
	return 0;
}

/**
 * @brief Report information of submatrices.
 *
 * @internal
 * @param[in] submat A submatrix object.
 * @param[in] npartitions The number of partitioned submatrices.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_report_submat(poski_submat_t *submat, poski_int_t npartitions)
{
	// error check
	if (submat == NULL) { printf("*! Warning: <submat> is not set!\n"); return 1;}
	if (npartitions < 1) { printf("*! Warning: <npartitions> is less than 1!\n"); return 1;}

	int i;
	
	for (i=0;i<npartitions;i++)
	{
		poski_matstruct_t mat = submat[i].submat;	
		if (mat == NULL) { printf("\t\t+ %d-th submatrix  = %p\n",i,mat); break; }
		printf("\t\t[submatrix %d]\n", i);
		printf("\t\t+ nrows = %d, ncols = %d, nnz = %d\n",mat->props.num_rows, mat->props.num_cols, mat->props.num_nonzeros);
		printf("\t\t+ type_id = %ld, *repr=%p\n",mat->input_mat.type_id, mat->input_mat.repr);
	}

	return 0;	// success!
}


