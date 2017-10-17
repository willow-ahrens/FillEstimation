/**
 * @file src/TuneMat/poski_TuneMatHandler.c
 * @brief Tuning module implementation. 
 *  
 *  This module implements the routines to handle the tuning operations.
 *
 * Also, refer
 * @ref poski_TuneMat_common.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <poski/poski_matrixcommon.h>
#include <poski/poski_tunematcommon.h>
#include <poski/poski_malloc.h>
#include <poski/poski_print.h>

//-----------------------------------------------------------------------------------------
// User interface
/**
 * @brief Tune matrix.
 *
 * @param[in,out] A_tunable Tunable matrix object.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_TuneMat(poski_mat_t A_tunable)
{
	poski_printMessage(1, "Tuning the tunable matrix object...\n");
	
	// (1) tune the tunable matrix object.
	poski_TuneMat_run(A_tunable);

	return 0;	// success!
}

/**
 * @brief Set structural hints.
 *
 * @param[in,out] A_tunable Tunable matrix object.
 * @param[in] hint User-specified structural hint.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_TuneHint_Structure(poski_mat_t A_tunable, poski_tunehint_t hint, ...)
{
	poski_printMessage(1, "Setting the structural tuning hints...\n");

	// (1) error-check.
	poski_Check_Matrix(A_tunable);

	if (!poski_check_tunehint(hint))
	{
		POSKI_ERR_BAD_HINT (poski_TuneHint_Structure, 2 ,hint);
		return ERR_BAD_HINT_ARG;
	}

	va_list argp;
	int k=ARGS_NONE;
	int *r=NULL, *c=NULL;
	va_start (argp, hint);
	switch (hint)
	{
		case HINT_SINGLE_BLOCK:
		{
			r = poski_malloc(int, 1, 0);
			c = poski_malloc(int, 1, 0);
			r[0] = va_arg (argp, int);
			if (r[0] != ARGS_NONE)	/* Process optional arguments, if any. */
			{
				c[0] = va_arg (argp, int);
			}
			break;
		}
		case HINT_MULTIPLE_BLOCKS:
		{
			k = va_arg (argp, int);
			if (k >= 1)
			{
				int i;
				r = poski_malloc(int, k, 0);
				c = poski_malloc(int, k, 0);
				for (i = 0; i < k; i++)
				{
					r[i] = va_arg (argp, int);
					c[i] = va_arg (argp, int);
				}
			}
			break;
		}
		default:
			break;
	}
	va_end (argp);
	
	// (2) set the structural hints.
	poski_TuneHint_Structure_run(A_tunable, hint, k, r, c);

	// (3) clean-up. 
	poski_free(r);
	poski_free(c);

	return 0;
}

/**
 * @brief Set workload hint for the kernel operation poski_MatMult.
 *
 * @param[in,out] A_tunable Tunable matrix object.
 * @param[in] op Specifies op(A).
 * @param[in] alpha Scalar constant.
 * @param[in] x_view Symbolic vector or Vector-view object for a vector x.
 * @param[in] beta Scalar constant.
 * @param[in] y_view Symbolic vector or Vector-view object for a vector y.
 * @param[in] num_calls The number of times this kernel will be called with these arguments.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_TuneHint_MatMult(poski_mat_t A_tunable, poski_operation_t op, poski_value_t alpha, poski_vec_t x_view, poski_value_t beta, poski_vec_t y_view, int num_calls)
{
	poski_printMessage(1, "Setting the workload hints...\n");

	// (1) error-check.
	if (num_calls <= 0)
		return POSKI_INVALID;	// fail!

	// (2) set the workload hints.
	poski_TuneHint_MatMult_run(A_tunable, op, alpha, x_view, beta, y_view, num_calls);

	return 0;	// success!

}
// User interface
//-----------------------------------------------------------------------------------------



