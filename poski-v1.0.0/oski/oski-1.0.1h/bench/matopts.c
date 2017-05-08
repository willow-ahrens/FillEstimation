/**
 *  \file bench/matopts.c
 *  \brief 
 *  \ingroup OFFBENCH
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <oski/oski.h>

#include "abort_prog.h"
#include "readhbpat.h"
#include "testvec.h"
#include "parse_opts.h"
#include "matopts.h"

void
matopts_Init (matopts_t * opts)
{
  if (opts == NULL)
    return;
  opts->mattype = MATTYPE_DENSE;
  opts->matfile = NULL;
  opts->expand_symm = 0;
  opts->min_rows = 1;
  opts->min_cols = 1;
  opts->min_row_nnz = 60;
  opts->min_nnz = 5000000;
  opts->row_multiple = 1;
  opts->col_multiple = 1;
}

void
matopts_Display (const matopts_t * opts, FILE * fp)
{
  if (fp == NULL || opts == NULL)
    return;

  if (IS_MATTYPE (opts->mattype, MATTYPE_HARWELL_BOEING))
    {
      fprintf (fp, "\tMatrix type: Harwell-Boeing\n\tMatrix file: %s\n",
               opts->matfile);
      fprintf (fp, "\tExpand symmetric matrices to non-symmetric format? %s\n",
               opts->expand_symm ? "Yes" : "No");
    }
  else
    fprintf (fp, "\tMatrix type: %s\n", opts->mattype);
  fprintf (fp, "\tMin. no. of rows: %ld\n", (long) opts->min_rows);
  fprintf (fp, "\tMin. no. of columns: %ld\n", (long) opts->min_cols);
  fprintf (fp, "\tMin. no. of non-zeros per row: %ld\n",
	   (long) opts->min_row_nnz);
  fprintf (fp, "\tMin. no. of non-zeros: %ld\n", (long) opts->min_nnz);
  fprintf (fp, "\tRow dimension will be a multiple of %ld\n",
	   (long) opts->row_multiple);
  fprintf (fp, "\tColumn dimension will be a multiple of %ld\n",
	   (long) opts->col_multiple);
}

void
matopts_Usage (FILE * fp)
{
  if (fp == NULL)
    return;
  fprintf (fp,
	   "   --matrix <matrix-type> [type-specific-options]\n"
	   "      The kind of matrix to use for benchmarking:\n"
	   "         %s <filename>   Harwell-Boeing matrix\n"
	   "         %s           Dense blocked matrix [DEFAULT]\n"
	   "         %s            Blocked and banded matrix\n"
	   "         %s          Random\n"
	   "         %s          Lower triangular\n"
	   "         %s           Upper triangular\n"
	   "         %s            Symmetric dense\n"
	   "         %s        Random rectangular (short and fat)\n"
	   "  --row-multiple <r>     Make row count a multiple of <r>\n"
	   "  --col-multiple <c>     Make column count a multiple of <c>\n"
	   "  --min-nnz <count>      Minimum no. of non-zeros\n"
	   "  --min-row-nnz <count>  Minimum no. of non-zeros per row\n"
	   "  --min-rows <count>     Minimum no. of rows\n"
	   "  --min-cols <count>     Minimum no. of columns\n"
           "  --expand-symm | --no-expand-symm\n"
           "      Whether to expand symmetric matrices to non-symmetric format.\n"
	   "\n",
	   MATTYPE_HARWELL_BOEING,
	   MATTYPE_DENSE,
	   MATTYPE_BANDED,
	   MATTYPE_RANDOM,
	   MATTYPE_TRI_LOWER,
	   MATTYPE_TRI_UPPER, MATTYPE_SYMM, MATTYPE_RANDRECT);
}

int
matopts_Process (int argc, char *argv[], int i, matopts_t * opts)
{
  int i_orig = i;
  if (StringMatches (argv[i], "--row-multiple", NULL) && (i + 1) < argc)
    {
      i++;
      opts->row_multiple = atoi (argv[i]);
    }
  else if (StringMatches (argv[i], "--col-multiple", NULL) && (i + 1) < argc)
    {
      i++;
      opts->col_multiple = atoi (argv[i]);
    }
  else if (StringMatches (argv[i], "--min-nnz", NULL) && (i + 1) < argc)
    {
      i++;
      opts->min_nnz = atoi (argv[i]);
    }
  else if (StringMatches (argv[i], "--min-row-nnz", NULL) && (i + 1) < argc)
    {
      i++;
      opts->min_row_nnz = atoi (argv[i]);
    }
  else if (StringMatches (argv[i], "--min-rows", NULL) && (i + 1) < argc)
    {
      i++;
      opts->min_rows = atoi (argv[i]);
    }
  else if (StringMatches (argv[i], "--min-cols", NULL) && (i + 1) < argc)
    {
      i++;
      opts->min_cols = atoi (argv[i]);
    }
  else if (StringMatches (argv[i], "--matrix", NULL) && (i + 1) < argc)
    {
      i++;
      if (IS_MATTYPE (argv[i], "hb") && (i + 1) < argc)
	{
	  i++;
	  opts->mattype = MATTYPE_HARWELL_BOEING;
	  opts->matfile = argv[i];
	}
      else if (StringMatches (argv[i],
			      MATTYPE_DENSE,
			      MATTYPE_BANDED,
			      MATTYPE_RANDOM,
			      MATTYPE_TRI_LOWER,
			      MATTYPE_TRI_UPPER,
			      MATTYPE_SYMM, MATTYPE_RANDRECT, NULL))
	{
	  opts->mattype = argv[i];
	}
      else
	{			/* bad matrix type? */
	  fprintf (stderr, "*** Unknown matrix type, '%s'. ***\n", argv[i]);
	  return i_orig - 1;
	}
    }
  else if (StringMatches (argv[i], "--expand-symm", NULL))
    opts->expand_symm = 1;
  else if (StringMatches (argv[i], "--no-expand-symm", NULL))
    opts->expand_symm = 0;
  else
    {				/* unrecognized */
      return i_orig;
    }

  /* processed at least one option */
  return i + 1;
}

