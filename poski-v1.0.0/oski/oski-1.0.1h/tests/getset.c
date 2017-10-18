/**
 *  \file tests/getset.c
 *  \brief Get/set matrix values.
 *  \ingroup TESTING
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <oski/oski.h>

#include "abort_prog.h"
#include "testvec.h"
#include "rand_util.h"

/** Returns the larger of two input arguments. */
#define MAX(a, b)  ((a) > (b) ? (a) : (b))

/** Returns the smaller of two input arguments. */
#define MIN(a, b)  ((a) < (b) ? (a) : (b))

/**
 *  Scatters the non-zero values in a compressed sparse vector
 *  representation into a dense (full) vector.
 *
 *  For all 0 <= k < len, sets dest[ind[k]-b] = val[k], where b
 *  is the index base for values in ind.
 */
static void
scatter (const oski_index_t * ind, const oski_value_t * val,
	 oski_index_t len, oski_index_t index_base, oski_value_t * dest)
{
  oski_index_t k;
  for (k = 0; k < len; k++)
    {
      oski_index_t j = ind[k] - index_base;
      VAL_SET_ZERO (dest[j]);
    }
  for (k = 0; k < len; k++)
    {
      oski_index_t j = ind[k] - index_base;
      VAL_INC (dest[j], val[k]);
    }
}

/**
 *  Scatters the value '0' into a dense vector from a compressed
 *  sparse index vector.
 *
 *  For all 0 <= k < len, sets dest[ind[k]-b] = 0, where b is
 *  the index base for values in ind.
 */
static void
scatter_zero (const oski_index_t * ind,
	      oski_index_t len, oski_index_t index_base, oski_value_t * dest)
{
  oski_index_t k;
  for (k = 0; k < len; k++)
    {
      oski_index_t j = ind[k] - index_base;
      VAL_SET_ZERO (dest[j]);
    }
}

/**
 *  Looks at the absolute difference in magnitude between two
 *  scalar values, and aborts the program if they exceed
 *  (constant) * (machine epsilon).
 */
static void
check_values (const oski_value_t a, const oski_value_t b,
	      oski_index_t row, oski_index_t col)
{
  double abs_diff;
  const double threshold = VAL_EPS * 1e2;

#if IS_VAL_COMPLEX
  abs_diff = MAX (fabs (_RE (a) - _RE (b)), fabs (_IM (a) - _IM (b)));
#else
  abs_diff = (double) fabs ((a) - (b));
#endif

  if (abs_diff > threshold)
    {
      fprintf (stderr, "**** Error detected ****\n");
#if IS_VAL_COMPLEX
      fprintf (stderr, "A(i, j) = A(%d, %d) = %g+%gi\n",
	       (int) row, (int) col, (double) _RE (a), (double) _IM (a));
      fprintf (stderr, "A_true(i, j) = A_true(%d, %d) = %g+%gi\n",
	       (int) row, (int) col, (double) _RE (b), (double) _IM (b));
#else
      fprintf (stderr, "A(i, j) = A(%d, %d) = %g\n",
	       (int) row, (int) col, (double) (a));
      fprintf (stderr, "A_true(i, j) = A_true(%d, %d) = %g\n",
	       (int) row, (int) col, (double) (b));
#endif
      fprintf (stderr, "--> Max abs component difference = %g > %g\n",
	       abs_diff, threshold);
      ABORT (abs_diff > threshold, check_values, ERR_WRONG_ANS);
    }
}

/**
 *  Check that all the non-zeros physically stored in the raw CSR
 *  representation are correctly returned by oski_GetMatEntry().
 */
static void
compare_row (const oski_matrix_t A, oski_index_t row,
	     const oski_index_t * indp, oski_index_t len,
	     oski_index_t index_base, const oski_value_t * full_row)
{
  oski_index_t k;

  /* First, make sure we can read every non-zero */
  for (k = 0; k < len; k++)
    {
      oski_index_t col;
      oski_value_t aij_true;
      oski_value_t aij;

      col = indp[k] + 1 - index_base;	/* 1-based */

      VAL_ASSIGN (aij_true, full_row[col - 1]);
      aij = oski_GetMatEntry (A, row, col);

      check_values (aij, aij_true, row, col);
    }
}

