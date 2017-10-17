/**
 * @file src/Partition/poski_Partition_common.c
 * @brief Commonly used routines in poski_PartitionHandler's routines.
 * 
 * @attention These routines are usually for the library's internal use
 * and should not normally be called directly by users.
 * @attention Called by
 * @ref poski_PartitionHandler.c
 *
 * Also, refer
 * @ref poski_Partition_OneD.c, 
 * @ref poski_Partition_SemiOneD.c
 */

#include <stdio.h>
#include <stdlib.h>

#include <poski/poski_common.h>
#include <poski/poski_partitioncommon.h>
#include <poski/poski_threadcommon.h>
#include <poski/poski_kernelcommon.h>
#include <poski/poski_matrixcommon.h>
#include <poski/poski_vectorcommon.h>
#include <poski/poski_malloc.h>
#include <poski/poski_print.h>
#include <poski/poski_permute.h>


// internal functions, but used by others
/**
 * @brief [library's internal use] Initialize or copy a partition object.
 *
 * @internal
 * This routine sets the partition object by default or from a given partition object.
 *
 * @param[in,out] dst Destination partition object.
 * @param[in] src Source partition object.
 * @param[in] nthreads Number of threads. 
 *
 * @return 0 if success, or an error message otherwise.
 */ 
int poski_Init_partitionargs(poski_partitionarg_t *dst, poski_partitionarg_t *src, poski_int_t nthreads)
{
	/* @ Set partitionargs */
	if (src == NULL) 
	{	/* @ Use default partitioning model (1D row-wise) */
		dst->ptype = OneD;
		dst->npartitions = nthreads;
		dst->kernel = KERNEL_MatMult;	
		dst->op = OP_NORMAL;
	}
	else 	
	{	/* @ Use a specified partitionging model by user's hint in partitionarg */
		poski_copyarg(dst, src, poski_partitionarg_t);
	}
	return 0;	// success!
}

/**
 * @brief [library's internal use] Initialize a partition-matrix object.
 *
 * @internal
 * @param[in,out] partitionmat Partition-matrix object.
 *
 * @return 1 if success, or an error message otherwise.
 */
int poski_Initpartitionmat(poski_partitionmat_t *partitionmat)
{
	partitionmat->npartitions = 0;
	partitionmat->pid = 0;
	partitionmat->row_start = 0;
	partitionmat->row_end = 0;
	partitionmat->col_start = 0;
	partitionmat->nnz = 0;
	partitionmat->nrows = 0;
	partitionmat->ncols = 0;
	partitionmat->permuteRows = NULL;
	partitionmat->permuteCols = NULL;
	partitionmat->submat.Aptr = NULL;
	partitionmat->submat.Aind = NULL;
	partitionmat->submat.Aval = NULL;
	partitionmat->submat.nrows = 0;
	partitionmat->submat.ncols = 0;
	partitionmat->submat.nnz = 0;

	return 1;	// success!
}

// internal functions
/** 
 * @brief [library's internal use] Adjust the number of partitions.
 *
 * @internal
 * @param[in] npartitions Number of current partitions.
 * @param[in] nrows Number of rows of a sparse matrix.
 *
 * @return Adjusted number of partitions if success, or an error message otherwise.
 */
static poski_int_t poski_adjust_npartition(poski_int_t npartitions, poski_index_t nrows)
{
	if (npartitions > nrows) 
	{
		npartitions = nrows;
	}

	return npartitions;	// success!	
}


/**
 * @brief [library's internal use] Adjust the number of threads and partitions.
 *
 * @internal
 * @param[in] ptype Specified partitioning type.
 * @param[in,out] npartitions Current number of partitions
 * @param[in,out] nthreads Current number of threads
 * @param[in] nrows Number of rows of a sparse matrix.
 * @param[in] nclos Number of columns of a sparse matrix.
 * @param[in] nnz Number of non-zeros of a sparse matrix.
 *
 * @return 1 if success, or an error message otherwise.
 */
