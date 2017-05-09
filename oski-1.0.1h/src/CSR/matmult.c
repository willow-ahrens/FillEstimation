/**
 *  \file src/CSR/matmult.c
 *  \ingroup MATTYPE_CSR
 *  \brief Sparse matrix-vector multiply implementation for a
 *  compressed sparse row (CSR) matrix.
 */

#include <assert.h>
#include <oski/config.h>	/* for 'restrict' keyword */
#include <oski/common.h>
#include <oski/blas.h>
#include <oski/matmult.h>
#include <oski/CSR/format.h>
#include <oski/CSR/module.h>

#include "MatMult/CSR_MatMult.h"
#include "SymmMatMult/CSR_SymmMatMult.h"
#include "SymmMatMult/CSR_SymmMatHermMult.h"
#include "SymmMatMult/CSR_HermMatMult.h"
#include "SymmMatMult/CSR_HermMatTransMult.h"

#if !IS_VAL_COMPLEX
/** \brief Base-adjusted, matrix times single-vector multiply. */
#define CSR_MatConjMult_v1 CSR_MatMult_v1
#define MatReprMult_Conj MatReprMult_Normal
#else
/** \brief Base-adjusted, matrix times single-vector multiply. */
static void
CSR_MatConjMult_v1 (oski_index_t m, oski_index_t n, const oski_index_t * ptr,
		    const oski_index_t * ind, const oski_value_t * val,
		    oski_value_t alpha, const oski_value_t * x,
		    oski_index_t incx, oski_value_t * y, oski_index_t incy)
{
  if (incx == 1)
    {
      if (IS_VAL_ONE (alpha))
	{
	  CSR_MatConjMult_v1_a1_b1_xs1_ysX (m, n, ptr, ind, val, x, y, incy);
	}
      else if (IS_VAL_NEG_ONE (alpha))
	{
	  CSR_MatConjMult_v1_aN1_b1_xs1_ysX (m, n, ptr, ind, val, x, y, incy);
	}
      else			/* general alpha */
	{
	  CSR_MatConjMult_v1_aX_b1_xs1_ysX (m, n, ptr, ind, val,
					    alpha, x, y, incy);
	}
    }
  else				/* general X stride */
    {
      if (IS_VAL_ONE (alpha))
	{
	  CSR_MatConjMult_v1_a1_b1_xsX_ysX (m, n, ptr, ind, val,
					    x, incx, y, incy);
	}
      else if (IS_VAL_NEG_ONE (alpha))
	{
	  CSR_MatConjMult_v1_aN1_b1_xsX_ysX (m, n, ptr, ind, val,
					     x, incx, y, incy);
	}
      else			/* general alpha */
	{
	  CSR_MatConjMult_v1_aX_b1_xsX_ysX (m, n, ptr, ind, val,
					    alpha, x, incx, y, incy);
	}
    }
}

/**
 *  \brief CSR-specific matrix-vector multiply operation,
 *  \f$y \leftarrow y + \alpha \cdot \bar{A} \cdot x\f$.
 *
 *  At present, this implementation does not handle the multiple
 *  vector case specially.
 *
 *  \pre This implementation assumes \f$\alpha \neq 0\f$.
 *  \pre This implementation assumes full storage.
 */
static int
MatReprMult_Conj (const oski_matCSR_t * A,
		  const oski_matcommon_t * props,
		  oski_value_t alpha, const oski_vecview_t x_view,
		  oski_vecview_t y_view)
{
  /* Store base index-adjusted pointers. */
  const oski_index_t *ind = A->ind - A->base_index;
  const oski_value_t *val = A->val - A->base_index;
  const oski_value_t *x = x_view->val - (A->base_index * x_view->rowinc);

  /* Symmetric/Hermitian and half-storage */
  if ((props->pattern.is_symm || props->pattern.is_herm)
      && (A->stored.is_upper != A->stored.is_lower))
    {
      return ERR_NOT_IMPLEMENTED;
    }

  /* FULL STORAGE. */

  if (x_view->num_cols == 1)	/* single vector */
    {
      CSR_MatConjMult_v1 (props->num_rows, props->num_cols, A->ptr,
			  ind, val, alpha, x, x_view->rowinc, y_view->val,
			  y_view->rowinc);
    }
  else				/* multiple vector */
    {
		/**
		 *  At present, the multiple-vector implementation just
		 *  repeatedly calls the single-vector implementation.
		 */
      oski_index_t j;		/* column number */

      const oski_value_t *xpj;	/* X(:, j) */
      oski_value_t *ypj;	/* Y(:, j) */

      for (j = 0, xpj = x, ypj = y_view->val; j < x_view->num_cols;
	   j++, xpj += x_view->colinc, ypj += y_view->colinc)
	{
	  CSR_MatConjMult_v1 (props->num_rows, props->num_cols, A->ptr,
			      ind, val, alpha, xpj, x_view->rowinc, ypj,
			      y_view->rowinc);
	}
    }

  if (A->has_unit_diag_implicit)
    {
      /* Additional: y <-- y + alpha * x operation */
      int err = oski_RectScaledIdentityMult (alpha, x_view, y_view);
      if (err)
	return err;
    }

  return 0;
}
#endif

