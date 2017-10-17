/**
 *  \file tests/create.c
 *  \brief Test matrix handle creation.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <oski/common.h>
#include <oski/matcreate.h>

static void
test_should_fail_1 (void)
{
	/**
	 * User's initial matrix and data for
	 *
	 *         /  1   0   0  \
	 *  A  =  |  -2   1   0   |
	 *         \ .5   0   1  /
	 *
	 *  This CSR representation has the following properties:
	 *    - 0-based indices
	 *    - Lower triangular
	 *    - Implicit unit diagonal
	 */
  oski_index_t Aptr[] = { 0, 0, 1, 2 };
  oski_index_t Aind[] = { 0, 0 };
  oski_value_t Aval[] = { -2, 0.5 };

	/** Tunable matrix handle */
  oski_matrix_t A_tunable;

  /* Try to create a tunable handle with inconsistent properties */
  oski_PrintDebugMessage (1, "-- FAIL CASE: 1a");
  A_tunable = oski_CreateMatCSR (Aptr, Aind, Aval, 3, 3,	/* CSR arrays */
				 SHARE_INPUTMAT,	/* "copy mode" */
				 4, INDEX_ZERO_BASED, INDEX_ONE_BASED,	/* should cause a failure */
				 MAT_TRI_LOWER, MAT_UNIT_DIAG_IMPLICIT);
  if (A_tunable != NULL)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 1b");
  A_tunable = oski_CreateMatCSR (Aptr, Aind, Aval, 3, 3, SHARE_INPUTMAT, 4, INDEX_ZERO_BASED, MAT_TRI_LOWER, MAT_GENERAL,	/* should cause a failure */
				 MAT_UNIT_DIAG_IMPLICIT);
  if (A_tunable != NULL)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 1c");
  A_tunable = oski_CreateMatCSR (Aptr, Aind, Aval, 3, 3, SHARE_INPUTMAT, 4, INDEX_ZERO_BASED, MAT_TRI_LOWER, MAT_UNIT_DIAG_IMPLICIT, MAT_DIAG_EXPLICIT	/* should cause a failure */
    );
  if (A_tunable != NULL)
    exit (1);
}

static void
test_should_fail_2 (void)
{
	/**
	 * User's initial matrix and data for
	 *
	 *         /  1  -2  .5  \
	 *  A  =  |  -2   1   0   |
	 *         \ .5   0   1  /
	 *
	 *  This CSR representation has the following properties:
	 *    - 0-based indices
	 *    - Symmetric, w/ lower triangular storage
	 *    - Implicit unit diagonal
	 */
  oski_index_t Aptr[] = { 0, 1, 3, 5 };
  oski_index_t Aind[] = { 0, 0, 1, 0, 2 };
  oski_value_t Aval[] = { 1, -2, 1, .5, 1 };

	/** Tunable matrix handle */
  oski_matrix_t A_tunable;

  oski_PrintDebugMessage (1, "-- FAIL CASE: 2a");
  A_tunable = oski_CreateMatCSR (Aptr, Aind, Aval,	/* CSR arrays */
				 3, 3, SHARE_INPUTMAT,	/* copy mode */
				 2, INDEX_ONE_BASED,	/* should fail */
				 MAT_SYMM_LOWER);
  if (A_tunable != NULL)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 2b");
  A_tunable = oski_CreateMatCSR (Aptr, Aind, Aval,	/* CSR arrays */
				 3, 3, SHARE_INPUTMAT,	/* copy mode */
				 2, INDEX_ZERO_BASED, MAT_SYMM_UPPER	/* should fail */
    );
  if (A_tunable != NULL)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 2c");
  A_tunable = oski_CreateMatCSR (Aptr, Aind, Aval,	/* CSR arrays */
				 3, 3, SHARE_INPUTMAT,	/* copy mode */
				 3, INDEX_ZERO_BASED, MAT_SYMM_LOWER, MAT_UNIT_DIAG_IMPLICIT	/* should fail */
    );
  if (A_tunable != NULL)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 2d");
  A_tunable = oski_CreateMatCSR (Aptr, Aind, Aval,	/* CSR arrays */
				 3, 0,	/* should fail */
				 SHARE_INPUTMAT,	/* copy mode */
				 2, INDEX_ZERO_BASED, MAT_SYMM_LOWER);
  if (A_tunable != NULL)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 2e");
  A_tunable = oski_CreateMatCSR (Aptr, Aind, Aval,	/* CSR arrays */
				 -1, 3,	/* should fail */
				 SHARE_INPUTMAT,	/* copy mode */
				 2, INDEX_ZERO_BASED, MAT_SYMM_LOWER);
  if (A_tunable != NULL)
    exit (1);
}