// matrixhandler
int poski_Adjust_args(poski_partitiontype_t ptype, poski_int_t *npartitions, poski_int_t *nthreads, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz)
{
	// FIX it! more smart way!
	poski_index_t num;

	if (ptype == OneD) { num = nrows;}
	else if (ptype == SemiOneD) { num = nnz;}

	*nthreads = poski_adjust_npartition(*nthreads, num);
	*npartitions = poski_adjust_npartition(*npartitions, num);
	if (*nthreads > *npartitions) { *npartitions = *nthreads; }
	else if (*nthreads < *npartitions) 
	{ 
		if ( ( (*npartitions)%(*nthreads) ) != 0 ) { *npartitions = *npartitions - (*npartitions)%(*nthreads) ; }
	}
	return 0;	// success
}
/**
 * @brief [library's internal use] Create a partition-subvector object.
 *
 * @internal
 * @param[in] vec Vector data.
 * @param[in] length Logical vector length.
 * @param[in] inc Physical stride between consecutive elements.
 * @param[in] partitionvec partition-vector object.
 *
 * @return A valid partition-subvector object if success, or an error message otherwise.
 */
poski_subvec_t *poski_PartitionVec(poski_value_t *vec, poski_index_t length, poski_int_t inc, poski_partitionvec_t *partitionvec)
{
	poski_printMessage(2, "Partition vector...\n");
	if (partitionvec == NULL )	/* No partitioning vector <x> */
	{
		return (poski_subvec_t *)NULL;
	}
	else	/* Partitioning vector <x> depended on <partitionvec> */
	{
		poski_subvec_t *subvec;
		
		/* @ Create partitioned subvetors */
		poski_partitionvec_funcpt funcpt;
		funcpt = poski_find_partitionvec_funcpt(partitionvec->A->threadargs.ttype, partitionvec->A->partitionargs.ptype, partitionvec->kernel, partitionvec->op, partitionvec->vecprop);
		subvec = (poski_subvec_t *)(*funcpt)(vec,length, inc, partitionvec->A);

		return subvec;
	}
}
/**
 * @brief [library's internal use] Find the appropriate function pointer for partitioning a matrix.
 *
 * @internal
 * This routine finds the appropriate function for partitioning a matrix based on threading and partitioning models.
 *
 * @param[in] ttype Specified threading model type.
 * @param[in] ptype Specified partitioning model type.
 *
 * @return Function pointer of the appropriata function for partitioning a matrix if success, or an error message otherwise.
 */
poski_partitionmat_funcpt poski_find_partitionmat_funcpt(poski_threadtype_t ttype, poski_partitiontype_t ptype)
{
	if ( (ttype == POSKI_PTHREAD) && (ptype == OneD) )
		return &POSKI_MAKENAME(PartitionMat, Pthread, OneD,,,);
	else if ( (ttype == POSKI_PTHREAD) && (ptype == SemiOneD) )
		return &POSKI_MAKENAME(PartitionMat, Pthread, SemiOneD,,,);
	else if ( (ttype == POSKI_THREADPOOL) && (ptype == OneD) )
		return &POSKI_MAKENAME(PartitionMat, Threadpool, OneD,,,);
	else if ( (ttype == POSKI_THREADPOOL) && (ptype == SemiOneD) )
		return &POSKI_MAKENAME(PartitionMat, Threadpool, SemiOneD,,,);
	else if ( (ttype == POSKI_OPENMP) && (ptype == OneD) )
		return &POSKI_MAKENAME(PartitionMat, Openmp, OneD,,,);
	else if ( (ttype == POSKI_OPENMP) && (ptype == SemiOneD) )
		return &POSKI_MAKENAME(PartitionMat, Openmp, SemiOneD,,,);
	else
	{
		poski_error("poski_find_partitionmat_funcpt", "the type of thread or partition is not properly set");
		return NULL; 
	}
}

/**
 * @brief [library's internal use] Find the appropriate function pointer for partitioning a vector.
 *
 * @internal
 * This routine finds the appropriate function for partitioning a vector based on threading and partitioning models, kernel operation, and vector property.
 *
 * @param[in] ttype Specified threading model type.
 * @param[in] ptype Specified partitioning model type.
 * @param[in] kernel Specified kernel type.
 * @param[in] op Specifed transpose operation.
 * @param[in] vecprop Specified vector property.
 *
 * @return Function pointer of the appropriata function for partitioning a vector if success, or an error message otherwise.
 */