/**
 *  Given a symmetric or Hermitian matrix for which A(i,j) is stored,
 *  checks that calling oski_GetMatEntry() to retrieve the (j, i)
 *  element returns the correct value.
 */
static void
compare_row_transimage (const oski_matrix_t A, oski_index_t row,
			const oski_index_t * indp, oski_index_t len,
			oski_inmatprop_t shape, oski_index_t index_base,
			const oski_value_t * full_row)
{
  oski_index_t k;
  oski_index_t col;
  oski_value_t aij_true;
  oski_value_t aij;

  if (shape == MAT_SYMM_UPPER || shape == MAT_SYMM_LOWER
      || shape == MAT_SYMM_FULL)
    {
      for (k = 0; k < len; k++)
	{
	  col = indp[k] + 1 - index_base;	/* 1-based */

	  VAL_ASSIGN (aij_true, full_row[col - 1]);
	  aij = oski_GetMatEntry (A, col, row);

	  check_values (aij, aij_true, col, row);
	}
    }
  else if (shape == MAT_HERM_UPPER || shape == MAT_HERM_LOWER
	   || shape == MAT_HERM_FULL)
    {
      for (k = 0; k < len; k++)
	{
	  col = indp[k] + 1 - index_base;	/* 1-based */

	  if (col != row)
            VAL_ASSIGN_CONJ (aij_true, full_row[col - 1]);
	  else
	    VAL_ASSIGN (aij_true, full_row[col - 1]);
	  aij = oski_GetMatEntry (A, col, row);

	  check_values (aij, aij_true, col, row);
	}
    }
}

/**
 *  Randomly selects entries from the specified row of the given
 *  matrix and confirms that they correspond to the correct
 *  raw representation value.
 */
static void
spot_check_row_read (const oski_matrix_t A, oski_index_t row,
		     oski_index_t n, const oski_index_t * indp,
		     oski_index_t len, oski_inmatprop_t shape,
		     oski_index_t index_base, const oski_value_t * full_row)
{
  oski_index_t k;
  oski_index_t min_col = 1;
  oski_index_t max_col = n;

  if (shape == MAT_TRI_UPPER || shape == MAT_SYMM_UPPER
      || shape == MAT_HERM_UPPER)
    min_col = row;
  else if (shape == MAT_TRI_LOWER
	   || shape == MAT_SYMM_LOWER || shape == MAT_HERM_LOWER)
    max_col = row;

  /* Randomly spot check some others */
  for (k = 0; k < 2 * len; k++)
    {
      oski_index_t col;
      oski_value_t aij_true;
      oski_value_t aij;

      col = rand_GenIndex (min_col, max_col);
      VAL_ASSIGN (aij_true, full_row[col - 1]);
      aij = oski_GetMatEntry (A, row, col);

      check_values (aij, aij_true, row, col);
    }
}

/**
 *  Expands a sparse vector to a dense vector representation.
 *
 *  Assumes indp has base given by index_base, but that row
 *  is always 1-based.
 */
static void
expand_full_row (const oski_index_t * indp, const oski_value_t * valp,
		 oski_index_t len, oski_index_t index_base,
		 oski_index_t implicit_diag, oski_index_t row,
		 oski_index_t max_row_len, oski_value_t * full_row)
{
  scatter (indp, valp, len, index_base, full_row);
  if (implicit_diag && row <= max_row_len)
    VAL_SET_ONE (full_row[row - 1]);
}

/**
 *  Zeros the elements of a dense vector (full row) representation
 *  based on the sparse pattern.
 *
 *  Assumes indp has base given by index_base, but that row
 *  is always 1-based.
 */
static void
zero_full_row (const oski_index_t * indp, oski_index_t len,
	       oski_index_t index_base, oski_index_t implicit_diag,
	       oski_index_t row, oski_index_t max_row_len,
	       oski_value_t * full_row)
{
  scatter_zero (indp, len, index_base, full_row);
  if (implicit_diag && row <= max_row_len)
    VAL_SET_ZERO (full_row[row - 1]);
}

