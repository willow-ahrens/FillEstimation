/**
 *  \file src/debug.c
 *  \ingroup AUXIL
 *  \brief Debugging support module.
 *
 *  This module implements some rudimentary support for debugging,
 *  and is intended for use by developers only.
 */

#include <assert.h>
#include <stdarg.h>
#include <oski/common.h>

/**
 *  \brief File to which debugging output should be directed.
 */
static FILE *g_debug_file = NULL;

/**
 *  \brief Current debugging level.
 */
static unsigned g_cur_debug_level = 0;

unsigned
oski_GetDebugLevel (void)
{
  return g_cur_debug_level;
}

/**
 *  \brief
 *
 *  \param[in] new_level New debugging level.
 *  \post Sets g_cur_debug_level to the given level.
 */
void
oski_SetDebugLevel (unsigned new_level)
{
  g_cur_debug_level = new_level;
}

/**
 *  \brief
 *
 *  \returns A pointer to the FILE to which debugging output is
 *  currently directed. This return should never be NULL.
 *
 *  \post If g_debug_file is NULL, sets it to stderr and
 *  returns stderr.
 */
FILE *
oski_GetDebugOutput (void)
{
  if (g_debug_file == NULL)
    {
      g_debug_file = stderr;
      return stderr;
    }
  else
    return g_debug_file;
}

/**
 *  \brief
 *
 *  \param[in,out] fp New file to which debugging output should be
 *  redirected.
 *  \post Sets g_debug_file to fp <em>unless</em> g_debug_file is
 *  NULL, in which case fp is set to stderr.
 */
void
oski_SetDebugOutput (FILE * fp)
{
  if (fp == NULL)
    g_debug_file = stderr;
  else
    g_debug_file = fp;
}

/**
 *  \brief
 *
 *  \param[in] level Level at and above which this message should be
 *  printed to the current debug file.
 *  \param[in] fmt A printf-compatible format string.
 *  \param[in] ... Options for the formatting string.
 *  \post The message is displayed to the current debug file
 *  only if level >= the current debugging level.
 *
 *  \pre oski_GetDebugOutput() != NULL
 *  \post Prints a newline character to the debug file.
 */
void
oski_PrintDebugMessage (unsigned level, const char *fmt, ...)
{
  FILE *fp_debug;
  va_list ap;
  unsigned i;

  if ((oski_GetDebugLevel () >= level) && (fmt != NULL))
    {
      fp_debug = oski_GetDebugOutput ();
      assert (fp_debug != NULL);

      /* Indent this text according to level */
      fprintf (fp_debug, "!");
      for (i = 0; i < level; i++)
	fprintf (fp_debug, "   ");

      va_start (ap, fmt);
      vfprintf (fp_debug, fmt, ap);
      va_end (ap);

      fprintf (fp_debug, "\n");
    }
}

/**
 *  \brief
 *
 *  This routine is similar to oski_PrintDebugMessage(), except
 *  that it prints neither a prefix string nor a trailing newline.
 *
 *  \see oski_PrintDebugMessage
 */
void
oski_PrintDebugMessageShort (unsigned level, const char *fmt, ...)
{
  FILE *fp_debug;
  va_list ap;

  if ((oski_GetDebugLevel () >= level) && (fmt != NULL))
    {
      fp_debug = oski_GetDebugOutput ();
      assert (fp_debug != NULL);

      va_start (ap, fmt);
      vfprintf (fp_debug, fmt, ap);
      va_end (ap);
    }
}

/* eof */
