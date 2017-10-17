/**
 * @file src/Common/poski_SysConfigureHandler.c
 * @brief Check the current system configurations.
 * 
 *  This module implements the system configuration handler routines.
 *
 *  NOTE: Currently, only support Linux and Solaris.
 * @attention These routines are usually for the library's internal use
 *  and should not normally be called directly by users.
 * @attention Called by
 * @ref poski_ThreadHandler.c 
 */

#include <stdio.h>

#include <poski/poski_system.h>
#include <poski/poski_print.h>

//#ifndef __USE_GNU
//#       define __USE_GNU
//#endif	// __USE_GNU

//-----------------------------------------------------------------------------------------
// Internal functions
/**
 * @internal 
 * @brief Get the system pagesize
 *
 *  NOTE: currently, this routine is for the library's internal use only.
 * @return The system pagesize on success, an error message otherwise.
 */
static long int poski_get_pagesize(void)
{
	long int pagesize = get_pagesize();
	
	return pagesize;
}

/**
 * @internal
 * @brief Get the number of system cores for Solaris.
 * @return The number of available cores on system.
 */
#ifdef PS_MYID /* Solaris processor set API */
static int poski_pset_get_nprocs (void)
{
        uint_t available = 0;

        pset_info (PS_MYID, NULL, &available, NULL);
        return (int)available;
}
#endif	// PS_MYID

/**
 * @internal
 * @brief Get process id.
 * @return The current process id.
 */
#ifdef CPU_SETSIZE /* Linux CPU affinity system calls */
pid_t poski_gettid (void)
{
#if defined( SYS_gettid )
        return (pid_t) syscall (SYS_gettid);
#else
        return getpid();
#endif	// SYS_gettid
}

/**
 * @internal
 * @brief Get the number of system cores for Linux.
 * @return The number of available cores on system.
 */
static int poski_sched_get_nprocs (void)
{
        int available = 0;
        cpu_set_t cpu_set;
	int i;
        if (0 == sched_getaffinity (poski_gettid(), sizeof (cpu_set), &cpu_set))
                for (i = 0; i < CPU_SETSIZE; i++)
                        if (CPU_ISSET (i, &cpu_set))
                                available++;

        return available;
}
#endif	// CPU_SETSIZE

/**
 * @internal
 * @brief Get the number of system cores.
 * @return The number of available cores on system.
 */
#if defined( _SC_NPROCESSORS_ONLN ) || defined( _SC_NPROCESSORS_CONF )
static int poski_sysconf_get_nprocs (void)
{
	int available = 0, online = 0, configured = 0;

#ifdef _SC_NPROCESSORS_ONLN
	online = (int)sysconf (_SC_NPROCESSORS_ONLN);
#endif
#ifdef _SC_NPROCESSORS_CONF
	configured = (int)sysconf (_SC_NPROCESSORS_CONF);
#endif

	if (online > configured) online = configured;

#if defined( PS_MYID )	/* Solaris */
	available = poski_pset_get_nprocs();
#elif defined( CPU_SETSIZE )	/* Linux */
	available = poski_sched_get_nprocs();
#endif

	if (available > online) available = online;

	poski_printMessage(3, "+ Detected %d available %d online %d configured CPUs.\n",available, online, configured);

	return (available > 0) ? available : ((online > 0) ? online : configured);
}
#endif	// _SC_NPROCESSROS_ONLN || _SC_NPROCESSROS_CONF

/**
 * @internal
 * @brief Get the number of system cores.
 *
 *  NOTE: currently, this routine is for the library's internal use only.
 * @return The number of available cores on system.
 */
int poski_get_nprocs (void)
{
#if defined( _SC_NPROCESSORS_ONLN ) || defined( _SC_NPROCESSORS_CONF )
	return poski_sysconf_get_nprocs();	// success!
#else	
	return -1;	// fail!
//#       error "Unsupported processor enumeration on this platform."
#endif	
}

/**
 * @brief Check the system configurations.
 *
 *  This routine is for checking the system configurations including the pagesize and the number of cores on the system.
 * @return The number of available cores on system.
 */
int poski_system_configure(void)
{
	poski_printMessage(2, "Checking system configuration...\n");
	
	// check the number of available cores
	int ncores = poski_get_nprocs();
	poski_printMessage(2, "\t + The number of available cores for this system: %d cores.\n",ncores);

	// check the system pagesize
	long int pagesize = poski_get_pagesize();
	poski_printMessage(2, "\t + The page size for this system: %ld bytes\n",pagesize);
	
	// check the parallel programming
	poski_CheckConfig();

	return ncores;	// success if ncores > 0, else fail!
}

int poski_CheckConfig()
{
#ifdef HAVE_CONFIG_H
	poski_printMessage(2, "\t + HAVE_CONFIG_H has been defined!\n");
#endif
#if defined(HAVE_PTHREAD_H)
	poski_printMessage(2, "\t + HAVE_PTHREAD_H has been defined!\n");
#endif
#if defined(HAVE_OPENMP_H)
	poski_printMessage(2, "\t + HAVE_OPENMP_H has been defined!\n");
#endif
#if defined(_USE_PTHREAD)
	poski_printMessage(2, "\t + USE_PTHREAD has been defined!\n");
#endif
#if defined(_USE_OPENMP)
	poski_printMessage(2, "\t + USE_OPENMP has been defined!\n");
#endif
	return 0;
}
// Internal functions
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// Test functions
/**
 * @brief Test the poski_SySConfigureHandler's routines.
 *
 *  This routine is for testing the functionality of the poski_SySConfigureHandler's routines.
 * @return 0 on success, an error message otherwise.
 */
int test_main_poski_system_configure()
{
	LINECUT(0);
	poski_printMessage(0,"Test poski_system_configure...\n");
	if (poski_system_configure() != -1 )
	{ 
		poski_printMessage(0,"... Passed!\n");
	}
	else
	{
		poski_printMessage(0,"... Failed!\n");
	}
	LINECUT(0);

	return 0;
}
// Test functions
//-----------------------------------------------------------------------------------------

/* eof */
