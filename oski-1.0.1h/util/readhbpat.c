/**
 *  \file util/readhbpat.c
 *  \brief Routine to read the pattern of a file stored in
 *  Harwell-Boeing format.
 *
 *  Adapted from the "dreadhb.c" routine written by Xiaoye Li
 *  <xiaoye@nersc.gov> for SuperLU v2.0.
 *
 *  -- SuperLU routine (version 2.0) --
 *  Univ. of California Berkeley, Xerox Palo Alto Research Center,
 *  and Lawrence Berkeley National Lab.
 *  November 15, 1997
 *
 *  Updated: August 2, 2001 by rich vuduc <richie@cs.berkeley.edu>
 *    -  Added a "filename" parameter to dreadhb.
 *
 *  Updated: November 10, 2002 by rich vuduc <richie@cs.berkeley.edu>
 *    -  Patched dParseIntFormat and dParseFloatFormat to handle
 *       implicit-1s correctly (i.e., "(I7)" --> 1 int of size 7).
 *
 *  Updated: October 2004 by rich vuduc <richie@cs.berkeley.edu>
 *     -  Changed some routine names.
 *     -  Changed malloc calls to oski_Malloc calls.
 *     -  Changed the return types of routines that always
 *     return the same value to 'void'.
 *     -  Changed dReadVector so that it does *not* convert integers
 *     to 0-based indices.
 *
 *  Updated: November 2004 by rich vuduc <richie@cs.berkeley.edu>
 *     -  Changed main read routine to use size_t parameters instead
 *        of int parameters (for dParseIntFormat, for example).
 */

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/matcreate.h>

#include "array_util.h"
#include "readhbpat.h"

/**
 *  \brief Eat up the rest of the current line.
 *
 *  Reads characters one by one from 'fp' beginning at the current
 *  file position until a new-line ('\\n') or carriage-return ('\\r')
 *  is encountered.
 *
 *  \param[in,out] fp File.
 */
static void
dDumpLine (FILE * fp)
{
  register int c;
  do
    c = fgetc (fp);
  while ((c != '\n') && (c != '\r'));
}

/**
 *  \brief Parse a Fortran-style integer format string.
 *
 *  This routine parses integer format strings given by
 *  the regular expression:
 *
 *     .* \( [0-9]* [Ii] [0-9]+
 *
 *  e.g., "(5I7)" (5 integers, each of maximum width 7),
 *  "(I14junk" (1 integer, of maximum width 14).
 *
 *  \param[in] buf Buffer containing the format.
 *  \param[out] num Number of integers.
 *  \param[out] size String width of integer.
 */
static void
dParseIntFormat (const char *buf, size_t * num, size_t * size)
{
  const char *tmp;
  int t_size;
  int num_int = 0;

  tmp = buf;
  while (*tmp++ != '(');

  *num = 0;
  sscanf (tmp, "%d", &num_int);
  *num = (size_t) num_int;
  if (*num == 0)
    *num = 1;

  while (tolower (*tmp) != 'i')
    ++tmp;
  ++tmp;
  sscanf (tmp, "%d", &t_size);
  *size = (size_t) t_size;
}

/**
 *  \brief Read a stream of formatted integers from an input file.
 *
 *  \param[in,out] fp File.
 *  \param[in] n Total number of integers to read.
 *  \param[out] where Array in which to return the integers read.
 *  \param[in] perline Number of integers per line of the input
 *  file.
 *  \param[in] persize Exact distance (in characters) between
 *  consecutive integer starting locations in the file.
 *
 *  \pre This routine assumes there are at most 99 characters
 *  between newlines within the input file, and that every
 *  line has the exact number 'perline' integers of string-width
 *  persize.
 */
static void
dReadVector (FILE * fp, size_t n,
	     oski_index_t * where, size_t perline, size_t persize)
{
  size_t i;

  oski_PrintDebugMessage (2,
			  "... reading %d-vector (ints; width %d chars, %d per line) ...",
			  (int) n, (int) persize, (int) perline);

  i = 0;
  while (i < n)
    {
      char buf[100];
      size_t j;

      fgets (buf, 100, fp);

      for (j = 0; j < perline && i < n; j++)
	{
	  /* save the char after the current integer */
	  char tmp;

	  tmp = buf[(j + 1) * persize];
	  buf[(j + 1) * persize] = 0;	/* null terminate */
	  where[i] = atoi (&buf[j * persize]);
	  buf[(j + 1) * persize] = tmp;	/* restore the char */

	  i++;
	}
    }
}

