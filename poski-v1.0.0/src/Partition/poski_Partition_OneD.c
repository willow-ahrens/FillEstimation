/**
 * @file src/Partition/poski_Partition_OneD.c
 * @brief Handle One dimensional partitioning model.
 *
 * This module implments the routines to handle One dimensional partitioning model.
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

// Pthread, OneD, MatMult, Normal
/**
 * @brief [library's internal use] Create a partition-matrix object using one dimensional partitioning model.
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
poski_partitionmat_t *poski_PartitionMatCSR_OneD(poski_int_t npartitions, poski_index_t nrows, poski_index_t ncols, poski_index_t nnz, poski_index_t *Aptr)
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
	pid = 0;
	poski_int_t remained_nnz;
	poski_int_t remained_npartitions;
	poski_int_t remained_nrows;
	poski_int_t average_nnz;
	int i;
	for (i=0;i<nrows;i++)
	{
		if (pid == 0) new_partition[pid].row_start = 0;
		else new_partition[pid].row_start = new_partition[pid-1].row_end + 1;
		
		if (pid == (npartitions-1))
		{
			new_partition[pid].row_end = nrows - 1;
			new_partition[pid].col_start = Aptr[new_partition[pid].row_start];
			break;
		}

		remained_nnz = nnz - Aptr[new_partition[pid].row_start];
		remained_npartitions = npartitions - pid;
		remained_nrows = nrows - i;
		average_nnz = (int)( (remained_nnz - (remained_nnz%remained_npartitions)) / remained_npartitions );
		if (remained_npartitions == remained_nrows)
		{
			while(pid < npartitions)
			{	
				if (pid == 0) new_partition[pid].row_start = 0;
				else new_partition[pid].row_start = new_partition[pid-1].row_end + 1;
				new_partition[pid].row_end = i;
				new_partition[pid].col_start = Aptr[new_partition[pid].row_start];
				pid++; i++;
			}	
			break;		
		}
		// (remained_nrows > remained_npartitions)
		if ((Aptr[i+1] - Aptr[new_partition[pid].row_start]) >= average_nnz)
		{
			new_partition[pid].row_end = i;
			new_partition[pid].col_start = Aptr[new_partition[pid].row_start];
			pid++;
		}
	}

	// [parallel]	
	for(pid=0;pid<npartitions;pid++)
	{
		new_partition[pid].npartitions = npartitions;
		new_partition[pid].pid = pid;
		new_partition[pid].ncols = ncols;
	
		new_partition[pid].nrows = new_partition[pid].row_end - new_partition[pid].row_start + 1;
		new_partition[pid].nnz = Aptr[new_partition[pid].row_end + 1] - Aptr[new_partition[pid].row_start];
	}

	return new_partition;
}	

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
poski_sparse_matrix_t *poski_CreateSubMatCSR_OneD(poski_partitionmat_t *partitionmat, poski_index_t *Aptr, poski_index_t *Aind, poski_value_t *Aval, poski_index_t Anrows, poski_index_t Ancols, poski_index_t Annz, poski_copymode_t mode)
{
	// error check
	if (partitionmat == NULL) { poski_error("poski_CreateSubMatCSR_OneD","invalid input parameters"); }
	if (Aptr == NULL || Aind == NULL || Aval == NULL) { poski_error("poski_CreateSubMatCSR_OneD","invalid input parameters"); }

	poski_index_t row_start = partitionmat->row_start;
	poski_index_t row_end = partitionmat->row_end;
	poski_index_t nrows = partitionmat->nrows;
	poski_index_t ncols = partitionmat->ncols;
	poski_index_t nnz = partitionmat->nnz;

	/* @ Set permute information for submatrix */
	partitionmat->permuteRows = NULL;
	partitionmat->permuteCols = NULL;	
	
	/* @ Set submatrix in CSR */
	partitionmat->submat.Aptr = (poski_index_t *)malloc(sizeof(poski_index_t)*(nrows+1));