/**
 *  Performs a number of correctness tests related to reading
 *  entries in a given row of a matrix and its raw CSR
 *  representation.
 *
 *  full_row is a temporary storage buffer of length at least
 *  'n'. It must be initialized to zero, and on return from
 *  this routine, will be returned to a state of containing
 *  all zeros.
 */
static void
check_case_row_read (const oski_matrix_t A, oski_index_t row,
		     const oski_index_t * ptr, const oski_index_t * ind,
		     const oski_value_t * val, oski_index_t m, oski_index_t n,
		     oski_inmatprop_t shape, int implicit_diag,
		     int index_base, oski_value_t * full_row)
{
  oski_index_t nnz_row;
  const oski_index_t *indp;
  const oski_value_t *valp;

  if (A == INVALID_MAT)
    return;

  indp = ind + ptr[row - 1] - index_base;
  valp = val + ptr[row - 1] - index_base;

  nnz_row = ptr[row] - ptr[row - 1];

  /*  ------------------------------------------------
   *  Read tests
   */

  /* Compute a full-row (dense) representation */
  expand_full_row (indp, valp, nnz_row, index_base, implicit_diag,
		   row, n, full_row);

  /* Check that all raw values can be read */
  compare_row (A, row, indp, nnz_row, index_base, full_row);
  compare_row_transimage (A, row, indp, nnz_row, shape, index_base, full_row);

  /* Check a random mix of zeros and non-zeros are read correctly */
  spot_check_row_read (A, row, n, indp, nnz_row, shape, index_base, full_row);

  /* Reset full row representation */
  zero_full_row (indp, nnz_row, index_base, implicit_diag, row, n, full_row);
}

/**
 *  Checks that all stored values in a given row are changed
 *  correctly.
 *
 *  Leaves new values in full_row.
 */
static void
check_row_write (oski_matrix_t A,
		 oski_matrix_t A_copy, oski_index_t row,
		 const oski_index_t * indp, oski_index_t len,
		 oski_index_t index_base, oski_value_t * full_row)
{
  oski_index_t k;

  /* Write a sequence of new values */
  for (k = 0; k < len; k++)
    {
      oski_index_t col = indp[k] + 1 - index_base;	/* 1-based */
      oski_value_t aij_new = rand_GenValue (-1, 1);
      int err;

      /* Record newly generated value for A(row, col) */
      VAL_ASSIGN (full_row[col - 1], aij_new);

      /* Change corresponding matrix entry */
      err = oski_SetMatEntry (A, row, col, aij_new);
      ABORT (err != 0, check_row_write, err);

      if (A_copy != INVALID_MAT)
	{
	  err = oski_SetMatEntry (A_copy, row, col, aij_new);
	  ABORT (err != 0, check_row_write, err);
	}
    }

  /* Read back these values */
  for (k = 0; k < len; k++)
    {
      oski_index_t col = indp[k] + 1 - index_base;	/* 1-based */
      oski_value_t aij;

      aij = oski_GetMatEntry (A, row, col);
      check_values (full_row[col - 1], aij, row, col);

      if (A_copy != INVALID_MAT)
	{
	  aij = oski_GetMatEntry (A_copy, row, col);
	  check_values (full_row[col - 1], aij, row, col);
	}
    }
}

/**
 *  Performs a number of correctness tests related to modifying
 *  entries in a given row of a matrix and its raw CSR
 *  representation.
 *
 *  full_row is a temporary storage buffer of length at least
 *  'n'. It must be initialized to zero, and on return from
 *  this routine, will be returned to a state of containing
 *  all zeros.
 */