/**
 *  \brief
 *
 *  \param[in] filename Name of the Harwell-Boeing file.
 *  \param[out] p_m Number of rows in the matrix.
 *  \param[out] p_n Number of columns in the matrix.
 *  \param[out] p_nnz Number of logical (NOT stored) non-zeros.
 *  \param[out] p_ptr Stores the column pointers.
 *  \param[out] p_ind Stores the row indices.
 *  \param[out] mattype Pointer to a buffer of size 4, used to
 *  return the Harwell-Boeing matrix type (see Note below).
 *
 *  \returns 0 on success, or an error code with a call to
 *  the error handler. If an error occurs, then none of
 *  the output parameters are changed.
 *
 *  \pre The file must be in [RP][RUSH]A format.
 *  Harwell-Boeing format: a line-by-line accounting of a matrix
 *  stored in Harwell-Boeing format.
 * 
 *  -# (A72,A8) 
 *     - Col. 1 - 72:   Title (TITLE) 
 *     - Col. 73 - 80:  Key (KEY) 
 *  -# (5I14) 
 *     - Col. 1 - 14:   Total number of lines excluding header (TOTCRD) 
 *     - Col. 15 - 28:  Number of lines for pointers (PTRCRD) 
 *     - Col. 29 - 42:  Number of lines for row (or variable) indices
 *     (INDCRD) 
 *     - Col. 43 - 56:  Number of lines for numerical values (VALCRD) 
 *     - Col. 57 - 70:  Number of lines for right-hand sides (RHSCRD).
 *     This includes starting guesses and solution vectors if present,
 *     or zero to indicate no right-hand side data are present.
 *  -# (A3, 11X, 4I14) 
 *     - Col. 1 - 3:    Matrix type (see below) (MXTYPE) 
 *     - Col. 15 - 28:  Number of rows (or variables) (NROW) 
 *     - Col. 29 - 42:  Number of columns (or elements) (NCOL) 
 *     - Col. 43 - 56:  Number of row (or variable) indices (NNZERO) 
 *     (equal to number of entries for assembled matrices) 
 *     - Col. 57 - 70  Number of elemental matrix entries (NELTVL) 
 *     (zero in the case of assembled matrices) 
 *  -# (2A16, 2A20) 
 *     - Col. 1 - 16:   Format for pointers (PTRFMT) 
 *     - Col. 17 - 32:  Format for row (or variable) indices (INDFMT) 
 *     - Col. 33 - 52:  Format for numerical values of coefficient
 *     matrix (VALFMT) 
 *     - Col. 53 - 72 Format for numerical values of right-hand
 *     sides (RHSFMT) 
 *  -# (A3, 11X, 2I14) Only present if there are right-hand sides present 
 *     - Col. 1:  Right-hand side type: 
 *     F for full storage or M for same format as matrix 
 *     - Col. 2:  G if a starting vector(s) (Guess) is supplied. (RHSTYP) 
 *     - Col. 3:  X if an exact solution vector(s) is supplied. 
 *     - Col. 15 - 28:  Number of right-hand sides (NRHS) 
 *     - Col. 29 - 42:  Number of row indices (NRHSIX) 
 *     (ignored in case of unassembled matrices) 
 *
 *  \note The three character type field on line 3 describes the matrix
 *  type. The following table lists the permitted values for each
 *  of the three characters. As an example of the type field, 'RSA'
 *  denotes that the matrix is real, symmetric, and assembled. 
 *  -# First character
 *	   - R Real matrix 
 *     - C Complex matrix 
 *     - P Pattern only (no numerical values supplied) 
 *  -# Second character
 *	   - S Symmetric 
 *	   - U Unsymmetric 
 *	   - H Hermitian 
 *	   - Z Skew symmetric 
 *	   - R Rectangular 
 *  -# Third character
 *	   - A Assembled 
 *	   - E Elemental matrices (unassembled) 
 *  The type is returned through the string 'mattype', using all
 *  uppercase letters.
 */
