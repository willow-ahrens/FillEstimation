/** 
 * @file src/Thread/poski_Thread_common.c
 * @brief Commonly used routines in poski_ThreadHandler's routines.
 *
 * @attention These routines are usually for the library's internal use 
 * and should not normally be called directly by users.
 * @attention Called by
 * @ref poski_ThreadHandler.c
 *
 * Also, refer
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <poski/poski_threadcommon.h>
#include <poski/poski_partitioncommon.h>
#include <poski/poski_matrixcommon.h>
#include <poski/poski_vectorcommon.h>
#include <poski/poski_kernelcommon.h>
#include <poski/poski_tunematcommon.h>
#include <poski/poski_malloc.h>
#include <poski/poski_print.h>

// internal 
/**
 * @brief [library's internal use] Error-checking for the number of threads.
 *
 * @internal
 * @param[in] nthreads Number of threads.
 *
 * @return 0 if success, or -1 otherwise.
 */
int poski_Check_numthreads(poski_int_t nthreads)
{
	if (nthreads < 1) return -1;	// error!
	
	return 0;	// success!
}

/**
 * @brief [library's internal use] Error-checking for a threading model type.
 *
 * @internal
 * This routine checks a given threading model type is valid or not.
 *
 * @param[in] ttype Threading model type in {POSKI_PTHREAD, POSKI_THREADPOOL, POSKI_OPENMP}.
 *
 * @return 0 if success, or -1 otherwise.
 */
int poski_Check_threadingmodeltype(poski_threadtype_t ttype)
{
	if ( (ttype != POSKI_SINGLE) && (ttype != POSKI_PTHREAD) \
			&& (ttype != POSKI_OPENMP) && (ttype != POSKI_THREADPOOL) ) 
		return -1;	// error!

	return 0;	// success!
}

/**
 * @brief [library's internal use] Find the threading model type for reporting.
 *
 * @internal
 * @param[in] ttype Specified threading model type.
 *
 * @return The current threading model type if success, or an error message otherwise.
 */
char *poski_findthreadtype(poski_threadtype_t ttype)
{
	switch (ttype)
	{
		case (POSKI_PTHREAD): return "POSKI_PTHREAD"; break;
		case (POSKI_OPENMP): return "POSKI_OPENMP"; break;
		case (POSKI_THREADPOOL): return "POSKI_THREADPOOL"; break;
		default: return "Invalid Thread Type"; break;
	}
}


// internal but used by others
/**
 * @brief [library's internal use] Copy the thread object.
 *
 * @internal
 * @param[in] dest A destination thread object.
 * @param[in] src A source thread object.
 *
 * @return 1 if success, or an error message otherwise.
 *
 * @attention Called by 
 * @ref poski_MatrixHandler.c
 */
int poski_Copy_threadargs(poski_threadarg_t *dest, poski_threadarg_t *src)
{
	if (src == NULL || dest == NULL) return -1;	// error!

	memcpy (dest, src, sizeof(poski_threadarg_t));
	
	return 0;	// success!
}

/**
 * @brief [library's internal use] Error-checking for thread object.
 *
 * @internal
 * @param[in] threadargs A thread object.
 *
 * @return 0 if success, or -1 otherwise.
 */
// MatrixHandler, ThreadHandler
int poski_Check_threadargs(poski_threadarg_t *threadargs)
{
	if (threadargs == NULL) return -1;	// error!
	return 0;	// success!
}


/**
 * @brief [library's internal use] Initialize threadpool object.
 *
 * @internal
 * @param[in] thread Threadpool object.
 * @param[in] nthreads Number of threads.
 *
 * @return 1 if success, or an error message otherwise.
 */
