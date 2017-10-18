/**
 * @file src/Partition/poski_Partition_SemiOneD.c
 * @brief Handle SemiOne dimensional partitioning model.
 *
 * This module implements the routines to handle Semi-OneD dimensional partitioning model.
 *
 * @attention Called by
 * @ref poski_PartitionHandler.c
 *
 * Also, refer
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <poski/poski_partitioncommon.h>
#include <poski/poski_threadcommon.h>
#include <poski/poski_vectorcommon.h>
#include <poski/poski_matrixcommon.h>
#include <poski/poski_malloc.h>
#include <poski/poski_print.h>

/**
 * @brief [library's internal use] Create a submatrix object.
 *
 * @internal
 * @param[in] partitionmat Partition-matrix object.
 * @param[in] Aptr CSR row pointers.
 * @param[in] Aind CSR column pointers.
 * @param[in] Aval CSR non-zero values.
 * @param[in] Anrows Number of rows.
 * @param[in] Ancols Number of columns.
 * @param[in] Annz Number of non-zeros.
 *
 * @return A valid submatrix object if success, or an error message otherwise.
 */
poski_sparse_matrix_t *poski_CreateSubMatCSR_SemiOneD(poski_partitionmat_t *partitionmat, poski_index_t *Aptr, poski_index_t *Aind, poski_value_t *Aval, poski_index_t Anrows, poski_index_t Ancols, poski_index_t Annz, poski_copymode_t mode)
{
	// error check
	if (partitionmat == NULL) { poski_error("poski_CreateSubMatCSR_SemiOneD","invalid input parameters"); }
	if (Aptr == NULL || Aind == NULL || Aval == NULL) { poski_error("poski_CreateSubMatCSR_SemiOneD","invalid input parameters"); }


	poski_index_t row_start = partitionmat->row_start;
	poski_index_t row_end = partitionmat->row_end;
	poski_index_t col_start = partitionmat->col_start;
	poski_index_t nrows = partitionmat->nrows;
	poski_index_t ncols = partitionmat->ncols;
	poski_index_t nnz = partitionmat->nnz;


	/* @ Set permute information for submatrix */
	partitionmat->permuteRows = NULL;
	partitionmat->permuteCols = NULL;	
	
	/* @ Set submatrix in CSR */
	partitionmat->submat.Aptr = (poski_index_t *)malloc(sizeof(poski_index_t)*(nrows+1));
	if (partitionmat->submat.Aptr == NULL) assert(0);

	int i,j;
	poski_index_t currnnz = 0;

	for ( i=0; i<(nrows); i++ )
	{
		if (i==0) partitionmat->submat.Aptr[0] = 0;
		else partitionmat->submat.Aptr[i] = Aptr[row_start+i] - col_start;
	}
	partitionmat->submat.Aptr[i] = nnz;
	
	// TODO: make sure!
	if (mode==COPY_INPUTMAT)
	{
		partitionmat->submat.Aind = (poski_index_t *)malloc(sizeof(poski_index_t)*(nnz));
		partitionmat->submat.Aval = (poski_value_t *)malloc(sizeof(poski_value_t)*(nnz));
		memcpy((void *)partitionmat->submat.Aind, (const void *)&Aind[col_start], sizeof(poski_index_t)*(nnz));
		memcpy((void *)partitionmat->submat.Aval, (const void *)&Aval[col_start], sizeof(poski_value_t)*(nnz));
	}
	else
	{
		partitionmat->submat.Aind = &Aind[col_start];
		partitionmat->submat.Aval = &Aval[col_start];
	}

	partitionmat->submat.nrows = nrows;
	partitionmat->submat.ncols = ncols;	
	partitionmat->submat.nnz = nnz;

	return &partitionmat->submat;
}



/**
 * @brief [library's internal use] Create a partition-matrix object using semi-one dimensional partitioning model.
 *
 * @internal
 * @param[in] npartitions Number of partitions
 * @param[in] nrows Number of rows.
 * @param[in] ncols Number of columns.
 * @param[in] nnz Number of non-zeros.
 * @param[in] Aptr CSR row pointers.
 *
 * @return A valid partition-matrix object if success, or an error message otherwise.
 */
