/**
 *  \file oski/sprintf.h
 *  \brief Macros for typed block memory copy operations.
 */

#if !defined(INC_OSKI_SPRINTF_H)
/** oski/sprintf.h included. */
#define INC_OSKI_SPRINTF_H

/**
 *  \brief Safe implementation of 'sprintf' which returns a newly
 *  allocated string of the appropriate length to contain the
 *  desired formatted string.
 */
char *oski_StringPrintf (const char *fmt, ...);

#endif

/* eof */
