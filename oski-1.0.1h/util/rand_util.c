/**
 *  \file util/rand_util.c
 *  \brief Wrappers around the available random number generators.
 */

#include <config/config.h>

#include <stdio.h>
#include <stdlib.h>

#include <oski/common.h>
#include <oski/mangle.h>

#include "rand_util.h"

#if HAVE_DRAND48
	/** \todo Why is this prototype needed with icc 8.1? */
extern double drand48 (void);

	/** Returns a random value in the interval [0,1] */
#define RAND_DOUBLE   (drand48())

#elif HAVE_RANDOM
	/** Returns a random value in the interval [0,1] */
#define RAND_DOUBLE   ((double)random()/RAND_MAX)

#elif HAVE_RAND
	/** Returns a random value in the interval [a,b] */
#define RAND_DOUBLE   ((double)rand()/RAND_MAX)
#else

#error "Didn't find a suitable random number generator, having tried drand48(), random(), and rand()."

#endif

oski_index_t
rand_GenIndex (oski_index_t a, oski_index_t b)
{
  double raw_val = RAND_DOUBLE;
  return a + (oski_index_t) (raw_val * (b - a) + .5);
}

oski_value_t
rand_GenValueReal (double a, double b)
{
  double re = a + RAND_DOUBLE * (b - a);
  oski_value_t val = MAKE_VAL_COMPLEX (re, 0.0);
  return val;
}

oski_value_t
rand_GenValue (double a, double b)
{
  oski_value_t val;

#if IS_VAL_COMPLEX
  _RE (val) = a + RAND_DOUBLE * (b - a);
  _IM (val) = a + RAND_DOUBLE * (b - a);
#else
  val = a + RAND_DOUBLE * (b - a);
#endif

  return val;
}

static void
swap_inds (oski_index_t * p_i, oski_index_t * p_j)
{
  oski_index_t t = *p_i;
  *p_i = *p_j;
  *p_j = t;
}

static void
swap_vals (oski_value_t * p_i, oski_value_t * p_j)
{
  oski_value_t t;
  VAL_ASSIGN (t, *p_i);
  VAL_ASSIGN (*p_i, *p_j);
  VAL_ASSIGN (*p_j, t);
}

void
rand_PermuteIndices (oski_index_t * ind, oski_index_t len)
{
  oski_index_t i;
  for (i = 0; i < len; i++)
    {
      oski_index_t j = rand_GenIndex (0, len - 1);
      if (i != j)
	swap_inds (&(ind[i]), &(ind[j]));
    }
}

void
rand_PermuteIndValPairs (oski_index_t * ind, oski_value_t * val,
			 oski_index_t len)
{
  oski_index_t i;
  for (i = 0; i < len; i++)
    {
      oski_index_t j = rand_GenIndex (0, len - 1);
      if (i != j)
	{
	  swap_inds (&(ind[i]), &(ind[j]));
	  swap_vals (&(val[i]), &(val[j]));
	}
    }
}

/* eof */
