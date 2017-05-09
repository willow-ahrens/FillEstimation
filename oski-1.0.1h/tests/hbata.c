/**
 *  \file tests/hbata.c
 *  \brief Comprehensive test of sparse \f$A^TAx\f$ kernel
 *  routine, oski_MatTransMatMult(), for an arbitrary data
 *  structure transformation (specified by a OSKI-Lua
 *  program), where the matrix is read from a Harwell-Boeing
 *  file.
 *
 *  Like hbmatmult.c, also implicitly tests
 *  oski_ApplyMatTransforms().
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

/** \name Parameter test lists. */
/*@{*/
#define NUM_OP 4
static const oski_ataop_t TL_op[NUM_OP] = {
  OP_AT_A, OP_A_AT, OP_AH_A, OP_A_AH
};

#define NUM_NUM_VECS 2
static const oski_index_t TL_num_vecs[NUM_NUM_VECS] = { 1, 3 };

#define NUM_ALPHA 3
static const oski_value_t TL_alpha[NUM_ALPHA] = {
  MAKE_VAL_COMPLEX (1.0, 0.0),
  MAKE_VAL_COMPLEX (-1.0, 0.0),
  MAKE_VAL_COMPLEX (.5, -.25)
};

#define NUM_BETA 3
static const oski_value_t TL_beta[NUM_BETA] = {
  MAKE_VAL_COMPLEX (0.0, 0.0),
  MAKE_VAL_COMPLEX (1.0, 0.0),
  MAKE_VAL_COMPLEX (-.25, 0.6)
};

#define NUM_USE_MINSTRIDE 2
static const int TL_use_minstride[NUM_USE_MINSTRIDE] = { 0, 1 };

#define NUM_ORIENT 2
static const oski_storage_t TL_orient[NUM_ORIENT] = {
  LAYOUT_COLMAJ, LAYOUT_ROWMAJ
};

/*@}*/

static int
TestInstance (const oski_matrix_t A0, const oski_matrix_t A1,
	      oski_index_t m, oski_index_t n, oski_ataop_t opA,
	      oski_index_t num_vecs, oski_value_t alpha,
	      const oski_vecview_t x, oski_value_t beta, oski_vecview_t y0,
	      oski_vecview_t t0, int test_num, size_t max_tests, int min_test,
	      int max_test)
{
  int err;
  if (test_num >= min_test && (test_num <= max_test || max_test < 0))
    {
      oski_PrintDebugMessage (1, ">> TEST %d / %d", test_num, max_tests);
      err = check_MatTransMatMult_instance (A0, A1, opA,
					    alpha, x, beta, y0, t0);
      ABORT (err != 0, check_MatTransMatMult, err);
    }
  else
    oski_PrintDebugMessage (1, "SKIPPING >> TEST %d / %d",
			    test_num, max_tests);
  test_num++;
  return test_num;
}

static int
TestT (const oski_matrix_t A0, const oski_matrix_t A1,
       oski_index_t m, oski_index_t n, oski_ataop_t opA,
       oski_index_t num_vecs, oski_value_t alpha, const oski_vecview_t x,
       oski_value_t beta, oski_vecview_t y0, int test_num, size_t max_tests,
       int min_test, int max_test)
{
  int i8;
  for (i8 = 0; i8 < NUM_USE_MINSTRIDE; i8++)
    {
      int t_use_minstride = TL_use_minstride[i8];
      int i9;
      for (i9 = 0; i9 < NUM_ORIENT; i9++)
	{
	  oski_storage_t t_orient = TL_orient[i9];
	  int i10;
	  for (i10 = 1; i10 >= 0; i10--)
	    {
	      oski_vecview_t t0 = INVALID_VEC;
	      if (i10)
		{
		  if (opA == OP_AT_A || opA == OP_AH_A)
		    t0 = testvec_Create (m, num_vecs,
					 t_orient, t_use_minstride);
		  else		/* opA == OP_A_AT || opA == OP_A_AH */
		    t0 = testvec_Create (n, num_vecs,
					 t_orient, t_use_minstride);
		  ABORT (t0 == INVALID_VEC, check_MatTransMatMult,
			 ERR_OUT_OF_MEMORY);
		}
	      test_num = TestInstance (A0, A1, m, n, opA, num_vecs,
				       alpha, x, beta, y0, t0,
				       test_num, max_tests, min_test,
				       max_test);

	      if (t0 != INVALID_VEC)
		testvec_Destroy (t0);

	    }			/* i10 */
	}			/* i9 */
    }				/* i8 */
  return test_num;
}