/** \brief Base-adjusted, matrix times single-vector multiply. */
static void
CSR_MatMult_v1 (oski_index_t m, oski_index_t n, const oski_index_t * ptr,
		const oski_index_t * ind, const oski_value_t * val,
		oski_value_t alpha, const oski_value_t * x, oski_index_t incx,
		oski_value_t * y, oski_index_t incy)
{
  if (incx == 1)
    {
      if (IS_VAL_ONE (alpha))
	{
	  CSR_MatMult_v1_a1_b1_xs1_ysX (m, n, ptr, ind, val, x, y, incy);
	}
      else if (IS_VAL_NEG_ONE (alpha))
	{
	  CSR_MatMult_v1_aN1_b1_xs1_ysX (m, n, ptr, ind, val, x, y, incy);
	}
      else			/* general alpha */
	{
	  CSR_MatMult_v1_aX_b1_xs1_ysX (m, n, ptr, ind, val,
					alpha, x, y, incy);
	}
    }
  else				/* general X stride */
    {
      if (IS_VAL_ONE (alpha))
	{
	  CSR_MatMult_v1_a1_b1_xsX_ysX (m, n, ptr, ind, val,
					x, incx, y, incy);
	}
      else if (IS_VAL_NEG_ONE (alpha))
	{
	  CSR_MatMult_v1_aN1_b1_xsX_ysX (m, n, ptr, ind, val,
					 x, incx, y, incy);
	}
      else			/* general alpha */
	{
	  CSR_MatMult_v1_aX_b1_xsX_ysX (m, n, ptr, ind, val,
					alpha, x, incx, y, incy);
	}
    }
}

/**
 *  \brief CSR-specific matrix-vector multiply operation,
 *  \f$y \leftarrow y + \alpha \cdot A \cdot x\f$.
 *
 *  At present, this implementation does not handle the multiple
 *  vector case specially.
 *
 *  \pre This implementation assumes \f$\alpha \neq 0\f$.
 *  \pre This implementation assumes full storage.
 */
static int
MatReprMult_Normal (const oski_matCSR_t * A,
		    const oski_matcommon_t * props,
		    oski_value_t alpha, const oski_vecview_t x_view,
		    oski_vecview_t y_view)
{
  /* Store base index-adjusted pointers. */
  const oski_index_t *ind = A->ind - A->base_index;
  const oski_value_t *val = A->val - A->base_index;
  const oski_value_t *x = x_view->val - (A->base_index * x_view->rowinc);

  /* Symmetric/Hermitian and half-storage */
  if ((props->pattern.is_symm || props->pattern.is_herm)
      && (A->stored.is_upper != A->stored.is_lower))
    {
      return ERR_NOT_IMPLEMENTED;
    }

  /* FULL STORAGE. */

  if (x_view->num_cols == 1)	/* single vector */
    {
      CSR_MatMult_v1 (props->num_rows, props->num_cols, A->ptr,
		      ind, val, alpha, x, x_view->rowinc, y_view->val,
		      y_view->rowinc);
    }
  else				/* multiple vector */
    {
		/**
		 *  At present, the multiple-vector implementation just
		 *  repeatedly calls the single-vector implementation.
		 */
      oski_index_t j;		/* column number */

      const oski_value_t *xpj;	/* X(:, j) */
      oski_value_t *ypj;	/* Y(:, j) */

      for (j = 0, xpj = x, ypj = y_view->val; j < x_view->num_cols;
	   j++, xpj += x_view->colinc, ypj += y_view->colinc)
	{
	  CSR_MatMult_v1 (props->num_rows, props->num_cols, A->ptr,
			  ind, val, alpha, xpj, x_view->rowinc, ypj,
			  y_view->rowinc);
	}
    }

  if (A->has_unit_diag_implicit)
    {
      /* Additional: y <-- y + alpha * x operation */
      int err = oski_RectScaledIdentityMult (alpha, x_view, y_view);
      if (err)
	return err;
    }

  return 0;
}

