/**
 *  \file src/vecview.c
 *  \brief Multivector view implementation.
 */

#include <assert.h>
#include <math.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/vecview.h>

/** Statically declared symbolic single vector object. */
static oski_vecstruct_t OBJ_SYMBOLIC_VEC =
MAKE_VECSTRUCT_INIT (0, 1, LAYOUT_COLMAJ, 0, 1, 0, NULL);

/** Statically declared symbolic multivector object. */
static oski_vecstruct_t OBJ_SYMBOLIC_MULTIVEC =
MAKE_VECSTRUCT_INIT (0, 1, LAYOUT_COLMAJ, 0, 1, 0, NULL);

const oski_vecview_t SYMBOLIC_VEC = &OBJ_SYMBOLIC_VEC;
const oski_vecview_t SYMBOLIC_MULTIVEC = &OBJ_SYMBOLIC_MULTIVEC;

/**
 *  \brief Set the redundantly maintained row/column stride parameters.
 *
 *  \param[in,out] vec Partially initialized vector view (see precondition).
 *  \pre vec->orient and vec->stride have been initialized.
 *  \post Sets vec->row_stride and vec->col_stride.
 */
static void
SetRowColIncMultiVecView (oski_vecstruct_t * vec)
{
  if (vec == NULL)
    return;

  if (vec->orient == LAYOUT_ROWMAJ)
    {
      vec->rowinc = vec->stride;
      vec->colinc = 1;
    }
  else if (vec->orient == LAYOUT_COLMAJ)
    {
      vec->rowinc = 1;
      vec->colinc = vec->stride;
    }
  else
    assert (0);
}

/**
 *  \brief Create a multivector view object, but without error
 *  checking.
 *
 *  This routine is equivalent to oski_CreateMultiVecView(), but
 *  without the parameter checking.
 *
 *  \see oski_CreateMultiVecView
 */
static oski_vecview_t
CreateMultiVecView_NoError (oski_value_t * X,
			    oski_index_t length, oski_index_t num_vecs,
			    oski_storage_t orient, oski_index_t stride)
{
  oski_vecstruct_t *new_vec;

  new_vec = oski_Malloc (oski_vecstruct_t, 1);
  if (new_vec != NULL)
    {
      new_vec->num_rows = length;
      new_vec->num_cols = num_vecs;
      new_vec->orient = orient;
      new_vec->stride = stride;
      new_vec->val = X;

      SetRowColIncMultiVecView (new_vec);
    }

  return (oski_vecview_t) new_vec;
}

/**
 *  \brief
 *
 *  \param[in] x Vector view object.
 *  \param[in] val_fmt Format string to use when displaying values.
 *
 *  This format string MUST contain the following fields, in this
 *  order: "%f", "%c". That is, the implementation will print a
 *  single character to the right of the value. If val_mft
 *  is NULL, then the routines uses the default format of
 *  "%5.2f%c".
 *
 *  If oski_value_t is complex, then the format string must
 *  contain "%f", "%f", and "%c", to display the real
 *  and imaginary parts.
 *
 *  The extra fields are used to print special characters
 *  to distinguish between values of the logical matrix, and values
 *  lying in the "padding" when the stride is not equal to the
 *  matrix dimension.
 */
