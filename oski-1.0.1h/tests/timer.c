/**
 *  \file tests/timer.c
 *  \brief Tests timer module.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include <oski/timer.h>

int
main (int argc, char *argv[])
{
  oski_timer_t timer;
  unsigned int test_time;
  double t_elapsed;
  double t_final;

  if (argc < 2)
    {
      fprintf (stderr, "usage: %s [<num_seconds>]\n", argv[0]);
      fprintf (stderr, "\nIf not specified, <num_seconds> is chosen");
      fprintf (stderr, "\nto be a random integer number of seconds");
      fprintf (stderr, "\nbetween 1 and 3.");
      fprintf (stderr, "\n\n");

      srand ((unsigned int) time (NULL));
      test_time = 1 + (rand () % 3);
    }
  else
    test_time = atoi (argv[1]);

  oski_Init ();

  oski_PrintDebugMessage (1, "... Creating timer ...");
  timer = oski_CreateTimer ();

  oski_PrintDebugMessage (1, "... Pausing for %d seconds ...", test_time);
  oski_RestartTimer (timer);
  sleep (test_time);
  t_elapsed = oski_ReadElapsedTime (timer);

  oski_PrintDebugMessage (1, "... Observed %g second pause ...", t_elapsed);
  oski_StopTimer (timer);

  t_final = oski_ReadElapsedTime (timer);
  oski_PrintDebugMessage (1, "... Print/stop overhead: %g ...",
			  t_final - t_elapsed);

  oski_PrintDebugMessage (1, "... Cleaning up ...");
  oski_DestroyTimer (timer);

  oski_Close ();

  if (fabs (t_final - (double) test_time) < .1)
    {
      return 0;
    }
  else
    {
      fprintf (stderr, "WARNING: timer seems inaccurate... (delta=%g)\n",
	       t_final - (double) test_time);
      return 1;
    }
}

/* eof */
