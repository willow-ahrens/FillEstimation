/**
 *  \file tests/ata.c
 *  \brief Test sparse matrix-vector multiply.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <oski/common.h>
#include <oski/matcreate.h>
#include <oski/matmult.h>

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
static oski_value_t Aval[] =
  { MAKE_VAL_COMPLEX (-2, 0), MAKE_VAL_COMPLEX (0.5, 0) };
static oski_value_t x[] = {
  MAKE_VAL_COMPLEX (.25, 0),
  MAKE_VAL_COMPLEX (.45, 0),
  MAKE_VAL_COMPLEX (.65, 0)
};
static oski_value_t y[] = {
  MAKE_VAL_COMPLEX (1, 0),
  MAKE_VAL_COMPLEX (1, 0),
  MAKE_VAL_COMPLEX (1, 0)
};
static oski_value_t t[] = {
  MAKE_VAL_COMPLEX (-1, 0),
  MAKE_VAL_COMPLEX (-1, 0),
  MAKE_VAL_COMPLEX (-1, 0)
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
create_vector (oski_value_t * src, oski_index_t len)
{
  oski_value_t *copy = array_Duplicate (src, len);
  oski_vecview_t x_view = oski_CreateVecView (copy, len, STRIDE_UNIT);
  if (x_view == INVALID_VEC)
    exit (1);
  return x_view;
}

static void
test_should_pass_1 (void)
{
  int err;

  /* Create a tunable sparse matrix object. */
  oski_matrix_t A_tunable = create_matrix ();

  /* Create a common source vector */
  oski_vecview_t x0 = create_vector (x, 3);

  /* Stores y <-- beta*y + alpha*A^T*A*x, t = A*x
   * using building block SpMV */
  oski_vecview_t y0 = create_vector (y, 3);
  oski_vecview_t t0 = create_vector (t, 3);

  /* Store y <-- beta*y + alpha*A^T*A*x, t = A*x,
   * using special kernel call */
  oski_vecview_t y1 = create_vector (y, 3);
  oski_vecview_t t1 = create_vector (t, 3);

  oski_value_t alpha = MAKE_VAL_COMPLEX (1.0, 0.0);
  oski_value_t beta = MAKE_VAL_COMPLEX (1.0, 0.0);

  /* Method 1 */
  err = oski_MatMult (A_tunable, OP_NORMAL, TVAL_ONE, x0, TVAL_ZERO, t0);
  if (err)
    exit (1);
  err = oski_MatMult (A_tunable, OP_TRANS, alpha, t0, beta, y0);
  if (err)
    exit (1);

  /* Method 2 */
  err = oski_MatTransMatMult (A_tunable, OP_AT_A, alpha, x0, beta, y1, t1);

  array_CompareElems ("T", t0->val, t1->val, 3);
  array_CompareElems ("Y", y0->val, y1->val, 3);

  oski_DestroyMat (A_tunable);
  oski_Free (x0->val);
  oski_DestroyVecView (x0);

  oski_Free (y0->val);
  oski_DestroyVecView (y0);
  oski_Free (t0->val);
  oski_DestroyVecView (t0);

  oski_Free (y1->val);
  oski_DestroyVecView (y1);
  oski_Free (t1->val);
  oski_DestroyVecView (t1);
}

static void
tests_should_pass (void)
{
  oski_PrintDebugMessage (1,
			  "+---------------------------------------------+");
  oski_PrintDebugMessage (1,
			  "| Checking cases which should definitely pass |");
  oski_PrintDebugMessage (1,
			  "+---------------------------------------------+");

  test_should_pass_1 ();
}

int
main (int argc, char *argv[])
{
  /* Initialize library; will happen automatically eventually ... */
  if (!oski_Init ())
    return 1;

  tests_should_pass ();

  oski_Close ();
  return 0;
}

/* eof */
