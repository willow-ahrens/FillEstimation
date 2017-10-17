/**
 * \file src/Common/poski_InitCloseHandler.c
 * \brief Initialize and clean-up the pOSKI library.
 *
 * This module implements the library initialization and clean-up.
 *
 * The library checks for an environment variable, whose name is
 * defined by the compile-time macros POSKI_LOG_FILE, and 
 * POSKI_MESSAGE_LEVEL, for reporting messages.
 *
 * Also, refer
 * @ref poski_LogFileHandler.c
 * @ref poski_MessageHandler.c
 */

#include <stdio.h>
#include <stdlib.h>

#include <oski/oski.h>
#include <poski/poski_version.h>
#include <poski/poski_system.h>
#include <poski/poski_print.h>

//-----------------------------------------------------------------------------------------
// User interface
/**
 * \brief Initialize the pOSKI library. 
 * 
 * \return 0 on success, an error message on error.
 */
int poski_Init(void)
{

	/// (1) set POSKI_LOG_FILE
	poski_InitSetLogFile();		 

	/// (2) set POSKI_MESSAGE_LEVEL
	poski_InitSetMessageLevel();	

	poski_printMessage(1, "Initializing %s...\n", PACKAGE_STRING);	

	/// (3) check system configuration
	if (poski_system_configure() < 0)
	{
		poski_error("poski_Init", "failed to initialize the number of cores on the system");
		return -1;	// fail!
	}
	
	/// (4) initalize library
	oski_Init();	
		
	return 0;	// success!
}

/**
 * \brief Shut down the pOSKI library.
 * 
 * \return 0 on success, an error message on error.
 */
int poski_Close()
{
	/// (1) close library
	oski_Close();	

	poski_printMessage(1, "Shutting down %s...\n", PACKAGE_STRING);

	/// (2) close POSKI_LOG_FILE
	poski_CloseLogFile();	

	return 0;	// success!
}
// User interface
//-----------------------------------------------------------------------------------------

