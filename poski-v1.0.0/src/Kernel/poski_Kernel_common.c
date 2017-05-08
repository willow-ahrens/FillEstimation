/**
 * @file src/Kernel/poski_Kernel_common.c
 * @brief Commonly used routines in kernel Handler's modules.
 *
 *  This module implements the commonly used routines in kernel handler's modules.
 *  
 * @attention These routines are usually for the library's internal use
 *  and should not normally be called directly by users.
 * @attention Called by
 * @ref poski_KernelHandler.c
 *
 * Also, refer 
 * @ref poski_Kernel_matmult.c
 */

#include <stdio.h>
#include <stdlib.h>

#include <poski/poski_kernelcommon.h>
#include <poski/poski_threadcommon.h>
#include <poski/poski_partitioncommon.h>
#include <poski/poski_malloc.h>
#include <poski/poski_print.h>

static poski_int_t poski_max(poski_int_t a, poski_int_t b)
{
	if (a > b) return a;
	else return b;
}
static poski_int_t poski_min(poski_int_t a, poski_int_t b)
{
	if (a < b) return a;
	else return b;
}

static void reductionMatMult_OP_NORMAL(poski_vec_t vec, poski_int_t start, poski_int_t length, poski_value_t beta, poski_mat_t A, poski_int_t npartitions)
{
	int i,j;
	poski_value_t *temp = poski_malloc(poski_value_t, length, 0);
	poski_value_t *val;

	for (i=0;i<length;i++) temp[i]=0.0;

	poski_int_t row_start;
	poski_int_t nrows;

	int count=0;
	for (i=0;i<npartitions;i++)
	{
		row_start = poski_max(start, A->submatrix[i].partitionmat->row_start);

		nrows = poski_min((length+start-row_start), (A->submatrix[i].partitionmat->row_start + A->submatrix[i].partitionmat->nrows - row_start));
		if (row_start < (start+length))
		{
			for(j=0;j<nrows;j++)
			{	
				val = &vec->subvector[i].subvec->val[(row_start - (A->submatrix[i].partitionmat->row_start))+j];
				temp[row_start-start+j] += val[0]; //vec->subvector[i].subvec->val[(row_start - (A->submatrix[i].partitionmat->row_start))+j];
				val[0]=0.0;
				//vec->subvector[i].subvec->val[(row_start - (A->submatrix[i].partitionmat->row_start))+j] = 0.0;
			}
		}
	}

	for (i=0;i<length;i++)
	{
		vec->vec[start+i] = temp[i] + vec->vec[start+i]*beta;
	}
	
	poski_free(temp);
}

void reductionMatMult_OP_TRANS(poski_vec_t vec, poski_int_t start, poski_int_t length, poski_value_t beta, poski_mat_t A, poski_int_t npartitions)
{
	int i,j;

	poski_value_t temp,*val;
	for (i=0;i<length;i++) 
	{
		temp = 0.0;
		for (j=0;j<npartitions;j++)
		{
			val = &vec->subvector[j].subvec->val[start+i];
			temp += val[0];
			val[0] = 0.0;
		}

		vec->vec[start+i] = temp + vec->vec[start+i]*beta;
	}
}

void reductionMatMult(poski_int_t tid, poski_vec_t vec, poski_mat_t A, poski_value_t beta, poski_operation_t op, poski_int_t nthreads)
{
	poski_SetAffinity_np(tid);

	poski_int_t start;
	poski_int_t total_length;
	if (op==OP_NORMAL) total_length = A->nrows;
	else if (op==OP_TRANS) total_length = A->ncols;
	else poski_error("poski_reductionMatMult", "can not perform the reduction on thread %d\n", tid);

	poski_int_t reminder = (total_length%nthreads);
	poski_int_t length;
	
	if (nthreads==1) 
	{
		length = total_length;	
		start = 0;
	}
	else if (tid<(nthreads-1))
	{
		length = (poski_int_t)((total_length - reminder) / nthreads);
		start = tid*length;
	}
	else if (tid==(nthreads-1))
	{
		length = (poski_int_t)((total_length - reminder) / nthreads);
		start = tid*length;
		length = length + reminder;
	}
	else
	{
		poski_error("poski_reductionMatMult", "can not perform the reduction on thread %d\n", tid);
	}

	if (op==OP_NORMAL)
		reductionMatMult_OP_NORMAL(vec, start, length, beta, A, A->partitionargs.npartitions);
	else if (op==OP_TRANS)
		reductionMatMult_OP_TRANS(vec, start, length, beta, A, A->partitionargs.npartitions);
	else
		poski_error("poski_reductionMatMult", "can not perform the reduction on thread %d\n", tid);

}

