/**
 *  \file tests/err.c
 *  \brief Test OSKI error handling facilities.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <oski/common.h>

void
generate_error_DUMMY (void)
{
  if (oski_HandleError == NULL)
    {
      fprintf (stderr, "*** Error handler was NULL! ***\n");
      exit (1);
    }

  oski_PrintDebugMessage (1, "\tExecuting handler @ 0x%p...",
			  oski_HandleError);

  (*oski_HandleError) (ERR_OUT_OF_MEMORY,
		       "<< TEST >> No more memory",
		       __FILE__, __LINE__,
		       " (int, char, str, ptr) == (%d, '%c', \"%s\", 0x%p)",
		       1, '2', "333", oski_HandleError);

  oski_PrintDebugMessage (1, "\tHandler returned.");
}

void
my_test_handler (int e, const char *msg,
		 const char *file, unsigned long lineno, const char *fmt, ...)
{
  va_list ap;

  oski_PrintDebugMessage (1, "*** [Custom error handler] %d : '%s' ***", e,
			  msg);
  oski_PrintDebugMessage (1, "Some details:");

  va_start (ap, fmt);
  vfprintf (stderr, fmt, ap);
  va_end (ap);
  oski_PrintDebugMessage (1, "****************************************");
}

int
main (int argc, char *argv[])
{
  oski_errhandler_t cur_handler;
  oski_errhandler_t old_handler;

  oski_PrintDebugMessage (1, "... Testing error handling facilities ...");
  oski_PrintDebugMessage (1, "\tNOTE: &oski_HandleErrorDefault == 0x%p.",
			  oski_HandleErrorDefault);

  /* [1] Does default error handler work? */
  oski_PrintDebugMessage (1, "\tShould call oski_HandleErrorDefault()...");
  generate_error_DUMMY ();

  /* [2] Does disabling error handling work? */
  oski_PrintDebugMessage (1, "\tDisabling error handling...");
  cur_handler = oski_GetErrorHandler ();
  old_handler = oski_SetErrorHandler (NULL);
  if (cur_handler != old_handler)
    {
      fprintf (stderr,
	       "*** _SetErrorHandler did not return same routine as prior call to _GetErrorHandler. ***\n");
      return -1;
    }
  generate_error_DUMMY ();

  /* [3] Can we successfully restore the handler? */
  oski_SetErrorHandler (old_handler);
  if (oski_GetErrorHandler () != cur_handler)
    {
      fprintf (stderr, "*** Could not restore previous error handler! ***\n");
      return -1;
    }

  /* [4] Can we replace the handler? */
  oski_PrintDebugMessage (1, "\tInstalling a custom handler @ 0x%p...",
			  my_test_handler);
  oski_SetErrorHandler (my_test_handler);
  generate_error_DUMMY ();

  /* That's good enough for now. */
  oski_PrintDebugMessage (1, "Done!");
  return 0;
}

/* eof */
