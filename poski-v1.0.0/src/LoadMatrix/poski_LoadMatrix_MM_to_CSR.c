/**
 * @file src/LoadMatrix/poski_LoadMatrix_MM_to_CSR.c
 * @brief Read the pattern of a file stored in Matrix-Market format.
 *
 *  This module implemets the routines to read the pattern of a file stored in Matrix-Market format.
 *
 * @attention Called by
 * @ref poski_LoadMatrixHandler.c
 *
 *  Also, refer
 * @ref poski_LoadMatrix_common.c
 */

#include <stdio.h>
#include <stdlib.h>

#include <poski/poski_loadmatcommon.h>
#include <poski/poski_malloc.h>
#include <poski/poski_print.h>

//-----------------------------------------------------------------------------------------
// Internal functions
/**
 * @brief Read the pattern of a file stored in Matrix-Market format.
 *
 * @internal 
 * @param[in] fname File to read
 * @param[in] MakeUnSymmetric
 *
 * @return A sparse matrix in CSR format if success, or an error message otherwise.
 */
poski_sparse_matrix_t *poski_LoadMatrix_MM_to_CSR(char *fname, poski_index_t MakeUnsymmetric)
{
	FILE *fp;
	char temp[10];

	poski_printMessage(2, "Matrix information...\n");

	poski_sparse_matrix_t *SpA = (poski_sparse_matrix_t *)poski_malloc(poski_sparse_matrix_t, 1, 0);

	poski_InitSparseMatrix(SpA);

	fp = fopen(fname,"r");
	if(fp==NULL)
	{
		poski_error("poski_LoadMatrix_MM_to_CSR", "can not open the file %s\n",fname);
	}
	else
	{
		poski_printMessage(3, "Read %s MM file into CSR format\n",fname);
	}

	// Fix it!
	poski_warning("poski_LoadMatrix_MM_to_CSR", "Matrix-Market(MM) format is not supported yet");

	return(SpA);

}
// Internal functions
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// Test functions
/**
 * @brief Test the poski_LoadMatrix_MM_to_CSR's routine.
 *
 *  This routine is for testing the functionality of the poski_LoadMatrix_MM_to_CSR's routine.
 *
 *  @return 0 if success, or an error message otherwise.
 */
int test_main_poski_LoadMatrix_MM_to_CSR(char *fname)
{
	TEST_LINECUT(1);
	poski_printMessage(0, "\tTEST: checking poski_LoadMatrix_MM_to_CSR...\n");
	poski_printMessage(0, "\tWARNING: It might be an error with MM format currently...\n");

	char *matfile = NULL;

	if (matfile != NULL) 
	{
		poski_printMessage(0, "\tTEST: error on initializing matrix file!\n");
		exit(0);
	}
	
	matfile = fname;
	
	if (matfile == NULL)
	{	
		poski_printMessage(0, "\tTEST: error on initializing matrix file!\n");
		exit(0);
	}

	poski_sparse_matrix_t *SpA = NULL;
	
	if (SpA != NULL) 
	{
		poski_printMessage(0, "\tTEST: error on initializing sparse matrix!\n");
		exit(0);
	}

	SpA = poski_LoadMatrix_MM_to_CSR(fname, 1);
	
	if (SpA == NULL) 
	{
		poski_printMessage(0, "\tTEST: error on poski_LoadMatrix_MM_to_CSR!\n");
		exit(0);
	}
	
	free(SpA);

	poski_printMessage(0, "\tTEST: success on poski_LoadMatrix_MM_to_CSR!\n");

	return 0;
}
// Test functions
//-----------------------------------------------------------------------------------------
