/**
 * @file /src/Matrix/poski_Matrix_getset.c
 * @brief Get/Set values routines in Matrix Handler's modules. 
 *
 *  This module implements the routines to get/set values in matrix handler's modules.
 *
 * @attention These routines are usually for the library's internal use
 *  and should not normally be called directly by users.
 * @attention Called by
 * @ref poski_MatrixHandler.c
 *
 *  Also, refer
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <poski/poski_threadcommon.h>
#include <poski/poski_matrixcommon.h>
#include <poski/poski_partition.h>
#include <poski/poski_partitioncommon.h>
#include <poski/poski_malloc.h>
#include <poski/poski_print.h>

int
oski_pOSKIGetMatReprEntry (const void *mat, const oski_matcommon_t * props,
		      oski_index_t row, oski_index_t col,
		      oski_value_t * p_value)
{
	const oski_matCSR_t *A = (const oski_matCSR_t *) mat;
	oski_index_t b = A->base_index;

	oski_value_t aij;
	oski_index_t i0, j0, k;
	int transposed;

	assert (A != NULL);

	if (p_value == NULL)
	{
		oski_HandleError (ERR_BAD_ARG,
				"Nowhere to put return value", __FILE__, __LINE__,
				"Parameter #%d to parameter %s() is NULL",
				5, MACRO_TO_STRING (oski_GetMatReprEntry));
		return ERR_BAD_ARG;
	}

	/* Quick return cases */
	if (row == col && A->has_unit_diag_implicit)
	{
		VAL_SET_ONE (aij);
		VAL_ASSIGN (*p_value, aij);
		return 0;
	}

	VAL_SET_ZERO (aij);
	if (props->pattern.is_tri_upper && col < row)
	{
		VAL_ASSIGN (*p_value, aij);
		return 0;
	}
	if (props->pattern.is_tri_lower && col > row)
	{
		VAL_ASSIGN (*p_value, aij);
		return 0;
	}

	/* Otherwise, need to scan to compute. */

	/* Normalize row/column index to match storage if symmetric */
	if ((props->pattern.is_symm || props->pattern.is_herm) && (A->stored.is_upper != A->stored.is_lower)	/* half-storage */
			&& ((A->stored.is_upper && col < row)
				|| (A->stored.is_lower && col > row)))
	{
		transposed = 1;
		i0 = col;
		j0 = row;
	}
	else
	{
		transposed = 0;
		i0 = row;
		j0 = col;
	}

	for (k = A->ptr[i0 - 1] - b; k < A->ptr[i0] - b; k++)
	{
		oski_index_t j = A->ind[k] + 1 - b;	/* 1-based */
		if (j == j0)
			VAL_INC (aij, A->val[k]);
	}

	if (transposed && props->pattern.is_herm && (i0 != j0))
		VAL_CONJ (aij);

	VAL_ASSIGN (*p_value, aij);
	return 0;
}


/**
 * @brief [library's internal use] Check the partition of the specified matrix element.
 *
 * @internal
 * @param[in] A_tunable The tunable matrix object.
 * @param[in] row The specified row of the element whose value is to be modified.
 * @param[in] col The specified column of the element whose value is to be modified.
 *
 * @return a vaild partition number if success, or -1 otherwise.
 */
static int poski_Check_partitionNum(poski_mat_t A_tunable, poski_int_t row, poski_int_t col)
{
	int npartitions = A_tunable->partitionargs.npartitions;
	poski_index_t row_start, row_end, nrows;
	poski_partitionmat_t *partitionmat;

	int pid;
	for (pid=0;pid<npartitions;pid++)
	{
		partitionmat = A_tunable->submatrix[pid].partitionmat;
		row_start = partitionmat->row_start;
		row_end = partitionmat->row_end;		
		nrows = partitionmat->nrows;
		if ( (row_start <= row) && (row <= row_end) )
		{
			if (A_tunable->partitionargs.ptype==SemiOneD)
			{
				// TODO: fix it with more efficient condition of column
				if ( row < row_end ) 
				{	
					return pid;
				}
				else	// check in the last row
				{
					if ((nrows > 1) && (col <= partitionmat->submat.Aind[partitionmat->nnz-1]))	// the partition has more than 1 rows
					{
						return pid;
					}
					else if (pid == (npartitions-1))	// the last partition
					{
						return pid;
					}
					else	// single row && the partition starts in middle of column
					{
						if (partitionmat->submat.Aind[0] <= col && col <= partitionmat->submat.Aind[partitionmat->nnz-1]) 
							return pid;
					} 
				}
			}
			else	// OneD partition 
			{
				return pid;
			}
		}
	}
	return -1;
}