void
oski_PrintDebugVecView (const oski_vecview_t x, const char *val_fmt)
{
  oski_index_t i;

  oski_PrintDebugMessage (1, "Vector view object:");
  if (x == INVALID_VEC)
    {
      oski_PrintDebugMessage (2, "(NULL)");
      return;
    }

  oski_PrintDebugMessage (1, "-- Logical dimensions: %d x %d",
			  x->num_rows, x->num_cols);
  oski_PrintDebugMessage (1, "-- Layout: %s",
			  (x->orient == LAYOUT_ROWMAJ) ? "row major"
			  : (x->orient == LAYOUT_COLMAJ) ? "column major"
			  : "UNKNOWN");
  oski_PrintDebugMessage (1, "-- Stride: %d", (int) x->stride);

  if (val_fmt == NULL)
#if !IS_VAL_COMPLEX
    val_fmt = "%5.2f%c";
#else
    val_fmt = "%5.2f+%5.2fi%c";
#endif

  for (i = 0; i < x->num_rows; i++)
    {
      oski_index_t j;

      for (j = 0; j < x->num_cols; j++)
	{
	  oski_value_t x_ij = VECVIEW_GET (x, i, j);

#if !IS_VAL_COMPLEX
	  oski_PrintDebugMessageShort (1, val_fmt, x_ij, ' ');
#else
	  oski_PrintDebugMessageShort (1, val_fmt,
				       _RE (x_ij), _IM (x_ij), ' ');
#endif
	}

      /* print "extra" elements */
      if (x->orient == LAYOUT_ROWMAJ)
	{
	  while (j < x->stride)
	    {
	      oski_value_t x_ij = VECRM_GET (x->val, i, j, x->stride);
#if !IS_VAL_COMPLEX
	      oski_PrintDebugMessageShort (1, val_fmt, x_ij, '*');
#else
	      oski_PrintDebugMessageShort (1, val_fmt,
					   _RE (x_ij), _IM (x_ij), '*');
#endif
	      j++;
	    }
	}

      oski_PrintDebugMessageShort (1, "\n");
    }

  /* print "extra" elements */
  if (x->orient == LAYOUT_COLMAJ)
    {
      while (i < x->stride)
	{
	  oski_index_t j;
	  for (j = 0; j < x->num_cols; j++)
	    {
	      oski_value_t x_ij = VECCM_GET (x->val, i, j, x->stride);
	      oski_PrintDebugMessageShort (1, val_fmt, x_ij, '*');
	    }

	  oski_PrintDebugMessageShort (1, "\n");
	  i++;
	}
    }

  /* done */
  oski_PrintDebugMessage (1, "(--- end vector view ---)");
}

/**
 *  \brief
 *
 *  \param[in] x Vector data.
 *  \param[in] length Logical vector length, >= 0.
 *  \param[in] inc Physical stride between consecutive elements.
 *  inc must be >= 1.
 *  \returns A vector view object (handle), or INVALID_VEC on
 *  error.
 */
oski_vecview_t
oski_CreateVecView (oski_value_t * x, oski_index_t length, oski_index_t inc)
{
  if ((length != 0) && (x == NULL))
    {
      OSKI_ERR_NULL_ARG (oski_CreateVecView, 1, "vector data");
      return INVALID_VEC;
    }

  if (length < 0)
    {
      OSKI_ERR_SMALL_ARG (oski_CreateVecView, 2, "length", 0);
      return INVALID_VEC;
    }

  if (inc < 1)
    {
      OSKI_ERR_SMALL_ARG (oski_CreateVecView, 3, "stride", 1);
      return INVALID_VEC;
    }

  return CreateMultiVecView_NoError (x, length, 1, LAYOUT_ROWMAJ, inc);
}

/**
 *  \brief
 *
 *  \param[in] X Multivector (matrix) data.
 *  \param[in] length Logical length of each (column) vector, i.e.,
 *  number of logical rows in the matrix.
 *  \param[in] num_vecs Number of vectors, i.e., number of logical
 *  columns in the matrix.
 *  \param[in] orient Row vs. column major layout.
 *  \param[in] stride Leading dimension, i.e., distance between
 *  vectors. stride must be >= length for column-major storage,
 *  or >= num_vecs for row-major storage.
 *  \returns A multivector view object (handle), or INVALID_VEC
 *  on error.
 */
oski_vecview_t
oski_CreateMultiVecView (oski_value_t * X,
			 oski_index_t length, oski_index_t num_vecs,
			 oski_storage_t orient, oski_index_t stride)
{
  /* Check parameters */
  if ((length != 0) && (num_vecs != 0) && (X == NULL))
    {
      OSKI_ERR_NULL_ARG (oski_CreateMultiViewVec, 1, "Multivector data");
      return INVALID_VEC;
    }

  if (length < 0)
    {
      OSKI_ERR_SMALL_ARG (oski_CreateMultiVecView, 2, "length", 0);
      return INVALID_VEC;
    }

  if (num_vecs < 0)
    {
      OSKI_ERR_SMALL_ARG (oski_CreateMultiVecView, 3, "number of vectors", 0);
      return INVALID_VEC;
    }

  switch (orient)
    {
    case LAYOUT_ROWMAJ:
      if (stride < num_vecs)
	{
	  OSKI_ERR_BAD_STRIDE (oski_CreateMultiVecView, 5, stride, num_vecs);
	  return INVALID_VEC;
	}
      break;

    case LAYOUT_COLMAJ:
      if (stride < length)
	{
	  OSKI_ERR_BAD_STRIDE (oski_CreateMultiVecView, 5, stride, length);
	  return INVALID_VEC;
	}
      break;

    default:
      OSKI_ERR_BAD_STORAGE (oski_CreateMultiViewVec, orient);
      return INVALID_VEC;
    }

  return CreateMultiVecView_NoError (X, length, num_vecs, orient, stride);
}