/** \brief Base-adjusted, matrix-transpose times single-vector multiply. */
static void
CSR_MatTransMult_v1 (oski_index_t m, oski_index_t n, const oski_index_t * ptr,
		     const oski_index_t * ind, const oski_value_t * val,
		     oski_value_t alpha, const oski_value_t * x,
		     oski_index_t incx, oski_value_t * y, oski_index_t incy)
{
  if (incy == 1)
    {
      if (IS_VAL_ONE (alpha))
	{
	  CSR_MatTransMult_v1_a1_b1_xsX_ys1 (m, n, ptr, ind, val, x, incx, y);
	}
      else if (IS_VAL_NEG_ONE (alpha))
	{
	  CSR_MatTransMult_v1_aN1_b1_xsX_ys1 (m, n, ptr, ind, val,
					      x, incx, y);
	}
      else			/* general alpha */
	{
	  CSR_MatTransMult_v1_aX_b1_xsX_ys1 (m, n, ptr, ind, val,
					     alpha, x, incx, y);
	}
    }
  else				/* general X stride */
    {
      if (IS_VAL_ONE (alpha))
	{
	  CSR_MatTransMult_v1_a1_b1_xsX_ysX (m, n, ptr, ind, val,
					     x, incx, y, incy);
	}
      else if (IS_VAL_NEG_ONE (alpha))
	{
	  CSR_MatTransMult_v1_aN1_b1_xsX_ysX (m, n, ptr, ind, val,
					      x, incx, y, incy);
	}
      else			/* general alpha */
	{
	  CSR_MatTransMult_v1_aX_b1_xsX_ysX (m, n, ptr, ind, val,
					     alpha, x, incx, y, incy);
	}
    }
}

/**
 *  \brief CSR-specific matrix-vector multiply operation,
 *  \f$y \leftarrow y + \alpha \cdot A^T \cdot x\f$.
 *
 *  At present, this implementation does not handle the multiple
 *  vector case specially.
 *
 *  \pre This implementation assumes \f$\alpha \neq 0\f$.
 *  \pre This implementation assumes full storage.
 */
static int
MatReprMult_Trans (const oski_matCSR_t * A,
		   const oski_matcommon_t * props,
		   oski_value_t alpha, const oski_vecview_t x_view,
		   oski_vecview_t y_view)
{
  /* Store base index-adjusted pointers. */
  const oski_index_t *ind = A->ind - A->base_index;
  const oski_value_t *val = A->val - A->base_index;
  oski_value_t *y = y_view->val - (A->base_index * y_view->rowinc);

  /* Symmetric/Hermitian and half-storage */
  if ((props->pattern.is_symm || props->pattern.is_herm)
      && (A->stored.is_upper != A->stored.is_lower))
    {
      return ERR_NOT_IMPLEMENTED;
    }

  /* FULL STORAGE. */

  if (x_view->num_cols == 1)	/* single vector */
    {
      CSR_MatTransMult_v1 (props->num_rows, props->num_cols, A->ptr,
			   ind, val, alpha, x_view->val, x_view->rowinc, y,
			   y_view->rowinc);
    }
  else				/* multiple vector */
    {
		/**
		 *  At present, the multiple-vector implementation just
		 *  repeatedly calls the single-vector implementation.
		 */
      oski_index_t j;		/* column number */

      const oski_value_t *xpj;	/* X(:, j) */
      oski_value_t *ypj;	/* Y(:, j) */

      for (j = 0, xpj = x_view->val, ypj = y; j < x_view->num_cols;
	   j++, xpj += x_view->colinc, ypj += y_view->colinc)
	{
	  CSR_MatTransMult_v1 (props->num_rows, props->num_cols, A->ptr,
			       ind, val, alpha, xpj, x_view->rowinc, ypj,
			       y_view->rowinc);
	}
    }

  if (A->has_unit_diag_implicit)
    {
      /* Additional: y <-- y + alpha * x operation */
      int err = oski_RectScaledIdentityMult (alpha, x_view, y_view);
      if (err)
	return err;
    }

  return 0;
}

#if IS_VAL_COMPLEX
/**
 *  \brief Base-adjusted, matrix-conjugate transpose times
 *  single-vector multiply.
 */