#if defined( HAVE_OPENMP_H )
static int poski_reductionMatMult_openmp(poski_vec_t vec, poski_mat_t A, poski_value_t beta, poski_operation_t op)
{
	poski_int_t nthreads = A->threadargs.nthreads;
	poski_int_t tid;

	#pragma omp parallel for num_threads(nthreads)
	for (tid=0;tid<nthreads;tid++)
	{
		reductionMatMult(tid, vec, A, beta, op, nthreads);
	}

	return 0;	// success!
}
#else
static int poski_reductionMatMult_openmp(poski_vec_t vec, poski_mat_t A, poski_value_t beta, poski_operation_t op)
{
	poski_error("poski_reductionMatMult_openmp", "The pOSKI library was not build with <omp.h>");
	return -1;
}
#endif	// HAVE_OPENMP_H

#if defined( HAVE_PTHREAD_H )
void *reductionMatMult_pthread(void *poski_thread)
{
	poski_reduction_t *thread = (poski_reduction_t *)poski_thread;
	reductionMatMult(thread->tid, thread->vec, thread->A, thread->beta, thread->op, thread->nthreads);

	poski_free(thread);
	pthread_exit(NULL);
}
static poski_reduction_t *set_reduction_args(poski_int_t tid, poski_vec_t vec, poski_mat_t A, poski_value_t beta, poski_operation_t op, poski_int_t nthreads)
{
	poski_reduction_t *poski_thread = poski_malloc(poski_reduction_t, 1, 0);
	poski_thread->tid = tid;
	poski_thread->vec = vec;
	poski_thread->A = A;
	poski_thread->beta = beta;
	poski_thread->op = op;
	poski_thread->nthreads = nthreads;

	return poski_thread;
}
static int poski_reductionMatMult_pthread(poski_vec_t vec, poski_mat_t A, poski_value_t beta, poski_operation_t op)
{
	poski_int_t nthreads = A->threadargs.nthreads;
	poski_int_t tid;

	poski_pthread_t *threads = poski_malloc(poski_pthread_t, nthreads,0);

	for (tid=0;tid<nthreads;tid++)
	{	
		pthread_create(&threads[tid], NULL, reductionMatMult_pthread, (void *)(set_reduction_args(tid,vec,A,beta,op,nthreads)));
	}
	for (tid=0;tid<nthreads;tid++)
	{
		pthread_join(threads[tid], NULL);
	}
	
	poski_free(threads);
	return 0;	// success!
}
#else
static int poski_reductionMatMult_pthread(poski_vec_t vec, poski_mat_t A, poski_value_t beta, poski_operation_t op)
{
	poski_error("poski_reductionMatMult_pthread", "The pOSKI library was not build with <pthread.h>");
	return -1;
}
#endif	// HAVE_PTHREAD_H

#if defined( HAVE_PTHREAD_H )

static int poski_reductionMatMult_threadpool(poski_vec_t vec, poski_mat_t A, poski_value_t beta, poski_operation_t op)
{
	poski_int_t nthreads = A->threadargs.nthreads;
	poski_int_t tid;
	poski_threadpool_t *threadpool = A->threadargs.thread;

	poski_pthread_t *threads = poski_malloc(poski_pthread_t, nthreads, 0);

	for (tid=0;tid<nthreads;tid++)
	{
		threadpool[tid].Job = REDUCTION;
		threadpool[tid].kernel = (void *)(set_reduction_args(tid,vec,A,beta,op,nthreads));
	}

	poski_barrier_wait (threadpool->StartBarrier, (nthreads+1));
	poski_barrier_wait (threadpool->EndBarrier, (nthreads+1));

	poski_free(threads);
	return 0;	// success!
}
#else
static int poski_reductionMatMult_threadpool(poski_vec_t vec, poski_mat_t A, poski_value_t beta, poski_operation_t op)
{
	poski_error("poski_reductionMatMult_threadpool", "The pOSKI library was not build with <pthread.h>");
	return -1;
}
#endif	// HAVE_PTHREAD_H

