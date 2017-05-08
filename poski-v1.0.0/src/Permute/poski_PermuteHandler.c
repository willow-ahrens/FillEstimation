/**
 * @file src/Permute/poski_PermuteHandler.c
 * @brief Handle the permuting routines.
 *
 *  This module implements the routines to permute a sparse matrix.
 * 
 * @attention Should fix it!
 */

#include <stdio.h>
#include <stdlib.h>

#include <poski/poski_commontype.h>
#include <poski/poski_permute.h>
#include <poski/poski_matrixtype.h>
#include <poski/poski_vectortype.h>

//-----------------------------------------------------------------------------------------
// Internal functions
/** 
 * @brief [library's internal use] Initialize permute object.
 *
 * @internal
 *
 * @param[in, out] permute 
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_Init_GlobalPermute(poski_index_t *permute)
{
	permute = NULL;
	return 0;
}

/**
 * @brief [library's internal use] Report the information of permutation.
 *
 * @internal
 *
 * @param[in] permute 
 * @param[in] name
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_report_permute(poski_index_t *permute, char *name)
{
	// error check
//	if (permute==NULL) { printf("*! Warning: %s permute is not set!\n", name); return 1;}

	printf("\t\t+ %s permute = %p\n", name, permute);
	return 0;	// success!
}

/** 
 * @brief [library's internal use] Perfoms a permuting operation for a matrix-vector kernel.
 *
 * @param[in] vec Vector view object.
 * @return 0 if success, or an error message otherwise.
 */
int poski_permuteMatMult(poski_vec_t vec)
{
	return 0;
}
// Internal functions
//-----------------------------------------------------------------------------------------