/**
 * @brief [library's internal use] Change the value of the specified matrix element.
 *
 * @internal
 * @param[in,out] A_tunable The tunable matrix object.
 * @param[in] row The specified row of the element whose value is to be modified.
 * @param[in] col The specified column of the element whose value is to be modified.
 * @param[in] val The specified value to modify.
 *
 * @return 0 if success, or an error message otherwise.
 */

static int SetMatEntry (const oski_matspecific_t *mat, const oski_matcommon_t *props, poski_int_t row, poski_int_t col, poski_value_t new_val)
{
	oski_SetMatReprEntry_funcpt func_Set;
	
	if (mat->type_id == INVALID_ID)
	{
		return ERR_BAD_ID;
	}
	func_Set = OSKI_MATTYPEID_METHOD (mat->type_id, SetMatReprEntry);
	if (func_Set == NULL)
	{
		OSKI_ERR_MATTYPEID_MEMBER (oski_SetMatEntry, mat->type_id, SetMatReprEntry);
		return ERR_NOT_IMPLEMENTED;
	}
	return (*func_Set) (mat->repr, props, row, col, new_val);
}

static int poski_SetMatEntry_va(const poski_matstruct_t A_tunable, poski_int_t row, poski_int_t col, poski_value_t new_val)
{
	int err_input, err_tuned;
	
	err_input = SetMatEntry (&(A_tunable->input_mat), &(A_tunable->props), row, col, new_val);

	if (err_input != ERR_BAD_ID)
	{
		return err_input;	
	}

	err_tuned  = SetMatEntry (&(A_tunable->tuned_mat), &(A_tunable->props), row, col, new_val);
	
	if (err_tuned != ERR_BAD_ID)
	{
		return err_tuned;	
	}

	return err_input;
}

int poski_SetMatEntry_run(poski_mat_t A_tunable, poski_int_t row, poski_int_t col, poski_value_t val)
{	
	// check whether the entry location is out of bound of not (one-index-based).
	if (poski_Check_Matrix_location(A_tunable, row-1, col-1))
	{ 
		poski_error("poski_SetMatEntry","the location of entry (%d,%d) is out of bound of A[%d, %d] (one-index-based)", (int)row, (int)col, (int)A_tunable->nrows, (int)A_tunable->ncols);
		return POSKI_INVALID;	// error: the entry is out of bound.
	}

	// check which partition includes the entry location
	int pid;
	pid = poski_Check_partitionNum(A_tunable, row-1, col-1);
	if (pid < 0) 
	{
		poski_error("poski_SetMatEntry","can't find the partition which holds the entry (%d,%d)", row, col); 
		return POSKI_INVALID;	// error: can't find the submatrix.
	}
	else
	{
		// TODO: make sure it works with other partitioning models (such as TwoD)
		// check whether the entry location is logical zero or not

		poski_printMessage(3, "+ setting the new value %lf in A(%d,%d)...\n", (double)val, (int)row, (int)col);	

		int row_start = A_tunable->submatrix[pid].partitionmat->row_start;
		poski_SetMatEntry_va(A_tunable->submatrix[pid].submat, (row-row_start), (col), val); 
	}
	return 0;
}

/**
 * @brief [library's internal use] Returns the value of the specified matrix element in A(row,col).
 *
 * @internal
 * @param[in] A_tunable The tunable matrix object.
 * @param[in] row The specified row of the element whose value is to be returned.
 * @param[in] col The specified column of the element whose value is to be returned.
 *
 * @return  the value of A(row,col) if success, or an error message otherwise.
 */
