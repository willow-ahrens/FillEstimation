/**
 *  \file src/init.c
 *  \ingroup AUXIL
 *  \brief Initialize the OSKI library.
 *
 *  This module implements the library initialization and clean-up.
 *
 *  The library checks for an environment variable, whose name is
 *  defined by the compile-time macro OSKI_DEBUG_LEVEL, for a
 *  debugging level.
 *
 *  \todo Make this module thread-safe!
 */

#if HAVE_CONFIG_H
#include <config/config.h>
#endif

#if !defined(PACKAGE_STRING)
#define PACKAGE_STRING "BeBOP-OSKI (unknown version)"
#endif

#include <string.h>
#include <stdlib.h>

#include <oski/common.h>
#include <oski/modloader.h>

#if !defined(OSKI_DEBUG_LEVEL)
/** Default environment variable to hold the initial debugging level */
#define OSKI_DEBUG_LEVEL "OSKI_DEBUG_LEVEL"
#endif

/**
 *  \brief Set the debugging level from the environment variable
 *  whose name is given by the compile-time macro, OSKI_DEBUG_LEVEL.
 *
 *  \post Changes the debug level via a call to #oski_SetDebugLevel.
 *  \see oski_SetDebugLevel
 */
static void
InitSetDebugLevel (void)
{
  int level;
  const char *val = getenv (OSKI_DEBUG_LEVEL);
  if (val != NULL)
    {
      level = atoi (val);
      oski_SetDebugLevel (level);
    }
}

/**
 *  \brief
 *
 *  \returns 0 on error, 1 on success.
 */
int
oski_Init (void)
{
  InitSetDebugLevel ();
  oski_PrintDebugMessage (1, "Initializing %s", PACKAGE_STRING);
  oski_PrintDebugMessage (2,
			  "This library was built with the following options:");
  oski_PrintDebugMessage (2, "* Default link path = %s",
			  OSKI_LIB_PATH_DEFAULT);
  oski_PrintDebugMessage (2, "* Default OSKI-Lua root = %s",
			  OSKI_LUA_ROOT_DIR);
  oski_PrintDebugMessage (2, "* CC = %s", OSKI_CC);
  oski_PrintDebugMessage (2, "* CFLAGS = %s", OSKI_CFLAGS);
  oski_PrintDebugMessage (2, "* LDFLAGS = %s", OSKI_LDFLAGS);
  oski_PrintDebugMessage (2, "* F77 = %s", OSKI_F77);
  oski_PrintDebugMessage (2, "* FFLAGS = %s", OSKI_FFLAGS);
  oski_PrintDebugMessage (2, "* Support libraries link options = %s",
			  OSKI_SUPPORT_LDFLAGS);
  oski_PrintDebugMessage (2, "* Default OSKI-Lua root directory = %s",
			  OSKI_LUA_ROOT_DIR);
  oski_PrintDebugMessage (2, "* Default benchmark data directory = %s",
			  OSKI_BENCH_DIR_DEFAULT);
  oski_PrintDebugMessage (2, "* Default shared library path = %s",
			  OSKI_LIB_PATH_DEFAULT);

  oski_InitModuleLoader ();
  oski_InitMatTypeManager ();	/* Register default types */
  oski_InitHeurManager ();	/* Register default heuristics */

  /* Initialize timing library */
  oski_DestroyTimer (oski_CreateTimer ());

  return 1;
}

/**
*  \brief
*
*  This library implementation does not require the user to
*  call oski_Close(), but an application should to free the
*  memory storing internal library data structures.
*/
void
oski_Close (void)
{
  oski_PrintDebugMessage (1, "Shutting down the OSKI Library");

  oski_CloseHeurManager ();
  oski_CloseMatTypeManager ();
  oski_CloseModuleLoader ();
}

/* eof */
