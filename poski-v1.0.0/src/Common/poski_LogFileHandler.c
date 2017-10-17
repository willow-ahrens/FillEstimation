/**
 * \file src/Common/poski_LogFileHandler.c
 * \brief Handle the message output file.
 *
 *  This module implements the message output file \<poski_logfile\> support routines based on the environment variable POSKI_LOG_FILE.
 *  
 *  This module sets \<poski_logfile\> to "poski_log.txt" if POSKI_LOG_FILE==1, or to stderr otherwise.
 *  
 *  NOTE: If the library is build with pthread support, these routines are thread-safe using pthread mutex.
 *
 * @attention These routines are usually for the library's internal use
 *  and should not normally be called directly by users.
 * @attention Called by 
 * @ref poski_InitCloseHandler.c
 *
 * Also, refer
 * @ref poski_MessageHandler.c 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include <poski/poski_print.h>
#include <poski/poski_system.h>

#ifndef LOGFILENAME
#define LOGFILENAME "poski_log.txt"
#endif

#ifndef POSKI_LOG_FILE
#define POSKI_LOG_FILE "POSKI_LOG_FILE"
#endif

/**
 * \brief [library's internal use] File to where output messages should be directed.
 */
#ifndef poski_logfile
FILE *poski_logfile;
#endif

/**
 * \internal
 * \brief [library's internal use] Internal only definitions and parameters
 */
#define POSKI_TRUE 1
#define POSKI_FALSE 0

#ifndef INIT_SETLOGFILE
static bool INIT_SETLOGFILE=POSKI_FALSE;
#endif

#ifndef ISLOGFILEOPENED
static bool ISLOGFILEOPENED=POSKI_FALSE;
#endif

#ifndef NUMTRIEDOPENLOGFILE
static int NUMTRIEDOPENLOGFILE=0;
#endif

//-----------------------------------------------------------------------------------------
// Internal functions
/**
 * \brief [library's internal use] Set the output file \<poski_logfile\>.
 * 
 * NOTE: This routine is for the library's internal use only and should
 * not normally be called directly by users.
 */
void poski_InitSetLogFile()
{
	int error;

	poski_initialize_lock(logfile_lock);
	poski_logfile_lock(error, logfile_lock);

	int level;
	const char *val = getenv (POSKI_LOG_FILE);

	poski_logfile = stderr;

	if (val != NULL)
	{
		level = atoi(val);
		if(level == 1)
		{
			if (ISLOGFILEOPENED==POSKI_FALSE)
			{
				poski_printMessage(0, "NOTE: Messages are printed on <%s> file.\n",LOGFILENAME);
				poski_logfile = fopen(LOGFILENAME,"w");	// open a new logfile
				ISLOGFILEOPENED = POSKI_TRUE;
			}
			else
			{
				poski_printMessage(1, "NOTE: <%s> file is already created.\n",LOGFILENAME);
				poski_logfile = fopen(LOGFILENAME,"a");	// add into the existing logfile
			}
		}
	}
	INIT_SETLOGFILE = POSKI_TRUE;
	NUMTRIEDOPENLOGFILE++;

	poski_logfile_unlock(error, logfile_lock);
}

/*
 * \brief [library's internal use] Reset the output file \<poski_logfile\>.
 * 
 * NOTE: This routine is for the library's internal use only and should
 * not normally be called directly by users.
 */
void poski_ResetLogFile()
{
	int error;
	poski_logfile_lock(error, logfile_lock);
	
	INIT_SETLOGFILE = POSKI_FALSE;
	ISLOGFILEOPENED = POSKI_FALSE;
	
	poski_logfile_unlock(error, logfile_lock);
}

/**
 * \brief [library's internal use] Unset the output file \<poski_logfile\>.
 * 
 * NOTE: This routine is for the library's internal use only and should
 * not normally be called directly by users.
 */
void poski_CloseLogFile()
{
	int error;
	poski_logfile_lock(error, logfile_lock);

	if (poski_logfile != stderr)
	{
		if(NUMTRIEDOPENLOGFILE==1)
		{
			fclose(poski_logfile);
			poski_logfile=stderr;
			poski_printMessage(1, "NOTE: <%s> is closed.\n",LOGFILENAME);
		}
	}
	NUMTRIEDOPENLOGFILE--;

	poski_logfile_unlock(error, logfile_lock);
	
	poski_destroy_lock(logfile_lock);
}
// Internal functions
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// Test functions
int test_main_poski_InitSetLogFile()
{
	TEST_LINECUT(1);
	poski_printMessage(1, "TEST: checking poski_InitSetLogFile...\n");
	bool pre_init_setlogfile = INIT_SETLOGFILE;
	INIT_SETLOGFILE = POSKI_FALSE;	
	poski_InitSetLogFile();
	if (INIT_SETLOGFILE != POSKI_TRUE)
	{
		poski_printMessage(1, "TEST: error on poski_InitSetLogFile!\n");
		assert(0);
	}
	poski_CloseLogFile();

	INIT_SETLOGFILE = pre_init_setlogfile;
	
	poski_printMessage(1, "TEST: success on poski_InitSetLogFile!\n");
	return 0;
}

int test_main_poski_ResetLogFile()
{
	TEST_LINECUT(1);
	poski_printMessage(1, "TEST: checking poski_ResetLogFile...\n");
	bool pre_init_setlogfile = INIT_SETLOGFILE;
	INIT_SETLOGFILE = POSKI_TRUE;
	poski_ResetLogFile();
	if (INIT_SETLOGFILE != POSKI_FALSE)
	{
		poski_printMessage(1, "TEST: error on poski_ResetLogFile!\n");	
		assert(1);
	}

	INIT_SETLOGFILE = pre_init_setlogfile;

	poski_printMessage(1, "TEST: success on poski_ResetLogFile!\n");
	return 0;
}

/**
 * \brief Test the poski_LogFileHandler's routines.
 *
 * This routine is for testing the functionality of the poski_LogFileHandler's routines.
 * \return 0 if success, or an error message otherwise.
 */ 
int test_main_poski_LogFileHandler()
{
	TEST_LINECUT(0);
	poski_printMessage(0, "TEST: poski_LogFileHandler...\n");

	FILE *pre_logfile=poski_logfile;
	bool pre_init_setlogfile = INIT_SETLOGFILE;
	bool pre_islogfileopened = ISLOGFILEOPENED;

	test_main_poski_InitSetLogFile();
	test_main_poski_ResetLogFile();

	poski_logfile = pre_logfile;
	INIT_SETLOGFILE = pre_init_setlogfile;
	ISLOGFILEOPENED = pre_islogfileopened;
	poski_InitSetLogFile();
	poski_CloseLogFile();
	TEST_LINECUT(1);
	poski_printMessage(0, "TEST: Success on poski_LogFileHandler!\n");

	return 0;
}
// Test functions
//-----------------------------------------------------------------------------------------

