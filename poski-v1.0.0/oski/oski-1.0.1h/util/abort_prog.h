/**
 *  \file abort_prog.h
 *  \brief Macro to abort a program on error.
 */

#if !defined(INC_UTIL_ABORT_PROG_H)
/** tests/test_util.h included. */
#define INC_UTIL_ABORT_PROG_H

#include <stdio.h>

#include <oski/common.h>

/**
 *  \brief If the given condition is true, calls the generic BeBOP
 *  error reporting macro, OSKI_ERR, and aborts the current program
 *  with exit code 1.
 *
 *  \param[in] cond A boolean condition expression.
 *  \param[in] method Calling function from which to report the error.
 *  This parameter is converted to a string via MACRO_TO_STRING(method).
 *  \param[in] err BeBOP error code.
 */
#define ABORT(cond, method, err) if(cond) { \
		fprintf( stderr, "*** Error condition '%s' detected ***\n", \
			MACRO_TO_STRING(cond) ); \
		OSKI_ERR(method, err); \
		exit(1); \
	}

#endif

/* eof */