static int GetMatEntry (const oski_matspecific_t *mat, const oski_matcommon_t *props, poski_int_t row, poski_int_t col, poski_value_t *p_aij)
{
	oski_GetMatReprEntry_funcpt func_Get;
	func_Get = OSKI_MATTYPEID_METHOD (mat->type_id, GetMatReprEntry);
	if (func_Get == NULL)
		return ERR_NOT_IMPLEMENTED;
	return (*func_Get) (mat->repr, props, row, col, p_aij);
}

static poski_value_t poski_GetMatEntry_va (const poski_matstruct_t A_tunable, poski_int_t row, poski_int_t col)
{
	int err;
	poski_value_t a_ij;
	VAL_SET_ZERO(a_ij);
	
	err = GetMatEntry (&(A_tunable->input_mat), &(A_tunable->props), row, col, &a_ij);
	if (!err)
	{
		return a_ij;	// return the value from the input matrix.
	}

	err = GetMatEntry (&(A_tunable->tuned_mat), &(A_tunable->props), row, col, &a_ij);
	if (!err)
	{
		return a_ij;	// return the value from the tuned matrix.
	}

	if (err == ERR_NOT_IMPLEMENTED)
	{
		oski_id_t id;
		if (A_tunable->input_mat.type_id != INVALID_ID)
			id = A_tunable->input_mat.type_id;
		else
			id = A_tunable->tuned_mat.type_id;
		OSKI_ERR_MATTYPEID_MEMBER (oski_GetMatEntry, id, GetMatReprEntry);
	}

	return a_ij;	// success!

}
poski_value_t poski_GetMatEntry_run(poski_mat_t A_tunable, poski_int_t row, poski_int_t col)
{

	// check whether the entry location is out of bound of not (one-index-based).
	if (poski_Check_Matrix_location(A_tunable, row-1, col-1))
	{ 
		poski_error("poski_GetMatEntry","the location of entry (%d,%d) is out of bound of A[%d, %d] (one-index-based)", (int)row, (int)col, (int)A_tunable->nrows, (int)A_tunable->ncols);
		return POSKI_INVALID;	// error: the entry is out of bound.
	}

	int pid;
	poski_value_t val = 0.0;
	pid = poski_Check_partitionNum(A_tunable, row-1, col-1);
	if (pid < 0) 
	{
	 	poski_error("poski_GetMatEntry","can't find the partition which holds the entry (%d,%d)", row, col);
		return val; 
	}
	else
	{	
		int row_start = A_tunable->submatrix[pid].partitionmat->row_start;
		val = poski_GetMatEntry_va(A_tunable->submatrix[pid].submat, (row-row_start), (col)); 
		poski_printMessage(3, "+ getting the value of a(%d,%d) = %lf\n", (int)row, (int)col, (double)val);
	}
	return val;
}

/**
 * @brief [library's internal use] Set a subset of non-zero values as a clique or indexed list.
 *
 * @internal
 * This imitates the USCR_INSERT_CLIQUE Sparse BLAS standard
 * interface.  See p. 128 (Chapter 3) of the <a
 * href="http://www.netlib.org/blas/blast-forum/">BLAS standard.</a> A
 * subset is typically how one defines an element for matrix assembly
 * in the finite element method.
 *
 * If insertion of any entry of the subset does not succeed, the
 * entry is not changed.
 *
 * @param A_tunable [in/out] The tunable matrix object.
 * @param numRows [in] Number of rows in the subset.
 * @param numCols [in] Number of columns in the subset.
 * @param vals [in] Dense matrix storing the subset entries.
 *   vals[i*rowStride + j*colStride] is entry (i,j) of the dense
 *   matrix.  That entry is to be inserted in position (rows[i], 
 *   cols[j]) of the sparse matrix.
 * @param rowStride [in] Stride between rows of vals.
 * @param colStride [in] Stride between columns of vals.
 * @param rows [in] Row indices of the subset.
 * @param cols [in] Column indices of the subset.
 * 
 * @return 0 if successful, else nonzero.
 */

/* poski_SetMatSubset_ArrayAll()
 * Ex) 	numRows=numCols=2
 *     	rowStride=colStride=1
 *	rows={1,2}, cols={3,4}
 *	vals={1.0,2.0}
 *	Try to set if all of A(i,j) is either original non-zero or explicit zero
 *	else no change in A	
 *		A(1,3) = 1.0
 *		A(2,4) = 2.0
 */
