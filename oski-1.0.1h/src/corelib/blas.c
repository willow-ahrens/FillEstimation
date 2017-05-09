/**
 *  \file blas.c
 *  \brief OSKI wrappers around the dense BLAS routines.
 *  \ingroup AUXIL
 */

#include <oski/common.h>
#include <oski/config.h>
#include <oski/blas.h>
#include <oski/blas_names.h>

#if !HAVE_BLAS || (IND_TAG_CHAR != 'i')
/**
 *  \brief
 *
 *  This implementation assumes a general (*alpha). The caller
 *  should implement special tests when alpha is a special value
 *  (e.g., 0, 1, -1).
 *
 *  \todo Correctly implement the negative stride case (not used
 *  in BeBOP).
 */
void
BLAS_xSCAL (const oski_index_t * restrict len, oski_value_t * restrict alpha,
	    oski_value_t * restrict x, const oski_index_t * restrict stride)
{
	/** Considers the unit-stride special case. */
  if ((*stride) == 1)
    {
      oski_index_t i;
      for (i = 0; i < (*len); i++)
	{
	  VAL_SCALE (x[i], *alpha);
	}
    }
  else
    {
      oski_index_t i;
      for (i = 0; i < (*len); i++)
	VAL_SCALE (x[i * (*stride)], *alpha);
    }
}

/**
 *  \brief
 *
 *  This implementation assumes a general (*alpha). The caller
 *  should implement special tests when alpha is a special value
 *  (e.g., 0, 1, -1).
 *
 *  \todo Correctly implement the negative stride case (not used
 *  in BeBOP).
 */
void
BLAS_xAXPY (const oski_index_t * restrict len,
	    const oski_value_t * restrict alpha,
	    const oski_value_t * restrict x,
	    const oski_index_t * restrict incx,
	    oski_value_t * restrict y,
	    const oski_index_t * restrict incy)
{
	/**
	 *  Considers the following special cases:
	 *    - Both x and y are unit-stride accessible.
	 *    - Only x or y are unit-stride accessible.
	 *    - Both x and y require strided access.
	 */

  if (((*incx) == 1) && ((*incy) == 1))
    {
      oski_index_t i;
      for (i = 0; i < (*len); i++)
	VAL_MAC (y[i], *alpha, x[i]);
    }
  else if ((*incx) == 1)
    {
      oski_index_t i;
      for (i = 0; i < (*len); i++)
	VAL_MAC (y[i * (*incy)], *alpha, x[i]);
    }
  else if ((*incy) == 1)
    {
      oski_index_t i;
      for (i = 0; i < (*len); i++)
	VAL_MAC (y[i], *alpha, x[i * (*incx)]);
    }
  else				/* general strided vectors */
    {
      oski_index_t i;
      for (i = 0; i < (*len); i++)
	VAL_MAC (y[i * (*incy)], *alpha, x[i * (*incx)]);
    }
}
#endif

/**
 *  \brief Set all elements of a dense matrix, stored using row-major
 *  layout, to 0.
 *
 *  \param[in,out] x Array storing the matrix.
 *  \param[in] m Logical number of logical rows.
 *  \param[in] n Logical number of logical columns.
 *  \param[in] stride Leading-dimension.
 *
 *  \post All elements of the logical matrix are set to 0.
 */
static void
ZeroDenseMatRowmaj (oski_value_t * x, oski_index_t m, oski_index_t n,
		    oski_index_t stride)
{
  if (stride == n)
    oski_ZeroMem (x, sizeof (oski_value_t) * m * n);
  else
    {
      oski_index_t i;
      for (i = 0; i < m; i++)
	{
	  oski_index_t j;
	  for (j = 0; j < n; j++)
	    {
	      VAL_SET_ZERO (VECRM_GET (x, i, j, stride));
	    }
	}
    }
}

/**
 *  \brief Set all elements of a dense matrix, stored using
 *  column-major layout, to 0.
 *
 *  \param[in,out] x Array storing the matrix.
 *  \param[in] m Logical number of logical rows.
 *  \param[in] n Logical number of logical columns.
 *  \param[in] stride Leading-dimension.
 *
 *  \post All elements of the logical matrix are set to 0.
 */
static void
ZeroDenseMatColmaj (oski_value_t * x, oski_index_t m, oski_index_t n,
		    oski_index_t stride)
{
  if (stride == m)
    oski_ZeroMem (x, sizeof (oski_value_t) * m * n);
  else
    {
      oski_index_t i;
      for (i = 0; i < m; i++)
	{
	  oski_index_t j;
	  for (j = 0; j < n; j++)
	    {
	      VAL_SET_ZERO (VECCM_GET (x, i, j, stride));
	    }
	}
    }
}

/**
 *  \brief
 *
 *  \param[in] x Valid vector view.
 *  \returns 0 on success, or an error code on err.
 */