/**
 *  \brief
 *
 *  \param[in,out] x (Multi)vector view to free.
 *  \returns 0 on success, or an error code on error.
 */
int
oski_DestroyVecView (oski_vecview_t x)
{
  oski_Free (x);
  return 0;
}

/**
 *  \brief
 *
 *  \param[in] x (Multi)vector view to duplicate.
 *  \returns A handle for a new multivector view object which
 *  is a duplicate of x, or INVALID_VEC on error.
 */
oski_vecview_t
oski_CopyVecView (const oski_vecview_t x)
{
  if (x == INVALID_VEC)
    return INVALID_VEC;

  return oski_CreateMultiVecView (x->val, x->num_rows, x->num_cols,
				  x->orient, x->stride);
}

/**
 *  \brief
 *
 *  \param[in] x A valid vector view.
 *  \param[in] j Column for which to compute the infinity norm
 *  (in 1-based indices).
 *  \returns Let \f$x_j\f$ denote column \f$j\f$ of \f$x\f$.
 *  Then this routine returns \f$||x_j||_\infty\f$, or -1 on error.
 */
double
oski_CalcVecViewInfNorm (const oski_vecview_t x, oski_index_t j)
{
  const oski_value_t *xp;
  oski_index_t i;
  double inf_norm;

  if (x == INVALID_VEC || x == SYMBOLIC_VEC || x == SYMBOLIC_MULTIVEC)
    {
      OSKI_ERR_BAD_VEC (oski_CalcVecViewInfNorm, 1);
      return -1;
    }

  if (!IS_VAL_IN_RANGE (j, 1, x->num_cols))
    {
      oski_HandleError (ERR_BAD_ARG,
			"Requested column index is invalid.", __FILE__,
			__LINE__,
			"Requested column %d, but vector only has %d columns.",
			(int) j, (int) x->num_cols);
      return -1;
    }

  inf_norm = 0.0;
  for (i = 0, xp = x->val + (j - 1) * x->colinc;
       i < x->num_rows; i++, xp += x->rowinc)
    {
      oski_value_t xi;
      double _xi;

      VAL_ASSIGN (xi, xp[0]);
#if IS_VAL_COMPLEX
      _xi = sqrt ((double) (_RE (xi) * _RE (xi) + _IM (xi) * _IM (xi)));
#else
      _xi = (double) fabs (xi);
#endif

      if (_xi > inf_norm)
	inf_norm = _xi;
    }

  return inf_norm;
}

/**
 *  \brief
 *
 *  \param[in] x0 A valid vector view.
 *  \param[in] j0 Column of x0 to use (1-based).
 *  \param[in] x1 A valid vector view.
 *  \param[in] j1 Column of x1 to use (1-based).
 *
 *  \returns \f$||x0_{j0} - x1_{j1}||_\infty\f$, where
 *  \f$x0_{j0}\f$ is column \f$j0\f$ of \f$x0\f$, and
 *  \f$x1_{j1}\f$ is column \f$j1\f$ of \f$x1\f$.
 *  Returns -1 on error.
 */
