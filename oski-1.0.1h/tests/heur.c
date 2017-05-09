/**
 *  \file tests/heur.c
 *  \brief Test sparse matrix-vector multiply tuning.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/matcreate.h>
#include <oski/hint.h>
#include <oski/tune.h>

#include "array_util.h"

/*
 *  User's initial data:
 *
 *       /  1     0     0  \          / .25 \            / 1 \
 *  A = |  -2     1     0   | ,  x = |  .45  |  , y_0 = |  1  |
 *       \  .5    0     1  /          \ .65 /            \ 1 /
 *
 *  A is a sparse lower triangular matrix with a unit diagonal,
 *  stored implicitly. This representation uses 0-based indices.
 */
static oski_index_t Aptr[] = { 0, 0, 1, 2 };
static oski_index_t Aind[] = { 0, 0 };

static oski_value_t Aval[] = {
  MAKE_VAL_COMPLEX (-2, 0), MAKE_VAL_COMPLEX (0.5, 0)
};

static oski_value_t x[] = {
  MAKE_VAL_COMPLEX (.25, 0),
  MAKE_VAL_COMPLEX (.45, 0),
  MAKE_VAL_COMPLEX (.65, 0)
};
static oski_value_t y[] =
  { MAKE_VAL_COMPLEX (1, 0), MAKE_VAL_COMPLEX (1, 0), MAKE_VAL_COMPLEX (1,
									0) };

static oski_value_t alpha = MAKE_VAL_COMPLEX (-1, 0);
static oski_value_t beta = MAKE_VAL_COMPLEX (1, 0);

/* Solution vector */
static oski_value_t y_true[] = {
  MAKE_VAL_COMPLEX (.75, 0),
  MAKE_VAL_COMPLEX (1.05, 0),
  MAKE_VAL_COMPLEX (.225, 0)
};

/* ----------------------------------------------------------------- */
static oski_matrix_t
create_matrix (void)
{
  oski_matrix_t A_tunable = oski_CreateMatCSR (Aptr, Aind, Aval, 3, 3,	/* CSR arrays */
					       SHARE_INPUTMAT,	/* Copy mode */
					       /* non-zero pattern semantics */
					       3, INDEX_ZERO_BASED,
					       MAT_TRI_LOWER,
					       MAT_UNIT_DIAG_IMPLICIT);

  if (A_tunable == INVALID_MAT)
    exit (1);

  return A_tunable;
}

static oski_vecview_t
create_x (void)
{
  oski_vecview_t x_view = oski_CreateVecView (x, 3, STRIDE_UNIT);
  if (x_view == INVALID_VEC)
    exit (1);
  return x_view;
}

static oski_vecview_t
create_y (void)
{
  oski_vecview_t y_view = oski_CreateVecView (y, 3, STRIDE_UNIT);
  if (y_view == INVALID_VEC)
    exit (1);
  return y_view;
}

static void
test_should_pass_1 (int num_times)
{
  int err;

  /* Create a tunable sparse matrix object. */
  oski_matrix_t A_tunable = create_matrix ();
  oski_vecview_t x_view = create_x ();
  oski_vecview_t y_view = create_y ();

  int i;

  err = oski_SetHintMatMult (A_tunable, OP_NORMAL, alpha, x_view,
			     beta, y_view, num_times);
  if (err)
    exit (1);

  err = oski_TuneMat (A_tunable);
  switch (err)
    {
    case TUNESTAT_AS_IS:
      printf ("Did NOT tune.\n");
      break;

    case TUNESTAT_NEW:
      printf ("Did tune.\n");
      break;

    default:			/* error */
      exit (1);
    }

  /* One execution to verify correctness. */
  err = oski_MatMult (A_tunable, OP_NORMAL, alpha, x_view, beta, y_view);
  if (err)
    exit (1);

  /* check answer */
  array_CompareElems ("test_should_pass_1", y_true, y_view->val, 3);

  /* Now execute num_times more times, just to compare observed time and
   * time estimate. */
  for (i = 0; i < num_times; i++)
    {
      err = oski_MatMult (A_tunable, OP_NORMAL, alpha, x_view, beta, y_view);
      if (err)
	exit (1);
    }

  oski_DestroyMat (A_tunable);
  oski_DestroyVecView (x_view);
  oski_DestroyVecView (y_view);
}

static void
tests_should_pass (int num_times)
{
  oski_PrintDebugMessage (1,
			  "+---------------------------------------------+");
  oski_PrintDebugMessage (1,
			  "| Checking cases which should definitely pass |");
  oski_PrintDebugMessage (1,
			  "+---------------------------------------------+");

  test_should_pass_1 (num_times);
}

int
main (int argc, char *argv[])
{
  int num_times = 0;

  /* Initialize library; will happen automatically eventually ... */
  if (!oski_Init ())
    return 1;

  if (argc >= 2)
    {
      num_times = atoi (argv[1]);
      oski_PrintDebugMessage (1, "Tuning in anticipation of %d calls.",
			      num_times);
    }
  else
    oski_PrintDebugMessage (1, "usage: %s <num_times>", argv[0]);

  if (num_times)
    tests_should_pass (num_times);

  oski_Close ();
  return 0;
}

/* eof */