int
oski_ZeroVecView (oski_vecview_t x)
{
  if (x == INVALID_VEC)
    {
      OSKI_ERR_BAD_VEC (oski_ZeroVecView, 1);
      return ERR_BAD_VECVIEW;
    }

  switch (x->orient)
    {
    case LAYOUT_ROWMAJ:
      ZeroDenseMatRowmaj (x->val, x->num_rows, x->num_cols, x->stride);
      break;

    case LAYOUT_COLMAJ:
      ZeroDenseMatColmaj (x->val, x->num_rows, x->num_cols, x->stride);
      break;

    default:
      OSKI_ERR_BAD_VEC (oski_ZeroVecView, 1);
      return ERR_BAD_VECVIEW;
    }

  return 0;
}

/**
 *  \brief
 *
 *  \param[in,out] x A valid vector view.
 *  \param[in] alpha Scalar multiplier.
 */
int
oski_ScaleVecView (oski_vecview_t x, oski_value_t alpha)
{
  if (IS_VAL_ZERO (alpha))
    {
      return oski_ZeroVecView (x);
    }
  else if (IS_VAL_ONE (alpha))
    return 0;

  if (x->orient == LAYOUT_ROWMAJ)
    {
      oski_index_t j;		/* column index */
      for (j = 0; j < x->num_cols; j++)
	BLAS_xSCAL (&(x->num_rows), &alpha, x->val + j, &(x->stride));
    }
  else if (x->orient == LAYOUT_COLMAJ)
    {
      const oski_index_t ONE = 1;
      oski_index_t j;		/* column index */

      for (j = 0; j < x->num_cols; j++)
	BLAS_xSCAL (&(x->num_rows), &alpha, x->val + j * x->stride, &ONE);
    }
  else
    {
      OSKI_ERR_NOT_IMPLEMENTED (oski_ScaleVecView, "Vector view scale");
      return ERR_NOT_IMPLEMENTED;
    }

  return 0;
}

/**
 *  \brief
 *
 *  \pre x and y must be valid views.
 */
int
oski_RectScaledIdentityMult (oski_value_t alpha,
			     const oski_vecview_t x, oski_vecview_t y)
{
  /* Declared on the stack so as to avoid a possible
   * memory allocation failure. */
  oski_vecstruct_t x_sq;
  oski_vecstruct_t y_sq;

  oski_index_t min_rows;

  min_rows = x->num_rows < y->num_rows ? x->num_rows : y->num_rows;

  x_sq.num_rows = min_rows;
  x_sq.num_cols = x->num_cols;
  x_sq.orient = x->orient;
  x_sq.stride = x->stride;
  x_sq.rowinc = x->rowinc;
  x_sq.colinc = x->colinc;
  x_sq.val = (oski_value_t *) x->val;	/* Cast removes 'const' qualifier */

  y_sq.num_rows = min_rows;
  y_sq.num_cols = y->num_cols;
  y_sq.orient = y->orient;
  y_sq.stride = y->stride;
  y_sq.rowinc = y->rowinc;
  y_sq.colinc = y->colinc;
  y_sq.val = y->val;

  return oski_AXPYVecView (&x_sq, alpha, &y_sq);
}

int
oski_AXPYVecView (const oski_vecview_t x, oski_value_t alpha,
		  oski_vecview_t y)
{
  oski_index_t j;		/* column index */

  if (!ARE_VECVIEW_DIMS_EQUAL (x, y))
    {
      OSKI_ERR_DIM_UNEQUAL (oski_AXPYVecView, x->num_rows, x->num_cols,
			    y->num_rows, y->num_cols);
      return ERR_DIM_MISMATCH;
    }

  if (IS_VAL_ZERO (alpha))
    return 0;

  for (j = 0; j < x->num_cols; j++)
    {
      BLAS_xAXPY (&(x->num_rows), &alpha,
		  x->val + j * (x->colinc), &(x->rowinc),
		  y->val + j * (y->colinc), &(y->rowinc));
    }

  return 0;
}

#if IS_VAL_COMPLEX
int
oski_ConjVecView (oski_vecview_t x)
{
  int err = 0;

  if (x == INVALID_VEC || x->num_rows == 0 || x->num_cols == 0)
    return 0;

  if (x->orient == LAYOUT_ROWMAJ)
    {
      oski_index_t i;
      for (i = 0; i < x->num_rows; i++)
	{
	  oski_index_t j;
	  oski_value_t *xp;
	  for (j = 0, xp = x->val + i * x->stride; j < x->num_cols; j++, xp++)
	    VAL_CONJ (xp[0]);
	}
    }
  else if (x->orient == LAYOUT_COLMAJ)
    {
      oski_index_t j;
      for (j = 0; j < x->num_cols; j++)
	{
	  oski_index_t i;
	  oski_value_t *xp;
	  for (i = 0, xp = x->val + j * x->stride; i < x->num_rows; i++, xp++)
	    VAL_CONJ (xp[0]);
	}
    }
  else
    {
      OSKI_ERR_BAD_STORAGE (oski_ConjVecView, x->orient);
      err = ERR_BAD_ARG;
    }

  return err;
}
#else /* in the real case, this routine is a nop */
int
oski_ConjVecView (oski_vecview_t x)
{
  return 0;
}
#endif

/* eof */