static void
check_case_row_write (oski_matrix_t A,
		      oski_matrix_t A_copy, oski_index_t row,
		      const oski_index_t * ptr, const oski_index_t * ind,
		      oski_index_t m, oski_index_t n, oski_inmatprop_t shape,
		      int implicit_diag, int index_base,
		      oski_value_t * full_row)
{
  oski_index_t nnz_row;
  const oski_index_t *indp;

  indp = ind + ptr[row - 1] - index_base;
  nnz_row = ptr[row] - ptr[row - 1];

  check_row_write (A, A_copy, row, indp, nnz_row, index_base, full_row);

  /* Reset full row */
  zero_full_row (indp, nnz_row, index_base, implicit_diag, row, n, full_row);

  /*
   *  Make sure we can't modify certain properties affected by shape.
   *  Here, we temporarily disable the error handler so no unsettling
   *  messages are printed to the user.
   */
  oski_SetErrorHandler (NULL);
  if (implicit_diag)
    {
      oski_value_t dummy = rand_GenValue (-1, 1);
      int err = oski_SetMatEntry (A, row, row, dummy);
      ABORT (err == 0, check_case_row_write, ERR_WRONG_ANS);
    }

  if (shape == MAT_TRI_LOWER || shape == MAT_TRI_UPPER)
    {
      oski_index_t min_col = 1;
      oski_index_t max_col = n;
      oski_index_t k;

      if (shape == MAT_TRI_LOWER)
	min_col = row + 1;
      if (shape == MAT_TRI_UPPER)
	max_col = row - 1;

      for (k = 0; (k < 10) && (min_col <= max_col); k++)
	{
	  oski_index_t col = rand_GenIndex (min_col, max_col);
	  oski_value_t dummy = rand_GenValue (-1, 1);
	  int err = oski_SetMatEntry (A, row, col, dummy);
	  ABORT (err == 0, check_case_row_write, ERR_WRONG_ANS);
	}
    }
  oski_SetErrorHandler (oski_HandleErrorDefault);
}

static const char *
tostring_shape (oski_inmatprop_t shape)
{
  switch (shape)
    {
    case MAT_GENERAL:
      return "general";
    case MAT_SYMM_UPPER:
      return "symmetric (upper)";
    case MAT_SYMM_LOWER:
      return "symmetric (lower)";
    case MAT_SYMM_FULL:
      return "symmetric (full)";
    case MAT_HERM_UPPER:
      return "Hermitian (upper)";
    case MAT_HERM_LOWER:
      return "Hermitian (lower)";
    case MAT_HERM_FULL:
      return "Hermitian (full)";
    case MAT_TRI_LOWER:
      return "lower-triangular";
    case MAT_TRI_UPPER:
      return "upper-triangular";
    default:
      break;
    }
  return "(unknown)";
}

/**
 *  Given a matrix object and its corresponding raw CSR representation,
 *  executes a battery of tests to exercise oski_GetMatEntry() and
 *  oski_SetMatEntry().
 */
static void
run_read_write_test_battery (oski_matrix_t A, oski_matrix_t A_copy,
			     const oski_index_t * ptr,
			     const oski_index_t * ind, oski_value_t * val,
			     oski_index_t m, oski_index_t n,
			     oski_inmatprop_t shape, int implicit_diag,
			     int index_base)
{
  oski_value_t *full_row;
  oski_index_t i;

  full_row = oski_Malloc (oski_value_t, n);
  ABORT (full_row == NULL, check_case, ERR_OUT_OF_MEMORY);
  oski_ZeroMem (full_row, sizeof (oski_value_t) * n);

  for (i = 1; i <= m; i++)
    {
      check_case_row_read (A, i, ptr, ind, val,
			   m, n, shape, implicit_diag, index_base, full_row);
      check_case_row_read (A_copy, i, ptr, ind, val,
			   m, n, shape, implicit_diag, index_base, full_row);

      check_case_row_write (A, A_copy, i, ptr, ind,
			    m, n, shape, implicit_diag, index_base, full_row);

      /* Check that reading still works */
      check_case_row_read (A, i, ptr, ind, val,
			   m, n, shape, implicit_diag, index_base, full_row);
      check_case_row_read (A_copy, i, ptr, ind, val,
			   m, n, shape, implicit_diag, index_base, full_row);
    }

  oski_Free (full_row);
}