#if defined( HAVE_PTHREAD_H )
static int poski_Init_threadpool(poski_threadpool_t *thread, poski_int_t nthreads)
{
	poski_int_t *WorkAllDone = poski_malloc(poski_int_t, 1, 0);
	poski_barrier_t *StartBarrier = poski_malloc(poski_barrier_t, 1, 0);
	poski_barrier_t *EndBarrier = poski_malloc(poski_barrier_t, 1, 0);

	int tid;
	for (tid=0;tid<nthreads;tid++)
	{
		thread[tid].nthreads = nthreads;
		thread[tid].tid = tid;
		thread[tid].Job = KERNEL_NULL;
		thread[tid].WorkAllDone = WorkAllDone;
		thread[tid].StartBarrier = StartBarrier;
		thread[tid].EndBarrier = EndBarrier;
		thread[tid].kernel = NULL;
	}
	
	poski_barrier_init(StartBarrier, (nthreads+1));
	poski_barrier_init(EndBarrier, (nthreads+1));

	return 0;
}
#else
static int poski_Init_threadpool(poski_threadpool_t *thread, poski_int_t nthreads)
{
	poski_error("poski_Init_threadpool", "The pOSKI library was not built with <pthread.h>");
	return -1;
}
#endif	// HAVE_PTHREAD_H

/**
 * @brief [library's internal use] Operate a sparse matrix-vector computation in a single thread for PthreadPool model.
 *
 * @internal
 *
 * @param[in] thread A kernel object for sparse matrix-vector computations.
 *
 */
