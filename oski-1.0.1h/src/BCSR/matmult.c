/**
 *  \file src/BCSR/matmult.c
 *  \brief BCSR implementation of SpMV.
 *  \ingroup MATTYPE_BCSR
 */

#include <assert.h>
#include <oski/config.h>
#include <oski/common.h>
#include <oski/blas.h>
#include <oski/blas_names.h>
#include <oski/modloader.h>
#include <oski/matrix.h>
#include <oski/BCSR/format.h>
#include <oski/BCSR/module.h>

/**
 *  \brief Performs \f$y \leftarrow y + \alpha A\cdot x\f$ on
 *  the fully blocked part of \f$A\f$.
 *
 *  This implementation is used when a specialized block-size
 *  specific routine cannot be found.
 */
static int
MatMultDefault_Normal (const oski_matBCSR_t * A,
		       oski_value_t alpha, const oski_vecview_t x_view,
		       oski_vecview_t y_view)
{
  char op_blas = 'T';
  oski_index_t j;

  for (j = 0; j < x_view->num_cols; j++)
    {
      const oski_value_t *xp = x_view->val + j * x_view->colinc;
      oski_value_t *yp = y_view->val + j * y_view->colinc;
      const oski_value_t *ap = A->bval;

      oski_index_t I;
      for (I = 0; I < A->num_block_rows; I++)
	{
	  oski_index_t K;
	  for (K = A->bptr[I]; K < A->bptr[I + 1]; K++)
	    {
	      oski_index_t j0;
	      const oski_value_t *xpp;

	      j0 = A->bind[K];
	      xpp = xp + j0 * x_view->rowinc;

	      BLAS_xGEMV (&op_blas,
			  &(A->col_block_size), &(A->row_block_size),
			  &alpha,
			  ap, &(A->col_block_size),
			  xpp, &(x_view->rowinc),
			  &TVAL_ONE, yp, &(y_view->rowinc));

	      ap += A->row_block_size * A->col_block_size;
	    }
	  yp += A->row_block_size * y_view->rowinc;
	}
    }

  return 0;
}

/**
 *  \brief Performs \f$y \leftarrow y + \alpha A^T\cdot x\f$ on
 *  the fully blocked part of \f$A\f$.
 *
 *  This implementation is used when a specialized block-size
 *  specific routine cannot be found.
 */
static int
MatMultDefault_Trans (const oski_matBCSR_t * A,
		      oski_value_t alpha, const oski_vecview_t x_view,
		      oski_vecview_t y_view)
{
  char op_blas = 'N';
  oski_index_t j;

  for (j = 0; j < x_view->num_cols; j++)
    {
      const oski_value_t *xp = x_view->val + j * x_view->colinc;
      oski_value_t *yp = y_view->val + j * y_view->colinc;
      const oski_value_t *ap = A->bval;

      oski_index_t I;
      for (I = 0; I < A->num_block_rows; I++)
	{
	  oski_index_t K;
	  for (K = A->bptr[I]; K < A->bptr[I + 1]; K++)
	    {
	      oski_index_t j0;
	      oski_value_t *ypp;

	      j0 = A->bind[K];
	      ypp = yp + j0 * y_view->rowinc;

	      BLAS_xGEMV (&op_blas,
			  &(A->col_block_size), &(A->row_block_size),
			  &alpha,
			  ap, &(A->col_block_size),
			  xp, &(x_view->rowinc),
			  &TVAL_ONE, ypp, &(y_view->rowinc));

	      ap += A->row_block_size * A->col_block_size;
	    }

	  xp += A->row_block_size * x_view->rowinc;
	}
    }

  return 0;
}


#if IS_VAL_COMPLEX
/**
 *  \brief Performs \f$y \leftarrow y + \alpha \bar{A}^T\cdot x\f$ on
 *  the fully blocked part of \f$A\f$.
 *
 *  This implementation is used when a specialized block-size
 *  specific routine cannot be found.
 *
 *  Unlike MatMultDefault_Normal() and MatMultDefault_ConjTrans(),
 *  this implementation cannot use xGEMV, since the blocks are
 *  stored in row-major format and xGEMV cannot apply just the
 *  conjugate of a matrix.
 */
static int
MatMultDefault_ConjTrans (const oski_matBCSR_t * A,
			  oski_value_t alpha, const oski_vecview_t x_view,
			  oski_vecview_t y_view)
{
  oski_index_t r;
  oski_index_t c;
  oski_index_t j;

  r = A->row_block_size;
  c = A->col_block_size;

  for (j = 0; j < x_view->num_cols; j++)
    {
      const oski_value_t *xp = x_view->val + j * x_view->colinc;
      oski_value_t *yp = y_view->val + j * y_view->colinc;
      const oski_value_t *ap = A->bval;

      oski_index_t I;
      for (I = 0; I < A->num_block_rows; I++)
	{
	  oski_index_t K;
	  for (K = A->bptr[I]; K < A->bptr[I + 1]; K++)
	    {
	      oski_index_t j0;

	      j0 = A->bind[K];

	      /* Conjugate-transpose block multiply */
	      {
		oski_index_t di;
		for (di = 0; di < r; di++)
		  {
		    oski_index_t dj;
		    oski_value_t *ypp = yp + j0 * y_view->rowinc;
		    oski_value_t _x0;

		    VAL_MUL (_x0, alpha, xp[di]);

		    for (dj = 0; dj < c; dj++)
		      {
			oski_value_t _y0;
			VAL_MUL_CONJ (_y0, ap[0], _x0);
			VAL_INC (ypp[0], _y0);

			ypp += y_view->rowinc;
			ap++;
		      }
		  }
	      }			/* end block multiply */
	    }

	  xp += A->row_block_size * x_view->rowinc;
	}
    }

  return 0;
}
#endif