int
readhb_pattern (const char *filename,
		oski_index_t * p_m, oski_index_t * p_n, oski_index_t * p_nnz,
		oski_index_t ** p_ptr, oski_index_t ** p_ind, char *mattype)
{
  register int i, numer_lines = 0, rhscrd = 0;
  int num_rows, num_cols, num_stored_nz;
  int tmp;
  size_t colnum, colsize, rownum, rowsize;
  char buf[100], buf_token[100], key[10];

  oski_index_t *ptr;
  oski_index_t *ind;

  FILE *fp;

  if (p_m == NULL || p_n == NULL || p_nnz == NULL
      || p_ptr == NULL || p_ind == NULL)
    {
      OSKI_ERR (readhb_pattern, ERR_BAD_ARG);
      return ERR_BAD_ARG;
    }

  oski_PrintDebugMessage (1,
			  "Reading sparse matrix pattern from the Harwell-Boeing file, %s",
			  filename);

  fp = fopen (filename, "rt");
  if (fp == NULL)
    {
      OSKI_ERR_FILE_READ (readhb_pattern, filename);
      return ERR_FILE_READ;
    }

  /* Line 1 */
  memset (buf, 0, sizeof (buf));
  fgets (buf, sizeof (buf), fp);
  sscanf (buf, "%8c", key);
  buf[72] = 0;
  key[8] = 0;

  oski_PrintDebugMessage (2, "Matrix: '%s'", buf);
  oski_PrintDebugMessage (2, "Key: '%s'", key);

  /* Line 2 */
  memset (buf, 0, sizeof (buf));
  fgets (buf, sizeof (buf), fp);
  for (i = 0; i < 5; i++)
    {
      strncpy (buf_token, buf + 14*i, 14);
      buf_token[14] = 0;
      tmp = 0;
      sscanf (buf_token, "%d", &tmp);

      if (i == 3)
	numer_lines = tmp;
      if (i == 4 && tmp > 0)
	rhscrd = tmp;
    }

  oski_PrintDebugMessage (2,
			  "Lines with non-zero values (excluding header): %d",
			  numer_lines);
  oski_PrintDebugMessage (2, "Right-hand side lines: %d", rhscrd);

  /* Line 3 */
  memset (buf, 0, sizeof (buf));
  fgets (buf, sizeof (buf), fp);
  sscanf (buf, "%3c", mattype); mattype[3] = 0;

  oski_PrintDebugMessage (2, "Type: '%s'", mattype);

  sscanf (buf + 14, "%d", &num_rows);
  sscanf (buf + 28, "%d", &num_cols);
  sscanf (buf + 42, "%d", &num_stored_nz);
  sscanf (buf + 56, "%d", &tmp);
  if (tmp != 0)
    {
      oski_HandleError (ERR_FILE_READ,
			"Can't read an unassembled matrix.", __FILE__,
			__LINE__, "(%d)", tmp);
      fclose (fp);
      return ERR_FILE_READ;
    }

  oski_PrintDebugMessage (2, "Dimensions: %d x %d", num_rows, num_cols);
  oski_PrintDebugMessage (2, "No. of stored non-zeros: %d", num_stored_nz);

  /* Allocate space for the pattern. */
  ptr = oski_Malloc (oski_index_t, num_cols + 1);
  if (ptr == NULL)
    {
      OSKI_ERR (readhb_pattern, ERR_OUT_OF_MEMORY);
      fclose (fp);
      return ERR_OUT_OF_MEMORY;
    }

  ind = oski_Malloc (oski_index_t, num_stored_nz);
  if (ind == NULL && num_stored_nz > 0)
    {
      OSKI_ERR (readhb_pattern, ERR_OUT_OF_MEMORY);
      oski_Free (ptr);
      fclose (fp);
    }

  /* Line 4: format statement */
  memset (buf, 0, sizeof (buf));
  fgets (buf, sizeof (buf), fp);
  dParseIntFormat (buf, &colnum, &colsize);
  dParseIntFormat (buf + 16, &rownum, &rowsize);

  oski_PrintDebugMessage (2, "Column pointers per line: %d", colnum);
  oski_PrintDebugMessage (2, "Column pointer width: %d", colsize);
  oski_PrintDebugMessage (2, "Row indices per line: %d", rownum);
  oski_PrintDebugMessage (2, "Row index width: %d", rowsize);

  /* Line 5: right-hand side */
  if (rhscrd)
    dDumpLine (fp);		/* skip RHSFMT */

  /* Read row pointers */
  dReadVector (fp, num_cols + 1, ptr, colnum, colsize);
  dReadVector (fp, num_stored_nz, ind, rownum, rowsize);

  fclose (fp);

  /* Return values */
  *p_m = num_rows;
  *p_n = num_cols;
  *p_ptr = ptr;
  *p_ind = ind;

  /* Determine number of logical non-zeros, based on pattern
   * and symmetry considerations. Return this value in
   * *p_nnz.
   */
  switch (toupper (mattype[1]))
    {
    case 'S':
    case 'H':
      {
	int num_diag_entries = 0;
	oski_index_t i;

	/* Count diagonal entries */
	for (i = 0; i < num_cols; i++)
	  {
	    oski_index_t k;
	    for (k = ptr[i]; k < ptr[i + 1]; k++)
	      {
		oski_index_t j = ind[k - 1] - 1;	/* 0-based */
		if (i == j)
		  num_diag_entries++;
	      }
	  }

	*p_nnz = 2 * num_stored_nz - num_diag_entries;
      }
      break;

    default:
      *p_nnz = num_stored_nz;
      break;
    }

  oski_PrintDebugMessage (2,
			  "No. of logical matrix non-zeros: %d",
			  (int) (*p_nnz));

  return 0;
}