static int poski_SetMatSubset_ArrayAll (poski_mat_t A_tunable, const int numRows, const int numCols, const poski_value_t* __restrict vals, const int rowStride, const int colStride, const poski_int_t* __restrict rows, const poski_int_t* __restrict cols)
{
	// (1) check all entries to be set

	// (2) if all entries are either original non-zero or explicit zero, then set all entries
	//     else return unchanged matrix A_tunable
	int err = 0; 

	int i, j;
	for (i = 0; i < numRows; i++) 
	{
		const poski_value_t curVal = vals[i];
		const poski_int_t curRow = rows[i];
		const poski_int_t curCol = cols[i];

		err = poski_SetMatEntry_run (A_tunable, curRow, curCol, curVal);
		if (err != 0) {
			/* Inserting the current entry failed. */
			poski_warning("poski_SetMatSubset","fail to set (%d,%d) entry with value %lf",(int)curRow,(int)curCol,(double)curVal);
		}
	}
	return 0;	// success!
}
/* poski_SetMatSubset_ArrayEntries()
 * Ex) 	numRows=numCols=2
 *     	rowStride=colStride=1
 *	rows={1,2}, cols={3,4}
 *	vals={1.0,2.0}
 *	Try to set if A(i,j) is either original non-zero or explicit zero
 *		A(1,3) = 1.0
 *		A(2,4) = 2.0
 */

static int poski_SetMatSubset_ArrayEntries (poski_mat_t A_tunable, const int numRows, const int numCols, const poski_value_t* __restrict vals, const int rowStride, const int colStride, const poski_int_t* __restrict rows, const poski_int_t* __restrict cols)
{
	int err = 0; 

	int i, j;
	for (i = 0; i < numRows; i++) 
	{
		const poski_value_t curVal = vals[i];
		const poski_int_t curRow = rows[i];
		const poski_int_t curCol = cols[i];
		err = poski_SetMatEntry_run (A_tunable, curRow, curCol, curVal);
		if (err != 0) {
			/* Inserting the current entry failed. */
			poski_warning("poski_SetMatSubset","fail to set (%d,%d) entry with value %lf",(int)curRow,(int)curCol,(double)curVal);
		}
	}
	return 0;	// success!
}

/* poski_SetMatSubset_BlockAll()
 * Ex) 	numRows=2, numCols=2
 *     	rowStride=2, colStride=1
 *	rows={1,2}, cols={2,3}
 *	vals={1.0,2.0,3.0,4.0}
 *	Try to set if all of A(i,j) is either original non-zero or explicit zero
 *	else no change in A	
 *		A(1,2) = 1.0
 *		A(1,3) = 2.0
 *		A(2,2) = 3.0
 *		A(2,3) = 4.0
 */
static int poski_SetMatSubset_BlockAll (poski_mat_t A_tunable, const int numRows, const int numCols, const poski_value_t* __restrict vals, const int rowStride, const int colStride, const poski_int_t* __restrict rows, const poski_int_t* __restrict cols)
{
	// (1) check all entries to be set

	// (2) if all entries are either original non-zero or explicit zero, then set all entries
	//     else return unchanged matrix A_tunable
	
	int err = 0; 

	int i, j;
	for (i = 0; i < numRows; i++) {
		for (j = 0; j < numCols; j++) {
			const int valInd = i * rowStride + j * colStride;
			const poski_value_t curVal = vals[valInd];
			const poski_int_t curRow = rows[i];
			const poski_int_t curCol = cols[j];
			err = poski_SetMatEntry_run (A_tunable, curRow, curCol, curVal);
			if (err != 0) {
				/* Inserting the current entry failed.  Return one plus the
				   first index that failed.  Do not promise this in the
				   interface of the function, because a different
				   implementation might not insert entries one at a time. */
				return POSKI_INVALID;
			}
		}
	}
	return 0;	// success!
}

/* poski_SetMatSubset_BlockEntries()
 * Ex) 	numRows=2, numCols=2
 *     	rowStride=2, colStride=1
 *	rows={1,2}, cols={2,3}
 *	vals={1.0,2.0,3.0,4.0}
 *	Try to set if A(i,j) is either original non-zero or explicit zero	
 *		A(1,2) = 1.0
 *		A(1,3) = 2.0
 *		A(2,2) = 3.0
 *		A(2,3) = 4.0
 */

