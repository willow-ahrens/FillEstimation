/**
 * @file /src/Matrix/poski_MatrixHandler.c
 * @brief Handle the tunable sparse matrix object. 
 *
 *  This module implements the routines to handle the tunable sparse matrix object.
 *
 * @attention Currently, only support a sparse matrix in CSR format.
 *
 *  Also, refer
 * @ref poski_Matrix_common.c, poski_Matrix_getset.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <poski/poski_threadcommon.h>
#include <poski/poski_thread.h>
#include <poski/poski_partitioncommon.h>
#include <poski/poski_partition.h>
#include <poski/poski_matrixcommon.h>
#include <poski/poski_matrixgetset.h>
#include <poski/poski_kernelcommon.h>
#include <poski/poski_malloc.h>
#include <poski/poski_print.h>

//-----------------------------------------------------------------------------------------
// User interface
/**
 * @brief Create a tunable matrix object in CSR format.
 *
 * @param[in] ptr CSR row pointers.
 * @param[in] ind CSR column indices.
 * @param[in] val CSR non-zero values.
 * @param[in] nrows Number of rows.
 * @param[in] ncols Number of columns.
 * @param[in] nnz Number of non-zeros.
 * @param[in] mode Copy mode in {SHARE_INPUTMAT, COPY_INPUTMAT} for matrix creation routines.
 * @param[in] threadargs Threading model information.
 * @param[in] partitionargs Partitioning model information.
 * @param[in] k Number of explicitly specified semantic properties of a sparse matrix (ptr, ind, val).
 * @param[in] ... Specified semantic properties \f$<property_1>, ..., <property_k>\f$. See poski_inmatprop_t.
 *
 * @return A valid, tunable matrix object if success, or an error message otherwise.
 *
 *  This routine performs as follows:
 */
poski_mat_t poski_CreateMatCSR(poski_index_t *ptr, poski_index_t *ind, poski_value_t *val, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz, poski_copymode_t mode, poski_threadarg_t *threadargs, poski_partitionarg_t *partitionargs, int k, ...)
{
	poski_printMessage(1, "Creating a valid tunable matrix object...\n");

	/// (1) error-check for the input parameters.
	poski_Check_CreateMatCSRInput(threadargs, ptr, ind, val, nrows, ncols, nnz, mode);
	
	/// (2) collect the specified semantic properies. 
	va_list ap; 
	va_start(ap,k); 
	poski_inmatprop_t *in_props = poski_Collect_valist(k, ap); 
	va_end(ap); 
	
	/// (3) allocate the space for a new tunable matrix object, and initailize it.  
	poski_mat_t newA = poski_InitMat(threadargs, partitionargs, ptr, ind, val, nrows, ncols, nnz, mode);
	
	/// (4) create the partitioned submatrices.
	poski_partitionmat_funcpt funcpt;
	funcpt = poski_find_partitionmat_funcpt(newA->threadargs.ttype, newA->partitionargs.ptype);
	newA->submatrix = (poski_submat_t *)(*funcpt)(newA, mode, k, in_props);
	
	/// (5) clean-up the temporary stroage and return the tunable matrix object.  
	poski_free(in_props);

	return newA;	// success!
}
/**
 * @brief Create a tunable matrix object in CSR format.
 *
 * @param[in] SpA Sparse matrix in CSR format.
 * @param[in] mode Copy mode in {SHARE_INPUTMAT, COPY_INPUTMAT} for matrix creation routines.
 * @param[in] threadargs Threading model information.
 * @param[in] partitionargs Partitioning model information.
 * @param[in] k Number of explicitly specified semantic properties of a sparse matrix (ptr, ind, val).
 * @param[in] ... Specified semantic properties \f$<property_1>, ..., <property_k>\f$. See poski_inmatprop_t.
 *
 * @return A valid, tunable matrix object if success, or an error message otherwise.
 *
 *  This routine performs as follows:
 */