static void
test_should_fail_3 (void)
{
	/**
	 * User's initial matrix and data for
	 *
	 *         /  1   0   0  \
	 *  A  =  |  -2   1   0   |
	 *         \ .5   0   1  /
	 *
	 *  This CSR representation has the following properties:
	 *    - 0-based indices
	 *    - Lower triangular
	 *    - Implicit unit diagonal
	 */
  oski_index_t Aptr[] = { 0, 0, 1, 2 };
  oski_index_t Aind[] = { 0, 0 };
  oski_value_t Aval[] = { -2, 0.5 };

	/** Tunable matrix handle */
  oski_matrix_t A_tunable;

  oski_PrintDebugMessage (1, "-- FAIL CASE: 3a");
  A_tunable = oski_CreateMatCSR (NULL,	/* should fail */
				 Aind, Aval, 3, 3,
				 SHARE_INPUTMAT,
				 3,
				 INDEX_ZERO_BASED,
				 MAT_TRI_LOWER, MAT_UNIT_DIAG_IMPLICIT);
  if (A_tunable != NULL)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 3b");
  A_tunable = oski_CreateMatCSR (Aptr, NULL,	/* should fail */
				 Aval, 3, 3,
				 SHARE_INPUTMAT,
				 3,
				 INDEX_ZERO_BASED,
				 MAT_TRI_LOWER, MAT_UNIT_DIAG_IMPLICIT);
  if (A_tunable != NULL)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 3c");
  A_tunable = oski_CreateMatCSR (Aptr, Aind, NULL,	/* should fail */
				 3, 3,
				 SHARE_INPUTMAT,
				 3,
				 INDEX_ZERO_BASED,
				 MAT_TRI_LOWER, MAT_UNIT_DIAG_IMPLICIT);
  if (A_tunable != NULL)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 3d");
  A_tunable = oski_CreateMatCSR (Aptr, Aind, Aval, 3, 3, -100,	/* should fail */
				 3,
				 INDEX_ZERO_BASED,
				 MAT_TRI_LOWER, MAT_UNIT_DIAG_IMPLICIT);
  if (A_tunable != NULL)
    exit (1);

  oski_PrintDebugMessage (1, "-- FAIL CASE: 3e");
  A_tunable = oski_CreateMatCSR (Aptr, Aind, Aval, 3, 3, SHARE_INPUTMAT, -10,	/* should fail */
				 INDEX_ZERO_BASED,
				 MAT_TRI_LOWER, MAT_UNIT_DIAG_IMPLICIT);
  if (A_tunable != NULL)
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
  test_should_fail_3 ();
}

static void
test_should_pass_1 (void)
{
	/**
	 * User's initial matrix and data for
	 *
	 *         /  1   0   0  \
	 *  A  =  |  -2   1   0   |
	 *         \ .5   0   1  /
	 *
	 *  This CSR representation has the following properties:
	 *    - 0-based indices
	 *    - Lower triangular
	 *    - Implicit unit diagonal
	 */
  oski_index_t Aptr[] = { 0, 0, 1, 2 };
  oski_index_t Aind[] = { 0, 0 };
  oski_value_t Aval[] = { -2, 0.5 };

	/** Tunable matrix handle */
  oski_matrix_t A_tunable;

  /* Create tunable matrix handle */
  oski_PrintDebugMessage (1, "-- PASS CASE: 1a");
  A_tunable = oski_CreateMatCSR (Aptr, Aind, Aval, 3, 3,	/* CSR arrays */
				 SHARE_INPUTMAT,	/* "copy mode" */
				 3, INDEX_ZERO_BASED, MAT_TRI_LOWER,
				 MAT_UNIT_DIAG_IMPLICIT);
  if (A_tunable == NULL)
    exit (1);

  oski_DestroyMat (A_tunable);
}

static void
test_should_pass_2 (void)
{
	/**
	 * User's initial matrix and data for
	 *
	 *         /  1  -2  .5  \
	 *  A  =  |  -2   1   0   |
	 *         \ .5   0   1  /
	 *
	 *  This CSR representation has the following properties:
	 *    - 0-based indices
	 *    - Lower triangular
	 *    - Implicit unit diagonal
	 */
  oski_index_t Aptr[] = { 0, 0, 1, 2 };
  oski_index_t Aind[] = { 0, 0 };
  oski_value_t Aval[] = { -2, 0.5 };

	/** Tunable matrix handle */
  oski_matrix_t A_tunable;

  /* Create tunable matrix handle */
  oski_PrintDebugMessage (1, "-- PASS CASE: 2a");
  A_tunable = oski_CreateMatCSR (Aptr, Aind, Aval, 3, 3,	/* CSR arrays */
				 SHARE_INPUTMAT,	/* "copy mode" */
				 3, INDEX_ZERO_BASED, MAT_SYMM_LOWER,
				 MAT_UNIT_DIAG_IMPLICIT);
  if (A_tunable == NULL)
    exit (1);

  oski_DestroyMat (A_tunable);
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
