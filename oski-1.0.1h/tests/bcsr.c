/**
 *  \file tests/bcsr.c
 *  \brief Test basic kernel operations for a matrix in BCSR format.
 *  Also implicitly tests oski_ApplyMatTransforms().
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/matcreate.h>
#include <oski/tune.h>

#include "abort_prog.h"
#include "array_util.h"
#include "readhbpat.h"

/**
 *  \brief Verify that the input and tuned matrix return approximately
 *  the same answers for \f$A\cdot x\f$.
 */
void
check_MatMult (const oski_matrix_t A0, const oski_matrix_t A1,
	       oski_index_t m, oski_index_t n)
{
  oski_value_t *x_val;
  oski_vecview_t x;
  oski_value_t *y0_val;
  oski_vecview_t y0;
  oski_value_t *y1_val;
  oski_vecview_t y1;

  oski_value_t alpha = MAKE_VAL_COMPLEX (1.0, 0.0);
/*	oski_value_t beta = MAKE_VAL_COMPLEX( -1.0, 0.0 ); */
  oski_value_t beta = MAKE_VAL_COMPLEX (0.0, 0.0);


  int err;

  x_val = array_CreateRandom (n);
  y0_val = array_CreateRandom (m);
  y1_val = array_Duplicate (y0_val, m);

  ABORT (x_val == NULL, check_MatMult, ERR_OUT_OF_MEMORY);
  ABORT (y0_val == NULL, check_MatMult, ERR_OUT_OF_MEMORY);
  ABORT (y1_val == NULL, check_MatMult, ERR_OUT_OF_MEMORY);

  x = oski_CreateVecView (x_val, n, STRIDE_UNIT);
  y0 = oski_CreateVecView (y0_val, m, STRIDE_UNIT);
  y1 = oski_CreateVecView (y1_val, m, STRIDE_UNIT);

  ABORT (x == INVALID_VEC, check_MatMult, ERR_BAD_VECVIEW);
  ABORT (y0 == INVALID_VEC, check_MatMult, ERR_BAD_VECVIEW);
  ABORT (y1 == INVALID_VEC, check_MatMult, ERR_BAD_VECVIEW);

  err = oski_MatMult (A0, OP_NORMAL, alpha, x, beta, y0);
  ABORT (err != 0, check_MatMult, err);

  err = oski_MatMult (A1, OP_NORMAL, alpha, x, beta, y1);
  ABORT (err != 0, check_MatMult, err);

  array_CompareElems ("check_MatMult", y0_val, y1_val, m);

  oski_DestroyVecView (x);
  oski_DestroyVecView (y0);
  oski_DestroyVecView (y1);
  oski_FreeAll (3, x_val, y0_val, y1_val);
}

/**
 *  \brief Verify that the input and tuned matrix return approximately
 *  the same answers for \f$A^T\cdot x\f$.
 */
void
check_MatTransMult (const oski_matrix_t A0, const oski_matrix_t A1,
		    oski_index_t m, oski_index_t n)
{
  oski_value_t *x_val;
  oski_vecview_t x;
  oski_value_t *y0_val;
  oski_vecview_t y0;
  oski_value_t *y1_val;
  oski_vecview_t y1;

  oski_value_t alpha = MAKE_VAL_COMPLEX (1.0, 0.0);
  oski_value_t beta = MAKE_VAL_COMPLEX (-1.0, 0.0);


  int err;

  x_val = array_CreateRandom (m);
  y0_val = array_CreateRandom (n);
  y1_val = array_Duplicate (y0_val, n);

  ABORT (x_val == NULL, check_MatTransMult, ERR_OUT_OF_MEMORY);
  ABORT (y0_val == NULL, check_MatTransMult, ERR_OUT_OF_MEMORY);
  ABORT (y1_val == NULL, check_MatTransMult, ERR_OUT_OF_MEMORY);

  x = oski_CreateVecView (x_val, m, STRIDE_UNIT);
  y0 = oski_CreateVecView (y0_val, n, STRIDE_UNIT);
  y1 = oski_CreateVecView (y1_val, n, STRIDE_UNIT);

  ABORT (x == INVALID_VEC, check_MatTransMult, ERR_BAD_VECVIEW);
  ABORT (y0 == INVALID_VEC, check_MatTransMult, ERR_BAD_VECVIEW);
  ABORT (y1 == INVALID_VEC, check_MatTransMult, ERR_BAD_VECVIEW);

  err = oski_MatMult (A0, OP_TRANS, alpha, x, beta, y0);
  ABORT (err != 0, check_MatTransMult, err);

  err = oski_MatMult (A1, OP_TRANS, alpha, x, beta, y1);
  ABORT (err != 0, check_MatTransMult, err);

  array_CompareElems ("check_MatTransMult", y0_val, y1_val, n);

  oski_DestroyVecView (x);
  oski_DestroyVecView (y0);
  oski_DestroyVecView (y1);
  oski_FreeAll (3, x_val, y0_val, y1_val);
}

/**
 *  \brief Verify that the input and tuned matrix return approximately
 *  the same answers for \f$\bar{A}^T\cdot x\f$.
 */