//	partitionmat->submat.Aptr = poski_malloc(poski_index_t,(nrows+1),1);
	if (partitionmat->submat.Aptr == NULL) assert(0);

	int i,j;
	poski_index_t currnnz = 0;

	partitionmat->submat.Aptr[0] = 0;
	for ( i=0; i<(nrows+1); i++ )
	{
		partitionmat->submat.Aptr[i] = Aptr[row_start+i] - Aptr[row_start];
	}
	
	if (nnz != partitionmat->submat.Aptr[i-1])  { poski_error("poski_CreateSubMatCSR_OneD","nnz not matching (%d : %d)",nnz, partitionmat->submat.Aptr[i-1]); }
	
	if (mode==COPY_INPUTMAT)
	{
		partitionmat->submat.Aind = (poski_index_t *)malloc(sizeof(poski_index_t)*(nnz));
		partitionmat->submat.Aval = (poski_value_t *)malloc(sizeof(poski_value_t)*(nnz));
		memcpy((void *)partitionmat->submat.Aind, (const void *)&Aind[Aptr[row_start]], sizeof(poski_index_t)*(nnz));
		memcpy((void *)partitionmat->submat.Aval, (const void *)&Aval[Aptr[row_start]], sizeof(poski_value_t)*(nnz));
	}
	else
	{
		partitionmat->submat.Aind = &Aind[Aptr[row_start]];
		partitionmat->submat.Aval = &Aval[Aptr[row_start]];
	}
	partitionmat->submat.nrows = nrows;
	partitionmat->submat.ncols = ncols;	
	partitionmat->submat.nnz = nnz;

	return &partitionmat->submat;
}

/**
 * @brief [library's internal use] Operate one dimensional partitioning for a matrix in a single thread for Pthread model.
 *
 * @internal
 * @param[in] poski_thread
 *
 */
#if defined( HAVE_PTHREAD_H )
void *PartitionMat_PthreadOneD(void *poski_thread)
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
		poski_sparse_matrix_t *mat = poski_CreateSubMatCSR_OneD(thread->submat[pid].partitionmat, thread->A->Aptr, thread->A->Aind, thread->A->Aval, thread->A->nrows, thread->A->ncols, thread->A->nnz, thread->mode);
		thread->submat[pid].submat = (poski_matstruct_t)poski_CreateSubMatCSR(mat->Aptr, mat->Aind, mat->Aval, mat->nrows, mat->ncols, SHARE_INPUTMAT, thread->k, thread->in_props);

	}
	pthread_exit(NULL);
}
#else
void *PartitionMat_PthreadOneD(void *poski_thread)
{
	poski_error("PartitionMat_PthreadOneD", "The pOSKI library was not built with <pthread.h>");
}
#endif	// HAVE_PTHREAD_H

/**
 * @brief [library's internal use] Operate one dimensional partitioning for a matrix in a single thread for OpenMP model.
 *
 * @internal
 * @param[in] poski_thread
 *
 */