static void
CSR_MatHermMult_v1 (oski_index_t m, oski_index_t n, const oski_index_t * ptr,
		    const oski_index_t * ind, const oski_value_t * val,
		    oski_value_t alpha, const oski_value_t * x,
		    oski_index_t incx, oski_value_t * y, oski_index_t incy)
{
  if (incy == 1)
    {
      if (IS_VAL_ONE (alpha))
	{
	  CSR_MatHermMult_v1_a1_b1_xsX_ys1 (m, n, ptr, ind, val, x, incx, y);
	}
      else if (IS_VAL_NEG_ONE (alpha))
	{
	  CSR_MatHermMult_v1_aN1_b1_xsX_ys1 (m, n, ptr, ind, val, x, incx, y);
	}
      else			/* general alpha */
	{
	  CSR_MatHermMult_v1_aX_b1_xsX_ys1 (m, n, ptr, ind, val,
					    alpha, x, incx, y);
	}
    }
  else				/* general X stride */
    {
      if (IS_VAL_ONE (alpha))
	{
	  CSR_MatHermMult_v1_a1_b1_xsX_ysX (m, n, ptr, ind, val,
					    x, incx, y, incy);
	}
      else if (IS_VAL_NEG_ONE (alpha))
	{
	  CSR_MatHermMult_v1_aN1_b1_xsX_ysX (m, n, ptr, ind, val,
					     x, incx, y, incy);
	}
      else			/* general alpha */
	{
	  CSR_MatHermMult_v1_aX_b1_xsX_ysX (m, n, ptr, ind, val,
					    alpha, x, incx, y, incy);
	}
    }
}
#endif /* IS_VAL_COMPLEX */

#if IS_VAL_COMPLEX
/**
 *  \brief CSR-specific matrix-vector multiply operation,
 *  \f$y \leftarrow y + \alpha \cdot A^H \cdot x\f$.
 *
 *  At present, this implementation does not handle the multiple
 *  vector case specially.
 *
 *  \pre This implementation assumes \f$\alpha \neq 0\f$.
 *  \pre This implementation assumes full storage.
 */
static int
MatReprMult_Herm (const oski_matCSR_t * A,
		  const oski_matcommon_t * props,
		  oski_value_t alpha, const oski_vecview_t x_view,
		  oski_vecview_t y_view)
{
  /* Store base index-adjusted pointers. */
  const oski_index_t *ind = A->ind - A->base_index;
  const oski_value_t *val = A->val - A->base_index;
  oski_value_t *y = y_view->val - (A->base_index * y_view->rowinc);

  /* Symmetric/Hermitian and half-storage */
  if ((props->pattern.is_symm || props->pattern.is_herm)
      && (A->stored.is_upper != A->stored.is_lower))
    {
      return ERR_NOT_IMPLEMENTED;
    }

  /* FULL STORAGE. */

  if (x_view->num_cols == 1)	/* single vector */
    {
      CSR_MatHermMult_v1 (props->num_rows, props->num_cols, A->ptr,
			  ind, val, alpha, x_view->val, x_view->rowinc, y,
			  y_view->rowinc);
    }
  else				/* multiple vector */
    {
		/**
		 *  At present, the multiple-vector implementation just
		 *  repeatedly calls the single-vector implementation.
		 */
      oski_index_t j;		/* column number */

      const oski_value_t *xpj;	/* X(:, j) */
      oski_value_t *ypj;	/* Y(:, j) */

      for (j = 0, xpj = x_view->val, ypj = y; j < x_view->num_cols;
	   j++, xpj += x_view->colinc, ypj += y_view->colinc)
	{
	  CSR_MatHermMult_v1 (props->num_rows, props->num_cols, A->ptr,
			      ind, val, alpha, xpj, x_view->rowinc, ypj,
			      y_view->rowinc);
	}
    }

  if (A->has_unit_diag_implicit)
    {
      /* Additional: y <-- y + alpha * x operation */
      int err = oski_RectScaledIdentityMult (alpha, x_view, y_view);
      if (err)
	return err;
    }

  return 0;
}
#endif /* IS_VAL_COMPLEX */

/**
 *  \brief Single-vector implementation of symmetric CSR SpMV
 */
