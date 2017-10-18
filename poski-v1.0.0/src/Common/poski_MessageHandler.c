/** 
 * \file src/Common/poski_MessageHandler.c
 * \brief Handle the output messages.
 * 
 * This module implements the output message handler routines based on the environment variable POSKI_MESSAGE_LEVEL.
 * 
 * NOTE: If the library is build with pthread support, these routines are thread-safe using pthread mutex.
 * @attention These routines are usually for the library's internal use
 *  and should not normally be called directly by users.
 * @attention Called by
 * @ref poski_InitCloseHandler.c 
 *
 * Also, refer
 * @ref poski_LogFileHandler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#include <poski/poski_print.h>
#include <poski/poski_system.h>

#ifndef POSKI_MESSAGE_LEVEL
#define POSKI_MESSAGE_LEVEL "POSKI_MESSAGE_LEVEL"
#endif

extern FILE *poski_logfile;

/**
 * \brief [library's internal use] Current message level.
 */
#ifndef g_curr_message_level
static unsigned g_curr_message_level = 0;
#endif

//-----------------------------------------------------------------------------------------
// Internal functions
#define poski_check_logfile(logfile)	\
{	\
	if (logfile == NULL) 	\
	{	\
		fprintf(stderr, "!*** pOSKI ERROR: poski_logfile is not set!\n"); 	\
		assert(0);	\
	}	\
}

#define poski_valist_logfile(logfile, fmt, ap)	\
{	\
	va_start(ap,fmt);	\
	vfprintf(logfile,fmt,ap);	\
	va_end(ap);	\
}

void TEST_LINECUT(int i) 
{	
	if (i==0) poski_printMessage(0, "==================================================================\n");	
	if (i==1) poski_printMessage(0, "------------------------------------------------------------------\n");	
}
void LINECUT(int i) 
{	
	if (g_curr_message_level > 0)	
	{	
		if (i==0) poski_printMessage(1, "=================================================================\n");	
		if (i==1) poski_printMessage(1, "-----------------------------------------------------------------\n");	
	}	
}

/**
 * \internal
 * \brief [library's internal use] Get current message level.
 *
 *  NOTE: This routine is for the library's internal use only and should not normally be called directly by users.
 */
static unsigned poski_GetMessageLevel(void)
{
	return g_curr_message_level;
}

/**
 * \internal
 * \brief [library's internal use] Set new message level.
 *
 *  Sets g_curr_message_level to the given level.
 *  NOTE: This routine is for the library's internal use only and should not normally be called directly by users.
 * \param[in] new_level New message level.
 */
static void poski_SetMessageLevel (unsigned new_level)
{
	g_curr_message_level = new_level;
}

/**
 * \brief [library's internal use] Set message level based on the environment variable POSKI_MESSAGE_LEVEL.
 *
 * NOTE: This routine is for the library's internal use only and should not normally be called directly by users.
 */
void poski_InitSetMessageLevel (void)
{
	int error;
	poski_logfile_lock(error, logfile_lock);

	int level;
	const char *val = getenv (POSKI_MESSAGE_LEVEL);
	if (val != NULL)
	{
		level = atoi (val);
		poski_SetMessageLevel (level);
	}

	poski_logfile_unlock(error, logfile_lock);
}

void poski_printMessage1(unsigned level, const char *fmt, ...)
{
	if ((poski_GetMessageLevel() >= level) && (fmt != NULL))	
	{
		poski_check_logfile(poski_logfile);
		
		int error;
		unsigned i;
		va_list ap;

		poski_logfile_lock(error, logfile_lock);

		fprintf(poski_logfile,"  ");

		poski_valist_logfile(poski_logfile, fmt, ap);

		fflush(poski_logfile);

		poski_logfile_unlock(error, logfile_lock);
	}
}

void poski_printContinue()
{
	int error;
	poski_logfile_lock(error, logfile_lock);
	fprintf(poski_logfile,".");
	poski_logfile_unlock(error, logfile_lock);
}
// Internal functions
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// User interface
/**
 * \brief Print an message
 *
 * The message is displayed to the \<poski_logfile\> only if level >= the current message level (g_curr_message_level).
 * \param[in] level Level at and above which this message should be printed.
 * \param[in] fmt A printf-compatible format string.
 * \param[in] ... Options for the formatting string.
 */
void poski_printMessage(unsigned level, const char *fmt, ...)
{
	if ((poski_GetMessageLevel() >= level) && (fmt != NULL))	
	{
		poski_check_logfile(poski_logfile);

		int error;
		unsigned i;
		va_list ap;

		poski_logfile_lock(error, logfile_lock);

		fprintf(poski_logfile,"!* ");
		for (i=0; i<level; i++)
			fprintf(poski_logfile,"  ");

		poski_valist_logfile(poski_logfile, fmt, ap);

		fflush(poski_logfile);

		poski_logfile_unlock(error, logfile_lock);
	}
}



/**
 * \brief Report an error message
 *
 * The error message is displayed to the \<poski_logfile\> when an error is occured. 
 * \param[in] func_name The function name which occurs an error.
 * \param[in] msg A printf-compatible format string for the error message.
 * \param[in] ... Options for the formatting string.
 */
// TODO: message level?
void poski_error(const char *func_name, const char *msg, ...)
{
	if ((poski_GetMessageLevel() >= 0) && (func_name != NULL) && (msg != NULL))	
	{
		int error;
		poski_logfile_lock(error, logfile_lock);

		poski_check_logfile(poski_logfile);

		FILE *logfile = stderr;

		fprintf(logfile, "!*** pOSKI ERROR: ");

		va_list ap;

		poski_valist_logfile(logfile, msg, ap);

		fprintf(logfile, " on %s!\n", func_name);

		fflush(logfile);

		poski_logfile_unlock(error, logfile_lock);
	}
}

/**
 * \brief Report a warning message
 *
 * The warning message is displayed to the \<poski_logfile\> when an warning is occured. 
 * \param[in] func_name The function name which occurs an warning.
 * \param[in] msg A printf-compatible format string for the warning message.
 * \param[in] ... Options for the formatting string.
 */
// TODO: message level?
void poski_warning(const char *func_name, const char *msg, ...)
{
	if ((poski_GetMessageLevel() >= 0) && (func_name != NULL) && (msg != NULL))	
	{
		int error;
		poski_logfile_lock(error, logfile_lock);

		poski_check_logfile(poski_logfile);

		FILE *logfile = stderr;

		fprintf(logfile, "!*** pOSKI WARNING: ");

		va_list ap;

		poski_valist_logfile(logfile, msg, ap);

		fprintf(logfile, " on %s!\n", func_name);

		fflush(logfile);

		poski_logfile_unlock(error, logfile_lock);
	}
}
// User interface
//-----------------------------------------------------------------------------------------



