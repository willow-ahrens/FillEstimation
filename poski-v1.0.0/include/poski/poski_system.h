#ifndef _POSKI_SYSTEM_H
#define _POSKI_SYSTEM_H

#include "poski_config.h"

#if defined( HAVE_PTHREAD_H )
#	include <pthread.h>
#	define _USE_PTHREAD 1
#endif	// HAVE_PTHREAD

#if defined( HAVE_OPENMP_H )
#	include <omp.h>
#	define _USE_OPENMP 1
#endif
 
#ifndef _WIN32
#       include <sched.h>
#	if defined(HAVE_UNISTD_H)
#       	include <unistd.h>
#	endif
#       if defined( __sun )
#               include <sys/pset.h>
#       endif	// __sun
#       include <sys/types.h>
#       if defined( __APPLE__ )
#               include <sys/sysctl.h>
#       endif	// __APPLE__
#endif	// _WIN32

#ifdef __USE_POSIX
#	define get_pagesize() sysconf(_SC_PAGESIZE)
#else	//__USE_POSIX
#	define get_pagesize() 1
#endif	//__USE_POSIX

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

/* Internal */

#if defined( _USE_PTHREAD )
	static pthread_mutex_t logfile_lock = PTHREAD_MUTEX_INITIALIZER;
#	define poski_initialize_lock(logfile_lock)
#	define poski_destroy_lock(logfile_lock)
#	define poski_logfile_lock(error, logfile_lock)	\
	{	\
		error = pthread_mutex_lock (&logfile_lock);	\
		if (error != 0)	\
		{	\
			fprintf(stderr, "!*** pOSKI ERROR: failed to lock the shared variable <poski_logfile>!\n");	\
			assert(0);	\
		}	\
	}
#	define poski_logfile_unlock(error, logfile_lock)	\
	{	\
		error = pthread_mutex_unlock (&logfile_lock);	\
		if (error != 0)	\
		{	\
			fprintf(stderr, "!*** pOSKI ERROR: failed to unlock the shared variable <poski_logfile>!\n");	\
			assert(0);	\
		}	\
	}
#elif defined( _USE_OPENMP )
	omp_lock_t logfile_lock;
#	define poski_initialize_lock(logfile_lock) omp_init_lock(&logfile_lock)
#	define poski_destroy_lock(logfile_lock) omp_destroy_lock(&logfile_lock)
#	define poski_logfile_lock(error, logfile_lock) omp_set_lock(&logfile_lock)
#	define poski_logfile_unlock(error, logfile_lock) omp_unset_lock(&logfile_lock)
#else
	static int logfile_lock;
#	define poski_initialize_lock(logfile_lock)
#	define poski_destroy_lock(logfile_lock)
#	define poski_logfile_lock(error, logfile_lock) 
#	define poski_logfile_unlock(error, logfile_lock)
#endif

pid_t poski_gettid(void);
int poski_get_nprocs();
int poski_system_configure(void);
int poski_CheckConfig();

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_SYSTEM_H