static void
SymmMatMult_v1 (oski_index_t m, oski_index_t n, const oski_index_t * ptr,
		const oski_index_t * ind, const oski_value_t * val,
		oski_index_t base, oski_value_t alpha, const oski_value_t * x,
		oski_index_t incx, oski_value_t * y, oski_index_t incy)
{
  if (incx == 1)
    {
      if (incy == 1)
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_SymmMatMult_v1_a1_b1_xs1_ys1 (m, n, ptr, ind, val,
						base, x, y);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_SymmMatMult_v1_aN1_b1_xs1_ys1 (m, n, ptr, ind, val,
						 base, x, y);
	    }
	  else
	    {
	      CSR_SymmMatMult_v1_aX_b1_xs1_ys1 (m, n, ptr, ind, val,
						base, alpha, x, y);
	    }
	}
      else			/* incx == 1, incy == general */
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_SymmMatMult_v1_a1_b1_xs1_ysX (m, n, ptr, ind, val,
						base, x, y, incy);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_SymmMatMult_v1_aN1_b1_xs1_ysX (m, n, ptr, ind, val,
						 base, x, y, incy);
	    }
	  else			/* alpha == general */
	    {
	      CSR_SymmMatMult_v1_aX_b1_xs1_ysX (m, n, ptr, ind, val,
						base, alpha, x, y, incy);
	    }
	}
    }
  else				/* else incx == general */
    {
      if (incy == 1)
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_SymmMatMult_v1_a1_b1_xsX_ys1 (m, n, ptr, ind, val,
						base, x, incx, y);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_SymmMatMult_v1_aN1_b1_xsX_ys1 (m, n, ptr, ind, val,
						 base, x, incx, y);
	    }
	  else			/* alpha == general */
	    {
	      CSR_SymmMatMult_v1_aX_b1_xsX_ys1 (m, n, ptr, ind, val,
						base, alpha, x, incx, y);
	    }
	}
      else			/* incx == general, incy == general */
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_SymmMatMult_v1_a1_b1_xsX_ysX (m, n, ptr, ind, val,
						base, x, incx, y, incy);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_SymmMatMult_v1_aN1_b1_xsX_ysX (m, n, ptr, ind, val,
						 base, x, incx, y, incy);
	    }
	  else			/* alpha == general */
	    {
	      CSR_SymmMatMult_v1_aX_b1_xsX_ysX (m, n, ptr, ind, val,
						base, alpha, x, incx, y,
						incy);
	    }
	}
    }
}

#if IS_VAL_COMPLEX
/**
 *  \brief Single-vector implementation of symmetric CSR SpMV
 */
static void
SymmMatHermMult_v1 (oski_index_t m, oski_index_t n, const oski_index_t * ptr,
		    const oski_index_t * ind, const oski_value_t * val,
		    oski_index_t base, oski_value_t alpha,
		    const oski_value_t * x, oski_index_t incx,
		    oski_value_t * y, oski_index_t incy)
{
  if (incx == 1)
    {
      if (incy == 1)
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_SymmMatHermMult_v1_a1_b1_xs1_ys1 (m, n, ptr, ind, val,
						    base, x, y);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_SymmMatHermMult_v1_aN1_b1_xs1_ys1 (m, n, ptr, ind, val,
						     base, x, y);
	    }
	  else
	    {
	      CSR_SymmMatHermMult_v1_aX_b1_xs1_ys1 (m, n, ptr, ind, val,
						    base, alpha, x, y);
	    }
	}
      else			/* incx == 1, incy == general */
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_SymmMatHermMult_v1_a1_b1_xs1_ysX (m, n, ptr, ind, val,
						    base, x, y, incy);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_SymmMatHermMult_v1_aN1_b1_xs1_ysX (m, n, ptr, ind, val,
						     base, x, y, incy);
	    }
	  else			/* alpha == general */
	    {
	      CSR_SymmMatHermMult_v1_aX_b1_xs1_ysX (m, n, ptr, ind, val,
						    base, alpha, x, y, incy);
	    }
	}
    }
  else				/* else incx == general */
    {
      if (incy == 1)
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_SymmMatHermMult_v1_a1_b1_xsX_ys1 (m, n, ptr, ind, val,
						    base, x, incx, y);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_SymmMatHermMult_v1_aN1_b1_xsX_ys1 (m, n, ptr, ind, val,
						     base, x, incx, y);
	    }
	  else			/* alpha == general */
	    {
	      CSR_SymmMatHermMult_v1_aX_b1_xsX_ys1 (m, n, ptr, ind, val,
						    base, alpha, x, incx, y);
	    }
	}
      else			/* incx == general, incy == general */
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_SymmMatHermMult_v1_a1_b1_xsX_ysX (m, n, ptr, ind, val,
						    base, x, incx, y, incy);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_SymmMatHermMult_v1_aN1_b1_xsX_ysX (m, n, ptr, ind, val,
						     base, x, incx, y, incy);
	    }
	  else			/* alpha == general */
	    {
	      CSR_SymmMatHermMult_v1_aX_b1_xsX_ysX (m, n, ptr, ind, val,
						    base, alpha, x, incx, y,
						    incy);
	    }
	}
    }
}
#endif /* IS_VAL_COMPLEX */

/**
 *  \brief Symmetric matrix-vector multiply,
 *  \f$y \leftarrow y + \alpha\cdot op(A) \cdot x\f$,
 *  where \f$A = A^T\f$ and \f$op(A) \in \{A, A^H\}\f$.
 *
 *  \todo Test the case when indices are unsorted.
 */
