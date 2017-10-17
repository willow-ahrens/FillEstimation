/** 
 * @file src/Thread/poski_Thread_NUMA.c
 * @brief Control NUMA affinity.
 *
 * @attention These routines are usually for the library's internal use 
 * and should not normally be called directly by users.
 * @attention Called by
 * @ref
 *
 * Also, refer
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <poski/poski_threadnuma.h>
#include <poski/poski_print.h>

// internal 
/**
 * @brief [library's internal use] Set NUMA affinity.
 *
 * @internal
 * @param[in] tid Specifies the thread id.
 *
 * @return 0 if success, or -1 otherwise.
 */

// LINUX
#if defined( HAVE_NUMA_H )
#include <numa.h>
//nodemask_t mask;
int poski_SetAffinity_np(int tid)
{
//	nodemask_zero(&mask);
//	nodemask_set(&mask,tid);
	//numa_set_membind(&mask);	// same as numa_bind(&mask) and numa_run_on_node_mask(&mask).
//	numa_run_on_node(tid);

	numa_run_on_node(numa_preferred());

	return 0;
}
#elif defined( HAVE_PTHREAD_H )
#define __USE_GNU
#include <pthread.h>
#include <sched.h>

int poski_SetAffinity_np(int tid)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(tid,&mask);
	if (pthread_setaffinity_np(pthread_self(),sizeof(cpu_set_t),&mask)<0)
	{
		poski_error("poski_SetAffinity_np", "can not set cpu affinity");
		return -1;
	}
	if (pthread_getaffinity_np(pthread_self(),sizeof(cpu_set_t),&mask)<0)
	{
		poski_error("poski_SetAffinity_np", "can not get cpu affinity");
		return -1;
	}
		
	return 0;	// success!
}
#elif defined( HAVE_SCHED_H )
#include <sched.h>
int poski_SetAffinity_np(int tid)
{
	unsigned long mask = (unsigned long)(tid);
	if (sched_setaffinity(getpid(), sizeof(mask), &mask)<0)
	{
		return -1;
	}

	//sched_getaffinity(0,sizeof(mask),&mask);

	return 0;
}
#endif	// HAVE_PTHREAD_H
