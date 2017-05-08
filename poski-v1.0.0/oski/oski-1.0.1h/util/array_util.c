/**
 *  \file util/array_util.c
 *  \brief Some utility functions for the test suite.
 *
 *  This module must be compiled with instantiated default types.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "array_util.h"
#include "abort_prog.h"

/* ------------------------------------------------- */

/**
 *  \brief
 *
 *  \param[in] x0 Array to copy.
 *  \param[in] len Number of elements to copy from x0.
 *  \returns A newly allocated copy of x0, or NULL on error.
 */
oski_value_t *
array_Duplicate (const oski_value_t * x0, oski_index_t len)
{
  oski_value_t *x = oski_Malloc (oski_value_t, len);
  oski_CopyMem (x, x0, oski_value_t, len);
  return x;
}

/**
 *  \brief
 *
 *  \param[in,out] x Array to destroy.
 *  \post x no longer usable.
 */
void
array_Destroy (oski_value_t * x)
{
  oski_Free (x);
}

/**
 *  \brief
 *
 *  If a 'large' difference is detected, this routine
 *  prints a message to the user (using the error handler)
 *  and returns ERR_WRONG_ANS.
 *
 *  \param[in] test_func Name of the calling test function.
 *  \param[in] x Expected results.
 *  \param[in] y Observed/computed results.
 *  \param[in] len Length of the x and y arrays.
 */
int
array_CompareElems_NoAbort (const char *test_func,
			    const oski_value_t * x, const oski_value_t * y,
			    oski_index_t len)
{
  oski_index_t i;
  double max_abs_diff = 0.0;

  oski_PrintDebugMessage (1, "... CHECKING %s() ...", test_func);

  for (i = 0; i < len; i++)
    {
      double abs_diff;

      /* Compute diff = x[i] - y[i] */
      oski_value_t diff = x[i];
      VAL_DEC (diff, y[i]);

#if IS_VAL_COMPLEX
      abs_diff = (fabs (_RE (diff)) > fabs (_IM (diff)))
	? fabs (_RE (diff)) : fabs (_IM (diff));
#else
      abs_diff = fabs (diff);
#endif

      if (abs_diff > len * VAL_EPS)
	{
	  oski_PrintDebugMessage (1, "abs_diff = %g > %d*%g = %g",
				  abs_diff, len, VAL_EPS, len * VAL_EPS);

#if IS_VAL_COMPLEX
	  OSKI_ERR_WRONG_ANS_ELEM_COMPLEX (array_CompareElems, x[i], y[i], i);
#else
	  OSKI_ERR_WRONG_ANS_ELEM (array_CompareElems, x[i], y[i], i);
#endif
	  return ERR_WRONG_ANS;
	}

      if (abs_diff > max_abs_diff)
	max_abs_diff = abs_diff;
    }

  oski_PrintDebugMessage (1, "PASSED %s() ...", test_func);
  oski_PrintDebugMessage (1, "--> %g MAX ABS DIFF", max_abs_diff);
  return 0;
}

/**
 *  \brief
 *
 *  If a 'large' difference is detected, this routine
 *  prints a message to the user (using the error handler)
 *  and aborts the program with an exit code of 1.
 *
 *  \param[in] test_func Name of the calling test function.
 *  \param[in] x Expected results.
 *  \param[in] y Observed/computed results.
 *  \param[in] len Length of the x and y arrays.
 */
void
array_CompareElems (const char *test_func,
		    const oski_value_t * x, const oski_value_t * y,
		    oski_index_t len)
{
  if (array_CompareElems_NoAbort (test_func, x, y, len) != 0)
    exit (1);
}

#include "util/rand_util.h"

void
array_FillRandom (oski_value_t * x, oski_index_t n)
{
  oski_index_t i;
  if (x == NULL)
    return;

  for (i = 0; i < n; i++)
    {
      oski_value_t r = rand_GenValue (-1.0, 1.0);
      VAL_ASSIGN (x[i], r);
    }
}

oski_value_t *
array_CreateRandom (oski_index_t n)
{
  oski_value_t *x_new = oski_Malloc (oski_value_t, n);
  if (x_new != NULL)
    array_FillRandom (x_new, n);
  return x_new;
}

oski_value_t *
array_Create (oski_index_t n, oski_value_t val)
{
  oski_value_t *x_new = oski_Malloc (oski_value_t, n);
  if (x_new != NULL)
    {
      oski_index_t i;
      for (i = 0; i < n; i++)
	x_new[i] = val;
    }
  return x_new;
}

/* ----------------------------------------------------- */
typedef struct
{
  oski_index_t i;
  oski_value_t v;
} indvalpair_t;

static int
compare_indvalpair (const void *pa, const void *pb)
{
  const indvalpair_t *a = (const indvalpair_t *) pa;
  const indvalpair_t *b = (const indvalpair_t *) pb;
  if (a->i < b->i)
    return -1;
  if (a->i > b->i)
    return 1;
  return 0;
}

void
array_SortIndValPairs (oski_index_t * ind, oski_value_t * val, oski_index_t n)
{
  indvalpair_t *pairs;		/* temp array containing pairs */
  oski_index_t i;

  if (n == 0)
    return;

  pairs = oski_Malloc (indvalpair_t, n);
  ABORT (pairs == NULL, array_SortIndValPairs, ERR_OUT_OF_MEMORY);

  for (i = 0; i < n; i++)
    {
      pairs[i].i = ind[i];
      VAL_ASSIGN (pairs[i].v, val[i]);
    }

  qsort (pairs, n, sizeof (indvalpair_t), compare_indvalpair);

  for (i = 0; i < n; i++)
    {
      ind[i] = pairs[i].i;
      VAL_ASSIGN (val[i], pairs[i].v);
    }
  oski_Free (pairs);
}

/* eof */
