/**
 *  \file tests/multimalloc.c
 *  \brief Test OSKI multiple-malloc routine.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <oski/common.h>

#define MAX_ELEMS 65536

int
main (int argc, char *argv[])
{
  size_t n1 = 10;
  size_t n2 = 5;
  size_t n3 = 20;
  double *b1 = NULL;
  char *b2 = NULL;
  int *b3 = NULL;
  int err;

  oski_Init ();

  oski_PrintDebugMessage (1, "... Allocating 3 arrays ...");
  oski_PrintDebugMessage (2, "b1: %d doubles", (int) n1);
  oski_PrintDebugMessage (2, "b2: %d characters", (int) n2);
  oski_PrintDebugMessage (2, "b3: %d ints", (int) n3);

  err = oski_MultiMalloc (__FILE__, __LINE__, 3,
			  n1 * sizeof (double), &b1,
			  n2 * sizeof (char), &b2, n3 * sizeof (int), &b3);

  if (!err)
    {
      size_t i;

      oski_PrintDebugMessage (2, "Success! Writing to the arrays...");
      for (i = 0; i < n1; i++)
	b1[i] = (double) i;
      for (i = 0; i < n2; i++)
	b2[i] = (char) ('a' + i);
      for (i = 0; i < n3; i++)
	b3[i] = (int) i;

      oski_PrintDebugMessage (2, "Freeing the arrays...");
      oski_FreeAll (3, b1, b2, b3);
    }

  oski_Close ();
  return err;
}

/* eof */