static int poski_SetMatSubset_BlockEntries (poski_mat_t A_tunable, const int numRows, const int numCols, const poski_value_t* __restrict vals, const int rowStride, const int colStride, const poski_int_t* __restrict rows, const poski_int_t* __restrict cols)
{
	int err = 0; 

	int i, j;
	for (i = 0; i < numRows; i++) {
		for (j = 0; j < numCols; j++) {
			const int valInd = i * rowStride + j * colStride;
			const poski_value_t curVal = vals[valInd];
			const poski_int_t curRow = rows[i];
			const poski_int_t curCol = cols[j];
			err = poski_SetMatEntry_run (A_tunable, curRow, curCol, curVal);
			if (err != 0) {
				/* Inserting the current entry failed. */
				poski_warning("poski_SetMatSubset","fail to set (%d,%d) entry with value %lf",(int)curRow,(int)curCol,(double)curVal);
			}
		}
	}
	return 0;	// success!
}

int poski_SetMatSubset_run (poski_mat_t A_tunable, const int numRows, const int numCols, const poski_value_t* vals, const int rowStride, const int colStride, const poski_int_t* rows, const poski_int_t* cols, poski_subsettype_t subsettype)
{
	
	if (subsettype==POSKI_BLOCKALL)
		poski_SetMatSubset_BlockAll (A_tunable, numRows, numCols, vals, rowStride, colStride, rows, cols);
	else if (subsettype==POSKI_BLOCKENTRIES)
		poski_SetMatSubset_BlockEntries (A_tunable, numRows, numCols, vals, rowStride, colStride, rows, cols);
	else if (subsettype==POSKI_ARRAYALL)
		poski_SetMatSubset_ArrayAll (A_tunable, numRows, numCols, vals, rowStride, colStride, rows, cols);
	else if (subsettype==POSKI_ARRAYENTRIES)
		poski_SetMatSubset_ArrayEntries (A_tunable, numRows, numCols, vals, rowStride, colStride, rows, cols);

	return 0;
}

/**
 * @brief [library's internal use] Get a subset of values as a clique or indexed list.
 *
 * @internal
 * @param A_tunable [in] The tunable matrix object.
 * @param numRows [in] Number of rows in the subset.
 * @param numCols [in] Number of columns in the subset.
 * @param vals [in/out] Dense matrix storing the subset entries.
 *   vals[i*rowStride + j*colStride] is entry (i,j) of the dense
 *   matrix.  That entry is to be inserted in position (rows[i], 
 *   cols[j]) of the sparse matrix.
 * @param rowStride [in] Stride between rows of vals.
 * @param colStride [in] Stride between columns of vals.
 * @param rows [in] Row indices of the subset.
 * @param cols [in] Column indices of the subset.
 * 
 * @return 0 if successful, else nonzero.
 */

/* poski_GetMatSubset_ArrayAll()
 * Ex) 	numRows=numCols=2
 *     	rowStride=colStride=1
 *	rows={1,2}, cols={3,4}
 *	vals[2]={...}
 *	Try to get all of A(i,j)
 *	else no change in A	
 *		vals[0] = A(1,3)
 *		vals[1] = A(2,4)
 */
static int poski_GetMatSubset_ArrayAll (poski_mat_t A_tunable, const int numRows, const int numCols, poski_value_t* vals, const int rowStride, const int colStride, const poski_int_t* __restrict rows, const poski_int_t* __restrict cols)
{
	// (1) check all entries to be set

	// (2) if all entries are either original non-zero or explicit zero, then set all entries
	//     else return unchanged matrix A_tunable
	int i, j;
	for (i = 0; i < numRows; i++) 
	{
		const poski_int_t curRow = rows[i];
		const poski_int_t curCol = cols[i];
		vals[i] = poski_GetMatEntry_run (A_tunable, curRow, curCol);
	}
	return 0;	// success!
}
/* poski_GetMatSubset_ArrayEntries()
 * Ex) 	numRows=numCols=2
 *     	rowStride=colStride=1
 *	rows={1,2}, cols={3,4}
 *	vals[2]={...}
 *	Try to get all of A(i,j)
 *		vals[0] = A(1,3)
 *		vals[1] = A(2,4)
 */

