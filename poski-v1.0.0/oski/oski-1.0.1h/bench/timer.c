/**
 *  \file timer.c
 *  \brief Simple command-line timing utility.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <oski/common.h>
#include <oski/timer.h>

static void
usage (const char *progname)
{
  fprintf (stderr, "%s [options] prog [prog_options ...]\n", progname);
  fprintf (stderr, "\n");
  fprintf (stderr, "Similar to 'time' utility, but uses OSKI's timer.\n");
  fprintf (stderr, "\n");
  fprintf (stderr, "Valid [options]:\n");
  fprintf (stderr, "  --help\n    Prints this message.\n");
  fprintf (stderr, "  --calibrate\n"
	   "    Print the ticks-to-seconds conversion factor.\n");
  fprintf (stderr, "\n");
}

int
main (int argc, char *argv[])
{
  oski_timer_t timer;
  int i_arg = 1;
  char **new_args;
  int err;

  /* Process arguments */
  while (i_arg < argc && argv[i_arg][0] == '-')
    {
      if (strcmp (argv[i_arg], "--calibrate") == 0)
	{
	  double secs_per_tick = oski_GetTimerSecsPerTick ();
	  printf ("%g\n", 1.0 / secs_per_tick);
	  return 0;
	}
      else if (strcmp (argv[i_arg], "--help") == 0
	       || strcmp (argv[i_arg], "-h") == 0
	       || strcmp (argv[i_arg], "-H") == 0)
	{
	  usage (argv[0]);
	  return 0;
	}
      else
	{
	  fprintf (stderr, "*** Unrecognized option, '%s' ***\n",
		   argv[i_arg]);
	  usage (argv[0]);
	  return 1;
	}
      i_arg++;
    }

  if (i_arg >= argc)
    {
      usage (argv[0]);
      return 0;
    }

  /* Duplicate arguments */
  if ((i_arg + 1) < argc)
    {
      new_args = oski_Malloc (char *, argc - i_arg + 1);
      if (new_args == NULL)
	{
	  fprintf (stderr, "*** Out of memory ***\n");
	  return 1;
	}
      else
	{
	  int i;
	  new_args[0] = argv[i_arg];
	  for (i = 1; i <= argc - i_arg; i++)
	    new_args[i] = argv[i_arg + i];
	  new_args[i] = NULL;

	  for (i = 0; i < argc - i_arg + 1; i++)
	    {
	      fprintf (stderr, "[%d] '%s'\n", i, new_args[i]);
	    }
	}
    }

  timer = oski_CreateTimer ();
  if (timer == NULL)
    {
      fprintf (stderr, "*** Couldn't create a timer. (?) ***\n");
      return 1;
    }

  oski_RestartTimer (timer);
  err = execv (argv[i_arg], new_args);
  oski_StopTimer (timer);

  printf ("%g\n", oski_ReadElapsedTime (timer));

  oski_Free (new_args);
  oski_DestroyTimer (timer);
  return err;
}

/* eof */