poski_partitionmat_t *poski_PartitionMatCSR_SemiOneD(poski_int_t npartitions, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz, poski_index_t *Aptr)
{
	// npartitions, pid, row_start, row_end, col_start, nnz, nrows, ncols, *permuteRows, *permuteCols, submat(Aptr, Aind, Aval, nrows, ncols, nnz) 
	poski_partitionmat_t *new_partition = poski_malloc(poski_partitionmat_t, npartitions, 0);

	int pid;
	for (pid=0; pid<npartitions; pid++)
	{
		poski_Initpartitionmat(&new_partition[pid]);
	}

	if (npartitions == 1)
	{
		new_partition[0].npartitions = npartitions;
		new_partition[0].pid = 0;
		new_partition[0].row_start = 0;
		new_partition[0].row_end = (nrows - 1);
		new_partition[0].col_start = 0;
		new_partition[0].nnz = nnz;
		new_partition[0].nrows = nrows;
		new_partition[0].ncols = ncols;

		return new_partition;
	}

	// npartitions > 1
	// @ check minimum nnz per partition (pnnz) and distributed rest of nnz
	poski_int_t pnnz = (poski_int_t)( (nnz-(nnz%npartitions)) / npartitions);
	poski_int_t snnz = 0;

	int i;
	for(i=0;i<npartitions;i++)
	{
		new_partition[i].npartitions=npartitions;
		new_partition[i].pid = i;

		new_partition[i].nnz = pnnz;
		if (i < (nnz%npartitions)) new_partition[i].nnz++;		
		new_partition[i].col_start = snnz;
		snnz += new_partition[i].nnz;
	}

	if (snnz != nnz) poski_error("poski_PartitionMatCSR_SemiOneD", "failed to partition");

	// @ set start row and end row per partitioin
	pid = 0;
	new_partition[pid].row_start = 0;
	snnz = new_partition[pid].nnz;

	for (i=0;i<(nrows); i++)
	{
		if (pid == (npartitions-1))
		{
			new_partition[pid].row_end = nrows - 1;
			break;
		}

		if (Aptr[i+1] > snnz)
		{
			new_partition[pid].row_end = (poski_int_t)(i);
			new_partition[pid+1].row_start = (poski_int_t)(i);
			pid++;i--;
			snnz += new_partition[pid].nnz;		
		}
		else if (Aptr[i+1] == snnz)
		{
			new_partition[pid].row_end = (poski_int_t)(i);
			new_partition[pid+1].row_start = (poski_int_t)(i+1);
			pid++;
			snnz += new_partition[pid].nnz;		
		}	
	}
	if (snnz != nnz) assert(0);

	for (i=0;i<npartitions;i++)
	{
		new_partition[i].nrows = new_partition[i].row_end - new_partition[i].row_start + 1;
		new_partition[i].ncols = ncols;
	}

	return new_partition;
}

/**
 * @brief [library's internal use] Operate semi-one dimensional partitioning for a matrix in a single thread for Pthread model.
 *
 * @internal
 * @param[in] poski_thread
 *
 */
#if defined( HAVE_PTHREAD_H )
void *PartitionMat_PthreadSemiOneD(void *poski_thread)
{
	poski_thread_t *thread = (poski_thread_t *)poski_thread;

	// NUMA affinity
	poski_SetAffinity_np(thread->tid);

	int pid;
	int num_partitions = thread->num_partitions;

	for (pid=0; pid<num_partitions; pid++)
	{
		thread->submat[pid].partitionmat = (poski_partitionmat_t *)malloc(sizeof(poski_partitionmat_t)*1);
		poski_copyarg(thread->submat[pid].partitionmat, &thread->partitionmat[pid], poski_partitionmat_t);
		poski_sparse_matrix_t *mat = poski_CreateSubMatCSR_SemiOneD(thread->submat[pid].partitionmat, thread->A->Aptr, thread->A->Aind, thread->A->Aval, thread->A->nrows, thread->A->ncols, thread->A->nnz, thread->mode);
		thread->submat[pid].submat = (poski_matstruct_t)poski_CreateSubMatCSR(mat->Aptr, mat->Aind, mat->Aval, mat->nrows, mat->ncols, SHARE_INPUTMAT, thread->k, thread->in_props);
	}

	pthread_exit(NULL);
}
#else
void *PartitionMat_PthreadSemiOneD(void *poski_thread)
{
	poski_error("PartitionMat_PthreadSemiOneD", "The library was not built with <pthread.h>");
}
#endif // HAVE_PTHREAD_H

