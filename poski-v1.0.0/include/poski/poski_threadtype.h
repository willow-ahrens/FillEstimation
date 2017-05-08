#ifndef _POSKI_THREADTYPE_H
#define _POSKI_THREADTYPE_H

#include "poski_config.h"
#include "poski_commontype.h"

typedef enum
{
	POSKI_SINGLE,
	POSKI_PTHREAD,
	POSKI_OPENMP,
	POSKI_THREADPOOL,
	/* add other threading models */
	POSKI_THREAD_LAST
}poski_threadtype_t;

#if defined ( HAVE_PTHREAD_H )
#	include <pthread.h>
#	define poski_pthread_t				pthread_t
#	define poski_barrier_t 				pthread_barrier_t
#	define poski_barrier_init(barrier, count)	pthread_barrier_init(barrier, NULL, count)
#	define poski_barrier_destroy(barrier) 		pthread_barrier_destroy(barrier)
#	define poski_barrier_wait(barrier, thread)	pthread_barrier_wait(barrier)
#else
#	define poski_pthread_t				int
#	define poski_barrier_t				int
#	define poski_barrier_init(barrier, count)	
#	define poski_barrier_destroy(barrier)
#	define poski_barrier_wait(barrier, thread)
#endif	// HAVE_PTHREAD_H

#if defined ( HAVE_PTHREAD_H )
#	ifndef POSKI_DEFAULT_THREAD
#		define POSKI_DEFAULT_THREAD POSKI_THREADPOOL
#	endif // POSKI_DEFAULT_THREAD
#elif defined ( HAVE_OPENMP_H )
#	ifndef POSKI_DEFAULT_THREAD
#		define POSKI_DEFAULT_THREAD POSKI_OPENMP
#	endif // POSKI_DEFAULT_THREAD
#else
#	ifndef POSKI_DEFAULT_THREAD
#		define POSKI_DEFAULT_THREAD POSKI_SINGLE 
#	endif // POSKI_DEFAULT_THREAD
#endif

typedef struct
{
	poski_int_t nthreads;
	poski_int_t tid;	
	poski_pthread_t thread;
	poski_int_t Job;
	poski_int_t *WorkAllDone;
	poski_barrier_t *StartBarrier;
	poski_barrier_t *EndBarrier;
	void *kernel;
}poski_threadpool_struct;
typedef poski_threadpool_struct poski_threadpool_t;

typedef struct 
{
	poski_int_t cores;		/* number of system available cores */
	poski_threadtype_t ttype;	/* threading model type */
	poski_int_t nthreads;		/* number of threads */
	poski_threadpool_t *thread;
	/* Future work: other information of threading model
	 * {NUMA affinities, thread affinities, environments, etc.} */
	poski_int_t *numa;
}poski_threadarg_struct;
typedef poski_threadarg_struct poski_threadarg_t;


#endif // _POSKI_THREADTYPE_H
