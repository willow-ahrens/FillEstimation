/**
 *  \file util/parse_opts.c
 *  \brief Type-dependent routines for command-line option parsing.
 */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "abort_prog.h"
#include "parse_opts.h"

static const char *
skip_leading_spaces (const char *s)
{
  if (s == NULL)
    return NULL;
  while (s[0] && isspace (s[0]))
    s++;
  return s;
}

oski_value_t
ParseValue (const char *s)
{
  oski_value_t x = MAKE_VAL_COMPLEX (0.0, 0.0);
  const char *p = s;
  double re = 0.0;
#if IS_VAL_COMPLEX
  double im = 0.0;
  int sign = 0;
#endif

  if (s == NULL)
    return x;

  /* Process against the format,
   *     (%g)[ \t]*[+-][ \t]*(%g)[ij]
   * where '%g' is
   *     [0-9]+(.[0-9]*)*[eE][+-]?[0-9]+
   */
  s = skip_leading_spaces (s);
  if (s[0] == 0)
    return x;
  re = strtod (s, (char **) (&p));

#if !IS_VAL_COMPLEX
  x = (oski_value_t) re;
#else
  _RE (x) = re;
  s = skip_leading_spaces (p);
  if (s == NULL)
    return x;
  switch (s[0])
    {
    case '+':
      sign = 1;
      break;
    case '-':
      sign = -1;
      break;
    default:
      return x;
    }
  s++;
  s = skip_leading_spaces (s);
  im = strtod (s, (char **) (&p));
  if (p != NULL && tolower (p[0]) == 'i' || tolower (p[0]) == 'j')
    _IM (x) = im;
#endif
  return x;
}

void
PrintValue (FILE * fp, const char *name, oski_value_t x)
{
  const char *name_s = name == NULL ? "v" : name;
  if (fp == NULL)
    fp = stderr;
#if IS_VAL_COMPLEX
  fprintf (fp, "    %s = %g+%gi\n", name_s, (double) _RE (x),
	   (double) _IM (x));
#else
  fprintf (fp, "    %s = %g\n", name_s, (double) x);
#endif
}

void
PrintDebugValue (int level, const char *name, oski_value_t x)
{
  const char *name_s = name == NULL ? "v" : name;
#if IS_VAL_COMPLEX
  oski_PrintDebugMessage (level,
			  "    %s = %g+%gi", name_s, (double) _RE (x),
			  (double) _IM (x));
#else
  oski_PrintDebugMessage (level, "    %s = %g", name_s, (double) x);
#endif
}

void
PrintVecViewInfo (FILE * fp, const char *name, const oski_vecview_t x)
{
  const char *name_s = name == NULL ? "x" : name;
  if (fp == NULL)
    fp = stderr;
  if (x == INVALID_VEC)
    {
      fprintf (fp, "    Vector %s is nil.\n", name_s);
    }
  else if (x == SYMBOLIC_VEC)
    {
      fprintf (fp, "    Vector %s is a symbolic vector.\n", name_s);
    }
  else if (x == SYMBOLIC_MULTIVEC)
    {
      fprintf (fp, "    Vector %s is a symbolic multivector.\n", name_s);
    }
  else
    {
      fprintf (fp, "    Vector %s data layout: %s\n", name_s,
	       x->orient == LAYOUT_ROWMAJ ? "row major" : "column major");
      fprintf (fp, "        length: %d\n", (int) (x->num_rows));
      fprintf (fp, "        number: %d\n", (int) (x->num_cols));
      fprintf (fp, "        stride: %d\n", (int) (x->stride));
    }
}

void
PrintDebugVecViewInfo (int level, const char *name, const oski_vecview_t x)
{
  const char *name_s = name == NULL ? "x" : name;
  if (x == INVALID_VEC)
    {
      oski_PrintDebugMessage (level, "    Vector %s is nil.", name_s);
    }
  else if (x == SYMBOLIC_VEC)
    {
      oski_PrintDebugMessage (level,
			      "    Vector %s is a symbolic vector.", name_s);
    }
  else if (x == SYMBOLIC_MULTIVEC)
    {
      oski_PrintDebugMessage (level,
			      "    Vector %s is a symbolic multivector.",
			      name_s);
    }
  else
    {
      oski_PrintDebugMessage (level,
			      "    Vector %s data layout: %s", name_s,
			      x->orient ==
			      LAYOUT_ROWMAJ ? "row major" : "column major");
      oski_PrintDebugMessage (level, "        length: %d",
			      (int) (x->num_rows));
      oski_PrintDebugMessage (level, "        number: %d",
			      (int) (x->num_cols));
      oski_PrintDebugMessage (level, "        stride: %d", (int) (x->stride));
    }
}

/* eof */
