/**
 *  \file tests/vecview.c
 *  \brief Test matrix handle creation.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <oski/common.h>
#include <oski/vecview.h>

static void
test_should_fail_1 (void)
{
  oski_value_t x[] = { .1, .2, .3, .4, .5, .6, .7, .8, .9, 1.0 };
  oski_vecview_t x_view;

  oski_PrintDebugMessage (1, "-- FAIL CASE: 1a");
  x_view = oski_CreateVecView (NULL, 10, 1);
  if (x_view != INVALID_VEC)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 1b");
  x_view = oski_CreateVecView (x, -1, 1);
  if (x_view != INVALID_VEC)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 1c");
  x_view = oski_CreateVecView (x, 10, -1);
  if (x_view != INVALID_VEC)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 1d");
  x_view = oski_CreateVecView (x, 10, 0);
  if (x_view != INVALID_VEC)
    exit (1);
}

static void
test_should_fail_2 (void)
{
  oski_value_t x[] = { .1, .2, .3, .4, .5, .6, .7, .8, .9, 1.0 };
  oski_vecview_t x_view;

  /*
   *  The correct call:
   *
   *  x_view = oski_CreateMultiVecView( x, 5, 2, LAYOUT_ROWMAJ, 2 );
   */

  oski_PrintDebugMessage (1, "-- FAIL CASE: 2a");
  x_view = oski_CreateMultiVecView (NULL, 5, 2, LAYOUT_ROWMAJ, 2);
  if (x_view != INVALID_VEC)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 2b");
  x_view = oski_CreateMultiVecView (x, -1, 2, LAYOUT_ROWMAJ, 2);
  if (x_view != INVALID_VEC)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 2c");
  x_view = oski_CreateMultiVecView (x, 5, -1, LAYOUT_ROWMAJ, 2);
  if (x_view != INVALID_VEC)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 2d");
  x_view = oski_CreateMultiVecView (x, 5, 2, -1, 2);
  if (x_view != INVALID_VEC)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 2e");
  x_view = oski_CreateMultiVecView (x, 5, 2, LAYOUT_ROWMAJ, -1);
  if (x_view != INVALID_VEC)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 2f");
  x_view = oski_CreateMultiVecView (x, 5, 2, LAYOUT_ROWMAJ, 1);
  if (x_view != INVALID_VEC)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 2g");
  x_view = oski_CreateMultiVecView (x, 5, 2, LAYOUT_COLMAJ, 4);
  if (x_view != INVALID_VEC)
    exit (1);
}

static void
tests_should_fail (void)
{
  oski_PrintDebugMessage (1,
			  "+---------------------------------------------+");
  oski_PrintDebugMessage (1,
			  "| Checking cases which should definitely fail |");
  oski_PrintDebugMessage (1,
			  "+---------------------------------------------+");


  test_should_fail_1 ();
  test_should_fail_2 ();
}

static void
test_should_pass_1 (void)
{
  oski_value_t x[] = { .1, .2, .3, .4, .5, .6, .7, .8, .9, 1.0, 1.1, 1.2 };
  oski_vecview_t x_view;
  int err;

  x_view = oski_CreateMultiVecView (x, 4, 2, LAYOUT_ROWMAJ, 3);
  if (x_view == NULL)
    exit (1);

  oski_PrintDebugVecView (x_view, NULL);

  err = oski_DestroyVecView (x_view);
  if (err != 0)
    exit (1);
}

static void
test_should_pass_2 (void)
{
  oski_value_t x[] = { .1, .2, .3, .4, .5, .6, .7, .8, .9, 1.0, 1.1, 1.2 };
  oski_vecview_t x_view;
  int err;

  x_view = oski_CreateMultiVecView (x, 3, 3, LAYOUT_COLMAJ, 4);
  if (x_view == NULL)
    exit (1);

  oski_PrintDebugVecView (x_view, NULL);

  err = oski_DestroyVecView (x_view);
  if (err != 0)
    exit (1);
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
  test_should_pass_2 ();
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