#if defined( HAVE_PTHREAD_H )
static void MatMult_threadpool(poski_matmult_t *thread)
{
	int pid;
	int num_partitions = thread->num_partitions;
	poski_submat_t *A = thread->submat;
	poski_operation_t op = thread->op;
	poski_value_t alpha = thread->alpha;
	poski_value_t beta = thread->beta;
	poski_subvec_t *x = thread->x;
	poski_subvec_t *y = thread->y;

	for (pid=0; pid<num_partitions; pid++)
	{
		poski_SubMatMult(A[pid].submat, op, alpha, x[pid].subvec, beta, y[pid].subvec);
	}
}
static void TuneMat_threadpool(poski_submat_t *thread, int num_partitions)
{
	int pid;

	for (pid=0; pid<num_partitions; pid++)
	{
		int err = poski_TuneSubMat(thread[pid].submat);
		poski_TuneMat_status(thread[pid].submat, pid, err);
	}
}
static void Reduction_threadpool(poski_reduction_t *thread)
{
	reductionMatMult(thread->tid, thread->vec, thread->A, thread->beta, thread->op, thread->nthreads);
	
	poski_free(thread);
}
static void PartitionMat_threadpoolOneD(poski_thread_t *poski_thread)
{
	poski_thread_t *thread = (poski_thread_t *)poski_thread;
	
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
static void PartitionMat_threadpoolSemiOneD(poski_thread_t *poski_thread)
{
	poski_thread_t *thread = (poski_thread_t *)poski_thread;
	
	int pid;
	int num_partitions = thread->num_partitions;

	for (pid=0; pid<num_partitions; pid++)
	{
		thread->submat[pid].partitionmat = (poski_partitionmat_t *)malloc(sizeof(poski_partitionmat_t)*1);
		poski_copyarg(thread->submat[pid].partitionmat, &thread->partitionmat[pid], poski_partitionmat_t);
		poski_sparse_matrix_t *mat = poski_CreateSubMatCSR_SemiOneD(thread->submat[pid].partitionmat, thread->A->Aptr, thread->A->Aind, thread->A->Aval, thread->A->nrows, thread->A->ncols, thread->A->nnz, thread->mode);
		thread->submat[pid].submat = (poski_matstruct_t)poski_CreateSubMatCSR(mat->Aptr, mat->Aind, mat->Aval, mat->nrows, mat->ncols, SHARE_INPUTMAT, thread->k, thread->in_props);
	}
}

/**
 * @brief [library's internal use] A single thread object in threadpool object.
 *
 * @internal 
 * This routine performs sparse matrix kernels on a single thread.
 *
 * @param[in] arg Argument for a single thread.
 *
 *
 */
static void *poski_threadpool(void *arg)
{
	poski_threadpool_t *thread = (poski_threadpool_t *)arg;
	
	poski_SetAffinity_np(thread->tid);
	
	poski_printMessage(3,"+ creating the threadpool %d [tid=0x%x]...\n", thread->tid, pthread_self());
	
	poski_int_t nthread = thread->nthreads;
	poski_barrier_t *StartBarrier = thread->StartBarrier;
	poski_barrier_t *EndBarrier = thread->EndBarrier;
	poski_int_t *WorkAllDone = thread->WorkAllDone;

	poski_barrier_wait (StartBarrier, (nthreads+1));
	poski_barrier_wait (EndBarrier, (nthreads+1));

	poski_barrier_wait (StartBarrier, (nthreads+1));

	while( (*WorkAllDone) != 1)
	{
		if ( (*WorkAllDone)==1 )
		{
			poski_barrier_wait(EndBarrier, (nthreads+1));
			break;
		}	
		switch(thread->Job)
		{
			case CREATEMAT_ONED:
				{
					poski_thread_t *kernel = (poski_thread_t *)thread->kernel;
					PartitionMat_threadpoolOneD(kernel);
					break;	
				}
			case CREATEMAT_SEMIONED:
				{
					poski_thread_t *kernel = (poski_thread_t *)thread->kernel;
					PartitionMat_threadpoolSemiOneD(kernel);
					break;	
				}
			case CREATEVEC_NORMAL:
				{
					break;
				}
			case CREATEVEC_PARTITION:
				{
					break;
				}
			case KERNEL_MatMult:
				{
					poski_matmult_t *kernel = (poski_matmult_t *)thread->kernel;
					MatMult_threadpool(kernel); 
					break;
				}
			case TUNEMAT:
				{
					poski_matmult_t *kernel = (poski_matmult_t *)thread->kernel;
					TuneMat_threadpool( (poski_submat_t *)kernel->submat, kernel->num_partitions);
					break;
				}
			case REDUCTION:
				{
					poski_reduction_t *kernel = (poski_reduction_t *)thread->kernel;
					Reduction_threadpool( kernel); 
					break;
				}
			default: break;
		}	
		poski_barrier_wait (EndBarrier, (nthreads+1));
		poski_barrier_wait (StartBarrier, (nthreads+1));
	}

	pthread_exit(NULL);
}

/**
 * @brief [library's internal use] Create a threadpool object.
 *
 * @internal
 *
 * @param[in] nthreads Number of threads.
 *
 * @return The threadpool object if success, or an error message otherwise.
 */
poski_threadpool_t *poski_CreateThreadPool(poski_int_t nthreads)
{
	int tid;

	poski_threadpool_t *threadpool = poski_malloc(poski_threadpool_t, nthreads, 0);
	
	poski_Init_threadpool(threadpool, nthreads);

	for (tid=0; tid<nthreads; tid++)
	{
		pthread_create(&threadpool[tid].thread, NULL, poski_threadpool, (void *)&threadpool[tid]);
	}

	poski_barrier_wait (threadpool->StartBarrier, (nthreads+1));
	poski_barrier_wait (threadpool->EndBarrier, (nthreads+1));

	return threadpool;
}
#else
poski_threadpool_t *poski_CreateThreadPool(poski_int_t nthreads)
{
	poski_error("poski_CreateThreadPool", "The pOSKI library was not built with <pthread.h>");

	return NULL;
}
#endif	// HAVE_PTHREAD_H

/**
 * @brief [library's internal use] Destroy the threadpool object.
 *
 * @internal
 * This routine destroys the threadpool object.
 *
 * @param[in] thread Threadpool object.
 *
 * @return 1 if success, or an error message otherwise.
 */
#if defined( HAVE_PTHREAD_H )
int poski_Destroy_threadpool(poski_threadpool_t *thread)
{
	poski_printMessage(2, "Destroying threadpool for %d threads\n", thread->nthreads);
	int tid;

	(*(thread->WorkAllDone)) = 1;
	for(tid=0;tid<thread->nthreads;tid++)
	{
		(*(thread[tid].WorkAllDone)) = 1;
	}
	int rc = poski_barrier_wait(thread->StartBarrier, (thread->nthreads+1));
	if (rc==PTHREAD_BARRIER_SERIAL_THREAD)
	{
		poski_barrier_destroy(thread->StartBarrier);
		poski_barrier_destroy(thread->EndBarrier);
	}	

	for(tid=0;tid<thread[0].nthreads;tid++)
	{
		pthread_join(thread[tid].thread, NULL);
	}

	poski_free(thread->WorkAllDone);
	poski_free(thread->StartBarrier);
	poski_free(thread->EndBarrier);
	poski_free(thread);

	return 0;
}
#else
int poski_Destroy_threadpool(poski_threadpool_t *thread)
{
	poski_error("poski_Destroy_threadpool", "The pOSKI library was not built with <pthread.h>");
	return -1;
}
#endif	// HAVE_PTHREAD_H