/**
 *  \brief Returns 1 <==> the given CSC matrix array pattern
 *  corresponds to that of a lower triangular matrix, and 0
 *  otherwise.
 */
static int
is_csc_lower_tri (oski_index_t n, const oski_index_t * ptr,
		  const oski_index_t * ind, oski_index_t base)
{
  oski_index_t j;
  for (j = 0; j < n; j++)
    {
      oski_index_t k;
      for (k = ptr[j] - base; k < ptr[j + 1] - base; k++)
	{
	  oski_index_t i = ind[k] - base;	/* row index */
	  if (i < j)
	    return 0;
	}
    }
  return 1;
}

/**
 *  \brief Returns 1 <==> the given CSC matrix array pattern
 *  corresponds to that of a upper triangular matrix, and 0
 *  otherwise.
 */
static int
is_csc_upper_tri (oski_index_t n, const oski_index_t * ptr,
		  const oski_index_t * ind, oski_index_t base)
{
  oski_index_t j;
  for (j = 0; j < n; j++)
    {
      oski_index_t k;
      for (k = ptr[j] - base; k < ptr[j + 1] - base; k++)
	{
	  oski_index_t i = ind[k] - base;	/* row index */
	  if (i > j)
	    return 0;
	}
    }
  return 1;
}

/**
 *  \brief Sets the imaginary part of the diagonal elements to 0.
 *  This routine is used primarily to ensure that the diagonal of
 *  a Hermitian matrix is pure real.
 */
static void
ResetDiagImag (oski_index_t m, oski_index_t * ptr, oski_index_t * ind,
	       oski_value_t * val, oski_index_t indbase)
{
#if IS_VAL_COMPLEX
  oski_index_t i;
  for (i = 0; i < m; i++)
    {
      oski_index_t k;
      for (k = ptr[i] - indbase; k < ptr[i + 1] - indbase; k++)
	{
	  oski_index_t j = ind[k] - indbase;	/* 0-based index */
	  if (i == j)
	    {
	      oski_value_t newval = MAKE_VAL_COMPLEX (_RE (val[k]), 0);
	      VAL_ASSIGN (val[k], newval);
	    }
	}
    }
#else /* !IS_VAL_COMPLEX */
  return;
#endif
}

/**
 *  \brief Reduce off-diagonal values by a factor of 2*n
 *  Main purpose of this routine is to make triangular
 *  matrices better-conditioned.
 */
