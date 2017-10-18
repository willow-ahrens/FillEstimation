/**
 *  \file tests/hbmatmult.c
 *  \brief Comprehensive test of matrix-vector multiply for an
 *  arbitrary data structure transformation (specified by a
 *  OSKI-Lua program), where the matrix is read from a
 *  Harwell-Boeing file.
 *
 *  Like bcsr.c, also implicitly tests oski_ApplyMatTransforms().
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
#define NUM_OP 3
static const oski_matop_t TL_op[NUM_OP] = {
  OP_NORMAL, OP_TRANS, OP_CONJ_TRANS
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
	      oski_index_t m, oski_index_t n, oski_matop_t opA,
	      oski_index_t num_vecs, oski_value_t alpha,
	      const oski_vecview_t x, oski_value_t beta, oski_vecview_t y0,
	      int test_num, int max_tests, int min_test, int max_test)
{
  if (test_num >= min_test && (test_num <= max_test || max_test < 0))
    {
      int err;
      oski_PrintDebugMessage (1, ">> TEST %d / %d", test_num, max_tests);
      err = check_MatMult_instance (A0, A1, opA, alpha, x, beta, y0);
      ABORT (err != 0, check_MatMult, err);
    }
  else
    oski_PrintDebugMessage (1, "SKIPPING >> TEST %d / %d", test_num,
			    max_tests);
  return test_num + 1;
}

static int
TestY (const oski_matrix_t A0, const oski_matrix_t A1,
       oski_index_t m, oski_index_t n, oski_matop_t opA,
       oski_index_t num_vecs, oski_value_t alpha, const oski_vecview_t x,
       oski_value_t beta, int test_num, int max_tests, int min_test,
       int max_test)
{
  int i;
  for (i = 0; i < NUM_USE_MINSTRIDE; i++)
    {
      int use_minstride = TL_use_minstride[i];
      int j;
      for (j = 0; j < NUM_ORIENT; j++)
	{
	  oski_storage_t orient = TL_orient[j];
	  oski_vecview_t y;

	  if (opA == OP_NORMAL || opA == OP_CONJ)
	    y = testvec_Create (m, num_vecs, orient, use_minstride);
	  else
	    y = testvec_Create (n, num_vecs, orient, use_minstride);
	  ABORT (y == INVALID_VEC, check_MatMult, ERR_OUT_OF_MEMORY);
	  test_num = TestInstance (A0, A1, m, n,
				   opA, num_vecs, alpha, x, beta, y,
				   test_num, max_tests, min_test, max_test);
	  testvec_Destroy (y);
	}
    }
  return test_num;
}

static int
TestBeta (const oski_matrix_t A0, const oski_matrix_t A1,
	  oski_index_t m, oski_index_t n, oski_matop_t opA,
	  oski_index_t num_vecs, oski_value_t alpha, const oski_vecview_t x,
	  int test_num, int max_tests, int min_test, int max_test)
{
  int i;
  for (i = 0; i < NUM_BETA; i++)
    {
      oski_value_t beta = TL_beta[i];
      test_num = TestY (A0, A1, m, n, opA, num_vecs, alpha, x, beta,
			test_num, max_tests, min_test, max_test);
    }
  return test_num;
}

static int
TestX (const oski_matrix_t A0, const oski_matrix_t A1,
       oski_index_t m, oski_index_t n, oski_matop_t opA,
       oski_index_t num_vecs, oski_value_t alpha, int test_num, int max_tests,
       int min_test, int max_test)
{
  int i;
  for (i = 0; i < NUM_USE_MINSTRIDE; i++)
    {
      int use_minstride = TL_use_minstride[i];
      int j;
      for (j = 0; j < NUM_ORIENT; j++)
	{
	  oski_storage_t orient = TL_orient[j];
	  oski_vecview_t x;

	  if (opA == OP_NORMAL || opA == OP_CONJ)
	    x = testvec_Create (n, num_vecs, orient, use_minstride);
	  else
	    x = testvec_Create (m, num_vecs, orient, use_minstride);
	  ABORT (x == INVALID_VEC, check_MatMult, ERR_OUT_OF_MEMORY);
	  test_num = TestBeta (A0, A1, m, n, opA, num_vecs, alpha, x,
			       test_num, max_tests, min_test, max_test);
	  testvec_Destroy (x);
	}
    }
  return test_num;
}

static int
TestAlpha (const oski_matrix_t A0, const oski_matrix_t A1,
	   oski_index_t m, oski_index_t n, oski_matop_t opA,
	   oski_index_t num_vecs, int test_num, int max_tests, int min_test,
	   int max_test)
{
  int i;
  for (i = 0; i < NUM_ALPHA; i++)
    {
      oski_value_t alpha = TL_alpha[i];
      test_num = TestX (A0, A1, m, n, opA, num_vecs, alpha,
			test_num, max_tests, min_test, max_test);
    }
  return test_num;
}

static int
TestNumVecs (const oski_matrix_t A0, const oski_matrix_t A1,
	     oski_index_t m, oski_index_t n, oski_matop_t opA,
	     int test_num, int max_tests, int min_test, int max_test)
{
  int i;
  for (i = 0; i < NUM_NUM_VECS; i++)
    {
      oski_index_t num_vecs = TL_num_vecs[i];
      test_num = TestAlpha (A0, A1, m, n, opA, num_vecs,
			    test_num, max_tests, min_test, max_test);
    }
  return test_num;
}

static int
TestOp (const oski_matrix_t A0, const oski_matrix_t A1,
	oski_index_t m, oski_index_t n,
	int test_num, int max_tests, int min_test, int max_test)
{
  int i;
  for (i = 0; i < NUM_OP; i++)
    {
      oski_matop_t opA = TL_op[i];
      test_num = TestNumVecs (A0, A1, m, n, opA,
			      test_num, max_tests, min_test, max_test);
    }
  return test_num;
}

static void
check_MatMult (const oski_matrix_t A0, const oski_matrix_t A1,
	       oski_index_t m, oski_index_t n, int min_test, int max_test)
{
  size_t max_tests = NUM_OP * NUM_NUM_VECS * NUM_ALPHA
    * NUM_USE_MINSTRIDE * NUM_ORIENT * NUM_BETA
    * NUM_USE_MINSTRIDE * NUM_ORIENT;
  size_t test_num = 1;

  TestOp (A0, A1, m, n, test_num, max_tests, min_test, max_test);
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
	       "This program tests oski_MatMult() on a sample matrix\n"
	       "pattern stored in Harwell-Boeing formatted file.\n"
	       "\n"
	       "Specifically, the test compares the results of SpMV\n"
	       "when the matrix is stored initially in compressed\n"
	       "sparse column (CSC) format to the results when stored\n"
	       "in the format specified by <xform_program>, a\n"
	       "OSKI-Lua transformation program.\n"
	       "\n"
	       "The test tries to exercise many possible values of\n"
	       "the following parameters:\n"
	       "\t- Multiplying by A, A^T, and A^H.\n"
	       "\t- Values of alpha, beta.\n"
	       "\t- Row vs. column major layout of x and y\n"
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

  oski_PrintDebugMessage (1, "... Checking matrix-vector multiply ...");
  check_MatMult (A_input, A_tunable, m, n, min_test, max_test);

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