static int
TestY (const oski_matrix_t A0, const oski_matrix_t A1,
       oski_index_t m, oski_index_t n, oski_ataop_t opA,
       oski_index_t num_vecs, oski_value_t alpha, const oski_vecview_t x,
       oski_value_t beta, int test_num, size_t max_tests, int min_test,
       int max_test)
{
  int i6;
  for (i6 = 0; i6 < NUM_USE_MINSTRIDE; i6++)
    {
      int y_use_minstride = TL_use_minstride[i6];
      int i7;
      for (i7 = 0; i7 < NUM_ORIENT; i7++)
	{
	  oski_storage_t y_orient = TL_orient[i7];
	  oski_vecview_t y0;

	  if (opA == OP_A_AT || opA == OP_A_AH)
	    y0 = testvec_Create (m, num_vecs, y_orient, y_use_minstride);
	  else
	    y0 = testvec_Create (n, num_vecs, y_orient, y_use_minstride);
	  ABORT (y0 == INVALID_VEC, check_MatTransMatMult, ERR_OUT_OF_MEMORY);

	  test_num = TestT (A0, A1, m, n, opA, num_vecs, alpha, x, beta, y0,
			    test_num, max_tests, min_test, max_test);

	  testvec_Destroy (y0);

	}			/* i7 */
    }				/* i6 */
  return test_num;
}

static int
TestBeta (const oski_matrix_t A0, const oski_matrix_t A1,
	  oski_index_t m, oski_index_t n, oski_ataop_t opA,
	  oski_index_t num_vecs, oski_value_t alpha, const oski_vecview_t x,
	  int test_num, size_t max_tests, int min_test, int max_test)
{
  int i5;
  for (i5 = 0; i5 < NUM_BETA; i5++)
    {
      oski_value_t beta;
      VAL_ASSIGN (beta, TL_beta[i5]);
      test_num = TestY (A0, A1, m, n, opA, num_vecs, alpha, x, beta,
			test_num, max_tests, min_test, max_test);
    }				/* i5 */
  return test_num;
}

static int
TestX (const oski_matrix_t A0, const oski_matrix_t A1,
       oski_index_t m, oski_index_t n, oski_ataop_t opA,
       oski_index_t num_vecs, oski_value_t alpha, int test_num,
       size_t max_tests, int min_test, int max_test)
{
  int i3;
  for (i3 = 0; i3 < NUM_USE_MINSTRIDE; i3++)
    {
      int x_use_minstride = TL_use_minstride[i3];
      int i4;
      for (i4 = 0; i4 < NUM_ORIENT; i4++)
	{
	  oski_storage_t x_orient = TL_orient[i4];
	  oski_vecview_t x;

	  if (opA == OP_AT_A || opA == OP_AH_A)
	    x = testvec_Create (n, num_vecs, x_orient, x_use_minstride);
	  else
	    x = testvec_Create (m, num_vecs, x_orient, x_use_minstride);
	  ABORT (x == INVALID_VEC, check_MatTransMatMult, ERR_OUT_OF_MEMORY);

	  test_num = TestBeta (A0, A1, m, n, opA, num_vecs, alpha, x,
			       test_num, max_tests, min_test, max_test);
	  testvec_Destroy (x);
	}			/* i4 */
    }				/* i3 */
  return test_num;
}

