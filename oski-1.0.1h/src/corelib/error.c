/**
 *  \file error.c
 *  \ingroup AUXIL
 *  \brief Error-handling module.
 *
 *  If the library is built with pthreads support, then
 *  #oski_GetErrorHandler and #oski_SetErrorHandler routines
 *  execute atomically using pthreads locking.
 *
 *  \todo Wrap error handler get/set routines in mutexes.
 */

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <oski/common.h>
#include <oski/config.h>

/**
 *  \brief Where default error handler's output messages go.
 *
 *  \todo Make g_errlogfile a static global variable that
 *  the user can change to redirect errors elsewhere.
 */
#define g_errlogfile stderr

const char *
oski_GetErrorName (int code)
{
  switch (code)
    {
    case 0:
      return "(No error)";

    case ERR_OUT_OF_MEMORY:
      return "Out of memory.";

    case ERR_INIT_LIB:
      return "Can't initialize the library.";

    case ERR_INIT_SHAREDLIB:
      return "Can't initialize shared library modules.";

    case ERR_BAD_ID:
      return "An invalid ID was specified.";

    case ERR_LOAD_MOD:
      return "Could not load a shared library module.";

    case ERR_UNLOAD_MOD:
      return "Could not unload a shared library module.";

    case ERR_NOT_IMPLEMENTED:
      return "Could not find a suitable implementation.";

    case ERR_BAD_ARG:
      return "The caller provided an invalid argument.";

    case ERR_INMATPROP_CONFLICT:
      return "Input matrix properties conflict.";

    case ERR_DIM_MISMATCH:
      return "Matrix multiply operand dimensions don't match.";

    case ERR_FALSE_INMATPROP:
      return "An asserted input matrix property is false.";

    case ERR_BAD_LEADDIM:
      return "A leading dimension (stride) is invalid.";

    case ERR_BAD_MAT:
      return "The given matrix handle (object) is invalid.";

    case ERR_BAD_VECVIEW:
      return "The given vector view handle (object) is invalid.";

    case ERR_WRONG_ANS:
      return "Computed an incorrect value or values.";

    case ERR_BAD_HINT_ARG:
      return "An invalid argument passed to a hint-setting routine.";

    case ERR_CANT_CONVERT:
      return "An error occurred while trying to change a data structure.";

    case ERR_FILE_READ:
      return "Could not open or read a necessary file.";

    case ERR_BAD_SYNTAX:
      return "OSKI-Lua transformation program contains an error.";

    case ERR_OLUA_NORET:
      return "OSKI-Lua transformation program did not return a new matrix.";

    case ERR_OLUA_STARTUP:
      return "Unable to start the OSKI-Lua interpreter.";

    case ERR_OLUA_TYPE_MISMATCH:
      return "Type mismatch detected in OSKI-Lua transformation program.";

    case ERR_BAD_PERM:
      return "Attempted to use an invalid permutation object.";

    case ERR_BAD_ENTRY:
      return "Attempted to access an invalid (out-of-range) entry.";

    case ERR_ZERO_ENTRY:
      return "Attempted to change an implicit zero entry.";
    }
  return "(No additional information)";
}

void
oski_HandleErrorDefault (int error_code, const char *message,
			 const char *source_filename,
			 unsigned long line_number, const char *format_string,
			 ...)
{
  fprintf (g_errlogfile,
	   "\n***********************************************************\n"
	   "* OSKI Error %d : %s\n"
	   "* Occurred at/near '%s', line %ld.\n",
	   error_code, message, source_filename, line_number);

  if (format_string != NULL)
    {
      va_list ap;
      fprintf (g_errlogfile, "\nAdditional information:\n  ");
      va_start (ap, format_string);
      vfprintf (g_errlogfile, format_string, ap);
      va_end (ap);
    }
  fprintf (g_errlogfile,
	   "\n**********************************************************\n");
}

/**
 *  \brief
 *
 *  This variable points to the current error handler. When an
 *  error occurs in the library, the error handler is called
 *  through this pointer.
 *
 *  Important note: This variable should never be equal to NULL!
 *  Instead, error handling should be disabled by setting this
 *  variable to be equal to #oski_HandleErrorNull, the 'null'
 *  error handler.
 */
static oski_errhandler_t gfuncp_oski_HandleError = oski_HandleErrorDefault;

#if defined(OSKI_ENABLE_MUTEX)
#error "*** \\todo: Implement pthreads support. ***"
#else
	/** Lock access to global error-handler variable */
#define oski_LockErrorHandler()  ;
	/** Release global error-handler variable */
#define oski_UnlockErrorHandler()  ;
#endif

/**
 *  \brief Null error handler.
 *
 *  This routine has the signature of an error handler, but contains
 *  no statements (i.e., is a no-op.)
 */
void
oski_HandleErrorNull (int error_code, const char *message,
		      const char *source_filename, unsigned long line_number,
		      const char *format_string, ...)
{
  return;
}

oski_errhandler_t
oski_GetErrorHandler (void)
{
  oski_errhandler_t cur_handler;
  oski_LockErrorHandler ();
  cur_handler = gfuncp_oski_HandleError;
  oski_UnlockErrorHandler ();
  return cur_handler;
}

oski_errhandler_t
oski_SetErrorHandler (oski_errhandler_t new_handler)
{
  oski_errhandler_t old_handler = oski_GetErrorHandler ();
  oski_LockErrorHandler ();
  gfuncp_oski_HandleError = (new_handler == NULL)
    ? oski_HandleErrorNull : new_handler;
  oski_UnlockErrorHandler ();
  return old_handler;
}

/* eof */