poski_mat_t poski_CreateMatCSRFile(poski_sparse_matrix_t *SpA, poski_copymode_t mode, poski_threadarg_t *threadargs, poski_partitionarg_t *partitionargs, int k, ...)
{
	poski_printMessage(1, "Creating a valid tunable matrix object...\n");

	poski_index_t *ptr = SpA->Aptr;
	poski_index_t *ind = SpA->Aind;
	poski_value_t *val = SpA->Aval;
	poski_index_t nrows = SpA->nrows;
	poski_index_t ncols = SpA->ncols;
	poski_index_t nnz = SpA->nnz;

	/// (1) error-check for the input parameters.
	poski_Check_CreateMatCSRInput(threadargs, ptr, ind, val, nrows, ncols, nnz, mode);
	
	/// (2) collect the specified semantic properies. 
	va_list ap; 
	va_start(ap,k); 
	poski_inmatprop_t *in_props = poski_Collect_valist(k, ap); 
	va_end(ap); 
	
	/// (3) allocate the space for a new tunable matrix object, and initailize it.  
	poski_mat_t newA = poski_InitMat(threadargs, partitionargs, ptr, ind, val, nrows, ncols, nnz, mode);
	
	/// (4) create the partitioned submatrices.
	poski_partitionmat_funcpt funcpt;
	funcpt = poski_find_partitionmat_funcpt(newA->threadargs.ttype, newA->partitionargs.ptype);
	newA->submatrix = (poski_submat_t *)(*funcpt)(newA, mode, k, in_props);
	
	/// (5) clean-up the temporary stroage and return the tunable matrix object.  
	poski_free(in_props);

	return newA;	// success!
}

/**
 * @brief Destroy the tunable matrix object.
 *
 * @param[in] A_tunable A tunable matrix object.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_DestroyMat(poski_mat_t A_tunable)
{
	poski_printMessage(1, "Destroying the tunable matrix object...\n");

	// (1) destroy the tunable matrix object.
	int pid;
	int npartitions = A_tunable->partitionargs.npartitions;
	
	for(pid=0;pid<npartitions;pid++)
	{
		poski_free(A_tunable->submatrix[pid].partitionmat->submat.Aptr);
		if (A_tunable->mode == COPY_INPUTMAT)
		{
			poski_free(A_tunable->submatrix[pid].partitionmat->submat.Aind);
			poski_free(A_tunable->submatrix[pid].partitionmat->submat.Aval);
		}
		poski_free(A_tunable->submatrix[pid].partitionmat->permuteRows);
		poski_free(A_tunable->submatrix[pid].partitionmat->permuteCols);
		poski_free(A_tunable->submatrix[pid].partitionmat);
		oski_DestroyMat(A_tunable->submatrix[pid].submat);
	}
	poski_free(A_tunable->permuteRows);
	poski_free(A_tunable->permuteCols);
	poski_free(A_tunable->submatrix);
	poski_free(A_tunable->kernel);
	poski_free(A_tunable);

	return 0;	// success
}

/**
 * @brief Change the value of the specified matrix element.
 *
 * @param[in,out] A_tunable The tunable matrix object.
 * @param[in] row The specified row of the element whose value is to be modified.
 * @param[in] col The specified column of the element whose value is to be modified.
 * @param[in] val The specified value to modify.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_SetMatEntry(poski_mat_t A_tunable, poski_int_t row, poski_int_t col, poski_value_t val)
{
	poski_printMessage(1, "Setting the new value...\n");	

	// (1) error-check: check whether the matrix object is valid or not.
	if (poski_Check_Matrix(A_tunable))
	{
		poski_error("poski_SetMatEntry","the tunable matrix object is not valid");
		return POSKI_INVALID;	// error: invalid matrix object.
	}

	// (2) set the value after checking whether the entry location is logical zero or not.
	poski_SetMatEntry_run(A_tunable, row, col, val);

	return 0;	// success!
}

/**
 * @brief Returns the value of the specified matrix element in A(row,col).
 *
 * @param[in] A_tunable The tunable matrix object.
 * @param[in] row The specified row of the elemenit whose value is to be returned.
 * @param[in] col The specified column of the element whose value is to be returned.
 *
 * @return  the value of A(row,col) if success, or an error message otherwise.
 */
poski_value_t poski_GetMatEntry(poski_mat_t A_tunable, poski_int_t row, poski_int_t col)
{
	poski_printMessage(1, "Getting the value...\n");

	// (1) error-check: check whether the matrix object is valid of not.
	if (poski_Check_Matrix(A_tunable))
	{
		poski_error("poski_GetMatEntry","the tunable matrix object is not valid");
		return POSKI_INVALID;	// error: invalid matrix object.
	}
	
	// (2) get the value after checking whether the entry location is logical zero or not.
	poski_value_t val = poski_GetMatEntry_run(A_tunable, row, col);

	// (3) return the value.
	return val;	// success!
}

