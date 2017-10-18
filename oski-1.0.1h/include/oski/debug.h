/**
 *  \file oski/debug.h
 *  \ingroup AUXIL
 *  \brief Debugging support module.
 *
 *  This module defines rudimentary support for debugging and is
 *  intended for use by developers only.
 *
 *  The library internally maintains a "current debug level,"
 *  defined by a non-negative integer. When the level is set
 *  to 0, no debugging messages are displayed, and as the level
 *  increases, more frequent (or more detailed) debugging
 *  messages are displayed.
 *
 *  Developers can call oski_PrintDebugMessage to display
 *  debgging information, specifying the level at which the
 *  message should appear to the user.
 *
 *  $Revision: 1.1.1.1 $
 *
 *  Revision history:
 *    - $Log: debug.h,v $
 *    - Revision 1.1.1.1  2005/06/29 22:07:47  rvuduc
 *    - Initial import
 *    -
 */

#if !defined(INC_OSKI_DEBUG_H)
/** oski/debug.h has been included */
#define INC_OSKI_DEBUG_H

#include <stdio.h>

/**
 *  \brief Return the current debugging level.
 */
unsigned oski_GetDebugLevel (void);

/**
 *  \brief Change the current debugging level.
 */
void oski_SetDebugLevel (unsigned new_level);

/**
 *  \brief Return the file to which debugging output is
 *  currently redirected.
 */
FILE *oski_GetDebugOutput (void);

/**
 *  \brief Change the file to which debugging output is
 *  to be redirected.
 */
void oski_SetDebugOutput (FILE * fp);

/**
 *  \brief Display a debugging message.
 */
void oski_PrintDebugMessage (unsigned level, const char *fmt, ...);

/**
 *  \brief Display a debugging message without a trailing newline.
 */
void oski_PrintDebugMessageShort (unsigned level, const char *fmt, ...);

#endif

/* eof */
