/**
 *  \file tests/convert.c
 *  \brief Test matrix conversion.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/matcreate.h>
#include <oski/tune.h>

#include "abort_prog.h"
#include "readhbpat.h"
#include "testvec.h"

static void
check_MatMult (const oski_matrix_t A0, const oski_matrix_t A1,
	       oski_index_t m, oski_index_t n)
{
  int err;
  oski_matop_t opA = OP_NORMAL;
  oski_index_t num_vecs = 1;
  oski_value_t alpha = MAKE_VAL_COMPLEX (1.0, 0.0);
  oski_value_t beta = MAKE_VAL_COMPLEX (0.0, 0.0);
  oski_vecview_t y = testvec_Create (m, num_vecs, LAYOUT_COLMAJ, 1);
  oski_vecview_t x = testvec_Create (n, num_vecs, LAYOUT_COLMAJ, 1);

  ABORT (x == INVALID_VEC || y == INVALID_VEC,
	 check_MatMult, ERR_OUT_OF_MEMORY);

  err = check_MatMult_instance (A0, A1, opA, alpha, x, beta, y);
  ABORT (err != 0, check_MatMult, err);

  testvec_Destroy (x);
  testvec_Destroy (y);
}

int
main (int argc, char *argv[])
{
  char *matfile;
  char *xform;

  oski_index_t m, n;		/* matrix dimensions */
  oski_matrix_t A_input;	/* matrix read from file */
  oski_matrix_t A_tunable;	/* a transformed copy */
  oski_timer_t timer;

  int err;

  if (argc < 3)
    {
      fprintf (stderr, "usage: %s <matfile> <xform_program>", argv[0]);
      fprintf (stderr, "\n");
      fprintf (stderr,
	       "This program tests oski_MatMult() on a sample matrix\n"
	       "pattern stored in Harwell-Boeing formatted file.\n"
	       "\n"
	       "Specifically, the test compares the results of SpMV\n"
	       "when the matrix is stored initially in compressed\n"
	       "sparse column (CSC) format to the results when stored\n"
	       "in the format specified by <xform_program>, a\n"
	       "OSKI-Lua transformation program.\n" "\n");
      return 1;
    }

  oski_Init ();
  timer = oski_CreateTimer ();

  matfile = argv[1];
  xform = argv[2];

  oski_PrintDebugMessage (1, "... Reading the input file, '%s' ...", matfile);
  oski_RestartTimer (timer);
  A_input = readhb_pattern_matrix (matfile, &m, &n, NULL, 0);
  assert (A_input != NULL);
  oski_StopTimer (timer);
  oski_PrintDebugMessage (1, "(Took %g seconds)",
			  oski_ReadElapsedTime (timer));

  oski_PrintDebugMessage (1, "... Making a copy ...");
  oski_RestartTimer (timer);
  A_tunable = oski_CopyMat (A_input);
  oski_StopTimer (timer);
  ABORT (A_tunable == NULL, main, ERR_BAD_MAT);
  oski_PrintDebugMessage (1, "(Took %g seconds)",
			  oski_ReadElapsedTime (timer));

  oski_PrintDebugMessage (1, "... Converting using this OSKI-Lua program:");
  oski_PrintDebugMessage (1, "-- BEGIN --\n\n%s\n\n-- END --\n", xform);

  oski_RestartTimer (timer);
  err = oski_ApplyMatTransforms (A_tunable, xform);
  oski_StopTimer (timer);
  ABORT (err != 0, main, err);
  oski_PrintDebugMessage (1, "(Took %g seconds)",
			  oski_ReadElapsedTime (timer));

  oski_PrintDebugMessage (1, "... Checking matrix-vector multiply ...");
  check_MatMult (A_input, A_tunable, m, n);

  oski_PrintDebugMessage (1, "... Cleaning up ...");
  oski_RestartTimer (timer);
  oski_DestroyMat (A_tunable);
  oski_DestroyMat (A_input);
  oski_StopTimer (timer);
  oski_PrintDebugMessage (1, "(Took %g seconds)",
			  oski_ReadElapsedTime (timer));

  oski_DestroyTimer (timer);
  oski_Close ();
  return 0;
}

/* eof */
