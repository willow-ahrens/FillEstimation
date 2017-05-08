/**
 *  \file util/parse_opts.c
 *  \brief Parse command-line options.
 */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "abort_prog.h"
#include "parse_opts.h"

/**
 *  \brief
 *
 *  \param[in] source Source string (NULL-terminated).
 *  \param[in] ... List of strings to compare to. The end
 *  of the list is marked by a NULL.
 *
 *  \returns 0 if source is NULL or does not exactly
 *  equal any of the list of comparison strings, or
 *  the index of the matching item (starting at 1).
 */
int
StringMatches (const char *source, ...)
{
  int i = 0;
  va_list ap;
  const char *target;

  va_start (ap, source);

  target = va_arg (ap, const char *);
  while (target != NULL)
    {
      i++;
      if (strcmp (source, target) == 0)
	return i;
      target = va_arg (ap, const char *);
    }
  va_end (ap);

  return 0;
}

size_t
ParseByteString (const char *s)
{
  size_t i, n, len;

  len = strlen (s);

  if (s == NULL || len == 0)
    return 0;

  i = 0;
  while (i < len && (isspace (s[i]) || isdigit (s[i])))
    i++;

  n = atoi (s);
  if (i == len)
    return n;

  while (i < len && isspace (s[i]))
    i++;
  if (i == len)
    return n;

  switch (s[i])
    {
    case 'k':
    case 'K':
      n *= 1024;
      break;

    case 'M':
      n *= 1024 * 1024;
      break;

    case 'G':
      n *= 1024 * 1024 * 1024;
      break;

    default:
      fprintf (stderr,
	       "*** Unrecognized/unsupported format string, '%c'.\n", s[i]);
      return 0;
    }

  while (i < n && isspace (s[i]))
    i++;
  if (i == len)
    return n;

  fprintf (stderr,
	   "*** Extra characters, beginning at the %d-th character ('%c').\n",
	   (int) (i + 1), s[i]);
  return 0;
}

oski_storage_t
ParseDenseMatLayout (const char *s)
{
  char s1 = s == NULL || strlen (s) == 0 ? 0 : s[0];
  switch (toupper (s1))
    {
    case 'R':
      return LAYOUT_ROWMAJ;
    case 'C':
      return LAYOUT_COLMAJ;
    }
  fprintf (stderr, "*** Unrecognized layout option, '%s'.\n", s);
  ABORT (0, process_options, ERR_BAD_ARG);
  return LAYOUT_ROWMAJ;		/* never executed */
}

oski_matop_t
ParseMatTransOp (const char *s)
{
  char s1 = s == NULL || strlen (s) == 0 ? 0 : s[0];
  switch (toupper (s1))
    {
    case 'N':
      return OP_NORMAL;
    case 'T':
      return OP_TRANS;
    case 'C':
      return OP_CONJ_TRANS;
    }
  fprintf (stderr, "*** Unrecognized transpose option, '%s'.\n", s);
  ABORT (0, process_options, ERR_BAD_ARG);
  return OP_NORMAL;		/* never executed */
}

void
PrintMatTransOp (FILE * fp, const char *matname, oski_matop_t op)
{
  const char *matname_s = matname == NULL ? "A" : matname;
  char *pre = "unknown_func(";
  char *post = ")";
  if (fp == NULL)
    fp = stderr;
  switch (op)
    {
    case OP_NORMAL:
      pre = "";
      post = "";
      break;
    case OP_TRANS:
      pre = "";
      post = "^T";
      break;
    case OP_CONJ:
      pre = "conj(";
      post = ")";
      break;
    case OP_CONJ_TRANS:
      pre = "conj(";
      post = ")^T";
      break;
    default:
      break;
    }
  fprintf (fp, "    op(%s) = %s%s%s\n", matname_s, pre, matname_s, post);
}

void
PrintDebugMatTransOp (int level, const char *matname, oski_matop_t op)
{
  const char *matname_s = matname == NULL ? "A" : matname;
  char *pre = "unknown_func(";
  char *post = ")";
  switch (op)
    {
    case OP_NORMAL:
      pre = "";
      post = "";
      break;
    case OP_TRANS:
      pre = "";
      post = "^T";
      break;
    case OP_CONJ:
      pre = "conj(";
      post = ")";
      break;
    case OP_CONJ_TRANS:
      pre = "conj(";
      post = ")^T";
      break;
    default:
      break;
    }
  oski_PrintDebugMessage (level,
			  "    op(%s) = %s%s%s", matname_s, pre, matname_s,
			  post);
}

oski_ataop_t
ParseMatATAOp (const char *s)
{
  if (strcasecmp (s, "ata") == 0)
    return OP_AT_A;
  else if (strcasecmp (s, "aat") == 0)
    return OP_A_AT;
  else if (strcasecmp (s, "aha") == 0)
    return OP_AH_A;
  else if (strcasecmp (s, "aah") == 0)
    return OP_A_AH;

  fprintf (stderr, "*** Unrecognized transpose option, '%s'.\n", s);
  ABORT (0, process_options, ERR_BAD_ARG);
  return (oski_ataop_t) (-1);	/* never executed */
}

void
PrintMatATAOp (FILE * fp, const char *matname, oski_ataop_t op)
{
  const char *matname_s = matname == NULL ? "A" : matname;
  char *fmt = "unknown_func(%s)";
  if (fp == NULL)
    fp = stderr;
  switch (op)
    {
    case OP_AT_A:
      fmt = "%s^T*%s";
      break;
    case OP_AH_A:
      fmt = "conj(%s)^T*%s";
      break;
    case OP_A_AT:
      fmt = "%s*%s^T";
      break;
    case OP_A_AH:
      fmt = "%s*conj(%s)^T";
      break;
    default:
      break;
    }
  fprintf (fp, "    op(%s) = ", matname_s);
  fprintf (fp, fmt, matname_s, matname_s);
  fprintf (fp, "\n");
}

void
PrintDebugMatATAOp (int level, const char *matname, oski_ataop_t op)
{
  const char *matname_s = matname == NULL ? "A" : matname;
  char *fmt = "unknown_func(%s)";
  switch (op)
    {
    case OP_AT_A:
      fmt = "%s^T*%s";
      break;
    case OP_AH_A:
      fmt = "conj(%s)^T*%s";
      break;
    case OP_A_AT:
      fmt = "%s*%s^T";
      break;
    case OP_A_AH:
      fmt = "%s*conj(%s)^T";
      break;
    default:
      break;
    }
  oski_PrintDebugMessageShort (level, "    op(%s) = ", matname_s);
  oski_PrintDebugMessageShort (level, fmt, matname_s, matname_s);
  oski_PrintDebugMessageShort (level, "\n");
}

/* eof */