static int poski_GetMatSubset_ArrayEntries (poski_mat_t A_tunable, const int numRows, const int numCols, poski_value_t* vals, const int rowStride, const int colStride, const poski_int_t* __restrict rows, const poski_int_t* __restrict cols)
{
	int i, j;
	for (i = 0; i < numRows; i++) 
	{
		const poski_int_t curRow = rows[i];
		const poski_int_t curCol = cols[i];
		vals[i] = poski_GetMatEntry_run (A_tunable, curRow, curCol);
	}
	return 0;	// success!
}

/* poski_GetMatSubset_BlockAll()
 * Ex) 	numRows=2, numCols=2
 *     	rowStride=2, colStride=1
 *	rows={1,2}, cols={2,3}
 *	vals[4]={...}
 *	Try to get all of A(i,j)
 *	else no change in A	
 *		vals[0] = A(1,2)
 *		vals[1] = A(1,3)
 *		vals[2] = A(2,2)
 *		vals[3] = A(2,3)
 */
static int poski_GetMatSubset_BlockAll (poski_mat_t A_tunable, const int numRows, const int numCols, poski_value_t* vals, const int rowStride, const int colStride, const poski_int_t* __restrict rows, const poski_int_t* __restrict cols)
{
	// (1) check all entries to be set

	// (2) if all entries are either original non-zero or explicit zero, then set all entries
	//     else return unchanged matrix A_tunable
	
	int i, j;
	for (i = 0; i < numRows; i++) {
		for (j = 0; j < numCols; j++) {
			const int valInd = i * rowStride + j * colStride;
			const poski_int_t curRow = rows[i];
			const poski_int_t curCol = cols[j];
			vals[valInd] = poski_GetMatEntry_run (A_tunable, curRow, curCol);
		}
	}
	return 0;	// success!
}

/* poski_SetMatSubset_BlockEntries()
 * Ex) 	numRows=2, numCols=2
 *     	rowStride=2, colStride=1
 *	rows={1,2}, cols={2,3}
 *	vals[4]={...}
 *	Try to get A(i,j) 	
 *		vals[0] = A(1,2)
 *		vals[1] = A(1,3)
 *		vals[2] = A(2,2)
 *		vals[3] = A(2,3)
 */

static int poski_GetMatSubset_BlockEntries (poski_mat_t A_tunable, const int numRows, const int numCols, poski_value_t* vals, const int rowStride, const int colStride, const poski_int_t* __restrict rows, const poski_int_t* __restrict cols)
{
	int i, j;
	for (i = 0; i < numRows; i++) {
		for (j = 0; j < numCols; j++) {
			const int valInd = i * rowStride + j * colStride;
			const poski_int_t curRow = rows[i];
			const poski_int_t curCol = cols[j];
			vals[valInd] = poski_GetMatEntry_run (A_tunable, curRow, curCol);
		}
	}
	return 0;	// success!
}
int poski_GetMatSubset_run (poski_mat_t A_tunable, const int numRows, const int numCols, poski_value_t* vals, const int rowStride, const int colStride, const poski_int_t* rows, const poski_int_t* cols, poski_subsettype_t subsettype)
{
	// TODO: implement for BLOCKALL & ARRARYALL
	if (subsettype==POSKI_BLOCKALL)
		poski_GetMatSubset_BlockAll (A_tunable, numRows, numCols, vals, rowStride, colStride, rows, cols);
	else if (subsettype==POSKI_BLOCKENTRIES)
		poski_GetMatSubset_BlockEntries (A_tunable, numRows, numCols, vals, rowStride, colStride, rows, cols);
	else if (subsettype==POSKI_ARRAYALL)
		poski_GetMatSubset_ArrayAll (A_tunable, numRows, numCols, vals, rowStride, colStride, rows, cols);
	else if (subsettype==POSKI_ARRAYENTRIES)
		poski_GetMatSubset_ArrayEntries (A_tunable, numRows, numCols, vals, rowStride, colStride, rows, cols);

	return 0;
}


