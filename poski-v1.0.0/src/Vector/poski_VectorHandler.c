/**
 * @file src/Vector/poski_VectorHandler.c
 * @brief Handle the vector view object.
 *
 * This module implements the routines to hanle the vector view object.
 *
 * Also, refer
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <poski/poski_vectorcommon.h>
#include <poski/poski_partition.h>
#include <poski/poski_partitioncommon.h>
#include <poski/poski_malloc.h>
#include <poski/poski_print.h>

//-----------------------------------------------------------------------------------------
// User interface
/**
 * @brief Create a vector-view object.
 *
 * @param[in] vec Vector data
 * @param[in] length Logical vector length
 * @param[in] inc Physical stride between consecutive elements.
 * @param[in] partitionvec Partitionvector object.
 *
 * @return A valid vector-view object if success, or an error message otherwise.
 *
 * This routine performs as follows:
 */
poski_vec_t poski_CreateVecView( poski_value_t *vec, poski_index_t length, poski_int_t inc, poski_partitionvec_t *partitionvec )
{
	poski_printMessage(1, "Creating a vector view object...\n");

	/// (1) error-check for the input parameters. 
	if (poski_Check_CreateVecViewInput(vec, length, inc, partitionvec) < 0)
	{
		poski_error("poski_CreateVecView", "invalid inputs");
		return NULL;
	}

	/// (2) allocate space for a new vector-view object, and initialize it.
	/* @ now with no error of input parameters */
	/* @ Set user's parameters */
	poski_vec_t new_vecview = poski_Init_Vector(vec, length, inc, partitionvec);
	if (new_vecview==NULL) 
	{
		poski_error("poski_CreateVecView", "can not create a vector-view object");
		return NULL;
	}

	/// (3) create partitioned subvectors if partitionvector object is given.
	new_vecview->subvector = poski_PartitionVec(vec, length, inc, partitionvec);

	/// (4) return the vector-view object.
	return new_vecview;	// success!
}

/**
 * @brief Destroy a vector-view object.
 *
 * @param[in] vec A vector-view object.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_DestroyVec ( poski_vec_t vec )
{
	poski_printMessage(1, "Destroying the vector view object...\n");

	// (1) destroy the vector-view object.
	int pid;
	int npartitions = vec->npartitions;
	 
	for (pid=0;pid<npartitions;pid++)
	{
		oski_DestroyVecView(vec->subvector[pid].subvec);
	}
	poski_free(vec->subvector);
	poski_free(vec);

	return 0;	// success!
}
// User interface
//-----------------------------------------------------------------------------------------

//#if HAVE_EPS_DOUBLE
//# 	include <float.h>
//# 	define VAL_EPS DBL_EPSILON /**< \brief Double-precision machine epsilon */
//#else /** \brief Approximate double-precision machine epsilon */
//# 	define VAL_EPS 1e-15
//#endif

//-----------------------------------------------------------------------------------------
// Check functions
/**
 * @brief Compare two vector data.
 *
 * This routine compares that two vector data are equal or not.
 *
 * @param[in] A The sparse matrix object in CSR format.
 * @param[in] vec1 First vector data.
 * @param[in] vec2 Second vector data.
 * @param[in] length Logical vector length.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_check_TwoVectors(poski_sparse_matrix_t *A, poski_value_t *vec1, poski_value_t *vec2, poski_index_t length)
{
	poski_printMessage(1, "Comparing two vectors...\n");

	// (1) error-check
	if (vec1 == NULL || vec2 == NULL)
	{
		poski_error("poski_check_TwoVectors","vectors are not properly set");
		return POSKI_INVALID;
	}

	// (2) compare two vectors. 
	int i;
	int error=0;
	poski_value_t diff, abs_diff;
	poski_value_t max_abs_diff=0.0;
	for (i=0;i<length;i++)
	{
		diff = vec1[i] - vec2[i];
		abs_diff = fabs(diff);
		//pow(2,(-53)
		if (abs_diff/vec1[i] > length*VAL_EPS)
		{
			if (error==0) poski_error("poski_check_TwoVectors","two vectors are not matching");
			poski_printMessage(2,"Warning: + vec1[%d] = %e : vec2[%d] = %e | abs_diff = %e\n",i, vec1[i], i, vec2[i],i, i, abs_diff);
			error++;
		}

		if (abs_diff > max_abs_diff)
			max_abs_diff = abs_diff;

	}
	if (error>0)
	{
		return POSKI_INVALID;
	}
	else
	{
		poski_printMessage(1, "two vectors are matching!\n");
		poski_printMessage(2, "+ sample: (vec1[%d] = %e : vec2[%d] = %e)\n",0, vec1[0],0,vec2[0]);
	}
	return 0;	// success!
}
// Check functions
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// Report functions
/**
 * @brief Report information of a vector data
 *
 * @param[in] vec Vector data
 * @param[in] length Logical vector length.
 * @param[in] name Specified vector name.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_report_vector(poski_value_t *vec, poski_index_t length, char *name)
{
	int i;

	// (1) print a vector 
	printf("\t\t+ %s = { ", name);
	for (i=0;i<length;i++)
	{
		printf("%4.1lf ",vec[i]);		
	}
	printf("}\n");

	return 0;	// success!
}

/**
 * @brief Report information of a vector-view object.
 *
 * @param[in] vec Vector-view object.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_report_vec(poski_vec_t vec)
{
	printf("\t## Global vector view ##\n");
	printf("\tnpartition = %d\n", vec->npartitions);
	printf("\tvec = %p\n",vec->vec);
	printf("\tlength = %d, inc = %d\n", vec->length, vec->inc);

	int i;
	if (vec->partitionvec!=NULL)
	{
		for(i=0;i<vec->npartitions;i++)
		{
			poski_report_partitionvec(&vec->partitionvec[i]);
		}
	}
	if (vec->subvector!=NULL) 
	{
		for (i=0;i<vec->npartitions;i++)
		{ 
			poski_report_subvec(vec->subvector[i].subvec);
		}
	}

	return 0;	// success!
}
// Report functioins
//-----------------------------------------------------------------------------------------