#if defined( HAVE_OPENMP_H )
void PartitionMat_OpenmpOneD(void *poski_thread)
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
		poski_sparse_matrix_t *mat = poski_CreateSubMatCSR_OneD(thread->submat[pid].partitionmat, thread->A->Aptr, thread->A->Aind, thread->A->Aval, thread->A->nrows, thread->A->ncols, thread->A->nnz, thread->mode);
		thread->submat[pid].submat = (poski_matstruct_t)poski_CreateSubMatCSR(mat->Aptr, mat->Aind, mat->Aval, mat->nrows, mat->ncols, SHARE_INPUTMAT, thread->k, thread->in_props);
	}
}
#else
void PartitionMat_OpenmpOneD(void *poski_thread)
{
	poski_error("PartitionMat_OpenmpOneD", "The pOSKI library was not built with <omp.h>");
}
#endif	// HAVE_OPENMP_H
/**
 * @brief [library's internal use]
 *
 * @internal
 * @param[in] thread
 * @param[in] tid
 * @param[in] num_partitions
 * @param[in] mode
 * @param[in] k
 * @param[in] in_props
 * @param[in] A
 * @param[in] partitionmat
 * @param[in] submat
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_Set_pthreadarg(poski_thread_t *thread, int tid, int num_partitions, poski_copymode_t mode, int k, poski_inmatprop_t *in_props, poski_mat_t A, poski_partitionmat_t *partitionmat, poski_submat_t *submat)
{
	thread->tid = tid;
	thread->num_partitions = num_partitions;
	thread->mode = mode;
	thread->k = k;
	thread->in_props = in_props;
	thread->A = A;
	thread->partitionmat = partitionmat;
	thread->submat = submat;	
	return 0;
}
// Pthread, OneD, MatMult, Normal
/**
 * @brief [library's internal use] Create a submatrix object using one dimensional partitioning model for Pthread model.
 * 
 * @internal
 * @param[in] A Tunable matrix object.
 * @param[in] mode Copy mode.
 * @param[in] in_props Specified semantic propeties.
 *
 * @return A valid submatrix object if success, or an error message otherwise.
 */
#if defined( HAVE_PTHREAD_H )
poski_submat_t *poski_PartitionMat_PthreadOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props)
{
	poski_int_t nthreads = A->threadargs.nthreads;
	poski_int_t npartitions = A->partitionargs.npartitions;

	// [single] partitionmat = {npartitions, pid, row_start, row_end, col_start, nnz, nrows, ncols, *permuteRows, *permuteCols, submat(Aptr, Aind, Aval, nrows, ncols, nnz)} 
	poski_partitionmat_t *partitionmat = poski_PartitionMatCSR_OneD(npartitions, A->nrows, A->ncols, A->nnz, A->Aptr);

	// [single] Global malloc, submat = {partitionmat, submat}
	poski_submat_t *submat = (poski_submat_t *)malloc(sizeof(poski_submat_t)*npartitions);

	poski_thread_t *poski_thread = (poski_thread_t *)malloc(sizeof(poski_thread_t)*nthreads);
	poski_pthread_t *threads = (poski_pthread_t *)malloc(sizeof(poski_pthread_t)*nthreads);
	int pid, tid, num_partitions;

	// [parallel] Local malloc, NUMA, assume (npartitions%nthreads == 0)

	num_partitions = (int)(npartitions/nthreads);
	
	for (tid=0; tid<nthreads; tid++)
	{
		poski_Set_pthreadarg(&poski_thread[tid], tid, num_partitions, mode, k, in_props, A, &partitionmat[tid*num_partitions], &submat[tid*num_partitions]);
		pthread_create(&threads[tid], NULL, PartitionMat_PthreadOneD, (void *)&poski_thread[tid]);
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
poski_submat_t *poski_PartitionMat_PthreadOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props)
{
	poski_error("poski_PartitionMat_PthreadOneD", "The pOSKI library was not built with <pthread.h>");
	return NULL;
}
#endif	// HAVE_PTHREAD_H
/**
 * @brief [library's internal use] Create a submatrix object using one dimensional partitioning model for OpenMP model.
 * 
 * @internal
 * @param[in] A Tunable matrix object.
 * @param[in] mode Copy mode.
 * @param[in] in_props Specified semantic propeties.
 *
 * @return A valid submatrix object if success, or an error message otherwise.
 */
#if defined( HAVE_OPENMP_H )
poski_submat_t *poski_PartitionMat_OpenmpOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props)
{
	poski_int_t nthreads = A->threadargs.nthreads;
	poski_int_t npartitions = A->partitionargs.npartitions;

	// [single] partitionmat = {npartitions, pid, row_start, row_end, col_start, nnz, nrows, ncols, *permuteRows, *permuteCols, submat(Aptr, Aind, Aval, nrows, ncols, nnz)} 
	poski_partitionmat_t *partitionmat = poski_PartitionMatCSR_OneD(npartitions, A->nrows, A->ncols, A->nnz, A->Aptr);

	// [single] Global malloc, submat = {partitionmat, submat}
	poski_submat_t *submat = (poski_submat_t *)malloc(sizeof(poski_submat_t)*npartitions);

	poski_thread_t *poski_thread = (poski_thread_t *)malloc(sizeof(poski_thread_t)*nthreads);
	int pid, tid, num_partitions;

	// [parallel] Local malloc, NUMA, assume (npartitions%nthreads == 0)
	num_partitions = (int)(npartitions/nthreads);
	#pragma omp parallel for num_threads(nthreads) 
	for (tid=0; tid<nthreads; tid++)
	{
		poski_Set_pthreadarg(&poski_thread[tid], tid, num_partitions, mode, k, in_props, A, &partitionmat[tid*num_partitions], &submat[tid*num_partitions]);
		PartitionMat_OpenmpOneD((void *)&poski_thread[tid]);
	}
	
	free(partitionmat);
	free(poski_thread);
	
	return submat;
}	
#else
poski_submat_t *poski_PartitionMat_OpenmpOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props)
{
	poski_error("poski_PartitionMat_OpenmpOneD", "The pOSKI library was not built with <omp.h>");
	return NULL;
}
#endif	// HAVE_OPENMP_H


