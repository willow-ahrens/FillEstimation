/**
 *  \file tests/blas1.c
 *  \brief Test BLAS 1 wrappers.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <oski/common.h>
#include <oski/blas.h>

/* ---------------------------------------------------------
 *  All of the tests below are based on these global vectors.
 *
 *  These vectors are initialized as if the underlying
 *  floating point type is complex-valued. If it is real,
 *  the imaginary part is ignored.
 */

#define X0_LEN 12
oski_value_t g_x0[X0_LEN] = {
  MAKE_VAL_COMPLEX (.1, -.1),
  MAKE_VAL_COMPLEX (.2, -.2),
  MAKE_VAL_COMPLEX (.3, -.3),
  MAKE_VAL_COMPLEX (.4, -.4),
  MAKE_VAL_COMPLEX (.5, -.5),
  MAKE_VAL_COMPLEX (.6, -.6),
  MAKE_VAL_COMPLEX (.7, -.7),
  MAKE_VAL_COMPLEX (.8, -.8),
  MAKE_VAL_COMPLEX (.9, -.9),
  MAKE_VAL_COMPLEX (1.0, -1.0),
  MAKE_VAL_COMPLEX (1.1, -1.1),
  MAKE_VAL_COMPLEX (1.2, -1.2)
};

#define Y0_LEN 12
oski_value_t g_y0[Y0_LEN] = {
  MAKE_VAL_COMPLEX (-.21, .21),
  MAKE_VAL_COMPLEX (-.22, .22),
  MAKE_VAL_COMPLEX (-.23, .23),
  MAKE_VAL_COMPLEX (-.24, .24),
  MAKE_VAL_COMPLEX (-.25, .25),
  MAKE_VAL_COMPLEX (-.26, .26),
  MAKE_VAL_COMPLEX (-.27, .27),
  MAKE_VAL_COMPLEX (-.28, .28),
  MAKE_VAL_COMPLEX (-.29, .29),
  MAKE_VAL_COMPLEX (-.30, .30),
  MAKE_VAL_COMPLEX (-.31, .31),
  MAKE_VAL_COMPLEX (-.32, .32),
};

static oski_value_t *
duplicate_vec (const oski_value_t * x0, oski_index_t len)
{
  oski_value_t *x = oski_Malloc (oski_value_t, len);
  oski_CopyMem (x, x0, oski_value_t, len);
  return x;
}

static void
delete_vec (oski_value_t * x)
{
  oski_Free (x);
}

/* ------------------------------------------------- */

static void
print_vecview (const oski_vecview_t x, const char *message)
{
  oski_SetDebugOutput (stdout);
  oski_PrintDebugMessage (1, message);
  oski_PrintDebugVecView (x, NULL);
  oski_SetDebugOutput (NULL);
}

/* ------------------------------------------------- */

static void
tests_should_fail (void)
{
  /* none */
}

static void
testzero_should_pass (oski_index_t m, oski_index_t n,
		      oski_storage_t orient, oski_index_t stride)
{
  oski_value_t *x;
  oski_vecview_t x_view;
  int err;

  oski_PrintDebugMessage (1, "--- PASS CASE 1: Zeroing out vector ---");

  x = duplicate_vec (g_x0, X0_LEN);

  oski_PrintDebugMessage (1, "... Creating multivector view, x ...");
  x_view = oski_CreateMultiVecView (x, m, n, orient, stride);
  if (x_view == NULL)
    exit (1);

  oski_PrintDebugMessage (1, "... Initial values ...");
  print_vecview (x_view, "BEFORE");

  oski_PrintDebugMessage (1, "... Setting x = 0 ...");
  err = oski_ZeroVecView (x_view);
  if (err != 0)
    exit (1);

  oski_PrintDebugMessage (1, "... Here's the result ...");
  print_vecview (x_view, "AFTER");

  err = oski_DestroyVecView (x_view);
  if (err != 0)
    exit (1);

  delete_vec (x);
}

static void
testscale_should_pass (oski_index_t m, oski_index_t n,
		       oski_storage_t orient, oski_index_t stride)
{
  oski_value_t *x;
  oski_vecview_t x_view;
  int err;

  oski_value_t alpha = MAKE_VAL_COMPLEX (10.0, 1.0);

  oski_PrintDebugMessage (1, "--- PASS CASE 2: Scaling a vector ---");

  x = duplicate_vec (g_x0, X0_LEN);

  oski_PrintDebugMessage (1, "... Creating multivector view, x ...");
  x_view = oski_CreateMultiVecView (x, m, n, orient, stride);
  if (x_view == NULL)
    exit (1);

  oski_PrintDebugMessage (1, "... Initial values ...");
  print_vecview (x_view, "BEFORE");

  oski_PrintDebugMessage (1, "... Setting x = 0 ...");
  err = oski_ScaleVecView (x_view, alpha);
  if (err != 0)
    exit (1);

  oski_PrintDebugMessage (1, "... Here's the result ...");
  print_vecview (x_view, "AFTER");

  err = oski_DestroyVecView (x_view);
  if (err != 0)
    exit (1);

  delete_vec (x);
}

static void
testaxpy_should_pass (oski_index_t m, oski_index_t n,
		      oski_storage_t orient, oski_index_t stride)
{
  oski_value_t *x = duplicate_vec (g_y0, Y0_LEN);
  oski_value_t *y = duplicate_vec (g_x0, X0_LEN);

  oski_vecview_t x_view = oski_CreateMultiVecView (x, m, n, orient, stride);
  oski_vecview_t y_view = oski_CreateMultiVecView (y, m, n, orient, stride);

  oski_value_t alpha = MAKE_VAL_COMPLEX (10.0, 0.0);

  oski_PrintDebugMessage (1, "--- PASS CASE 3: Scaling a vector ---");

  oski_PrintDebugMessage (1, "... x vector ...");
  print_vecview (x_view, "BEFORE");

  oski_PrintDebugMessage (1, "... y vector ...");
  print_vecview (y_view, "BEFORE");

#if IS_VAL_COMPLEX
  oski_PrintDebugMessage (1, "... Computing y <-- y + (%f + %fi)*x ...",
			  _RE (alpha), _IM (alpha));
#else
  oski_PrintDebugMessage (1, "... Computing y <-- y + %f*x ...", alpha);
#endif

  oski_AXPYVecView (x_view, alpha, y_view);

  oski_PrintDebugMessage (1, "... Results ...");
  print_vecview (x_view, "AFTER");
  print_vecview (y_view, "AFTER");
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

  testzero_should_pass (4, 2, LAYOUT_ROWMAJ, 3);
  testzero_should_pass (3, 2, LAYOUT_COLMAJ, 6);
  testscale_should_pass (4, 2, LAYOUT_ROWMAJ, 3);
  testaxpy_should_pass (4, 2, LAYOUT_ROWMAJ, 3);
}

int
main (int argc, char *argv[])
{
  /* Initialize library; will happen automatically eventually ... */
  if (!oski_Init ())
    return 1;

  tests_should_fail ();
  tests_should_pass ();

  oski_Close ();
  return 0;
}

/* eof */
