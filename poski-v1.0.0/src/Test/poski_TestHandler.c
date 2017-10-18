/**
 * @file src/Test/poski_TestHandler.c
 * @brief Commonly used routines to test the library's routines.
 *
 * This module implements the routines used to test the library's routines.
 *
 * Also, refer
 * @ref poski_Test_NaiveSpMV.c
 */

#include <stdio.h>
#include <stdlib.h>

#include <poski/poski_test.h>
#include <poski/poski_matrixtype.h>

/******************************************************************************/
/* User routine space */
/******************************************************************************/
/**
 * @brief Set a sample sparse matrix.
 * 
 * @return A sparse matrix in CSR format if success, or an error message otherwise.
 */
poski_sparse_matrix_t *SetTestSampleSparseMatrix()
{
	// @ Set a sample sparse matrix in CSR format
	//       0  1  2  3  4  5  6  7  8  9
	//       ----------------------------
	// A = ( 0  0  0  0  0  0  0  0  0  1 ) | 0 
	//     ( 0  0  0  0  0  0  0  0  2  0 ) | 1 
	//     ( 0  0  0  0  0  0  0  3  0  0 ) | 2
	//     ( 0  0  0  0  0  0  4  0  0  0 ) | 3
	//     ( 0  0  0  0  0  5  0  0  0  0 ) | 4
	//     ( 0  0  0  0  6  0  0  0  7  0 ) | 5
	//     ( 0  0  0  8  0  0  0  0  0  0 ) | 6
	//     ( 0  0  9  0  0  0  0  0  0  0 ) | 7
	//     ( 0 10  0  0  0  0  0  0  1  0 ) | 8
	//     (11 12 13 14 15 16 17 18 19 20 ) | 9

	poski_sparse_matrix_t *newA = (poski_sparse_matrix_t *)malloc(sizeof(poski_sparse_matrix_t)*1);
	if (newA == NULL) assert(0);

	newA->nrows = 10;
	newA->ncols = 10;
	newA->nnz = 20;

	newA->Aptr = (poski_index_t *)malloc(sizeof(poski_index_t)*(newA->nrows + 1));
	newA->Aind = (poski_index_t *)malloc(sizeof(poski_index_t)*(newA->nnz));
	newA->Aval = (poski_value_t *)malloc(sizeof(poski_value_t)*(newA->nnz));

	// @ Set a sample sparse matrix
	poski_index_t Aptr[11] = {0,1,2,3,4,5,7,8,9,10,20};
	int i;
	for (i=0;i<(newA->nrows+1);i++)
	{
		newA->Aptr[i] = (poski_index_t)Aptr[i];
	}	

	poski_index_t Aind[20] = {9,8,7,6,5,4,8,3,2,1,0,1,2,3,4,5,6,7,8,9};
	for (i=0;i<newA->nnz;i++)
	{
		newA->Aind[i] = Aind[i];
	}

	poski_value_t Aval[20] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
	for (i=0;i<newA->nnz;i++)
	{
		newA->Aval[i] = Aval[i];
	}

	return newA;
}

/**
 * @brief Destroy a sparse matrix obejct in CSR format.
 *
 * @param[in] A Sparse matrix object in CSR format.
 *
 */
void poski_Destroy_CSR(poski_sparse_matrix_t *A)
{
	if (A->Aptr != NULL) free(A->Aptr);
	if (A->Aind != NULL) free(A->Aind);
	if (A->Aval != NULL) free(A->Aval);
	if (A != NULL) free(A);
}

/**
 * @brief Create a sample vector data.
 *
 * @param[in] length Logical vector length.
 * @param[in] vtype Specified value type in {RANDOM, ZERO}.
 *
 * @return A valid vector data if success, or an error message otherwise.
 */
poski_value_t *CreateTestSingleVec(int length, poski_value_type_t vtype)
{
	// generate test single vector with length
	int i;
	poski_value_t *vec = (poski_value_t *) malloc (sizeof(poski_value_t)*length);

	if (vtype == RANDOM)
	{
	// vec[length] = {0,1,2,3,...,(length-1)} 
		for (i=0;i<length;i++)
		{
			vec[i] = (poski_value_t) i;
		}
	}
	else if (vtype == ZERO)
	{
	// vec[length] = {0,0,0,0,...,0} 
		for (i=0;i<length;i++)
		{
			vec[i] = (poski_value_t) 0;
		}

	}
	return vec;
}