#if defined( HAVE_PTHREAD_H )
poski_submat_t *poski_PartitionMat_ThreadpoolOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props)
{
	poski_int_t nthreads = A->threadargs.nthreads;
	poski_int_t npartitions = A->partitionargs.npartitions;

	// [single] partitionmat = {npartitions, pid, row_start, row_end, col_start, nnz, nrows, ncols, *permuteRows, *permuteCols, submat(Aptr, Aind, Aval, nrows, ncols, nnz)} 
	poski_partitionmat_t *partitionmat = poski_PartitionMatCSR_OneD(npartitions, A->nrows, A->ncols, A->nnz, A->Aptr);

	// [single] Global malloc, submat = {partitionmat, submat}
	poski_submat_t *submat = (poski_submat_t *)malloc(sizeof(poski_submat_t)*npartitions);

	poski_thread_t *poski_thread = (poski_thread_t *)malloc(sizeof(poski_thread_t)*nthreads);
	poski_threadpool_t *threadpool = A->threadargs.thread;
	int pid, tid, num_partitions;

	// [parallel] Local malloc, NUMA, assume (npartitions%nthreads == 0)
	num_partitions = (int)(npartitions/nthreads);
	
	for (tid=0; tid<nthreads; tid++)
	{
		threadpool[tid].Job = CREATEMAT_ONED;
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
poski_submat_t *poski_PartitionMat_ThreadpoolOneD(poski_mat_t A, poski_copymode_t mode, int k, poski_inmatprop_t *in_props)
{
	poski_error("poski_PartitionMat_ThreadpoolOneD", "The pOSKI library was not built with <pthread.h>");
	return NULL;
}
#endif	// HAVE_PTHREAD_H


/* vector partition (1D) for MatMult with OP_NORMAL */
/* original vector *x, original vector length, input/output, partitioning infor */
/* no permute, no reduction */
/* input no partition, output partition */
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
poski_subvec_t *poski_PartitionVec_PthreadOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	int i;
	poski_int_t npartitions = A->partitionargs.npartitions;

	poski_subvec_t *subvector = (poski_subvec_t *)malloc(sizeof(poski_subvec_t)*npartitions);
	if (subvector == NULL) assert(0);
	
	/* check original matrix's ncols and vector length */
	if ( poski_CheckLength(A->ncols, length) )
	{
		poski_error("poski_PartitionVec_PthreadOneDMatMultNormalInput", "The sizes are not matching");
		assert(0);	
	}
	
	for ( i=0; i<npartitions; i++ )
	{
		/* create a subvector for each submatrix */
		subvector[i].subvec = (poski_vecview_t)poski_CreateVec_s(vec, length, inc);
	}

	return subvector;
}
#else
poski_subvec_t *poski_PartitionVec_PthreadOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	poski_error("poski_PartitionVec_PthreadOneDMatMultNormalInput", "The pOSKI library was not built with <pthread.h>");
	return NULL;
}
#endif	// HAVE_PTHREAD_H