static int
SymmMatMult (const oski_matCSR_t * A, const oski_matcommon_t * props,
	     oski_matop_t opA, oski_value_t alpha,
	     const oski_vecview_t x_view, oski_vecview_t y_view)
{
  if (!A->has_sorted_indices)
    return ERR_NOT_IMPLEMENTED;

  switch (opA)
    {
    case OP_NORMAL:
    case OP_TRANS:
#if !IS_VAL_COMPLEX
    case OP_CONJ:
    case OP_CONJ_TRANS:
#endif /* !IS_VAL_COMPLEX */
      if (x_view->num_cols == 1)	/* single vector */
	{
	  SymmMatMult_v1 (props->num_rows, props->num_cols,
			  A->ptr, A->ind, A->val, A->base_index,
			  alpha, x_view->val, x_view->rowinc,
			  y_view->val, y_view->rowinc);
	}
      else			/* multiple vector */
	{
				/**
				 *  At present, the multiple-vector implementation just
				 *  repeatedly calls the single-vector implementation.
				 */
	  oski_index_t j;	/* column number */

	  const oski_value_t *xpj;	/* X(:, j) */
	  oski_value_t *ypj;	/* Y(:, j) */

	  for (j = 0, xpj = x_view->val, ypj = y_view->val;
	       j < x_view->num_cols;
	       j++, xpj += x_view->colinc, ypj += y_view->colinc)
	    {
	      SymmMatMult_v1 (props->num_rows, props->num_cols,
			      A->ptr, A->ind, A->val, A->base_index,
			      alpha, xpj, x_view->rowinc, ypj,
			      y_view->rowinc);
	    }
	}
      break;

#if IS_VAL_COMPLEX
    case OP_CONJ:
    case OP_CONJ_TRANS:
      if (x_view->num_cols == 1)	/* single vector */
	{
	  SymmMatHermMult_v1 (props->num_rows, props->num_cols,
			      A->ptr, A->ind, A->val, A->base_index,
			      alpha, x_view->val, x_view->rowinc,
			      y_view->val, y_view->rowinc);
	}
      else			/* multiple vector */
	{
				/**
				 *  At present, the multiple-vector implementation just
				 *  repeatedly calls the single-vector implementation.
				 */
	  oski_index_t j;	/* column number */

	  const oski_value_t *xpj;	/* X(:, j) */
	  oski_value_t *ypj;	/* Y(:, j) */

	  for (j = 0, xpj = x_view->val, ypj = y_view->val;
	       j < x_view->num_cols;
	       j++, xpj += x_view->colinc, ypj += y_view->colinc)
	    {
	      SymmMatHermMult_v1 (props->num_rows, props->num_cols,
				  A->ptr, A->ind, A->val, A->base_index,
				  alpha, xpj, x_view->rowinc, ypj,
				  y_view->rowinc);
	    }
	}
      break;
#endif /* IS_VAL_COMPLEX */

    default:
      assert (0);
    }

  if (A->has_unit_diag_implicit)
    {
      /* Additional: y <-- y + alpha * x operation */
      int err = oski_RectScaledIdentityMult (alpha, x_view, y_view);
      if (err)
	return err;
    }

  return 0;
}

#if IS_VAL_COMPLEX
/**
 *  \brief Single-vector implementation of symmetric CSR SpMV
 */
static void
HermMatMult_v1 (oski_index_t m, oski_index_t n, const oski_index_t * ptr,
		const oski_index_t * ind, const oski_value_t * val,
		oski_index_t base, oski_value_t alpha, const oski_value_t * x,
		oski_index_t incx, oski_value_t * y, oski_index_t incy)
{
  if (incx == 1)
    {
      if (incy == 1)
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_HermMatMult_v1_a1_b1_xs1_ys1 (m, n, ptr, ind, val,
						base, x, y);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_HermMatMult_v1_aN1_b1_xs1_ys1 (m, n, ptr, ind, val,
						 base, x, y);
	    }
	  else
	    {
	      CSR_HermMatMult_v1_aX_b1_xs1_ys1 (m, n, ptr, ind, val,
						base, alpha, x, y);
	    }
	}
      else			/* incx == 1, incy == general */
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_HermMatMult_v1_a1_b1_xs1_ysX (m, n, ptr, ind, val,
						base, x, y, incy);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_HermMatMult_v1_aN1_b1_xs1_ysX (m, n, ptr, ind, val,
						 base, x, y, incy);
	    }
	  else			/* alpha == general */
	    {
	      CSR_HermMatMult_v1_aX_b1_xs1_ysX (m, n, ptr, ind, val,
						base, alpha, x, y, incy);
	    }
	}
    }
  else				/* else incx == general */
    {
      if (incy == 1)
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_HermMatMult_v1_a1_b1_xsX_ys1 (m, n, ptr, ind, val,
						base, x, incx, y);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_HermMatMult_v1_aN1_b1_xsX_ys1 (m, n, ptr, ind, val,
						 base, x, incx, y);
	    }
	  else			/* alpha == general */
	    {
	      CSR_HermMatMult_v1_aX_b1_xsX_ys1 (m, n, ptr, ind, val,
						base, alpha, x, incx, y);
	    }
	}
      else			/* incx == general, incy == general */
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_HermMatMult_v1_a1_b1_xsX_ysX (m, n, ptr, ind, val,
						base, x, incx, y, incy);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_HermMatMult_v1_aN1_b1_xsX_ysX (m, n, ptr, ind, val,
						 base, x, incx, y, incy);
	    }
	  else			/* alpha == general */
	    {
	      CSR_HermMatMult_v1_aX_b1_xsX_ysX (m, n, ptr, ind, val,
						base, alpha, x, incx, y,
						incy);
	    }
	}
    }
}
#endif /* IS_VAL_COMPLEX */

