/**
 * @file src/Test/poski_Test_NaiveSpMV.c
 * @brief Naive sparse matrix-vector computation.
 *
 *  This module implements the routines to handle naive sparse matrix-vector computations.
 *
 * @attention Currently, only support with CSR format.
 *
 *  Also, refer
 * @ref poski_TestHandler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include <poski/poski_test.h>

/**
 * @brief Naive sparse matrix-vector computation (SpMV).
 *
 * @param[in] A Sparse matrix object in CSR format.
 * @param[in] alpha Scalar multiplier.
 * @param[in] x Vector data pointer.
 * @param[in] beta Scalar multiplier.
 * @param[in] y Vector data pointer.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_NavieMatMultCSR(poski_sparse_matrix_t *A, poski_value_t alpha, poski_value_t *x, poski_value_t beta, poski_value_t *y, poski_operation_t op)
{
	// Naive SpMV : y = alpha*Ax + beta*y
	int i, j;
	int count = 0;

	poski_index_t *Aptr = A->Aptr;
	poski_index_t *Aind = A->Aind;
	poski_value_t *Aval = A->Aval;
	poski_index_t nrows = A->nrows;
	poski_index_t ncols = A->ncols;
	poski_index_t nnz = A->nnz;

	poski_value_t temp = 0.0;

	if (op==OP_NORMAL)
	{
		for (i=0;i<(nrows);i++)
		{
			temp = 0.0;
			for(j=Aptr[i];j<Aptr[i+1];j++)
			{ 
				temp += Aval[j]*x[Aind[j]];
				count++;
			}
			y[i] = alpha*temp + beta*y[i];
		}

		// error check 
		if (count != nnz) assert(0);
	}
	if (op==OP_TRANS)
	{
		poski_value_t *temp_y = (poski_value_t *)malloc(sizeof(poski_value_t)*ncols);
		for (i=0;i<ncols;i++)
			temp_y[i] = 0;

		for (i=0;i<(nrows);i++)
		{
			for(j=Aptr[i];j<Aptr[i+1];j++)
			{ 
				temp = Aval[j]*x[i];
				temp_y[Aind[j]] += temp;
				count++;
			}
		}

		for (i=0;i<ncols;i++)
			y[i] = alpha*temp_y[i] + beta*y[i];

		// error check 
		if (count != nnz) assert(0);
		free(temp_y);

	}
	return 0;	// success;
}