/**
 * @brief [library's internal use] Operate semi-one dimensional partitioning for a matrix in a single thread for OpenMP model.
 *
 * @internal
 * @param[in] poski_thread
 *
 */
#if defined( HAVE_OPENMP_H )
void PartitionMat_OpenmpSemiOneD(void *poski_thread)
{
	poski_thread_t *thread = (poski_thread_t *)poski_thread;

	// NUMA affinity
	poski_SetAffinity_np(thread->tid);

	int pid;
	int num_partitions = thread->num_partitions;

	// NUMA affinity
	for (pid=0; pid<num_partitions; pid++)
	{
		thread->submat[pid].partitionmat = (poski_partitionmat_t *)malloc(sizeof(poski_partitionmat_t)*1);
		poski_copyarg(thread->submat[pid].partitionmat, &thread->partitionmat[pid], poski_partitionmat_t);
		//poski_CopyPartitionmat(thread->submat[pid].partitionmat, &thread->partitionmat[pid]);
		poski_sparse_matrix_t *mat = poski_CreateSubMatCSR_SemiOneD(thread->submat[pid].partitionmat, thread->A->Aptr, thread->A->Aind, thread->A->Aval, thread->A->nrows, thread->A->ncols, thread->A->nnz, thread->mode);
		thread->submat[pid].submat = (poski_matstruct_t)poski_CreateSubMatCSR(mat->Aptr, mat->Aind, mat->Aval, mat->nrows, mat->ncols, SHARE_INPUTMAT, thread->k, thread->in_props);
	}
}
#else
void PartitionMat_OpenmpSemiOneD(void *poski_thread)
{
	poski_error("PartitionMat_OpenmpSemiOneD", "The library was not built with <omp.h>");
}
#endif	// HAVE_OPENMP_H
/**
 * @brief [library's internal use] Create a submatrix object using semi-one dimensional partitioning model for Pthread model.
 * 
 * @internal
 * @param[in] A Tunable matrix object.
 * @param[in] mode Copy mode.
 * @param[in] in_props Specified semantic propeties.
 *
 * @return A valid submatrix object if success, or an error message otherwise.
 */
#if defined( HAVE_PTHREAD_H )
poski_submat_t *poski_PartitionMat_PthreadSemiOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props)
{
	poski_int_t nthreads = A->threadargs.nthreads;
	poski_int_t npartitions = A->partitionargs.npartitions;

	// [single] partitionmat = {npartitions, pid, row_start, row_end, col_start, nnz, nrows, ncols, *permuteRows, *permuteCols, submat(Aptr, Aind, Aval, nrows, ncols, nnz)} 
	poski_partitionmat_t *partitionmat = poski_PartitionMatCSR_SemiOneD(npartitions, A->nrows, A->ncols, A->nnz, A->Aptr);

	// [single] Global malloc, submat = {partitionmat, submat}
	poski_submat_t *submat = (poski_submat_t *)malloc(sizeof(poski_submat_t)*npartitions);
	int pid, tid, num_partitions;
	num_partitions = (int)(npartitions/nthreads);

	// [parallel] Local malloc, NUMA, assume (npartitions%nthreads == 0)
	poski_thread_t *poski_thread = (poski_thread_t *)malloc(sizeof(poski_thread_t)*nthreads);
	poski_pthread_t *threads = (poski_pthread_t *)malloc(sizeof(poski_pthread_t)*nthreads);

	for (tid=0; tid<nthreads; tid++)
	{
		poski_Set_pthreadarg(&poski_thread[tid], tid, num_partitions, mode, k, in_props, A, &partitionmat[tid*num_partitions], &submat[tid*num_partitions]);
		pthread_create(&threads[tid], NULL, PartitionMat_PthreadSemiOneD, (void *)&poski_thread[tid]);
	}
	for (tid=0; tid<nthreads; tid++)
	{
		pthread_join(threads[tid], NULL);
	}

	free(partitionmat);
	free(poski_thread);
	free(threads);

	return submat;
}	
#else
poski_submat_t *poski_PartitionMat_PthreadSemiOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props)
{
	poski_error("poski_PartitionMat_PthreadSemiOneD", "The library was not built with <pthread.h>");
	return NULL;
}
#endif	// HAVE_PTHREAD_H