poski_partitionvec_funcpt poski_find_partitionvec_funcpt(poski_threadtype_t ttype, poski_partitiontype_t ptype, poski_kernel_t kernel, poski_operation_t op, poski_vecprop_t vecprop)
{
	// TODO: more efficient function name call
	// OP_NORMAL
	if ( ttype == POSKI_PTHREAD && ptype == OneD && kernel == KERNEL_MatMult && op == OP_NORMAL && vecprop == INPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Pthread, OneD, MatMult, Normal, Input);
	else if ( ttype == POSKI_PTHREAD && ptype == OneD && kernel == KERNEL_MatMult && op == OP_NORMAL && vecprop == OUTPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Pthread, OneD, MatMult, Normal, Output);
	else if ( ttype == POSKI_PTHREAD && ptype == SemiOneD && kernel == KERNEL_MatMult && op == OP_NORMAL && vecprop == INPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Pthread, SemiOneD, MatMult, Normal, Input);
	else if ( ttype == POSKI_PTHREAD && ptype == SemiOneD && kernel == KERNEL_MatMult && op == OP_NORMAL && vecprop == OUTPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Pthread, SemiOneD, MatMult, Normal, Output);
	else if ( ttype == POSKI_THREADPOOL && ptype == OneD && kernel == KERNEL_MatMult && op == OP_NORMAL && vecprop == INPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Threadpool, OneD, MatMult, Normal, Input);
	else if ( ttype == POSKI_THREADPOOL && ptype == OneD && kernel == KERNEL_MatMult && op == OP_NORMAL && vecprop == OUTPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Threadpool, OneD, MatMult, Normal, Output);
	else if ( ttype == POSKI_THREADPOOL && ptype == SemiOneD && kernel == KERNEL_MatMult && op == OP_NORMAL && vecprop == INPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Threadpool, SemiOneD, MatMult, Normal, Input);
	else if ( ttype == POSKI_THREADPOOL && ptype == SemiOneD && kernel == KERNEL_MatMult && op == OP_NORMAL && vecprop == OUTPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Threadpool, SemiOneD, MatMult, Normal, Output);
	else if ( ttype == POSKI_OPENMP && ptype == OneD && kernel == KERNEL_MatMult && op == OP_NORMAL && vecprop == INPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Openmp, OneD, MatMult, Normal, Input);
	else if ( ttype == POSKI_OPENMP && ptype == OneD && kernel == KERNEL_MatMult && op == OP_NORMAL && vecprop == OUTPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Openmp, OneD, MatMult, Normal, Output);
	else if ( ttype == POSKI_OPENMP && ptype == SemiOneD && kernel == KERNEL_MatMult && op == OP_NORMAL && vecprop == INPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Openmp, SemiOneD, MatMult, Normal, Input);
	else if ( ttype == POSKI_OPENMP && ptype == SemiOneD && kernel == KERNEL_MatMult && op == OP_NORMAL && vecprop == OUTPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Openmp, SemiOneD, MatMult, Normal, Output);
	// OP_TRANS
	else if ( ttype == POSKI_PTHREAD && ptype == OneD && kernel == KERNEL_MatMult && op == OP_TRANS && vecprop == INPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Pthread, OneD, MatMult, Trans, Input);
	else if ( ttype == POSKI_PTHREAD && ptype == OneD && kernel == KERNEL_MatMult && op == OP_TRANS && vecprop == OUTPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Pthread, OneD, MatMult, Trans, Output);
	else if ( ttype == POSKI_PTHREAD && ptype == SemiOneD && kernel == KERNEL_MatMult && op == OP_TRANS && vecprop == INPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Pthread, SemiOneD, MatMult, Trans, Input);
	else if ( ttype == POSKI_PTHREAD && ptype == SemiOneD && kernel == KERNEL_MatMult && op == OP_TRANS && vecprop == OUTPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Pthread, SemiOneD, MatMult, Trans, Output);
	else if ( ttype == POSKI_THREADPOOL && ptype == OneD && kernel == KERNEL_MatMult && op == OP_TRANS && vecprop == INPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Threadpool, OneD, MatMult, Trans, Input);
	else if ( ttype == POSKI_THREADPOOL && ptype == OneD && kernel == KERNEL_MatMult && op == OP_TRANS && vecprop == OUTPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Threadpool, OneD, MatMult, Trans, Output);
	else if ( ttype == POSKI_THREADPOOL && ptype == SemiOneD && kernel == KERNEL_MatMult && op == OP_TRANS && vecprop == INPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Threadpool, SemiOneD, MatMult, Trans, Input);
	else if ( ttype == POSKI_THREADPOOL && ptype == SemiOneD && kernel == KERNEL_MatMult && op == OP_TRANS && vecprop == OUTPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Threadpool, SemiOneD, MatMult, Trans, Output);
	else if ( ttype == POSKI_OPENMP && ptype == OneD && kernel == KERNEL_MatMult && op == OP_TRANS && vecprop == INPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Openmp, OneD, MatMult, Trans, Input);
	else if ( ttype == POSKI_OPENMP && ptype == OneD && kernel == KERNEL_MatMult && op == OP_TRANS && vecprop == OUTPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Openmp, OneD, MatMult, Trans, Output);
	else if ( ttype == POSKI_OPENMP && ptype == SemiOneD && kernel == KERNEL_MatMult && op == OP_TRANS && vecprop == INPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Openmp, SemiOneD, MatMult, Trans, Input);
	else if ( ttype == POSKI_OPENMP && ptype == SemiOneD && kernel == KERNEL_MatMult && op == OP_TRANS && vecprop == OUTPUTVEC )
		return &POSKI_MAKENAME(PartitionVec, Openmp, SemiOneD, MatMult, Trans, Output);
	else
		poski_error("poski_find_partitionvec_funcpt", "the type of thread, partition, kernel, operation, or vecprop is not properly set");

	poski_warning("poski_find_partitionvec_funcpt","can not find the function point");

	return NULL;
}

