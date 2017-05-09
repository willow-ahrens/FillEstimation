/**
 *  \file src/CB/matmult.c
 *  \ingroup MATTYPE_CB
 *  \brief Sparse matrix-vector multiply implementation for a
 *  compressed sparse row (CB) matrix.
 */

#include <assert.h>
#include <oski/config.h>	/* for 'restrict' keyword */
#include <oski/common.h>
#include <oski/blas.h>
#include <oski/matmult.h>
#include <oski/CB/format.h>
#include <oski/CB/module.h>

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
  const oski_matCB_t *A = (const oski_matCB_t *) pA;
  simplelist_iter_t i;
  const oski_submatCB_t *b_i;

  assert (A != NULL);
  assert (props != NULL);

  oski_ScaleVecView (y_view, beta);
  if (IS_VAL_ZERO (alpha))
    {
      return 0;
    }

  for (b_i = simplelist_BeginIter (A->cache_blocks, &i);
       b_i != NULL; b_i = simplelist_NextIter (&i))
    {
      oski_vecstruct_t x_b;
      oski_vecstruct_t y_b;
      oski_index_t mx;		/* Dimension of x submatrix */
      oski_index_t dx;		/* Offset of x submatrix */
      oski_index_t my;		/* Dimension of y submatrix */
      oski_index_t dy;		/* Offset of y submatrix */
      int err;

      switch (opA)
	{
	case OP_NORMAL:
	case OP_CONJ:
	  mx = b_i->num_cols;
	  dx = b_i->col + 1;
	  my = b_i->num_rows;
	  dy = b_i->row + 1;
	  break;
	case OP_TRANS:
	case OP_CONJ_TRANS:
	  my = b_i->num_cols;
	  dy = b_i->col + 1;
	  mx = b_i->num_rows;
	  dx = b_i->row + 1;
	  break;
	default:
	  assert (0);
	}

      oski_InitSubVecView (x_view, dx, 1, mx, 0, &x_b);
      oski_InitSubVecView (y_view, dy, 1, my, 0, &y_b);
      err = oski_MatMult (b_i->mat, opA, alpha, &x_b, TVAL_ONE, &y_b);
      if (err)
	return err;
    }
  return 0;
}

/* eof */