double
oski_CalcVecViewInfNormDiff (const oski_vecview_t x0, oski_index_t j0,
			     const oski_vecview_t x1, oski_index_t j1)
{
  const oski_value_t *x0p;
  const oski_value_t *x1p;

  oski_index_t i;
  double inf_norm;

  if (x0 == INVALID_VEC || x0 == SYMBOLIC_VEC || x0 == SYMBOLIC_MULTIVEC)
    {
      OSKI_ERR_BAD_VEC (oski_CalcVecViewInfNormDiff, 1);
      return -1;
    }

  if (x1 == INVALID_VEC || x1 == SYMBOLIC_VEC || x1 == SYMBOLIC_MULTIVEC)
    {
      OSKI_ERR_BAD_VEC (oski_CalcVecViewInfNormDiff, 3);
      return -1;
    }

  if (!IS_VAL_IN_RANGE (j0, 1, x0->num_cols))
    {
      oski_HandleError (ERR_BAD_ARG,
			"Requested column index is invalid.", __FILE__,
			__LINE__,
			"In call to %s(): Requested column %d (parameter #%d),"
			" but vector (parameter #%d) only has %d columns.",
			MACRO_TO_STRING (oski_CalcVecViewInfNormDiff),
			(int) j0, (int) 2, (int) 1, (int) x0->num_cols);
      return -1;
    }

  if (!IS_VAL_IN_RANGE (j1, 1, x1->num_cols))
    {
      oski_HandleError (ERR_BAD_ARG,
			"Requested column index is invalid.", __FILE__,
			__LINE__,
			"In call to %s(): Requested column %d (parameter #%d),"
			" but vector (parameter #%d) only has %d columns.",
			MACRO_TO_STRING (oski_CalcVecViewInfNormDiff),
			(int) j1, (int) 4, (int) 3, (int) x1->num_cols);
      return -1;
    }

  if (x0->num_rows != x1->num_rows)
    {
      oski_HandleError (ERR_BAD_ARG,
			"Vector views have differing numbers of rows",
			__FILE__, __LINE__,
			"In call to %s(): One view (parameter #%d) has %d rows,"
			" while the other (#%d) has %d.",
			MACRO_TO_STRING (oski_CalcVecViewInfNormDiff),
			(int) 1, (int) (x0->num_rows), (int) 3,
			(int) (x1->num_rows));
      return -1;
    }

  inf_norm = 0.0;
  x0p = x0->val + (j0 - 1) * x0->colinc;
  x1p = x1->val + (j1 - 1) * x1->colinc;

  for (i = 0; i < x0->num_rows; i++, x0p += x0->rowinc, x1p += x1->rowinc)
    {
      oski_value_t dx;
      double _dxi;

      /* Compute difference between corresponding
       * elements of x0, x1. */
      VAL_ASSIGN (dx, x0p[0]);
      VAL_DEC (dx, x1p[0]);

#if IS_VAL_COMPLEX
      _dxi = sqrt ((double) (_RE (dx) * _RE (dx) + _IM (dx) * _IM (dx)));
#if 0
      fprintf (stderr, "[%d] %g == |%g+%gi - %g+%gi|\n", i, _dxi,
	       _RE (x0p[0]), _IM (x0p[0]), _RE (x1p[0]), _IM (x1p[0]));
#endif
#else
      _dxi = (double) fabs (dx);
#if 0
      fprintf (stderr, "[%d] %g == |%g - %g|\n", i, _dxi, x0p[0], x1p[0]);
#endif
#endif

      if (_dxi > inf_norm)
	inf_norm = _dxi;
    }

  return inf_norm;
}

/** Min of two side-effect-free values. */
#define MIN(a, b)  ((a) < (b) ? (a) : (b))

/**
 *  \brief
 *
 *  \param[in] x  The multivector object, \f$x\f$.
 *  \param[in] i  Starting row (1-based index).
 *  \param[in] j  Starting column (1-based index).
 *  \param[in] R  Number of rows in the submatrix,
 *  or 0 to include all rows.
 *  \param[in] C  Number of columns in the submatrix,
 *  or 0 to include all columns.
 *  \param[in,out] y  Pointer to a structure in which to store the
 *  shallow copy of the submatrix.
 *  
 *  \returns 0 on success, or an error code on error.
 */
int
oski_InitSubVecView (const oski_vecview_t x,
		     oski_index_t i, oski_index_t j, oski_index_t R,
		     oski_index_t C, oski_vecstruct_t * y)
{
  if (x == INVALID_VEC)
    {
      OSKI_ERR_BAD_VEC (oski_InitSubVecView, 1);
      return ERR_BAD_ARG;
    }
  if (i <= 0 || i > x->num_rows)
    {
      OSKI_ERR_BAD_ROW (oski_InitSubVecView, 2, i, 1, x->num_rows);
      return ERR_BAD_ARG;
    }
  if (j <= 0 || j > x->num_cols)
    {
      OSKI_ERR_BAD_ROW (oski_InitSubVecView, 3, j, 1, x->num_cols);
      return ERR_BAD_ARG;
    }
  if (y == INVALID_VEC)
    {
      OSKI_ERR_BAD_VEC (oski_InitSubVecView, 4);
      return ERR_BAD_ARG;
    }

  if (R == 0)
    y->num_rows = x->num_rows - (i - 1);
  else
    y->num_rows = MIN (R, x->num_rows - (i - 1));
  if (C == 0)
    y->num_cols = x->num_cols - (j - 1);
  else
    y->num_cols = MIN (C, x->num_cols - (j - 1));

  y->orient = x->orient;
  y->stride = x->stride;
  y->rowinc = x->rowinc;
  y->colinc = x->colinc;
  y->val = x->val + (i - 1) * x->rowinc + (j - 1) * x->colinc;
  return 0;
}

/* eof */