/**
 * @brief Set a subset of non-zero values, specified as a clique or indexed list.
 * 
 * This imitates the USCR_INSERT_CLIQUE Sparse BLAS standard
 * interface.  See p. 128 (Chapter 3) of the <a
 * href="http://www.netlib.org/blas/blast-forum/">BLAS standard.</a> A
 * clique is typically how one defines an element for matrix assembly
 * in the finite element method.
 *
 * If insertion of any entry of the subset does not succeed, the
 * entry is not changed.
 *
 * @param A_tunable [in/out] The tunable matrix object.
 * @param numRows [in] Number of rows in the subset.
 * @param numCols [in] Number of columns in the subset.
 * @param rowStride [in] Stride between rows of vals.
 * @param colStride [in] Stride between columns of vals.
 * @param rows [in] Row indices of the subset.
 * @param cols [in] Column indices of the subset.
 * @param vals [in] Dense matrix storing the subset entries.
 * @param subsettype [in] The type of subset in {POSKI_BLOCKENTRIES, POSKI_ARRAYENTRIES}
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_SetMatSubset (poski_mat_t A_tunable, const int numRows, const int numCols, const int rowStride, const int colStride, const poski_int_t* rows, const poski_int_t* cols, const poski_value_t* vals, poski_subsettype_t subsettype)
{
	poski_printMessage(1, "Setting the values...\n");;

	// (1) error-check.
	// check whether the matrix object is valid of not.
	if (poski_Check_Matrix(A_tunable))
	{
		poski_error("poski_SetMatSubset","the tunable matrix object is not valid");
		return POSKI_INVALID;	// error: invalid matrix object.
	}

	// (2) set the values after checking the each entry location of subset is logical zero of not
	return poski_SetMatSubset_run (A_tunable, numRows, numCols, vals, rowStride, colStride, rows, cols, subsettype);
}

/**
 * @brief Get a subset of values, specified as a subset or indexed list.
 * 
 * @param A_tunable [in] The tunable matrix object.
 * @param numRows [in] Number of rows in the subset.
 * @param numCols [in] Number of columns in the subset.
 * @param rowStride [in] Stride between rows of vals.
 * @param colStride [in] Stride between columns of vals.
 * @param rows [in] Row indices of the subset.
 * @param cols [in] Column indices of the subset.
 * @param vals [in/out] Dense matrix storing the subset entries.
 * @param subsettype [in] The type of subset in {POSKI_BLOCKENTRIES, POSKI_ARRAYENTRIES}
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_GetMatSubset (poski_mat_t A_tunable, const int numRows, const int numCols, const int rowStride, const int colStride, const poski_int_t* rows, const poski_int_t* cols, poski_value_t* vals, poski_subsettype_t subsettype)
{
	poski_printMessage(1, "Getting the values...\n");

	// (1) error-check.
	// check whether the matrix object is valid of not.
	if (poski_Check_Matrix(A_tunable))
	{
		poski_error("poski_GetMatSubset","the tunable matrix object is not valid");
		return POSKI_INVALID;	// error: invalid matrix object.
	}

	// (2) get the values after checking the each entry location of subset is logical zero or not 
	return poski_GetMatSubset_run (A_tunable, numRows, numCols, vals, rowStride, colStride, rows, cols, subsettype);
}


// User interface
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// Report functions
/**
 * @brief Report information of a tunable matrix object.
 *
 * @param[in] mat A tuanble matrix object.
 *
 * @return 0 if success, or an error message otherwise. 
 */ 
int poski_report_tunable_matrix(poski_mat_t mat)
{
	TEST_LINECUT(0);

	// error check
	if (mat == NULL) { printf("*! Warning: <matrix A> is not set!\n"); return 1;}

	printf("## Tunable matrix information ##\n");
	poski_report_threadmodel(&(mat->threadargs),NULL);
	poski_report_partitionmodel(&(mat->partitionargs),NULL);
	printf("\t< global matrix information >\n");
	poski_report_tunable_matrix_va(mat);

	printf("\t\t+ global Rows permute %p\n", mat->permuteRows);
	printf("\t\t+ global Cols permute %p\n", mat->permuteCols);
	printf("\t< partitioned submatrices information >\n");
	poski_report_partitionmat(mat->submatrix, mat->partitionargs.npartitions);

	printf("\t< tunable submatrices information >\n");
	poski_report_submat(mat->submatrix, mat->partitionargs.npartitions);
	
	TEST_LINECUT(0);

	return 0;
}
// Report functions
//-----------------------------------------------------------------------------------------