static void
check_case (oski_index_t m, oski_index_t n, oski_index_t max_nnz_row,
	    oski_inmatprop_t shape, int implicit_diag, int index_base,
	    const char *xform)
{
  oski_index_t *ptr;
  oski_index_t *ind;
  oski_value_t *val;

  oski_matrix_t A;
  oski_matrix_t A_copy;

  if (shape != MAT_GENERAL)
    {
      m = (m > n) ? m : n;
      n = m;
    }

  oski_PrintDebugMessage (1,
			  "Checking %d-based %dx%d %s matrix with an %s diagonal ...",
			  index_base, m, n, tostring_shape (shape),
			  implicit_diag ? "implicit unit" : "explicit");

  A = testmat_GenRandomCSR (m, n, 0, max_nnz_row,
			    shape, implicit_diag, index_base,
			    SHARE_INPUTMAT, &ptr, &ind, &val);

/* #define DO_DEBUG */
#if defined(DO_DEBUG)
  {
    oski_index_t i;
    oski_PrintDebugMessage (1, "::: DUMP :::");
    for (i = 0; i < m; i++)
      {
	oski_index_t k;
	oski_PrintDebugMessage (1, "[%d] ", i + 1);
	for (k = ptr[i] - index_base; k < ptr[i + 1] - index_base; k++)
	  {
	    oski_index_t j = ind[k] - index_base;
#if IS_VAL_COMPLEX
	    oski_PrintDebugMessage (1, " <%d>:%g+%gi", j + 1, _RE (val[k]),
				    _IM (val[k]));
#else
	    oski_PrintDebugMessage (1, " <%d>:%g", j + 1, val[k]);
#endif
	  }			/* k */
	oski_PrintDebugMessage (1, "");
      }				/* i */
  }
#endif

  /* Optionally test a matrix in an alternate format */
  if (xform != NULL)
    {
      int err;

      oski_PrintDebugMessage (1, "   Also checking transformed copy: '%s'...",
			      xform);

      A_copy = oski_CopyMat (A);
      ABORT (A_copy == INVALID_MAT, check_case, ERR_OUT_OF_MEMORY);
      err = oski_ApplyMatTransforms (A_copy, xform);
      ABORT (err != 0, check_case, ERR_OUT_OF_MEMORY);
    }
  else
    A_copy = INVALID_MAT;

  run_read_write_test_battery (A, A_copy, ptr, ind, val, m, n,
			       shape, implicit_diag, index_base);

  if (xform != NULL && A_copy != INVALID_MAT)
    oski_DestroyMat (A_copy);
  oski_DestroyMat (A);
  oski_Free (val);
  oski_Free (ind);
  oski_Free (ptr);
}

/**
 *  Checks oski_GetMatEntry() and oski_SetMatEntry() for a variety
 *  of randomly generated input matrices.
 */
static void
check_cases (oski_index_t max_rows, oski_index_t max_cols,
	     oski_index_t max_nnz_row, const char *xform)
{
  int implicit_diag;
  for (implicit_diag = 0; implicit_diag <= 1; implicit_diag++)
    {
      int index_base;
      for (index_base = 0; index_base <= 1; index_base++)
	{
	  oski_inmatprop_t shape;
	  for (shape = MAT_GENERAL; shape <= MAT_HERM_FULL; shape++)
	    {
	      check_case (max_rows, max_cols, max_nnz_row,
			  shape, implicit_diag, index_base, xform);
	    }
	}
    }
}

int
main (int argc, char *argv[])
{
  const char *xform;
  oski_index_t m = 0;
  oski_index_t n = 0;
  oski_index_t max_nnz_row = 0;

  ABORT (oski_Init () == 0, main, ERR_INIT_LIB);

  /* Optional transform to test */
  xform = (argc >= 2) ? argv[1] : NULL;
  m = (argc >= 3) ? atoi (argv[2]) : 35;
  n = (argc >= 4) ? atoi (argv[3]) : 52;
  max_nnz_row = (argc >= 5) ? atoi (argv[4]) : 9;

  oski_PrintDebugMessage (1,
			  "... Beginning testing: %d x %d, <= %d nnz/row ...",
			  m, n, max_nnz_row);
  check_cases (m, n, max_nnz_row, xform);

  oski_Close ();
  return 0;
}

/* eof */
