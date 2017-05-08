 /**
 * @file src/LoadMatrix/poski_LoadMatrixHandler.c
 * @brief Handle the pattern of a file stored in Harwell-Boeing or Matrix-Market format.
 * 
 *  This module implements the routines to read the pattern of a file stored in Harwell-Boeing or Matrix-Market format.
 *  
 * @attention Currently, only support Harwel-Boeing format.
 *  (1) Harwell-Boeing (HB) format: Modified from the HB format loader routines written by Sam Williams \<swwilliams@lbl.gov\>.
 *  (2) Matrix-Market (MM) format: not supported yet.
 *
 *  Also, refer
 * @ref poski_LoadMatrix_common.c, 
 * @ref poski_LoadMatrix_HB_to_CSR.c,
 * @ref poski_LoadMatrix_MM_to_CSR.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#include <poski/poski_loadmatcommon.h>
#include <poski/poski_print.h>

//-----------------------------------------------------------------------------------------
// User interface
/**
 * @brief Read the pattern of a file stored in Harwell-Boeing or Matrix-Market format, 
 * then store a sparse matrix in CSR format.
 *
 * @param[in] matfile File to read
 * @param[in] MatfileFormat File format in {HB, MM}
 * 
 * @return A sparse matrix in CSR format if success, or an error message otherwise.
 */
poski_sparse_matrix_t *poski_LoadMatrixFile_to_CSR(char *matfile, poski_MatFormat_t MatfileFormat)
{
	// (1) error-check
	if(matfile == NULL) 
	{
		poski_error("poski_LoadMatrixFile_to_CSR", "no file information");
		return NULL;
	}
	
	// (2) load a sparse matrix file (HB, MM) into CSR format
	poski_sparse_matrix_t *SpA;
	switch(MatfileFormat)
	{
		case HB: 
			poski_printMessage(1,"Loading Harwell-Boeing Format into CSR...\n");
			SpA = poski_LoadMatrix_HBF_to_CSR(matfile,1);
			break;
		case MM:
			poski_printMessage(1, "Loading Matrix-Market Format into CSR...\n");
			SpA = poski_LoadMatrix_MM_to_CSR(matfile,1);
			break;
		default: 
			poski_error("poski_LoadMatrixFile_to_CSR", "unknown matrix format (MM for Matrix-Market, HB for Harwell-Boeing)");
			return NULL;
			break;
	}

	return SpA;	// success!
}
// User interface
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// Test functions
/**
 * @brief Test the poski_LoadMatrixHandler's routines.
 *
 *  This routine is for testing the functionality of the poski_LoadMatrixHnalder's routines.
 *
 * @return 0 if success, or an error message otherwise.
 */
int test_main_poski_LoadMatrixFileHandler(char *matfile)
{
	LINECUT(0);
	printf("!* TEST: poski_LoadMatrixHandler...\n");

	poski_sparse_matrix_t *S1;

	LINECUT(1);
	printf("!*\tTEST: checking poski_LoadMatrixFile_to_CSR with HB format...\n");	
	S1 = poski_LoadMatrixFile_to_CSR(matfile, HB);
	if (S1 == NULL) 
	{
		fprintf(stderr, "!*\tTEST: error on poski_LoadMatrixFile_to_CSR with HB format!\n");
		assert(0);
	}
	free(S1);
	printf("!*\tTEST: success on poski_LoadMatrixFile_to_CSR with HB format...\n");	

	LINECUT(1);
	printf("!*\tTEST: checking poski_LoadMatrixFile_to_CSR with MM format...\n");	
	printf("!*\tWARNING: It might be an error with MM format currently...\n");
	S1 = poski_LoadMatrixFile_to_CSR(matfile, MM);
	if (S1 == NULL) 
	{
		fprintf(stderr, "!*\tTEST: error on poski_LoadMatrixFile_to_CSR with MM format!\n");
		assert(0);
	}
	free(S1);
	printf("!*\tTEST: success on poski_LoadMatrixFile_to_CSR with HB format...\n");	

	test_main_poski_LoadMatrix_HBF_to_CSR(matfile);
	test_main_poski_LoadMatrix_MM_to_CSR(matfile);
	
	LINECUT(1);
	printf("!* TEST: Success on poski_LoadMatrixHandler!\n");
	
	return 0;	
}
// Test functions
//-----------------------------------------------------------------------------------------