#if defined( HAVE_OPENMP_H )
poski_subvec_t *poski_PartitionVec_OpenmpOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	int i;
	poski_int_t npartitions = A->partitionargs.npartitions;

	poski_subvec_t *subvector = (poski_subvec_t *)malloc(sizeof(poski_subvec_t)*npartitions);
	if (subvector == NULL) assert(0);
	
	/* check original matrix's ncols and vector length */
	if ( poski_CheckLength(A->ncols, length) )
	{
		poski_error("poski_PartitionVec_OpenmpOneDMatMultNormalInput", "The sizes are not matching");
		assert(0);	
	}
	
	for ( i=0; i<npartitions; i++ )
	{
		/* create a subvector for each submatrix */
		subvector[i].subvec = (poski_vecview_t)poski_CreateVec_s(vec, length, inc);
	}

	return subvector;
}
#else
poski_subvec_t *poski_PartitionVec_OpenmpOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	poski_error("poski_PartitionVec_OpenmpOneDMatMultNormalInput", "The pOSKI library was not built with <omp.h>");
	return NULL;
}
#endif	// HAVE_OPENMP_H

#if defined( HAVE_PTHREAD_H )
poski_subvec_t *poski_PartitionVec_ThreadpoolOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	int i;
	poski_int_t npartitions = A->partitionargs.npartitions;

	poski_subvec_t *subvector = (poski_subvec_t *)malloc(sizeof(poski_subvec_t)*npartitions);
	if (subvector == NULL) assert(0);
	
	/* check original matrix's ncols and vector length */
	if ( poski_CheckLength(A->ncols, length) )
	{
		poski_error("poski_PartitionVec_ThreadpoolOneDMatMultNormalInput", "The sizes are not matching");
		assert(0);	
	}
	
	for ( i=0; i<npartitions; i++ )
	{
		/* create a subvector for each submatrix */
		subvector[i].subvec = (poski_vecview_t)poski_CreateVec_s(vec, length, inc);
	}

	return subvector;
}
#else
poski_subvec_t *poski_PartitionVec_ThreadpoolOneDMatMultNormalInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	poski_error("poski_PartitionVec_ThreadpoolOneDMatMultNormalInput", "The pOSKI library was not built with <pthread.h>");
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
poski_subvec_t *poski_PartitionVec_PthreadOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	int i;
	poski_int_t npartitions = A->partitionargs.npartitions;
	
	poski_subvec_t *subvector = (poski_subvec_t *)malloc(sizeof(poski_subvec_t)*npartitions);
	if (subvector == NULL) assert(0);

	/* check original matrix's nrows and vector length */
	if ( poski_CheckLength(A->nrows, length) )
	{
		poski_error("poski_PartitionVec_PthreadOneDMatMultNormalOutput", "The sizes are not matching");
		assert(0);
	}
	
	for ( i=0; i<npartitions; i++ )
	{
		poski_partitionmat_t *partitionmat = A->submatrix[i].partitionmat;

		/* create a subvector for each submatrix */
		//poski_CheckLength(A->partitionmat[i].nrows, A->partitionarg->nrows[i]);	/* check the vector length with submatrix ncols */
		subvector[i].subvec = (poski_vecview_t)poski_CreateVec_s(&vec[partitionmat->row_start], partitionmat->nrows, inc);
	}

	return subvector;
}
#else
poski_subvec_t *poski_PartitionVec_PthreadOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	poski_error("poski_PartitionVec_PthreadOneDMatMultNormalOutput", "The pOSKI library was not built with <pthread.h>");
	return NULL;
}
#endif	// HAVE_PTHREAD_H

