/**
 * @file src/Vector/poski_Vector_common.c
 * @brief Commonly used routines in Vector Handler's modules.
 *
 * This module implements the commonly used routines in matrix handler's modules.
 *
 * @attention These routines are usually for the library's internal use
 * and should not normally be called directly by users.
 * @attention Called by 
 * @ref poski_VectorHandler.c
 *
 * Also, refer
 */

#include <stdio.h>
#include <stdlib.h>

#include <poski/poski_vectorcommon.h>
#include <poski/poski_partition.h>
#include <poski/poski_partitioncommon.h>
#include <poski/poski_print.h>

/**
 * @brief [library's internal use] Initialize a vector object.
 *
 * @internal
 * @param[in] vec Vector data.
 * @param[in] length Logical vector length.
 * @param[in] inc Physical stride between consecutive elements.
 * @param[in] partitionvec Partitionvector object.
 *
 * @return A vector view object if success, or an error message otherwise.
 */
poski_vec_t poski_Init_Vector(poski_value_t *vec, poski_index_t length, poski_int_t inc, poski_partitionvec_t *partitionvec)
{
	poski_vec_t vecview = (poski_vec_t)malloc(sizeof(poski_vecview_struct_t)*1);
	if (vecview == NULL)
	{
		poski_error("poski_Init_Vector", "can not allocate the memory for vector-view object");
		return NULL;
	}

	vecview->vec = vec;
	vecview->length = length;
	vecview->inc = inc;
	if (partitionvec==NULL)
		vecview->npartitions = 1;
	else
		vecview->npartitions = partitionvec->A->partitionargs.npartitions;
	vecview->partitionvec = partitionvec;
	vecview->subvector = NULL;

	return vecview;
}

/**
 * @brief [library's internal use] Find the vector property for reporting.
 *
 * @internal
 * @param[in] vecprop Vector property in {INPUTVEC, OUTPUTVEC}.
 *
 * @return The current vector property if success, or an error message otherwise.
 */
char *poski_findvecprop(poski_vecprop_t vecprop)
{
	switch (vecprop)
	{
		case (INPUTVEC): return "INPUT"; break;
		case (OUTPUTVEC): return "OUTPUT"; break;
		default: return "Invalid vetor prop"; break;
	}
}

/**
 * @brief [library's internal use] Find the vector storage layout for reporting.
 *
 * @internal
 * @param[in] storage Vector storage layout in {LAYOUT_COLMAJ, LAYOUT_ROWMAJ}.
 *
 * @return The current vector storage layout if success, or an error message otherwise.
 */
char *poski_findvectorstorage(poski_storage_t storage)
{
	switch (storage)
	{
		case (LAYOUT_COLMAJ): return "LAYOUT_COLMAJ"; break;
		case (LAYOUT_ROWMAJ): return "LAYOUT_ROWMAJ"; break;
		default: return "Invalid vetor storage"; break;
	}
}

/**
 * @brief [library's internal use] Error-checking for vector property.
 *
 * @internal
 * @param[in] vecprop Vector property in {INPUTVEC, OUTPUTVEC}.
 *
 * @return 1 if success, or an error message otherwise.
 */
int poski_Check_vecprop(poski_vecprop_t vecprop)
{
	if ( (vecprop != INPUTVEC) && (vecprop != OUTPUTVEC) )
	{
		assert(0);
	}
	return 0; 	// success!
}

/**
 * @brief [library's internal use] Error-checking for a vector.
 *
 * @internal 
 * @param[in] x Vector data.
 * @param[in] length Logical vector length.
 * @param[in] inc Physical stride between consecutive elements.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_Check_vector(poski_value_t *x, poski_index_t length, poski_int_t inc)
{
	if ( (x==NULL) || (length < 1) || (inc < 1) )
	{
		return POSKI_INVALID;
	}
	return 0;
}

/** 
 * @brief [library's internal use] Error-checking input argument for creating a vector view object.
 *
 * @internal
 * @param[in] vec Vector data.
 * @param[in] length Logical vector length.
 * @param[in] inc Physical stride between consecutive elements.
 * @param[in] partitionvec Partitionvector object. 
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_Check_CreateVecViewInput(poski_value_t *vec, poski_index_t length, poski_int_t inc, poski_partitionvec_t *partitionvec)
{
	if (poski_Check_vector(vec, length, inc) < 0) return POSKI_INVALID;
//	if (poski_Check_partitionvec(partitionvec) < 0) return POSKI_INVALID;

	return 0;
}

/**
 * @brief [library's internal use] Error-checking for a vector-view object.
 *
 * @internal
 * @param[in] vec Vector-view object.
 * @param[in] length Logical vector length.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_Check_Vector(poski_vec_t vec, poski_index_t length)
{
//	if (vec==NULL) assert(0);
//	if (vec->length != length) assert(0);
	return 0;
}

/**
 * @brief Report information of a subvector object.
 *
 * @param[in] subvec Subvector object.
 *
 * @return
 */
void poski_report_subvec(poski_vecview_t subvec)
{
	printf("\t# sub vector information #\n");
	printf("\t+ num_rows = %d\n", subvec->num_rows);
	printf("\t+ num_cols = %d\n", subvec->num_cols);
	printf("\t+ orient = %s\n", poski_findvectorstorage(subvec->orient));
	printf("\t+ stride = %d\n", subvec->stride);
	printf("\t+ rowinc = %d\n", subvec->rowinc);
	printf("\t+ colinc = %d\n", subvec->colinc);
	if (subvec->val != NULL) printf("\t+ val = %p\n", subvec->val);
}