/**
 *  \brief Default register blocked implementation in which the
 *  block multiply is replaced by a call to GEMV.
 *
 *  This implementation is needed in case the user requests
 *  multiplication by a block size that is not available.
 */
static int
MatMultDefault (const oski_matBCSR_t * A,
		oski_matop_t opA,
		oski_value_t alpha,
		const oski_vecview_t x_view, oski_vecview_t y_view)
{
  int err;

  switch (opA)
    {
#if !IS_VAL_COMPLEX
    case OP_CONJ:		/* fall through to OP_NORMAL case */
#endif
    case OP_NORMAL:
      err = MatMultDefault_Normal (A, alpha, x_view, y_view);
      break;

    case OP_CONJ_TRANS:
#if IS_VAL_COMPLEX
      err = MatMultDefault_ConjTrans (A, alpha, x_view, y_view);
      break;
/* #else !IS_VAL_COMPLEX, so fall through to OP_TRANS case */
#endif
    case OP_TRANS:
      err = MatMultDefault_Trans (A, alpha, x_view, y_view);
      break;

    default:
      oski_HandleError (ERR_NOT_IMPLEMENTED,
			"Can't find BCSR implementation", __FILE__, __LINE__,
			"Block size: %d x %d; op(A) == %d",
			(int) A->row_block_size, (int) A->col_block_size,
			(int) opA);
      err = ERR_NOT_IMPLEMENTED;
    }

  return err;
}

/**
 *  \brief Performs \f$y \leftarrow y + \alpha\cdot \mathrm{op}(A)\cdot x\f$,
 *  using just the main stored portion of \f$A\f$, i.e., ignoring any
 *  implicit structure (diagonal).
 *
 *  This routine calls itself recursively to multiply by the leftover
 *  rows, adjusting the vectors accordingly.
 */
static int
MatReprMultCore (const oski_matBCSR_t * A, oski_matop_t opA,
		 oski_value_t alpha, const oski_vecview_t x_view,
		 oski_vecview_t y_view)
{
  int err;
  oski_BCSR_MatMult_funcpt func_MatMult =
    (oski_BCSR_MatMult_funcpt) BCSR_GetKernel (A, "MatReprMult");

  if (func_MatMult == NULL)
    func_MatMult = MatMultDefault;

  /* Multiply by main, fully blocked part of matrix */
  assert (func_MatMult != NULL);
  err = (*func_MatMult) (A, opA, alpha, x_view, y_view);

  /* Multiply by any leftover rows */
  if (!err && A->num_rows_leftover && A->leftover != NULL)
    {
      oski_vecstruct_t x_left;
      oski_vecstruct_t y_left;

      /* Copy vector wrappers */
      oski_CopyMem (&x_left, x_view, oski_vecstruct_t, 1);
      oski_CopyMem (&y_left, y_view, oski_vecstruct_t, 1);

      /* Adjust vectors to correspond to leftover rows */
      switch (opA)
	{
#if !IS_VAL_COMPLEX
	case OP_CONJ:
	  /* fall through to OP_NORMAL */
#endif
	case OP_NORMAL:
	  y_left.num_rows = A->num_rows_leftover;
	  y_left.val += y_left.rowinc * A->num_block_rows * A->row_block_size;
	  break;

	case OP_CONJ_TRANS:
#if IS_VAL_COMPLEX
	  x_left.num_rows = A->num_rows_leftover;
	  x_left.val += x_left.rowinc * A->num_block_rows * A->row_block_size;
	  break;
#endif
	case OP_TRANS:
	  x_left.num_rows = A->num_rows_leftover;
	  x_left.val += x_left.rowinc * A->num_block_rows * A->row_block_size;
	  break;

	default:
	  assert (0);
	}

      /* Multiply by leftover rows, if any */
      err = MatReprMultCore (A->leftover, opA, alpha, &x_left, &y_left);
    }

  return err;
}

/**
 *  \brief
 *
 *  \pre All arguments have legal values and meet basic dimensionality
 *  compatibility requirements.
 *
 *  This implementation does not need 'props', i.e., props == NULL is
 *  OK.
 */
int
oski_MatReprMult (const void *pA, const oski_matcommon_t * props,
		  oski_matop_t opA,
		  oski_value_t alpha, const oski_vecview_t x_view,
		  oski_value_t beta, oski_vecview_t y_view)
{
  const oski_matBCSR_t *A = (const oski_matBCSR_t *) pA;
  int err;

  assert (A != NULL);

  /* y <-- beta * y */
  oski_ScaleVecView (y_view, beta);
  if (IS_VAL_ZERO (alpha))
    return 0;			/* quick return */

  err = MatReprMultCore (A, opA, alpha, x_view, y_view);

  /* Account for implicit unit diagonal, if any */
  if (!err && A->has_unit_diag_implicit)
    err = oski_RectScaledIdentityMult (alpha, x_view, y_view);

  return err;
}

/* eof */