static void
reduce_offdiag_values (oski_index_t n,
		       const oski_index_t * ptr, const oski_index_t * ind,
		       oski_value_t * val, oski_index_t indbase)
{
  oski_index_t i;
  oski_value_t factor = MAKE_VAL_COMPLEX ((double) n * 2, 0);
  for (i = 0; i < n; i++)
    {
      oski_index_t k;
      for (k = ptr[i] - indbase; k < ptr[i + 1] - indbase; k++)
	{
	  oski_index_t j = ind[k] - indbase;
	  if (i != j)
	    {
	      VAL_DIVEQ (val[k], factor);
	    }
	}
    }
}

/** Returns the number of diagonal elements */
static oski_index_t
count_diag_elems (oski_index_t n,
		  const oski_index_t * ptr, const oski_index_t * ind,
		  oski_index_t indbase)
{
  oski_index_t num_diag_elems = 0;
  oski_index_t i;
  for (i = 0; i < n; i++)
    {
      oski_index_t k;
      for (k = ptr[i] - indbase; k < ptr[i + 1] - indbase; k++)
	{
	  num_diag_elems += (i == (ind[k] - indbase));
	}
    }
  return num_diag_elems;
}

oski_matrix_t
readhb_pattern_matrix (const char *matfile,
		       oski_index_t * p_m, oski_index_t * p_n,
		       char *p_mattype, int expand_symm)
{
  oski_index_t nnz_stored = 0, nnz_true = 0;
  return readhb_pattern_matrix_nnz (matfile, p_m, p_n,
				    &nnz_stored, &nnz_true, p_mattype,
                                    expand_symm);
}

/**
 *  \brief
 *
 *  Only the pattern is read from the file. The non-zero values
 *  are replaced by random numbers in the interval (0,1].
 *
 *  \param[in] matfile File to read.
 *  \param[in,out] p_m Pointer to scalar integer in which to return
 *  the number of matrix rows. May set to NULL if this information
 *  is not desired.
 *  \param[in,out] p_n Pointer to scalar integer in which to return
 *  the number of matrix columns. May set to NULL if this information
 *  is not desired.
 *  \param[in,out] p_nnz_stored Pointer to scalar integer in which
 *  to return the number of stored non-zeros.
 *  \param[in,out] p_nnz_true Pointer to scalar integer in which
 *  to return the number of true non-zeros.
 *  \param[in,out] p_mattype String buffer of length >= 3 in which
 *  to store the matrix type. May set to NULL if this information is
 *  not desired. This routine does NOT NULL-terminate mattype on
 *  return.
 *
 *  \returns A tunable matrix handle. On error, aborts the
 *  program with exit code 1, leaving *p_m, *p_n, and p_mattype
 *  unchanged. Only the first 3 characters of p_mattype are
 *  changed, and p_mattype is NOT NULL-terminated by this routine.
 */