/**
 * @brief [library's internal use] Create a submatrix object using semi-one dimensional partitioning model for OpenMP model.
 * 
 * @internal
 * @param[in] A Tunable matrix object.
 * @param[in] mode Copy mode.
 * @param[in] in_props Specified semantic propeties.
 *
 * @return A valid submatrix object if success, or an error message otherwise.
 */
#if defined( HAVE_OPENMP_H )
poski_submat_t *poski_PartitionMat_OpenmpSemiOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props)
{
	poski_int_t nthreads = A->threadargs.nthreads;
	poski_int_t npartitions = A->partitionargs.npartitions;

	// [single] partitionmat = {npartitions, pid, row_start, row_end, col_start, nnz, nrows, ncols, *permuteRows, *permuteCols, submat(Aptr, Aind, Aval, nrows, ncols, nnz)} 
	poski_partitionmat_t *partitionmat = poski_PartitionMatCSR_SemiOneD(npartitions, A->nrows, A->ncols, A->nnz, A->Aptr);

	// [single] Global malloc, submat = {partitionmat, submat}
	poski_submat_t *submat = (poski_submat_t *)malloc(sizeof(poski_submat_t)*npartitions);
	int pid, tid, num_partitions;
	num_partitions = (int)(npartitions/nthreads);
	// [parallel] Local malloc, NUMA, assume (npartitions%nthreads == 0)
	poski_thread_t *poski_thread = (poski_thread_t *)malloc(sizeof(poski_thread_t)*nthreads);
	poski_pthread_t *threads = (poski_pthread_t *)malloc(sizeof(poski_pthread_t)*nthreads);
	
	#pragma omp parallel for num_threads(nthreads)
	for (tid=0; tid<nthreads; tid++)
	{
		poski_Set_pthreadarg(&poski_thread[tid], tid, num_partitions, mode, k, in_props, A, &partitionmat[tid*num_partitions], &submat[tid*num_partitions]);
		PartitionMat_OpenmpSemiOneD((void *)&poski_thread[tid]);
	}

	free(partitionmat);
	free(poski_thread);
	free(threads);

	return submat;
}	
#else
poski_submat_t *poski_PartitionMat_OpenmpSemiOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props)
{
	poski_error("poski_PartitionMat_OpenmpSemiOneD", "The library was not built with <omp.h>");
	return NULL;
}
#endif	// HAVE_OPENMP_H

/**
 * @brief [library's internal use] Create a submatrix object using semi-one dimensional partitioning model for ThreadPool model.
 * 
 * @internal
 * @param[in] A Tunable matrix object.
 * @param[in] mode Copy mode.
 * @param[in] in_props Specified semantic propeties.
 *
 * @return A valid submatrix object if success, or an error message otherwise.
 */