static int
TestAlpha (const oski_matrix_t A0, const oski_matrix_t A1,
	   oski_index_t m, oski_index_t n, oski_ataop_t opA,
	   oski_index_t num_vecs, int test_num, size_t max_tests,
	   int min_test, int max_test)
{
  int i2;
  for (i2 = 0; i2 < NUM_ALPHA; i2++)
    {
      oski_value_t alpha;
      VAL_ASSIGN (alpha, TL_alpha[i2]);
      test_num = TestX (A0, A1, m, n, opA, num_vecs, alpha,
			test_num, max_tests, min_test, max_test);
    }				/* i2 */
  return test_num;
}

static int
TestNumVecs (const oski_matrix_t A0, const oski_matrix_t A1,
	     oski_index_t m, oski_index_t n, oski_ataop_t opA,
	     int test_num, size_t max_tests, int min_test, int max_test)
{
  int i1;
  for (i1 = 0; i1 < NUM_NUM_VECS; i1++)
    {
      oski_index_t num_vecs = TL_num_vecs[i1];
      test_num = TestAlpha (A0, A1, m, n, opA, num_vecs,
			    test_num, max_tests, min_test, max_test);
    }				/* i1 */
  return test_num;
}

static int
TestOpA (const oski_matrix_t A0, const oski_matrix_t A1,
	 oski_index_t m, oski_index_t n,
	 int test_num, size_t max_tests, int min_test, int max_test)
{
  int i0;
  for (i0 = 0; i0 < NUM_OP; i0++)
    {
      oski_ataop_t opA = TL_op[i0];
      test_num = TestNumVecs (A0, A1, m, n, opA,
			      test_num, max_tests, min_test, max_test);
    }				/* i0 */
  return test_num;
}

static void
check_MatTransMatMult (const oski_matrix_t A0, const oski_matrix_t A1,
		       oski_index_t m, oski_index_t n, int min_test,
		       int max_test)
{
  size_t max_tests = NUM_OP * NUM_NUM_VECS * NUM_ALPHA
    * NUM_USE_MINSTRIDE * NUM_ORIENT * NUM_BETA
    * NUM_USE_MINSTRIDE * NUM_ORIENT * NUM_USE_MINSTRIDE * NUM_ORIENT * 2;

  TestOpA (A0, A1, m, n, 1, max_tests, min_test, max_test);
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

  int min_test, max_test;

  int err;

  if (argc < 3)
    {
      fprintf (stderr, "usage: %s <matfile> <xform_program>"
	       " [min_test_num=0 max_test_num=-1]\n", argv[0]);
      fprintf (stderr, "\n");
      fprintf (stderr,
	       "This program tests oski_MatTransMatMult() on a sample\n"
	       "matrix pattern stored in Harwell-Boeing formatted file.\n"
	       "\n"
	       "Specifically, the test compares the results of SpMV\n"
	       "when the matrix is stored initially in compressed\n"
	       "sparse column (CSC) format to the results when stored\n"
	       "in the format specified by <xform_program>, a\n"
	       "OSKI-Lua transformation program.\n"
	       "\n"
	       "The test tries to exercise many possible values of\n"
	       "the following parameters:\n"
	       "\t- Multiplying by A*A^T, A*A^H, A^T*A, A^H*A.\n"
	       "\t- Values of alpha, beta.\n"
	       "\t- Row vs. column major layout of x, y, and t\n"
	       "\t  independently.\n"
	       "\t- Single vs. multiple-vectors.\n"
	       "\t- Minimum (e.g., unit) stride access vs.\n"
	       "\t  general strided access.\n"
	       "\n"
	       "To test only a consecutive subset of possible\n"
	       "tests, set the optional parameters,\n"
	       "min_test_num (default == 0) and max_test_num\n"
	       "(default == -1 to perform all tests).\n");
      return 1;
    }

  oski_Init ();
  timer = oski_CreateTimer ();

  matfile = argv[1];
  xform = argv[2];

  if (argc >= 4)
    min_test = atoi (argv[3]);
  else
    min_test = 0;

  if (argc >= 5)
    max_test = atoi (argv[4]);
  else
    max_test = -1;

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

  oski_PrintDebugMessage (1, "... Checking oski_MatTransMatMult() ...");
  check_MatTransMatMult (A_input, A_tunable, m, n, min_test, max_test);

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