/**
 * @brief [library's internal use] Find the partitioning model type for reporing.
 *
 * @internal
 * @param[in] ptype Specified partitioning model type.
 *
 * @return The current partitioning model type if success, or an error message otherwise.
 */
char *poski_findpartitiontype(poski_partitiontype_t ptype)
{
	switch (ptype)
	{
		case (OneD): return "OneD"; break;
		case (SemiOneD): return "SemiOneD"; break;
		case (TwoD): return "TwoD"; break;
		case (Mondriaan): return "Mondriaan"; break;
		case (PaToH): return "PaToH"; break;
		default: return "Invalid Partition Type"; break;
	}
}/**
 * @brief [library's internal use] Error-checking for partition object.
 *
 * @internal
 * @param[in] partitionargs A partition object.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_Check_partitionargs(poski_partitionarg_t *partitionargs)
{
	if (partitionargs != NULL)
	{
		poski_Check_partitioningmodeltype(partitionargs->ptype);		
		poski_Check_numpartitions(partitionargs->npartitions);
	}
	return 0;
}

/**
 * @brief [library's internal use] Error-checking for the number of partitions.
 *
 * @internal
 * @param[in] npartitions Number of partitions
 *
 * @return 0 if success, or an error message otherwise.
 */ 
int poski_Check_numpartitions(poski_int_t npartitions)
{
	if (npartitions < 1) assert(0);

	return 0; 	// success!
}

/**
 * @brief [library's internal use] Error-checking for a partitioning model type.
 *
 * @internal
 * This routine checks a given partitioning model type is valid or not.
 *
 * @param[in] ptype Partitioning model type.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_Check_partitioningmodeltype(poski_partitiontype_t ptype)
{
	if ( (ptype != OneD) && (ptype != SemiOneD) && (ptype != TwoD) )
	{
		assert(0);
	}
	return 0; 	// sucess!
}

/**
 * @brief [library's internal use] Error-checking for a partition-vector object.
 *
 * @internal
 * @param[in] partitionvec A partition-vector object.
 *
 * @return 1 if success, or an error message otherwise
 */
int poski_Check_partitionvec(poski_partitionvec_t *partitionvec)
{
	if (partitionvec == NULL)
	{
		return POSKI_INVALID;
	}
	return 0;
}


/**
 * @brief [library's internal use] Error-checking input parameters for creating a partition-vector object.
 *
 * @internal
 * @param[in] A Tunable matrix object.
 * @param[in] kernel Specified kernel.
 * @param[in] op Specified transpose operation.
 * @param[in] vecprop Specified vector property.
 *
 * @return 0 if success, or -1 otherwise.
 */
int poski_Check_partitionedVec(poski_mat_t A, poski_vec_t vec, poski_kernel_t kernel, poski_operation_t op, poski_vecprop_t vecprop)
{
	if (vec->subvector==NULL || vec->partitionvec == NULL) return -1;
	if (vec->partitionvec->A != A) return -1;
	if (vec->partitionvec->kernel != kernel || vec->partitionvec->op != op || vec->partitionvec->vecprop != vecprop) return -1;

	return 0;
}