#if defined( HAVE_PTHREAD_H )
poski_submat_t *poski_PartitionMat_ThreadpoolSemiOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props)
{
	poski_int_t nthreads = A->threadargs.nthreads;
	poski_int_t npartitions = A->partitionargs.npartitions;

	// [single] partitionmat = {npartitions, pid, row_start, row_end, col_start, nnz, nrows, ncols, *permuteRows, *permuteCols, submat(Aptr, Aind, Aval, nrows, ncols, nnz)} 
	poski_partitionmat_t *partitionmat = poski_PartitionMatCSR_SemiOneD(npartitions, A->nrows, A->ncols, A->nnz, A->Aptr);

	// [single] Global malloc, submat = {partitionmat, submat}
	poski_submat_t *submat = (poski_submat_t *)malloc(sizeof(poski_submat_t)*npartitions);
	int pid, tid, num_partitions;
	num_partitions = (int)(npartitions/nthreads);

	// [parallel] Local malloc, NUMA, assume (npartitions%nthreads == 0)
	poski_thread_t *poski_thread = (poski_thread_t *)malloc(sizeof(poski_thread_t)*nthreads);
	poski_threadpool_t *threadpool = A->threadargs.thread;

	for (tid=0; tid<nthreads; tid++)
	{
		threadpool[tid].Job=CREATEMAT_SEMIONED;
		poski_Set_pthreadarg(&poski_thread[tid], tid, num_partitions, mode, k, in_props, A, &partitionmat[tid*num_partitions], &submat[tid*num_partitions]);
		threadpool[tid].kernel = (void *)&poski_thread[tid];
	}
	poski_barrier_wait (threadpool->StartBarrier, (nthreads+1));
	poski_barrier_wait (threadpool->EndBarrier, (nthreads+1));
	
	free(partitionmat);
	free(poski_thread);

	return submat;
}	
#else
poski_submat_t *poski_PartitionMat_ThreadpoolSemiOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props)
{
	poski_error("poski_PartitionMat_ThreadpoolSemiOneD", "The library was not built with <pthread.h>");
	return NULL;
}
#endif	// HAVE_PTHREAD_H



/**
 * @brief [library's internal use] Create a subvector object based on a tunable matrix object for output vector.
 *
 * @internal
 * @param[in] vec Vector data.
 * @param[in] length Legical vector length.
 * @param[in] inc Physical stride between consecutive elements.
 * @param[in] A Tunable matrix object.
 *
 * @return A valid subvector object if success, or an error message otherwise.
 */
#if defined( HAVE_PTHREAD_H )
poski_subvec_t *poski_PartitionVec_PthreadSemiOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	int i,j;
	poski_int_t npartitions = A->partitionargs.npartitions;
	
	poski_subvec_t *subvector = (poski_subvec_t *)malloc(sizeof(poski_subvec_t)*npartitions);
	if (subvector == NULL) assert(0);

	/* check original matrix's nrows and vector length */
	if ( poski_CheckLength(A->nrows, length) )
	{
		poski_error("poski_PartitionVec_PthreadSemiOneDMatMultNormalOutput", "The sizes are not matching");
		assert(0);	
	}
	
	for ( i=0; i<npartitions; i++ )
	{	
		poski_partitionmat_t *partitionmat = A->submatrix[i].partitionmat;
		poski_value_t *new_vec = (poski_value_t *)malloc(sizeof(poski_value_t)*partitionmat->nrows);
		for (j=0;j<partitionmat->nrows;j++)
		{
			new_vec[j] = (poski_value_t)0;
		}
		/* create a subvector for each submatrix */
		subvector[i].subvec = (poski_vecview_t)poski_CreateVec_s(new_vec, partitionmat->nrows, inc);
	}

	return subvector;
}


#else
poski_subvec_t *poski_PartitionVec_PthreadSemiOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	poski_error("poski_PartitionVec_PthreadSemiOneDMatMultNormalOutput", "The pOSKI library was not built with <pthread.h>");
	return NULL;
}
#endif	// HAVE_PTHREAD_H

