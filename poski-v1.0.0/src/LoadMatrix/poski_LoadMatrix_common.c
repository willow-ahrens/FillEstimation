 /** 
 * @file src/LoadMatrix/poski_LoadMatrix_common.c
 * @brief Commonly used routines in the poski_LoadMatrixHandler's routines.
 *
 *  Also, refer
 * @ref poski_LoadMatrixHandler.c, 
 * @ref poski_LoadMatrix_HB_to_CSR.c, 
 * @ref poski_LoadMatrix_MM_to_CSR.c
 */

#include <stdio.h>

#include "../../include/poski/poski_matrixtype.h"

//-----------------------------------------------------------------------------------------
// Internal functions
/**
 * @brief Initialize a sparse matrix in CSR format.
 *
 * @internal
 * @param[in,out] SpA A sparse matrix in CSR format
 */
void poski_InitSparseMatrix(poski_sparse_matrix_t *SpA)
{
	SpA->nnz = 0;
	SpA->nrows = 0;
	SpA->ncols = 0;
	SpA->Aval = NULL;
	SpA->Aptr = NULL;
	SpA->Aind = NULL;
}
// Internal functions
//-----------------------------------------------------------------------------------------