oski_matrix_t
readhb_pattern_matrix_nnz (const char *matfile,
			   oski_index_t * p_m, oski_index_t * p_n,
			   oski_index_t * p_nnz_stored,
			   oski_index_t * p_nnz_true, char *p_mattype,
                           int expand_symm)
{
  oski_matrix_t A_tunable = INVALID_MAT;

  int is_symm, is_herm;
  int is_lower_tri, is_upper_tri;
  oski_inmatprop_t pattern;

  /* Matrix parameters */
  char mattype[4];
  oski_index_t m;
  oski_index_t n;
  oski_index_t nnz_stored;
  oski_index_t nnz_true;

  /* Raw CSC array matrix storage */
  oski_index_t *ptr;
  oski_index_t *ind;
  oski_value_t *val;

  char *desc;

  int err;

  oski_PrintDebugMessage (1, "Loading matrix pattern from '%s' ...", matfile);
  err = readhb_pattern (matfile, &m, &n, &nnz_true, &ptr, &ind, mattype);
  if (err)
    exit (1);
  nnz_stored = ptr[n] - 1; /* 'ptr' is 1-based */

  /* Create fictional values */
  val = oski_Malloc (oski_value_t, nnz_stored);
  if (val == NULL)
    {
      OSKI_ERR (read_matrix, ERR_OUT_OF_MEMORY);
      exit (1);
    }
  array_FillRandom (val, nnz_stored);

  /* Check for triangular pattern and fix values if needed */
  is_lower_tri = is_csc_lower_tri (n, ptr, ind, 1);
  is_upper_tri = is_csc_upper_tri (n, ptr, ind, 1);

  if (is_lower_tri)
    oski_PrintDebugMessage (2, "Is lower triangular");
  else if (is_upper_tri)
    oski_PrintDebugMessage (2, "Is upper triangular");
  else
    oski_PrintDebugMessage (2, "Full storage");

  if (is_lower_tri || is_upper_tri)
    reduce_offdiag_values (n, ptr, ind, val, 1);

  pattern = MAT_GENERAL;
  desc = MACRO_TO_STRING (MAT_GENERAL);
  is_symm = is_herm = 0;
  switch (mattype[1])
    {
    case 'S':
      is_symm = 1;
      if (is_lower_tri)
	{
	  pattern = MAT_SYMM_LOWER;
	  desc = MACRO_TO_STRING (MAT_SYMM_LOWER);
	}
      else if (is_upper_tri)
	{
	  pattern = MAT_SYMM_UPPER;
	  desc = MACRO_TO_STRING (MAT_SYMM_UPPER);
	}
      break;

    case 'H':
      is_herm = 1;
      ResetDiagImag (m, ptr, ind, val, 1);
      if (is_lower_tri)
	{
	  pattern = MAT_HERM_LOWER;
	  desc = MACRO_TO_STRING (MAT_HERM_LOWER);
	}
      else if (is_upper_tri)
	{
	  pattern = MAT_HERM_UPPER;
	  desc = MACRO_TO_STRING (MAT_HERM_UPPER);
	}
      break;

    default:
      if (is_lower_tri)
	{
	  pattern = MAT_TRI_LOWER;
	  desc = MACRO_TO_STRING (MAT_TRI_LOWER);
	}
      else if (is_upper_tri)
	{
	  pattern = MAT_TRI_UPPER;
	  desc = MACRO_TO_STRING (MAT_TRI_UPPER);
	}
      break;
    }

  if (expand_symm && (is_symm || is_herm))
    {
      oski_index_t* full_ptr;
      oski_index_t* full_ind;
      oski_value_t* full_val;
      int err = readhb_expand_symm (n, 1, is_herm,
                                    ptr, ind, val,
                                    &full_ptr, &full_ind, &full_val);
      if (err)
        {
          OSKI_ERR (read_matrix, err);
          exit (1);
        }

      oski_Free (ptr);
      oski_Free (ind);
      oski_Free (val);
      ptr = full_ptr;
      ind = full_ind;
      val = full_val;
      nnz_stored = ptr[n] - 1; /* 'ptr' is 1-based */

      if (nnz_stored != nnz_true)
	fprintf (stderr, "*** WARNING: nnz_stored = %d, nnz_true = %d ***\n", nnz_stored, nnz_true);

      if (is_herm)
        {
          pattern = MAT_HERM_FULL;
          desc = MACRO_TO_STRING (MAT_HERM_FULL);
        }
      else
        {
          pattern = MAT_SYMM_FULL;
          desc = MACRO_TO_STRING (MAT_SYMM_FULL);
        }
    }

  A_tunable = oski_CreateMatCSC (ptr, ind, val, m, n,
				 COPY_INPUTMAT, 1, pattern);
  if (A_tunable == NULL)
    exit (1);

  oski_Free (ptr);
  oski_Free (ind);
  oski_Free (val);

  oski_PrintDebugMessage (1, "Finished reading '%s'.", matfile);
  oski_PrintDebugMessage (2, "Shape: %s", desc);

  if (p_m != NULL)
    *p_m = m;
  if (p_n != NULL)
    *p_n = n;
  if (p_nnz_stored != NULL)
    *p_nnz_stored = nnz_stored;
  if (p_nnz_true != NULL)
    *p_nnz_true = nnz_true;
  if (p_mattype != NULL)
    strncpy (p_mattype, mattype, 3);

  return A_tunable;
}

/* eof */