#if defined( HAVE_OPENMP_H )
poski_subvec_t *poski_PartitionVec_OpenmpSemiOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	int i,j;
	poski_int_t npartitions = A->partitionargs.npartitions;
	
	poski_subvec_t *subvector = (poski_subvec_t *)malloc(sizeof(poski_subvec_t)*npartitions);
	if (subvector == NULL) assert(0);

	/* check original matrix's nrows and vector length */
	if ( poski_CheckLength(A->nrows, length) )
	{
		poski_error("poski_PartitionVec_OpenmpSemiOneDMatMultNormalOutput", "The sizes are not matching");
		assert(0);	
	}
	
	for ( i=0; i<npartitions; i++ )
	{	
		poski_partitionmat_t *partitionmat = A->submatrix[i].partitionmat;
		poski_value_t *new_vec = (poski_value_t *)malloc(sizeof(poski_value_t)*partitionmat->nrows);
		for (j=0;j<partitionmat->nrows;j++)
		{
			new_vec[j] = (poski_value_t)0;
		}
		/* create a subvector for each submatrix */
		//poski_CheckLength(A->partitionmat[i].nrows, A->partitionarg->nrows[i]);	/* check the vector length with submatrix ncols */
		subvector[i].subvec = (poski_vecview_t)poski_CreateVec_s(new_vec, partitionmat->nrows, inc);
	}

	return subvector;
}
#else
poski_subvec_t *poski_PartitionVec_OpenmpSemiOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	poski_error("poski_PartitionVec_OpenmpSemiOneDMatMultNormalOutput", "The pOSKI library was not built with <omp.h>");
	return NULL;
}
#endif	// HAVE_OPENMP_H

#if defined( HAVE_PTHREAD_H )
poski_subvec_t *poski_PartitionVec_ThreadpoolSemiOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	int i,j;
	poski_int_t npartitions = A->partitionargs.npartitions;
	
	poski_subvec_t *subvector = (poski_subvec_t *)malloc(sizeof(poski_subvec_t)*npartitions);
	if (subvector == NULL) assert(0);

	/* check original matrix's nrows and vector length */
	if ( poski_CheckLength(A->nrows, length) )
	{
		poski_error("poski_PartitionVec_ThreadpoolSemiOneDMatMultNormalOutput", "The sizes are not matching");
		assert(0);	
	}
	
	for ( i=0; i<npartitions; i++ )
	{	
		poski_partitionmat_t *partitionmat = A->submatrix[i].partitionmat;
		poski_value_t *new_vec = (poski_value_t *)malloc(sizeof(poski_value_t)*partitionmat->nrows);
		for (j=0;j<partitionmat->nrows;j++)
		{
			new_vec[j] = (poski_value_t)0;
		}
		/* create a subvector for each submatrix */
		//poski_CheckLength(A->partitionmat[i].nrows, A->partitionarg->nrows[i]);	/* check the vector length with submatrix ncols */
		subvector[i].subvec = (poski_vecview_t)poski_CreateVec_s(new_vec, partitionmat->nrows, inc);
	}

	return subvector;
}
#else
poski_subvec_t *poski_PartitionVec_ThreadpoolSemiOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	poski_error("poski_PartitionVec_ThreadpoolSemiOneDMatMultNormalOutput", "The pOSKI library was not built with <pthread.h>");
	return NULL;
}
#endif	// HAVE_PTHREAD_H


/**
 * @brief [library's internal use] Create a subvector object based on a tunable matrix object for input vector.
 *
 * @internal
 * @param[in] vec Vector data.
 * @param[in] length Legical vector length.
 * @param[in] inc Physical stride between consecutive elements.
 * @param[in] A Tunable matrix object.
 *
 * @return A valid subvector object if success, or an error message otherwise.
 */
#if defined( HAVE_PTHREAD_H ) 
poski_subvec_t *poski_PartitionVec_PthreadSemiOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	return poski_PartitionVec_PthreadOneDMatMultNormalInput(vec, length, inc, A);
}
#else
poski_subvec_t *poski_PartitionVec_PthreadSemiOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	poski_error("poski_PartitionVec_PthreadSemiOneDMatMultNormalOutput", "The pOSKI library was not built with <pthread.h>");
	return NULL;
}
#endif	// HAVE_PTHREAD_H