#if IS_VAL_COMPLEX
/**
 *  \brief Single-vector implementation of symmetric CSR SpMV
 */
static void
HermMatTransMult_v1 (oski_index_t m, oski_index_t n, const oski_index_t * ptr,
		     const oski_index_t * ind, const oski_value_t * val,
		     oski_index_t base, oski_value_t alpha,
		     const oski_value_t * x, oski_index_t incx,
		     oski_value_t * y, oski_index_t incy)
{
  if (incx == 1)
    {
      if (incy == 1)
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_HermMatTransMult_v1_a1_b1_xs1_ys1 (m, n, ptr, ind, val,
						     base, x, y);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_HermMatTransMult_v1_aN1_b1_xs1_ys1 (m, n, ptr, ind, val,
						      base, x, y);
	    }
	  else
	    {
	      CSR_HermMatTransMult_v1_aX_b1_xs1_ys1 (m, n, ptr, ind, val,
						     base, alpha, x, y);
	    }
	}
      else			/* incx == 1, incy == general */
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_HermMatTransMult_v1_a1_b1_xs1_ysX (m, n, ptr, ind, val,
						     base, x, y, incy);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_HermMatTransMult_v1_aN1_b1_xs1_ysX (m, n, ptr, ind, val,
						      base, x, y, incy);
	    }
	  else			/* alpha == general */
	    {
	      CSR_HermMatTransMult_v1_aX_b1_xs1_ysX (m, n, ptr, ind, val,
						     base, alpha, x, y, incy);
	    }
	}
    }
  else				/* else incx == general */
    {
      if (incy == 1)
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_HermMatTransMult_v1_a1_b1_xsX_ys1 (m, n, ptr, ind, val,
						     base, x, incx, y);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_HermMatTransMult_v1_aN1_b1_xsX_ys1 (m, n, ptr, ind, val,
						      base, x, incx, y);
	    }
	  else			/* alpha == general */
	    {
	      CSR_HermMatTransMult_v1_aX_b1_xsX_ys1 (m, n, ptr, ind, val,
						     base, alpha, x, incx, y);
	    }
	}
      else			/* incx == general, incy == general */
	{
	  if (IS_VAL_ONE (alpha))
	    {
	      CSR_HermMatTransMult_v1_a1_b1_xsX_ysX (m, n, ptr, ind, val,
						     base, x, incx, y, incy);
	    }
	  else if (IS_VAL_NEG_ONE (alpha))
	    {
	      CSR_HermMatTransMult_v1_aN1_b1_xsX_ysX (m, n, ptr, ind, val,
						      base, x, incx, y, incy);
	    }
	  else			/* alpha == general */
	    {
	      CSR_HermMatTransMult_v1_aX_b1_xsX_ysX (m, n, ptr, ind, val,
						     base, alpha, x, incx, y,
						     incy);
	    }
	}
    }
}
#endif /* IS_VAL_COMPLEX */

#if IS_VAL_COMPLEX
/**
 *  \brief Hermitian matrix-vector multiply,
 *  \f$y \leftarrow y + \alpha\cdot op(A) \cdot x\f$,
 *  where \f$A = A^H\f$ and \f$op(A) \in \{A, A^T\}\f$.
 *
 *  \todo Test the case when indices are unsorted.
 */
