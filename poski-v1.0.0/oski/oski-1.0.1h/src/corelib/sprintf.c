/**
 *  \file src/sprintf.c
 *  \brief Safe implementation of 'sprintf'.
 */

#include <assert.h>
#include <stdarg.h>
#include <oski/common.h>

/**
 *  \brief
 *
 *  The caller must free the returned string.
 *
 *  Returns NULL on error. Otherwise, returns a newly allocated
 *  string containing the formatted output.
 */
char *
oski_StringPrintf (const char *fmt, ...)
{
  char *output_string;
  int len;

  if (fmt == NULL)
    return NULL;
  if (strlen (fmt) == 0)
    {
      output_string = oski_Malloc (char, 1);
      if (output_string != NULL)
	output_string[0] = (char) 0;
      return output_string;
    }

  assert (strlen (fmt) > 0);
  len = 2 * strlen (fmt);
  output_string = NULL;
  while (output_string == NULL)
    {
      va_list ap;
      int vsn_return;

      /* allocate */
      output_string = oski_Malloc (char, len);
      if (output_string == NULL)
	return NULL;		/* out of memory */
      oski_ZeroMem (output_string, len * sizeof (char));

      /* try to write string */
      va_start (ap, fmt);
      vsn_return = vsnprintf (output_string, len, fmt, ap);
      va_end (ap);

		/**
		 *  \note From David Martin at Berkeley: On AIX, Linux, and
		 *  Solaris, vsnprintf() returns a value < len if the string
		 *  buffer is sufficiently large to contain the formatted
		 *  string, while the usual Irix implementation returns 0.
		 *  Other systems are rumored to return a negative value on
		 *  error. The 'if' condition below catches all 3 cases.
		 *
		 *  \note Here, we use 'len-1' to include the terminating
		 *  NULL character.
		 */
      if (vsn_return <= 0 || vsn_return >= (len - 1))
	{
	  oski_Free (output_string);
	  output_string = NULL;
	  len *= 2;
	}
    }

  assert (output_string != NULL);
  return output_string;
}

/* eof */