// user interfaces
// user interface for reporting
/**
 * @brief Report information of partitioning model.
 *
 * @param[in] partitionargs Partition object.
 * @param[in] A Tunable matrix object.
 *
 * @return 1 if success, or an error message otherwise.
 */
int poski_report_partitionmodel(poski_partitionarg_t *partitionargs, poski_mat_t A)
{
	printf("\t## Partitioning model ##\n");
	if (partitionargs==NULL) { printf("*! Warning: <partitionargs> is not set!\n"); return 1;}

	printf("\t\t+ ptype       = %s\n", poski_findpartitiontype(partitionargs->ptype));
	//printf("\t\t+ npartitions = %d\n", partitionargs->npartitions);
	//printf("\t\t+ kernel      = %s\n", poski_findkerneltype(partitionargs->kernel));
	//printf("\t\t+ op          = %s\n", poski_findoperation(partitionargs->op));

	return 0;	// success!
}

/**
 * @brief Report information of a single partition-matrix object.
 *
 * @param[in] partitionmat Partition-matrix object.
 */
void poski_report_partitioned(poski_partitionmat_t *partitionmat)
{
	printf("\t\t+ [Pid = %d] npartitions = %d, start = %d, end = %d\n", partitionmat->pid, partitionmat->npartitions, partitionmat->row_start, partitionmat->row_end);	
	printf("\t\t+            nrows = %d, ncols = %d, nnz = %d\n", partitionmat->nrows, partitionmat->ncols, partitionmat->nnz);
}

/**
 * @brief Report information of all partition-matrix object.
 *
 * @param[in] partitionmat Partition-matrix object.
 *
 */
void poski_report_allpartitioned(poski_partitionmat_t *partitionmat)
{
	int i;
	printf("\t## partitioned mat for %d submatrices ##\n", partitionmat->npartitions);
	for (i=0;i<partitionmat->npartitions;i++)
	{
		poski_report_partitioned(&partitionmat[i]);
	}
}

/**
 * @brief Report information of a partition-vector object.
 *
 * @param[in] partitionvec Partition-vector object.
 *
 */
void poski_report_partitionvec(poski_partitionvec_t *partitionvec)
{
	printf("\t# partition vector information #\n");
	printf("\t+ total npartitions = %d\n",partitionvec->npartitions);
	printf("\t+ pid = %d\n", partitionvec->pid);
	if (partitionvec->A != NULL) printf("\t+ matrix = %p\n",partitionvec->A);
	printf("\t+ kernel = %s\n", poski_findkerneltype(partitionvec->kernel));
	printf("\t+ op     = %s\n", poski_findoperation(partitionvec->op));
	printf("\t+ vecprop= %s\n", poski_findvecprop(partitionvec->vecprop));
}

/**
 * @brief Report information of a partition-matrix object.
 *
 * @param[in] partitionmat A single partition-matrix object.
 * @param[in] pid Specified partitions.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_report_partitionmat_va(poski_partitionmat_t *partitionmat, poski_int_t pid)
{
	// error check
	if (partitionmat == NULL) { printf("*! Warining: %d partitionmat is not set!\n", pid); return 1; }

	printf("\t     [%d-th partition information]\n",partitionmat->pid);
	printf("\t\t+ row range : %d ~ %d\n", partitionmat->row_start, partitionmat->row_end);
	printf("\t\t+ col point : Aind[%d]\n", partitionmat->col_start);
	printf("\t\t+ nnz       : %d\n", partitionmat->nnz);
	poski_report_permute(partitionmat->permuteRows, "ROW");
	poski_report_permute(partitionmat->permuteCols, "COL");

	poski_report_sparse_CSR(&partitionmat->submat);

	return 0;	// success!
}

/**
 * @brief Report information of a partition-matrix object.
 *
 * @param[in] submat A submatrix object.
 * @param[in] npartitions Number of partitions.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_report_partitionmat(poski_submat_t *submat, poski_int_t npartitions)
{
	// error check
	if (submat == NULL) { printf("*! Warning: <partitionmat> is not set!\n"); return 1; }
//	if (partitionmat->npartitions != npartitions) { printf("*! Warning: the number of partitions are not matching!\n"); return 1; }

	int i;
	printf("\t< Partition Mat >\n");
	for (i=0;i<npartitions;i++)
	{
		poski_partitionmat_t *partitionmat = submat[i].partitionmat;
		poski_report_partitionmat_va(partitionmat,i);
	}

	return 0;	// success!
}