#if defined( HAVE_OPENMP_H )
poski_subvec_t *poski_PartitionVec_OpenmpOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	int i;
	poski_int_t npartitions = A->partitionargs.npartitions;
	
	poski_subvec_t *subvector = (poski_subvec_t *)malloc(sizeof(poski_subvec_t)*npartitions);
	if (subvector == NULL) assert(0);

	/* check original matrix's nrows and vector length */
	if ( poski_CheckLength(A->nrows, length) )
	{
		poski_error("poski_PartitionVec_OpenmpOneDMatMultNormalOutput", "The sizes are not matching");
		assert(0);
	}
	
	for ( i=0; i<npartitions; i++ )
	{
		poski_partitionmat_t *partitionmat = A->submatrix[i].partitionmat;

		/* create a subvector for each submatrix */
		subvector[i].subvec = (poski_vecview_t)poski_CreateVec_s(&vec[partitionmat->row_start], partitionmat->nrows, inc);
	}

	return subvector;
}
#else
poski_subvec_t *poski_PartitionVec_OpenMPOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	poski_error("poski_PartitionVec_OpenmpOneDMatMultNormalOutput", "The pOSKI library was not built with <omp.h>");
	return NULL;
}
#endif	// HAVE_OPENMP_H

#if defined( HAVE_PTHREAD_H )
poski_subvec_t *poski_PartitionVec_ThreadpoolOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{

	int i;
	poski_int_t npartitions = A->partitionargs.npartitions;
	
	poski_subvec_t *subvector = (poski_subvec_t *)malloc(sizeof(poski_subvec_t)*npartitions);
	if (subvector == NULL) assert(0);

	/* check original matrix's nrows and vector length */
	if ( poski_CheckLength(A->nrows, length) )
	{
		poski_error("poski_PartitionVec_ThreadpoolOneDMatMultNormalOutput", "The sizes are not matching");
		assert(0);
	}
	
	for ( i=0; i<npartitions; i++ )
	{
		poski_partitionmat_t *partitionmat = A->submatrix[i].partitionmat;

		/* create a subvector for each submatrix */
		subvector[i].subvec = (poski_vecview_t)poski_CreateVec_s(&vec[partitionmat->row_start], partitionmat->nrows, inc);
	}

	return subvector;
}
#else
poski_subvec_t *poski_PartitionVec_ThreadpoolOneDMatMultNormalOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	poski_error("poski_PartitionVec_ThreadpoolOneDMatMultNormalOutput", "The pOSKI library was not built with <pthread.h>");
	return NULL;
}
#endif	// HAVE_PTHREAD_H


// TODO: parallel
poski_subvec_t *poski_PartitionVec_PthreadOneDMatMultTransInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	return poski_PartitionVec_PthreadSemiOneDMatMultTransInput( vec, length, inc, A );
}
poski_subvec_t *poski_PartitionVec_PthreadOneDMatMultTransOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	return poski_PartitionVec_PthreadSemiOneDMatMultTransOutput( vec, length, inc, A );
}
poski_subvec_t *poski_PartitionVec_OpenmpOneDMatMultTransInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	return poski_PartitionVec_PthreadSemiOneDMatMultTransInput( vec, length, inc, A );
}
poski_subvec_t *poski_PartitionVec_OpenmpOneDMatMultTransOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	return poski_PartitionVec_PthreadSemiOneDMatMultTransOutput( vec, length, inc, A );
}
poski_subvec_t *poski_PartitionVec_ThreadpoolOneDMatMultTransInput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	return poski_PartitionVec_PthreadSemiOneDMatMultTransInput( vec, length, inc, A );
}
poski_subvec_t *poski_PartitionVec_ThreadpoolOneDMatMultTransOutput( poski_value_t *vec, poski_index_t length, poski_index_t inc, poski_mat_t A )
{
	return poski_PartitionVec_PthreadSemiOneDMatMultTransOutput( vec, length, inc, A );
}


