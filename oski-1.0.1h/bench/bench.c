/**
 *  \file bench/bench.c
 *  \brief Generic workload benchmarking utility.
 *  \ingroup OFFBENCH
 *
 *  This utility can be used to benchmark an arbitrary workload
 *  under a specific OSKI-Lua transformation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <oski/oski.h>
#include <oski/simplelist.h>

#include "abort_prog.h"
#include "testvec.h"
#include "timing.h"
#include "parse_opts.h"
#include "readhbpat.h"
#include "matopts.h"
#include "keropts.h"
#include "workload.h"

/* Global program options */
typedef struct
{
  matopts_t matrix;  /**< Matrix generation options */
  const char *xform; /**< Tuning transformation to apply */
} progopts_t;

static void
init_default_opts (progopts_t * opts)
{
  matopts_Init (&(opts->matrix));
  opts->xform = "return CSR(InputMat)";
}

static void
display_opts (const progopts_t * opts)
{
  fprintf (stderr, "Summary of command-line options:\n");
  matopts_Display (&(opts->matrix), stderr);
}

/** \brief Print command-line options. */
static void
usage (const char *progname)
{
  fprintf (stderr,
	   "\n"
	   "usage: %s [options] <xform>"
	   " <kernel_options_1> [kernel_options_2 ...]\n"
	   "where:\n"
	   "   <xform>\n"
	   "      Tuning transformation to apply\n"
	   "   {<kernel_options_k>}\n"
	   "      Set describing a kernel workload.\n"
	   "\n"
	   "Valid [options] include matrix type specification\n"
	   "options listed below.\n", progname);
  fprintf (stderr, "\n---- Matrix Type Specification Options ----\n");
  matopts_Usage (stderr);
  fprintf (stderr, "\n---- Kernel Workload Specification ----\n");
  keropts_Usage (stderr);
}

static void
abort_option_error (const char *flag, const char *bad_value,
		    const char *good_value_1, ...)
{
  va_list ap;
  const char *value;

  fprintf (stderr, "*** '%s' is not a valid value for option '%s'.\n",
	   bad_value, flag);
  fprintf (stderr, "    Valid values are: %s", good_value_1);
  va_start (ap, good_value_1);
  value = va_arg (ap, const char *);

  while (value != NULL)
    {
      fprintf (stderr, ", %s", value);
      value = va_arg (ap, const char *);
    }
  va_end (ap);
  fprintf (stderr, "\n");
  exit (1);
}

static simplelist_t *
process_options (int argc, char *argv[], progopts_t * opts)
{
  simplelist_t *workload = simplelist_Create ();
  int i = 1;

  ABORT (workload == NULL, process_options, ERR_OUT_OF_MEMORY);

  /* Global options */
  while (i < argc && argv[i][0] == '-')
    {
      int i_next = matopts_Process (argc, argv, i, &(opts->matrix));
      if (i_next < i)
	{			/* error while processing matrix options */
	  exit (1);
	}
      else if (i_next > i)
	{
	  i = i_next;
	  continue;
	}
      else if (StringMatches (argv[i], "--help", "-h", "-H", NULL))
	{
	  usage (argv[0]);
	  exit (0);
	}
      else
	{
	  fprintf (stderr,
		   "*** Unrecognized or ill-specified option, '%s'. ***\n",
		   argv[i]);
	  exit (1);
	}
      i++;
    }

  /* Required options */
  if ((i + 1) >= argc)
    {
      usage (argv[0]);
      exit (1);
    }

  opts->xform = argv[i];
  i++;

  /* Workload specification */
  do
    {
      kernel_t *new_opt = NULL;
      int i_next = keropts_Process (argc, argv, i, &new_opt);
      size_t i_opt;

      /* error occurred while processing */
      if (new_opt == NULL || i_next <= i)
	{
	  fprintf (stderr,
		   "*** Option '%s' is unrecognized or specified incorrectly\n",
		   argv[i]);
	  exit (1);
	}
      i_opt = simplelist_Append (workload, new_opt);
      ABORT (i_opt == 0, process_options, ERR_OUT_OF_MEMORY);

      assert (i_next > i);
      i = i_next;
    }
  while (i < argc);

  if (workload == NULL || simplelist_GetLength (workload) == 0)
    {
      usage (argv[0]);
      exit (1);
    }
  return workload;
}