#define MAX(a, b)  ((a) > (b) ? (a) : (b))
#define MIN(a, b)  ((a) < (b) ? (a) : (b))

/** Returns the least multiple of \f$a\f$ that is also at least \f$x\f$ */
#define MIN_LEAST_MULTIPLE(x, a)  ((((x) + (a) - 1)/(a))*(a))

oski_matrix_t
matopts_CreateMat (const matopts_t * opts,
		   oski_index_t * p_num_rows, oski_index_t * p_num_cols,
		   oski_index_t * p_nnz_stored, oski_index_t * p_nnz)
{
  /* Return values */
  oski_matrix_t A = INVALID_MAT;
  oski_index_t m = 0;		/* No. of rows */
  oski_index_t n = 0;		/* No. of cols */
  oski_index_t k = 0;		/* No. of logical non-zeros */
  oski_index_t k_stored = 0;	/* No. of non-zeros stored */

  /* Re-interpreted constraints: */
  oski_index_t m_min;		/* Min. no. of rows */
  oski_index_t n_min;		/* Min. no. of rows */
  oski_index_t k_min;		/* Min. no. of non-zeros */
  oski_index_t k_row_min;	/* Min. no. of non-zeros per row */
  oski_index_t r;		/* row multiple */
  oski_index_t c;		/* col multiple */

  /* Try to come up with minimally mutually consistent constraints */
  r = MAX (opts->row_multiple, 1);
  c = MAX (opts->col_multiple, 1);
  k_row_min = opts->min_row_nnz;
  m_min = MAX (opts->min_rows, r);
  n_min = MAX (opts->min_cols, c);
  n_min = MAX (n_min, k_row_min);
  k_min = MAX (opts->min_nnz, m_min * k_row_min);

  /* The following 'switch' statement initializes m, n, k, k_stored,
   * and A according to the selected matrix type, opts->mattype.
   */
  /* ----------------------------------------------------------- */
  if (IS_MATTYPE (opts->mattype, MATTYPE_HARWELL_BOEING))
    {
      /* Note: Ignores constraints, since matrix comes from a file. */
      char mattype[4];
      A = readhb_pattern_matrix_nnz (opts->matfile, &m, &n, &k_stored, &k,
				     mattype, opts->expand_symm);
    }
  /* ----------------------------------------------------------- */
  else if (IS_MATTYPE (opts->mattype, MATTYPE_DENSE))
    {
      oski_index_t *ptr;
      oski_index_t *ind;
      oski_value_t *val;
      if (m_min * n_min >= k_min)
	{
	  m = m_min;
	  n = n_min;
	}
      else
	{			/* m_min*n_min < k_min */
	  m = n = (oski_index_t) (sqrt ((double) k_min));
	}

      /* Make (m, n) least multiples of (r, c) that are at least (m, n) */
      m = MIN_LEAST_MULTIPLE (m, r);
      n = MIN_LEAST_MULTIPLE (n, c);
      k = k_stored = m * n;

      /* Create dense matrix */
      testmat_GenDenseCSR (m, n, &ptr, &ind, &val);
      A = oski_CreateMatCSR (ptr, ind, val, m, n, COPY_INPUTMAT, 5,
			     MAT_GENERAL, MAT_DIAG_EXPLICIT,
			     INDEX_ZERO_BASED, INDEX_SORTED, INDEX_UNIQUE);
      oski_FreeAll (3, ptr, ind, val);
    }
  /* ----------------------------------------------------------- */
  else if (IS_MATTYPE (opts->mattype, MATTYPE_BANDED))
    {
      oski_index_t *ptr;
      oski_index_t *ind;
      oski_value_t *val;
      testmat_GenBlockedBandedCSR (k_min, k_row_min, r, c,
				   &n, &ptr, &ind, &val);
      m = n;
      k = k_stored = ptr[m];
      A = oski_CreateMatCSR (ptr, ind, val, m, n, COPY_INPUTMAT, 5,
			     MAT_GENERAL, MAT_DIAG_EXPLICIT,
			     INDEX_ZERO_BASED, INDEX_SORTED, INDEX_UNIQUE);
      oski_FreeAll (3, ptr, ind, val);
    }
  /* ----------------------------------------------------------- */
  else if (IS_MATTYPE (opts->mattype, MATTYPE_RANDOM))
    {
      oski_index_t *ptr;
      m = MAX (k_min / k_row_min, 1);
      m = MIN_LEAST_MULTIPLE (m, r);
      n = MIN_LEAST_MULTIPLE (m, c);
      A = testmat_GenRandomCSR (m, n, k_row_min, MIN (2 * k_row_min, n),
				MAT_GENERAL, 0, 0,
				COPY_INPUTMAT, &ptr, NULL, NULL);
      k = k_stored = ptr[m];
      oski_Free (ptr);
    }
  /* ----------------------------------------------------------- */
  else if (IS_MATTYPE (opts->mattype, MATTYPE_TRI_LOWER)
	   || IS_MATTYPE (opts->mattype, MATTYPE_TRI_UPPER)
	   || IS_MATTYPE (opts->mattype, MATTYPE_SYMM))
    {

      int is_lower = IS_MATTYPE (opts->mattype, MATTYPE_TRI_LOWER);
      oski_inmatprop_t shape;
      oski_index_t *ptr;
      oski_index_t *ind;
      oski_value_t *val;

      n = MAX (m_min, n_min);
      if ((n * n / 2) < k_min)
	{
	  n = (oski_index_t) (sqrt ((double) k_min * 2));
	}

      n = MIN_LEAST_MULTIPLE (n, r * c);
      m = n;

      /* Create dense matrix */
      testmat_GenDenseCSR (n, n, &ptr, &ind, &val);
      testmat_ChangeSizeDenseTriCSR (n, is_lower, 1, ptr, ind, val);

      if (IS_MATTYPE (opts->mattype, MATTYPE_TRI_LOWER))
	shape = MAT_TRI_LOWER;
      else if (IS_MATTYPE (opts->mattype, MATTYPE_TRI_UPPER))
	shape = MAT_TRI_UPPER;
      else
	shape = MAT_SYMM_UPPER;

      A = oski_CreateMatCSR (ptr, ind, val, n, n, COPY_INPUTMAT, 5,
			     shape, MAT_DIAG_EXPLICIT,
			     INDEX_ZERO_BASED, INDEX_SORTED, INDEX_UNIQUE);
      k_stored = ptr[n];
      k =
	IS_MATTYPE (opts->mattype,
		    MATTYPE_SYMM) ? (k_stored * 2 - n) : k_stored;
      oski_FreeAll (3, ptr, ind, val);
    }
  /* ----------------------------------------------------------- */
  else if (IS_MATTYPE (opts->mattype, MATTYPE_RANDRECT))
    {
      oski_index_t *ptr;
      oski_index_t k_row;
      m = MAX (100, m_min);
      n = MAX (8000000, n_min);
      m = MIN_LEAST_MULTIPLE (m, r);
      n = MIN_LEAST_MULTIPLE (n, c);
      k_row = MAX (k_row_min, k_min / m);
      A = testmat_GenRandomCSR (m, n, k_row, MIN (2 * k_row, n),
				MAT_GENERAL, 0, 0,
				COPY_INPUTMAT, &ptr, NULL, NULL);
      k = k_stored = ptr[m];
      oski_Free (ptr);
    }
  /* ----------------------------------------------------------- */

  if (p_num_rows != NULL)
    *p_num_rows = m;
  if (p_num_cols != NULL)
    *p_num_cols = n;
  if (p_nnz_stored != NULL)
    *p_nnz_stored = k_stored;
  if (p_nnz != NULL)
    *p_nnz = k;
  return A;
}

/* eof */