#if defined( HAVE_OPENMP_H ) 
poski_subvec_t *poski_PartitionVec_OpenmpSemiOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	return poski_PartitionVec_OpenmpOneDMatMultNormalInput(vec, length, inc, A);
}
#else
poski_subvec_t *poski_PartitionVec_OpenmpSemiOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	poski_error("poski_PartitionVec_OpenmpSemiOneDMatMultNormalOutput", "The pOSKI library was not built with <omp.h>");
	return NULL;
}
#endif	// HAVE_PTHREAD_H

#if defined( HAVE_PTHREAD_H ) 
poski_subvec_t *poski_PartitionVec_ThreadpoolSemiOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	return poski_PartitionVec_ThreadpoolOneDMatMultNormalInput(vec, length, inc, A);	// TODO: fix it with threadpool!
}
#else
poski_subvec_t *poski_PartitionVec_ThreadpoolSemiOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	poski_error("poski_PartitionVec_ThreadpoolSemiOneDMatMultNormalOutput", "The pOSKI library was not built with <pthread.h>");
	return NULL;
}
#endif	// HAVE_PTHREAD_H


// TODO: parallel
poski_subvec_t *poski_PartitionVec_PthreadSemiOneDMatMultTransOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	int i,j;
	poski_int_t npartitions = A->partitionargs.npartitions;
	
	poski_subvec_t *subvector = (poski_subvec_t *)malloc(sizeof(poski_subvec_t)*npartitions);
	if (subvector == NULL) assert(0);

	/* check original matrix's nrows and vector length */
	if ( poski_CheckLength(A->ncols, length) )
	{
		poski_error("poski_PartitionVec_PthreadSemiOneDMatMultNormalOutput", "The sizes are not matching");
		assert(0);	
	}
	
	for ( i=0; i<npartitions; i++ )
	{	
		poski_partitionmat_t *partitionmat = A->submatrix[i].partitionmat;
		poski_value_t *new_vec = (poski_value_t *)malloc(sizeof(poski_value_t)*partitionmat->ncols);
		for (j=0;j<partitionmat->ncols;j++)
		{
			new_vec[j] = (poski_value_t)0;
		}
		/* create a subvector for each submatrix */
		subvector[i].subvec = (poski_vecview_t)poski_CreateVec_s(new_vec, partitionmat->ncols, inc);
	}

	return subvector;
}
poski_subvec_t *poski_PartitionVec_PthreadSemiOneDMatMultTransInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	int i;
	poski_int_t npartitions = A->partitionargs.npartitions;

	poski_subvec_t *subvector = (poski_subvec_t *)malloc(sizeof(poski_subvec_t)*npartitions);
	if (subvector == NULL) assert(0);
	
	/* check original matrix's ncols and vector length */
	if ( poski_CheckLength(A->nrows, length) )
	{
		poski_error("poski_PartitionVec_PthreadOneDMatMultNormalInput", "The sizes are not matching");
		assert(0);	
	}
	
	for ( i=0; i<npartitions; i++ )
	{
		/* create a subvector for each submatrix */
		subvector[i].subvec = (poski_vecview_t)poski_CreateVec_s(&vec[A->submatrix[i].partitionmat->row_start], A->submatrix[i].partitionmat->nrows, inc);
	}

	return subvector;
}

poski_subvec_t *poski_PartitionVec_OpenmpSemiOneDMatMultTransInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	return poski_PartitionVec_PthreadSemiOneDMatMultTransInput( vec, length, inc, A );
}
poski_subvec_t *poski_PartitionVec_OpenmpSemiOneDMatMultTransOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	return poski_PartitionVec_PthreadSemiOneDMatMultTransOutput( vec, length, inc, A );
}
poski_subvec_t *poski_PartitionVec_ThreadpoolSemiOneDMatMultTransInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	return poski_PartitionVec_PthreadSemiOneDMatMultTransInput( vec, length, inc, A );
}
poski_subvec_t *poski_PartitionVec_ThreadpoolSemiOneDMatMultTransOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	return poski_PartitionVec_PthreadSemiOneDMatMultTransOutput( vec, length, inc, A );
}


