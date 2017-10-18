/**
 * @file src/Kernel/poski_KernelHandler.c
 * @brief Handle the Sparse matrix kernels.
 *
 *  This module implements the library's support kernels 
 *  which perform error-checking first and then calls the appropriate kernel implementation.
 *
 * @attention Currently, only support Ax and A^Tx kernels.
 * 
 * Also, refer
 * @ref poski_Kernel_common.c,
 * @ref poski_Kernel_matmult.c
 */

#include <stdio.h>
#include <stdlib.h>

#include <poski/poski_kernelcommon.h>
#include <poski/poski_partitioncommon.h>
#include <poski/poski_matrixcommon.h>
#include <poski/poski_vectorcommon.h>
#include <poski/poski_vector.h>
#include <poski/poski_print.h>

//-----------------------------------------------------------------------------------------
// User interface
/**
 * @brief Set parameters for sparse matrix-vector multiply.
 *
 * @param[in] A Sparse matrix object.
 * @param[in] op Specifies the transpose operations, \f$op(A)\f$ in {OP_NORMAL, OP_TRANS}.
 * @param[in] alpha Scalar multiplier.
 * @param[in] x Vector view object.
 * @param[in] beta Scalar multiplier.
 * @param[in] y Vector view object.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_Set_MatMult (poski_mat_t A, poski_operation_t op, poski_value_t alpha, poski_vec_t x, poski_value_t beta, poski_vec_t y)
{
	// (1) error-check for the input parameters.
	poski_Check_Matrix(A);
	poski_Check_Vector(x, A->ncols);
	poski_Check_Vector(y, A->nrows);

	// (2) partition-check for an appropriate kernel operation \f$op(A)\f$.
	poski_Check_PartitionVecMatMult(A, op, KERNEL_MatMult, x, y);

	return 0;	// success!
}

/**
 * @brief Sparse matrix-vector multiply implementation.
 *
 * @param[in] A Sparse matrix object.
 * @param[in] op Specifies the transpose operations, \f$op(A)\f$ in {OP_NORMAL, OP_TRANS}.
 * @param[in] alpha Scalar multiplier.
 * @param[in] x Vector view object.
 * @param[in] beta Scalar multiplier.
 * @param[in] y Vector view object.
 *
 * @return 0 if success, or an error message otherwise.
 *
 *  This routine performs as follows:
 */
int poski_MatMult(poski_mat_t A, poski_operation_t op, poski_value_t alpha, poski_vec_t x, poski_value_t beta, poski_vec_t y)
{
	poski_printMessage(1, "Runing SpMV with %s...\n", poski_findoperation(op));

	/// (1) error-check for the input parameters.
	poski_Set_MatMult(A, op, alpha, x, beta, y);

	/// (2) run an appropriate kernel operation, currently only support \f${A, A^T}\f$ transpose operations.
	poski_MatMult_run(A, op, alpha, x, beta, y);
	
	/// (3) run an appropriate permute operation if required.
//	poski_permuteMatMult(y);

	/// (4) run an appropriate reduction operation if required.
	poski_reductionMatMult(y,A,beta,op);

	return 0;	// succes!
}
// User interface
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// Report functions
/**
 * @brief Report the information related with sparse matrix-vector multiply computation.
 *
 * @param[in] A Sparse matrix object in CSR format.
 * @param[in] alpha Scalar multiplier.
 * @param[in] x Vector data pointer.
 * @param[in] beta Scalar multiplier.
 * @param[in] y Vector data pointer.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_report_MatMultCSR(poski_sparse_matrix_t *A, poski_value_t alpha, poski_value_t *x, poski_value_t beta, poski_value_t *y)
{
	/* report SpMV : y = alpha*Ax + beta*y */
	TEST_LINECUT(0);
	printf ("\t## SpMV : y = alpha*Ax + beta*y \n");

	poski_report_sparse_CSR(A);
	printf("\t\t+ alpha = %4.1lf, beta = %4.1lf\n",alpha,beta);
	poski_report_vector(x, A->ncols, "x");
	poski_report_vector(y, A->nrows, "y");

	TEST_LINECUT(0);

	return 0;	// success;
}
// Report functions 
//-----------------------------------------------------------------------------------------

