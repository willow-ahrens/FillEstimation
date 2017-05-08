/**
 *  \file tests/heur2.c
 *  \brief Additional testing of the heuristic.
 *  \ingroup TESTING
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <oski/oski.h>

#include "abort_prog.h"
#include "testvec.h"
#include "timing.h"
#include "parse_opts.h"
#include "readhbpat.h"

/** \brief Print command-line options. */
static void
usage (const char *progname)
{
  fprintf (stderr, "usage: %s <matfile> <num_times>\n", progname);
}

/** Main benchmark program. */
int
main (int argc, char *argv[])
{
  char *matfile = NULL;
  int num_times = 0;

  /* Test matrix and vector storage */
  oski_matrix_t A_orig = INVALID_MAT;
  oski_matrix_t A_tunable = INVALID_MAT;
  oski_index_t num_rows;
  oski_index_t num_cols;
  char mattype[4];

  oski_matop_t opA = OP_NORMAL;
  oski_value_t alpha = MAKE_VAL_COMPLEX (1.0, 0.0);
  oski_value_t beta = MAKE_VAL_COMPLEX (0.0, 0.0);

  int err;
  char *xform;
  double Mflops_observed;

  oski_index_t num_vecs;
  oski_vecview_t x, y;

  if (argc != 3)
    {
      usage (argv[0]);
      return -1;
    }
  matfile = argv[1];
  num_times = atoi (argv[2]);

  oski_Init ();

  A_orig = readhb_pattern_matrix (matfile, &num_rows, &num_cols, mattype, 0);
  ABORT (A_orig == INVALID_MAT, main, ERR_BAD_MAT);

  /* Create test vectors */
  num_vecs = 1;
  if (opA == OP_NORMAL || opA == OP_CONJ)
    {
      y = testvec_Create (num_rows, num_vecs, LAYOUT_ROWMAJ, 1);
      x = testvec_Create (num_cols, num_vecs, LAYOUT_ROWMAJ, 1);
    }
  else
    {
      y = testvec_Create (num_cols, num_vecs, LAYOUT_ROWMAJ, 1);
      x = testvec_Create (num_rows, num_vecs, LAYOUT_ROWMAJ, 1);
    }

  /* Make a copy, so we can tune it and compare to original */
  A_tunable = oski_CopyMat (A_orig);
  ABORT (A_tunable == INVALID_MAT, main, ERR_OUT_OF_MEMORY);

  /* Try tuning the copy */
  err = oski_SetHintMatMult (A_tunable, OP_NORMAL, alpha, x,
			     beta, y, num_times);
  ABORT (err != 0, main, err);

  err = oski_TuneMat (A_tunable);
  xform = "(no transformation)";
  switch (err)
    {
    case TUNESTAT_AS_IS:
      oski_PrintDebugMessage (1, "Did NOT tune.");
      break;

    case TUNESTAT_NEW:
      xform = oski_GetMatTransforms (A_tunable);
      oski_PrintDebugMessage (1, "DID tune: '%s'", xform);
      break;

    default:
      ABORT (err != 0, main, err);
    }

  /* Make sure we get the "same" answer */
  err = check_MatMult_instance (A_orig, A_tunable, opA, alpha, x, beta, y);
  ABORT (err != 0, main, err);

  /* Benchmark */
  TIMING_LOOP_BASIC (oski_MatMult (A_orig, opA, alpha, x, beta, y),
		     10, 2e-6 * A_orig->props.num_nonzeros, Mflops_observed);
  oski_PrintDebugMessageShort (1, "%g", Mflops_observed);
  TIMING_LOOP_BASIC (oski_MatMult (A_tunable, opA, alpha, x, beta, y),
		     10, 2e-6 * A_orig->props.num_nonzeros, Mflops_observed);
  oski_PrintDebugMessageShort (1, " %g", Mflops_observed);
  oski_PrintDebugMessageShort (1, " %% ref. vs. '%s'", xform);
  oski_PrintDebugMessageShort (1, "\n");

  /* Clean-up for the next iteration */
  oski_DestroyVecView (x);
  oski_DestroyVecView (y);

  oski_DestroyMat (A_tunable);
  oski_DestroyMat (A_orig);

  oski_Close ();
  return 0;
}

/* eof */