void
check_MatConjTransMult (const oski_matrix_t A0, const oski_matrix_t A1,
			oski_index_t m, oski_index_t n)
{
  oski_value_t *x_val;
  oski_vecview_t x;
  oski_value_t *y0_val;
  oski_vecview_t y0;
  oski_value_t *y1_val;
  oski_vecview_t y1;

  oski_value_t alpha = MAKE_VAL_COMPLEX (1.0, 0.0);
  oski_value_t beta = MAKE_VAL_COMPLEX (-1.0, 0.0);


  int err;

  x_val = array_CreateRandom (m);
  y0_val = array_CreateRandom (n);
  y1_val = array_Duplicate (y0_val, n);

  ABORT (x_val == NULL, check_MatConjTransMult, ERR_OUT_OF_MEMORY);
  ABORT (y0_val == NULL, check_MatConjTransMult, ERR_OUT_OF_MEMORY);
  ABORT (y1_val == NULL, check_MatConjTransMult, ERR_OUT_OF_MEMORY);

  x = oski_CreateVecView (x_val, m, STRIDE_UNIT);
  y0 = oski_CreateVecView (y0_val, n, STRIDE_UNIT);
  y1 = oski_CreateVecView (y1_val, n, STRIDE_UNIT);

  ABORT (x == INVALID_VEC, check_MatConjTransMult, ERR_BAD_VECVIEW);
  ABORT (y0 == INVALID_VEC, check_MatConjTransMult, ERR_BAD_VECVIEW);
  ABORT (y1 == INVALID_VEC, check_MatConjTransMult, ERR_BAD_VECVIEW);

  err = oski_MatMult (A0, OP_CONJ_TRANS, alpha, x, beta, y0);
  ABORT (err != 0, check_MatConjTransMult, err);

  err = oski_MatMult (A1, OP_CONJ_TRANS, alpha, x, beta, y1);
  ABORT (err != 0, check_MatConjTransMult, err);

  array_CompareElems ("check_MatConjTransMult", y0_val, y1_val, n);

  oski_DestroyVecView (x);
  oski_DestroyVecView (y0);
  oski_DestroyVecView (y1);
  oski_FreeAll (3, x_val, y0_val, y1_val);
}

int
main (int argc, char *argv[])
{
  char *matfile;
  int r, c;
  char xform[256];

  oski_index_t m, n;		/* matrix dimensions */
  oski_matrix_t A_input;	/* matrix read from file */
  oski_matrix_t A_tunable;	/* a transformed copy */
  oski_timer_t timer;

  int err;

  if (argc != 4)
    {
      fprintf (stderr, "usage: %s <matfile> <r> <c>\n", argv[0]);
      fprintf (stderr, "\n");
      fprintf (stderr, "This test program reads a matrix pattern from\n");
      fprintf (stderr, "a Harwell-Boeing file, converts it to <r> x <c>\n");
      fprintf (stderr, "BCSR format, and then tests several kernels\n");
      fprintf (stderr, "using it.\n");
      return 1;
    }

  oski_Init ();
  timer = oski_CreateTimer ();

  matfile = argv[1];
  r = atoi (argv[2]);
  c = atoi (argv[3]);

  oski_PrintDebugMessage (1, "... Reading the input file, '%s' ...", matfile);
  oski_RestartTimer (timer);
  A_input = readhb_pattern_matrix (matfile, &m, &n, NULL, 0);
  assert (A_input != NULL);
  oski_StopTimer (timer);
  oski_PrintDebugMessage (1, "(Took %g seconds)",
			  oski_ReadElapsedTime (timer));

  oski_PrintDebugMessage (1, "... Making a tunable copy ...");
  oski_RestartTimer (timer);
  A_tunable = oski_CopyMat (A_input);
  oski_StopTimer (timer);
  ABORT (A_tunable == NULL, main, ERR_BAD_MAT);
  oski_PrintDebugMessage (1, "(Took %g seconds)",
			  oski_ReadElapsedTime (timer));

#define CONV_TEMPLATE \
	"A_new = BCSR( InputMat, %d, %d )\n" \
	"return A_new"

  snprintf (xform, 255, CONV_TEMPLATE, r, c);
  xform[255] = (char) 0;	/* Just in case */

  oski_PrintDebugMessage (1, "... Converting using this BeBOP-Lua program:");
  oski_PrintDebugMessage (1, "-- begin\n%s\n-- end\n", xform);

  oski_RestartTimer (timer);
  err = oski_ApplyMatTransforms (A_tunable, xform);
  oski_StopTimer (timer);
  ABORT (err != 0, main, err);
  oski_PrintDebugMessage (1, "(Took %g seconds)",
			  oski_ReadElapsedTime (timer));

  oski_PrintDebugMessage (1, "... Checking matrix-vector multiply ...");
  check_MatMult (A_input, A_tunable, m, n);

  oski_PrintDebugMessage (1,
			  "... Checking matrix-transpose-vector multiply ...");
  check_MatTransMult (A_input, A_tunable, m, n);

  oski_PrintDebugMessage (1,
			  "... Checking matrix-conjugate-transpose-vector multiply ...");
  check_MatConjTransMult (A_input, A_tunable, m, n);

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