static int poski_reductionMatMult_seq(poski_vec_t vec, poski_mat_t A, poski_value_t beta, poski_operation_t op)
{
	int i,j;
	poski_int_t npartitions = A->partitionargs.npartitions;
	poski_int_t nthreads = A->threadargs.nthreads;

	poski_int_t row_start;
	poski_int_t nrows;

	poski_int_t length;

	if (op==OP_NORMAL) length = A->nrows;
	else if (op==OP_TRANS) length = A->ncols;

	poski_value_t *temp = poski_malloc(poski_value_t, length, 0);
	for(i=0;i<length;i++) temp[i] = 0.0;

	if (op==OP_NORMAL)
	{
		for (i=0;i<npartitions;i++)
		{
			row_start = A->submatrix[i].partitionmat->row_start;
			nrows = A->submatrix[i].partitionmat->nrows;
			for(j=0;j<nrows;j++)
			{
				temp[row_start+j] += vec->subvector[i].subvec->val[j];
				vec->subvector[i].subvec->val[j] = 0.0;
			}
		}
	}
	else if (op==OP_TRANS)
	{
		for (i=0;i<npartitions;i++)
		{
			row_start = 0;
			nrows = A->submatrix[i].partitionmat->ncols;
			for(j=0;j<nrows;j++)
			{
				temp[row_start+j] += vec->subvector[i].subvec->val[j];
				vec->subvector[i].subvec->val[j] = 0.0;
			}
		}
	}

	for (i=0;i<length;i++)
	{
		vec->vec[i] = temp[i] + vec->vec[i]*beta;
	}

	poski_free(temp);

	return 0;
}

/**
 * @brief [library's internal use] Reduction operation for a vector.
 *
 * @internal
 * @param[in] vec Vector view object.
 * @param[in] A Sparse matrix object.
 * @param[in] beta Scalar multiplier.
 * @return 0 if success, or an error message otherwise
 */
int poski_reductionMatMult(poski_vec_t vec, poski_mat_t A, poski_value_t beta, poski_operation_t op)
{
	int require = 0;
	if (A->partitionargs.ptype==SemiOneD) require=1;
	else if (A->partitionargs.ptype==OneD && op==OP_TRANS) require=1;

	if (require==1)
	{
		poski_printMessage(2, "+ performing reduction operation on %d threads with %s, %s and %s.\n", A->threadargs.nthreads, poski_findoperation(op), poski_findpartitiontype(A->partitionargs.ptype), poski_findthreadtype(A->threadargs.ttype));
		if (A->threadargs.nthreads==1)
		{
			poski_reductionMatMult_seq(vec, A, beta, op);
		}
		else
		{
			switch(A->threadargs.ttype)
			{
				case POSKI_PTHREAD: {poski_reductionMatMult_pthread(vec, A, beta, op); break;}
				case POSKI_THREADPOOL: {poski_reductionMatMult_threadpool(vec, A, beta, op); break;}
				case POSKI_OPENMP: {poski_reductionMatMult_openmp(vec, A, beta, op); break;}
				default: {poski_error("poski_reductionMatMult", "the thread type is not properly set"); return -1; break;}	// fail!
			}	
		}
	}

	return 0;	// success!
}

/**
 * @brief [library's internal use] Check the kernel type.
 *
 * @internal
 * @param[in] ktype Specifies the kernel type.
 * @return 0 if success, or an error message otherwise.
 */
int poski_Check_kerneltype(poski_kernel_t ktype)
{
	if ( (ktype != KERNEL_NULL) && (ktype != KERNEL_MatMult) \
	&& (ktype != KERNEL_MatTrisolve) && (ktype != KERNEL_MatTransMatMult) \
	&& (ktype != KERNEL_MatPowMult) )
	{
		return -1;
	}
	return 0;	// success!
}

/**
 * @brief [library's internal use] Find the kernel type for reporting.
 *
 * @internal
 * @param[in] ktype Specifies the kernel type.
 * @return The current kernel type if success, or an error message otherwise.
 */
char *poski_findkerneltype(poski_kernel_t ktype)
{
	switch (ktype)
	{
		case (KERNEL_MatMult): return "KERNEL_MatMult"; break;
		case (KERNEL_MatTrisolve): return "KERNEL_MatTrisolve"; break;
		default: return "Invalid Kernel Type"; break;
	}
}

/**
 * @brief [library's internal use] Check the transpose operation type.
 *
 * @internal 
 * @param[in] op Specifies the transpose operations, \f$op(A)\f$ in {OP_NORMAL, OP_TRANS} currently.
 * @return 0 if success, or an error message otherwise.
 */
int poski_Check_operations(poski_operation_t op)
{
	if ( (op != OP_NORMAL) && (op != OP_TRANS) )
	{
		return -1;
	}
	return 0; 	// success!
}

/** 
 * @brief [library's internal use] Find the transpose operation type for reporting.
 *
 * @internal
 * @param[in] op Specifies the transpose operations, 
 * @return The current transpose operation type if success, or an error message otherwise.
 */
char *poski_findoperation(poski_operation_t op)
{
	switch (op)
	{
		case (OP_NORMAL): return "OP_NORMAL"; break;
		case (OP_TRANS): return "OP_TRANS"; break;
		default: return "Invalid Kernel Operation"; break;
	}
}
