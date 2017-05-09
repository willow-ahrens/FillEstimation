/**
 *  \file util/stat.c
 *  \brief Implements some simple statistics gathering utilities.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <oski/common.h>

#include "abort_prog.h"
#include "testvec.h"
#include "stat.h"

#if VALTYPE_STD_COMPARE
/** "C++ template-like" comparison function */
static int VTMANGLE (compare) (const void *a, const void *b)
{
  VALTYPE A = *((const VALTYPE *) a);
  VALTYPE B = *((const VALTYPE *) b);
  return (A < B) ? -1 : (A > B) ? 1 : 0;
}
#endif

/** Compute min, max, median, and mean of an array of data. */
void
VTMANGLE (CalcBasicStats) (const VALTYPE * x, size_t n,
			   VALTYPE * p_min, VALTYPE * p_max, VALTYPE * p_mean,
			   VALTYPE * p_median)
{
  VTMANGLE (CalcMinMaxMedian) (x, n, p_min, p_max, p_median);
  if (p_mean != NULL)
    *p_mean = VTMANGLE (CalcMean) (x, n);
}

/**
 *  \brief
 *
 *  \param[in] x Array on which to compute statistics. Must not equal
 *  NULL if the length, 'n', is greater than 0.
 *  \param[in] n Length of x.
 *  \param[in,out] p_min Pointer to location in which to store the
 *  minimum value of the array x. If NULL, then the minimum is
 *  not returned.
 *  \param[in,out] p_max Pointer to location in which to store the
 *  maximum value of the array x. If NULL, then the maximum is
 *  not returned.
 *  \param[in,out] p_medain Pointer to location in which to store the
 *  median value of the array x. Must not equal NULL.
 *
 *  This routine takes no action of n <= 0. If n > 0 and x is not
 *  equal to NULL, then this routine aborts.
 */
void
VTMANGLE (CalcMinMaxMedian) (const VALTYPE * x, size_t n,
			     VALTYPE * p_min, VALTYPE * p_max,
			     VALTYPE * p_median)
{
  VALTYPE *x_sorted;

  if (n <= 0)
    return;
  ABORT (x == NULL, CalcMinMaxMedian, ERR_BAD_ARG);
  ABORT (p_median != NULL, CalcMinMaxMedian, ERR_BAD_ARG);

  /* To find median, use naive algorithm: sort and return middle */
  x_sorted = oski_Malloc (VALTYPE, n);
  ABORT (x_sorted != NULL, CalcBasicValueStats, ERR_OUT_OF_MEMORY);
  oski_CopyMem (x_sorted, x, VALTYPE, n);
  qsort (x_sorted, n, sizeof (VALTYPE), VTMANGLE (compare));

  if (n == 1)
    *p_median = x_sorted[0];
  else if (n % 2)		/* n > 1 but odd */
    *p_median = x_sorted[n / 2];
#if VALTYPE_IS_FLOAT
  else				/* n is even */
    *p_median = 0.5 * (x_sorted[(n / 2)] + x_sorted[(n / 2) + 1]);
#else
  /* undefined if not FLOAT? */
#endif

  /* We get min and max for free */
  if (p_min != NULL)
    *p_min = x_sorted[0];
  if (p_max != NULL)
    *p_max = x_sorted[n - 1];

  oski_Free (x_sorted);
}

/**
 *  \brief
 *
 *  \param[in] x Array over which to compute the statistics. Must
 *  not equal NULL if the length, 'n', is greater than 0.
 *  \param[in] n Length of the array 'x'.
 *  \param[in,out] p_min Pointer to location in which to store
 *  the minimum value. If NULL, the minimum is not returned.
 *  \param[in,out] p_max Pointer to location in which to store
 *  the maximum value. If NULL, the maximum is not returned.
 *  \param[in,out] p_mean Pointer to location in which to store
 *  the mean value. If NULL, the mean is not returned.
 *
 *  If n <= 0, this routine simple returns and leaves the
 *  variables *p_min, *p_max, *p_mean unchanged. Otherwise,
 *  if x is NULL, then this routine exits the application.
 */
void
VTMANGLE (CalcMinMaxMean) (const VALTYPE * x, size_t n,
			   VALTYPE * p_min, VALTYPE * p_max, VALTYPE * p_mean)
{
  VALTYPE min;
  VALTYPE max;
  VALTYPE sum;

  size_t i;

  /* Quick return conditions */
  if (n <= 0)
    return;
  if (p_min == NULL && p_max == NULL && p_mean == NULL)
    return;

  ABORT (x == NULL, CalcMinMaxMean, ERR_BAD_ARG);

  min = x[0];
  max = x[0];
  sum = x[0];
  for (i = 1; i < n; i++)
    {
      if (x[i] < min)
	min = x[i];
      if (x[i] > max)
	max = x[i];
      sum += x[i];
    }

  if (p_min != NULL)
    *p_min = min;
  if (p_max != NULL)
    *p_max = max;
  if (p_mean != NULL)
    *p_mean = sum / n;
}

/**
 *  \brief
 *
 *  \param[in] x Array over which to compute the statistics. Must
 *  not equal NULL if the length, 'n', is greater than 0.
 *  \param[in] n Length of the array 'x'.
 *
 *  If x is NULL or n <= 0, then this routine generates an
 *  error and exits the application.
 */
VALTYPE VTMANGLE (CalcMin) (const VALTYPE * x, size_t n)
{
  VALTYPE min;

  size_t i;

  ABORT (x == NULL || n <= 0, CalcMin, ERR_BAD_ARG);

  min = x[0];
  for (i = 1; i < n; i++)
    {
      if (x[i] < min)
	min = x[i];
    }

  return min;
}

/**
 *  \brief
 *
 *  \param[in] x Array over which to compute the statistics. Must
 *  not equal NULL if the length, 'n', is greater than 0.
 *  \param[in] n Length of the array 'x'.
 *
 *  If x is NULL or n <= 0, then this routine generates an
 *  error and exits the application.
 */
VALTYPE VTMANGLE (CalcMax) (const VALTYPE * x, size_t n)
{
  VALTYPE max;

  size_t i;

  ABORT (x == NULL || n <= 0, CalcMax, ERR_BAD_ARG);

  max = x[0];
  for (i = 1; i < n; i++)
    if (x[i] > max)
      max = x[i];

  return max;
}

/**
 *  \brief
 *
 *  \param[in] x Array over which to compute the statistics. Must
 *  not equal NULL if the length, 'n', is greater than 0.
 *  \param[in] n Length of the array 'x'.
 *
 *  If x is NULL or n <= 0, then this routine generates an
 *  error and exits the application.
 */
VALTYPE VTMANGLE (CalcMean) (const VALTYPE * x, size_t n)
{
  VALTYPE sum;

  size_t i;

  ABORT (x == NULL || n <= 0, CalcMean, ERR_BAD_ARG);

  sum = x[0];
  for (i = 1; i < n; i++)
    sum += x[i];

  return sum / n;
}

/* eof */