static int
HermMatMult (const oski_matCSR_t * A, const oski_matcommon_t * props,
	     oski_matop_t opA, oski_value_t alpha,
	     const oski_vecview_t x_view, oski_vecview_t y_view)
{
  if (!A->has_sorted_indices)
    return ERR_NOT_IMPLEMENTED;

  switch (opA)
    {
    case OP_NORMAL:
    case OP_CONJ_TRANS:
      if (x_view->num_cols == 1)	/* single vector */
	{
	  HermMatMult_v1 (props->num_rows, props->num_cols,
			  A->ptr, A->ind, A->val, A->base_index,
			  alpha, x_view->val, x_view->rowinc,
			  y_view->val, y_view->rowinc);
	}
      else			/* multiple vector */
	{
				/**
				 *  At present, the multiple-vector implementation just
				 *  repeatedly calls the single-vector implementation.
				 */
	  oski_index_t j;	/* column number */

	  const oski_value_t *xpj;	/* X(:, j) */
	  oski_value_t *ypj;	/* Y(:, j) */

	  for (j = 0, xpj = x_view->val, ypj = y_view->val;
	       j < x_view->num_cols;
	       j++, xpj += x_view->colinc, ypj += y_view->colinc)
	    {
	      HermMatMult_v1 (props->num_rows, props->num_cols,
			      A->ptr, A->ind, A->val, A->base_index,
			      alpha, xpj, x_view->rowinc, ypj,
			      y_view->rowinc);
	    }
	}
      break;

    case OP_CONJ:
    case OP_TRANS:
      if (x_view->num_cols == 1)	/* single vector */
	{
	  HermMatTransMult_v1 (props->num_rows, props->num_cols,
			       A->ptr, A->ind, A->val, A->base_index,
			       alpha, x_view->val, x_view->rowinc,
			       y_view->val, y_view->rowinc);
	}
      else			/* multiple vector */
	{
				/**
				 *  At present, the multiple-vector implementation just
				 *  repeatedly calls the single-vector implementation.
				 */
	  oski_index_t j;	/* column number */

	  const oski_value_t *xpj;	/* X(:, j) */
	  oski_value_t *ypj;	/* Y(:, j) */

	  for (j = 0, xpj = x_view->val, ypj = y_view->val;
	       j < x_view->num_cols;
	       j++, xpj += x_view->colinc, ypj += y_view->colinc)
	    {
	      HermMatTransMult_v1 (props->num_rows, props->num_cols,
				   A->ptr, A->ind, A->val, A->base_index,
				   alpha, xpj, x_view->rowinc, ypj,
				   y_view->rowinc);
	    }
	}
      break;

    default:
      assert (0);
    }

  if (A->has_unit_diag_implicit)
    {
      /* Additional: y <-- y + alpha * x operation */
      int err = oski_RectScaledIdentityMult (alpha, x_view, y_view);
      if (err)
	return err;
    }

  return 0;
}
#endif /* IS_VAL_COMPLEX */

/**
 *  \brief
 *
 *  \pre All arguments have legal values and meet basic dimensionality
 *  compatibility requirements.
 */
int
oski_MatReprMult (const void *pA, const oski_matcommon_t * props,
		  oski_matop_t opA,
		  oski_value_t alpha, const oski_vecview_t x_view,
		  oski_value_t beta, oski_vecview_t y_view)
{
  const oski_matCSR_t *A = (const oski_matCSR_t *) pA;

  assert (A != NULL);
  assert (props != NULL);

  oski_ScaleVecView (y_view, beta);
  if (IS_VAL_ZERO (alpha))
    return 0;

  /* Symmetric/Hermitian and half-storage */
  if (A->stored.is_upper != A->stored.is_lower)
    {
#if IS_VAL_COMPLEX
      if (props->pattern.is_symm)
	return SymmMatMult (A, props, opA, alpha, x_view, y_view);
      else if (props->pattern.is_herm)
	return HermMatMult (A, props, opA, alpha, x_view, y_view);
#else
      if (props->pattern.is_symm || props->pattern.is_herm)
	return SymmMatMult (A, props, opA, alpha, x_view, y_view);
#endif
      /* else, probably triangular, so fall through to code below */
    }

  /* FULL STORAGE. */

  switch (opA)
    {
    case OP_CONJ:
#if IS_VAL_COMPLEX
      return MatReprMult_Conj (A, props, alpha, x_view, y_view);
      /* #else is not complex, so fall through to the normal case */
#endif
    case OP_NORMAL:
      return MatReprMult_Normal (A, props, alpha, x_view, y_view);

    case OP_CONJ_TRANS:
#if IS_VAL_COMPLEX
      if (props->pattern.is_herm)
	return MatReprMult_Normal (A, props, alpha, x_view, y_view);
      else
	return MatReprMult_Herm (A, props, alpha, x_view, y_view);
      /* #else, fall through to transpose case */
#endif /* IS_VAL_COMPLEX */
    case OP_TRANS:
      if (props->pattern.is_symm)
	return MatReprMult_Normal (A, props, alpha, x_view, y_view);
      else
	return MatReprMult_Trans (A, props, alpha, x_view, y_view);

    default:
      assert (0);
    }

  return ERR_NOT_IMPLEMENTED;
}

/* eof */
