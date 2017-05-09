/**
 *  \file tests/debug.c
 *  \brief Test the rudimentary debugging/logging facility.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <oski/common.h>

void
print_message (unsigned level, const char *dest)
{
  oski_PrintDebugMessage (level, "A debug message on %s: %d", dest, level);
}

int
main (int argc, char *argv[])
{
  unsigned level_out = 0, level_err = 0;
  unsigned level;

  if (argc >= 2)
    {
      level_out = atoi (argv[1]);
      if (argc >= 3)
	level_err = atoi (argv[2]);
    }

  /* display messages 1, ..., level_out on stdout */
  oski_SetDebugOutput (stdout);
  oski_SetDebugLevel (level_out);
  for (level = 1; level <= 6; level++)
    print_message (level, "stdout");

  /* display messages 1, ..., level_err on stderr */
  oski_SetDebugOutput (NULL);
  oski_SetDebugLevel (level_err);
  for (level = 1; level <= 6; level++)
    print_message (level, "stderr");

  return 0;
}

/* eof */