/** Main benchmark program. */
int
main (int argc, char *argv[])
{
  /* Command-line options */
  progopts_t opts;

  /* Matrix */
  oski_index_t num_rows, num_cols;	/* Matrix dimensions */
  oski_index_t nnz_stored, nnz;	/* # of non-zeros */
  oski_matrix_t A_tuned;

  /* Workload */
  simplelist_t *workload;

  /* Timing stuff */
  oski_timer_t timer;
  double Mflop_count;
  double Mflops_obs_tuned;
  double t_create, t_genwork, t_xform, t_bench, t_total;

  fprintf (stderr, "Setup...\n");
  init_default_opts (&opts);
  workload = process_options (argc, argv, &opts);
  display_opts (&opts);

  oski_Init ();
  timer = oski_CreateTimer ();

  oski_RestartTimer (timer);
  A_tuned = matopts_CreateMat (&(opts.matrix),
			       &num_rows, &num_cols, &nnz_stored, &nnz);
  ABORT (A_tuned == INVALID_MAT, main, ERR_BAD_MAT);
  oski_StopTimer (timer);
  t_create = oski_ReadElapsedTime (timer);

  oski_RestartTimer (timer);
  workload_CreateData (num_rows, num_cols, workload);
  oski_StopTimer (timer);
  t_genwork = oski_ReadElapsedTime (timer);

  fprintf (stderr, "Matrix properties:\n");
  fprintf (stderr, "\tSize: %ld x %ld\n", (long) num_rows, (long) num_cols);
  fprintf (stderr, "\tNo. of non-zeros: %ld\n", (long) nnz);
  fprintf (stderr, "\tNo. of stored non-zeros: %ld\n", (long) nnz_stored);

  fprintf (stderr,
	   "Applying transformation:\n"
	   "# (begin)\n" "'%s'\n" "# (end transformation)\n", opts.xform);
  oski_RestartTimer (timer);
  oski_ApplyMatTransforms (A_tuned, opts.xform);
  oski_StopTimer (timer);
  t_xform = oski_ReadElapsedTime (timer);

  fprintf (stderr, "Benchmarking...\n");
  oski_RestartTimer (timer);
  Mflop_count = 1e-6 * nnz * workload_CountFlopsPerNz (workload);
  TIMING_LOOP_BASIC (workload_Run (A_tuned, workload),
		     5, Mflop_count, Mflops_obs_tuned);
  oski_StopTimer (timer);
  t_bench = oski_ReadElapsedTime (timer);

  fprintf (stderr, "Done! Results:\n");
  printf ("%g %g\n", Mflops_obs_tuned, 1e-6 * nnz / t_xform);

#define DUMP_PROFILE_STATS
#if defined(DUMP_PROFILE_STATS)
  t_total = t_create + t_genwork + t_xform + t_bench;
  fprintf (stderr, "[@ CreateMat: %g s (%g%%)]\n",
	   t_create, t_create / t_total);
  fprintf (stderr, "[@ Workload: %g s (%g%%)]\n",
	   t_genwork, t_genwork / t_total);
  fprintf (stderr, "[@ Transform: %g s (%g%%)]\n",
	   t_xform, t_xform / t_total);
  fprintf (stderr, "[@ Benchmark: %g s (%g%%)]\n",
	   t_bench, t_bench / t_total);
  fprintf (stderr, "[@ Total: %g s]\n", t_total);
#endif

  oski_DestroyTimer (timer);
  oski_Close ();
  return 0;
}

/* eof */
