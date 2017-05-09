/**
 *  \file tests/alloc.c
 *  \brief Test OSKI memory allocation routines.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <oski/common.h>

#define MAX_ELEMS 65536

static void *
my_malloc (size_t num_bytes)
{
  fprintf (stderr, "Failing %ld byte allocation attempt...\n",
	   (long) num_bytes);
  return NULL;
}

int
main (int argc, char *argv[])
{
  size_t num_elems = 1;
  void *buffer = NULL;

  oski_mallocfunc_t cur_malloc = NULL;
  oski_mallocfunc_t old_malloc = NULL;

  oski_Init ();

  oski_PrintDebugMessage (1, "... Allocating a sequence of arrays...");
  for (num_elems = 1; num_elems <= MAX_ELEMS; num_elems *= 2)
    {
      oski_PrintDebugMessage (1, "    %ld elements (%ld bytes)",
			      (long) num_elems,
			      (long) (sizeof (long) * num_elems));
      buffer = oski_Malloc (long, num_elems);
      oski_Free (buffer);
    }

  cur_malloc = oski_GetMalloc ();
  oski_PrintDebugMessage (1, "... C malloc function: 0x%p ...", malloc);
  oski_PrintDebugMessage (1, "... Current malloc function: 0x%p ...",
			  cur_malloc);
  if (cur_malloc != malloc)
    {
      fprintf (stderr, "*** ERROR: Malloc's not equal ***\n");
      return 1;
    }

  oski_PrintDebugMessage (1, "... Replacing current 'malloc' with a dummy");
  oski_PrintDebugMessage (1, "    routine, @ 0x%p ...", my_malloc);
  old_malloc = oski_SetMalloc (my_malloc);

  if (old_malloc != malloc)
    {
      fprintf (stderr, "*** ERROR: Old malloc incorrect ***\n");
      return 1;
    }

  buffer = oski_Malloc (long, MAX_ELEMS);
  if (buffer != NULL)
    {
      fprintf (stderr, "*** ERROR: Wrong malloc called! ***\n");
      fprintf (stderr, "    buffer == 0x%p\n", buffer);
      return 1;
    }

  oski_SetMalloc (old_malloc);
  oski_PrintDebugMessage (1, "Done!");

  oski_Close ();
  return 0;
}

/* eof */
