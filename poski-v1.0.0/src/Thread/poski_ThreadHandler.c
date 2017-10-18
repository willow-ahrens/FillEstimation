/** 
 * @file src/Thread/poski_ThreadHandler.c
 * @brief Handle threading model routines.
 *
 *  This module implements the routines to handle the threading model.
 *
 *  @attention Currently, only support Pthread, Pthreadpool, and OpenMP.
 *
 *  Also, refer
 *  @ref poski_Thread_common.c
 */

#include <stdio.h>
#include <stdlib.h>

#include <poski/poski_threadcommon.h>
#include <poski/poski_matrix.h>
#include <poski/poski_malloc.h>
#include <poski/poski_print.h>
#include <poski/poski_system.h>

//-----------------------------------------------------------------------------------------
// User interface
/** 
 * @brief Create and initialize a thread object.
 *
 *  This routine sets the threading model, number of threads, and map schedule for threads.
 *
 * @return A thread object if success, or an error message otherwise.
 */
poski_threadarg_t *poski_InitThreads()
{
	poski_printMessage(1, "Initializing a thread-object...\n");

	// (1) create a thread object. 
	poski_threadarg_t *threadargs = poski_malloc(poski_threadarg_t, 1, 0);
	if (threadargs==NULL) 
	{ 
		poski_error("poski_InitThread", "can not allocate the space"); 
		return NULL; 
	}
	
	// (2) set the default threading model=<POSKI_THREADPOOL>, nthreads=<available_system_cores>.
	threadargs->ttype = POSKI_DEFAULT_THREAD;

	threadargs->cores = threadargs->nthreads = poski_get_nprocs();
	
	if (threadargs->nthreads < 1) threadargs->nthreads = 1; // adjust the number of threads (at least using one thread).

	// (3) set the NUMA affinity	
	threadargs->numa = poski_malloc(poski_int_t, threadargs->cores, 0);
	if (threadargs->numa == NULL) 
	{ 
		poski_error("poski_InitThread", "can not allocate the space"); 
		return NULL; 
	}
	
	int i;
	for(i=0;i<threadargs->cores;i++)
	{
		threadargs->numa[i] = i;
	}

	// (4) create threadpool if user specifies to use threadpool threading model
	if ( threadargs->ttype == POSKI_THREADPOOL )
	{
		poski_printMessage(2, "Creating the threadpool for %d threads...\n",threadargs->nthreads);
		poski_threadpool_t *thread = poski_CreateThreadPool(threadargs->nthreads);
		if (threadargs != NULL) 
		{
			poski_printMessage(3, "+ updating the global thread object...\n");
			threadargs->thread = thread;
		}
	}

	// report the thread-object information
	poski_printMessage(3, "+ ttype    : %s\n", poski_findthreadtype(threadargs->ttype));
	poski_printMessage(3, "+ nthreads : %d\n", threadargs->nthreads);

	// (5) return a vaild thread object.
	return threadargs;	// success!
}	

/** 
 * @brief Destory a thread object. 
 *
 * @param[in] threadargs A thread object.
 *
 * @return 1 if success, or an error message otherwise.
 */
int poski_DestroyThreads (poski_threadarg_t *threadargs)
{
	poski_printMessage(1, "Destroying the thread object...\n");

	// (1) destroy the thread object.
	if (threadargs->ttype == POSKI_THREADPOOL) poski_Destroy_threadpool(threadargs->thread);
	poski_free(threadargs->numa);
	poski_free(threadargs);
	
	return 0;	// success!
}

/**
 * @brief Modify the threading model by user's threading hint.
 *
 * @param[in,out] threadargs A thread object.
 * @param[in,out] A_tunable A tunable matrix object.
 * @param[in] ttype The threading model type in {POSKI_PTHREAD, POSKI_THREADPOOL, POSKI_OPENMP}.
 * @param[in] nthreads Number of threads.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_ThreadHints(poski_threadarg_t *threadargs, poski_mat_t A_tunable, poski_threadtype_t ttype, poski_int_t nthreads)
{
	poski_printMessage(1, "Modifying the thread object...\n");

	// (1) error-check for the input parameters
	if ( (threadargs == NULL) && (A_tunable == NULL) )
	{
		poski_error("poski_ThreadHints", "Input arguments <threadargs>, <A_tunable> are not properly set");
		return POSKI_INVALID;
	}
	if ( poski_Check_threadingmodeltype(ttype) ) 
	{
		poski_error("poski_ThreadHints", "Input argument <ttype> is not properly set");
		return POSKI_INVALID;
	}
	if ( poski_Check_numthreads(nthreads) )
	{
		poski_error("poski_ThreadHints", "Input argument <nthreads> is not properly set");
		return POSKI_INVALID;
	}

	// (2) set user's hints into the thread object.
	if ( threadargs != NULL )
	{
		poski_printMessage(2, "Setting the hints into the global thread object...\n");
		poski_printMessage(3, "+ ttype    = %s -> %s\n", poski_findthreadtype(threadargs->ttype), poski_findthreadtype(ttype));
		poski_printMessage(3, "+ nthreads = %d -> %d\n", threadargs->nthreads, nthreads);
	
		if (threadargs->ttype == POSKI_THREADPOOL && threadargs->thread!=NULL) poski_Destroy_threadpool(threadargs->thread);
		threadargs->ttype = ttype;
		threadargs->nthreads = nthreads;
	}
	if ( A_tunable != NULL )
	{
		poski_printMessage(2, "Setting the hints into the local thread object of the tunable matrix object...\n");
		poski_printMessage(3, "+ ttype    = %s -> %s\n", poski_findthreadtype(A_tunable->threadargs.ttype), poski_findthreadtype(ttype));
		poski_printMessage(3, "+ nthreads = %d -> %d\n", A_tunable->threadargs.nthreads, nthreads);
		
		if (A_tunable->threadargs.ttype == POSKI_THREADPOOL && A_tunable->threadargs.thread!=NULL) poski_Destroy_threadpool(A_tunable->threadargs.thread);
		A_tunable->threadargs.ttype = ttype;
		A_tunable->threadargs.nthreads = nthreads;
	}

	// (3) create threadpool if user specifies to use threadpool threading model
	if ( ttype == POSKI_THREADPOOL )
	{
		poski_printMessage(2, "Creating the threadpool for %d threads...\n",nthreads);
		
		poski_threadpool_t *thread = poski_CreateThreadPool(nthreads);
		if (threadargs != NULL) 
		{
			poski_printMessage(3, "+ updating the global thread object...\n");
			threadargs->thread = thread;
		}
		if (A_tunable != NULL)
		{
			poski_printMessage(3, "+ updating the local thread object of the tunable matrix object...\n");
			A_tunable->threadargs.thread = thread;
		}
	}

	return 0;	// success!
}	
// User interface
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// Report functions
/**
 * @brief Report information of a thread object.
 *
 * @param[in] threadargs Thread object.
 * @param[in] A Tunable matrix object.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_report_threadmodel(poski_threadarg_t *threadargs, poski_mat_t A)
{
	printf("\t## Threading model ##\n");
	if (poski_Check_threadargs(threadargs) )
	{
		poski_warning("poski_report_threadmodel", "The <threadargs> is not properly set");
		return -1;
	}
	printf("\t\t+ ttype       = %s\n", poski_findthreadtype(threadargs->ttype));
	printf("\t\t+ nthreads    = %d\n", threadargs->nthreads);
	
	return 0;
}
// Report functions
//-----------------------------------------------------------------------------------------








