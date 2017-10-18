/**
 *  \file src/VBR/matmult.c
 *  \brief VBR implementation of SpMV.
 *  \ingroup MATTYPE_VBR
 */

#include <assert.h>
#include <oski/config.h>
#include <oski/common.h>
#include <oski/blas.h>
#include <oski/blas_names.h>
#include <oski/modloader.h>
#include <oski/matrix.h>

#include <oski/VBR/module.h>
#include <oski/matmodexport.h>

static int
MatMultNormal (oski_index_t M, oski_index_t N,
	       const oski_index_t* restrict brow,
	       const oski_index_t* restrict bcol,
	       const oski_index_t* restrict ptr,
	       const oski_index_t* restrict ind,
	       const oski_index_t* restrict valptr,
	       const oski_value_t* restrict val,
	       oski_value_t alpha,
	       const oski_value_t* restrict x, oski_index_t incx,
	       oski_value_t* restrict y, oski_index_t incy)
{
  if (incx == 1)
    {
      oski_index_t I;
      for (I = 0; I < M; I++)
	{
	  oski_index_t R = brow[I+1] - brow[I];
	  oski_index_t k;
	  for (k = ptr[I]; k < ptr[I+1]; k++)
	    {
	      oski_index_t K = ind[k];
	      oski_index_t j0 = bcol[K];
	      oski_index_t C = bcol[K+1] - j0;
	      const oski_value_t* vp = val + valptr[k];
	      oski_value_t* yp = y + brow[I]*incy;
	      oski_index_t di;
	      for (di = 0; di < R; di++, yp += incy)
		{
		  oski_value_t _y0 = MAKE_VAL_COMPLEX(0, 0);
		  const oski_value_t* xp = x + j0;
		  oski_index_t dj;
		  for (dj = 0; dj < C; dj++, xp++)
		    {
		      VAL_MAC (_y0, vp[0], xp[0]);
		      vp++;
		    }
		  VAL_MAC (yp[0], alpha, _y0);
		}
	    }
	}
    }
  else /* incx != 1 */
    {
      oski_index_t I;
      for (I = 0; I < M; I++)
	{
	  oski_index_t R = brow[I+1] - brow[I];
	  oski_index_t k;
	  for (k = ptr[I]; k < ptr[I+1]; k++)
	    {
	      oski_index_t K = ind[k];
	      oski_index_t j0 = bcol[K];
	      oski_index_t C = bcol[K+1] - j0;
	      const oski_value_t* vp = val + valptr[k];
	      oski_value_t* yp = y + brow[I]*incy;
	      oski_index_t di;
	      for (di = 0; di < R; di++, yp += incy)
		{
		  oski_value_t _y0 = MAKE_VAL_COMPLEX(0, 0);
		  const oski_value_t* xp = x + j0*incx;
		  oski_index_t dj;
		  for (dj = 0; dj < C; dj++, xp += incx)
		    {
		      VAL_MAC (_y0, vp[0], xp[0]);
		      vp++;
		    }
		  VAL_MAC (yp[0], alpha, _y0);
		}
	    }
	}
    }
  return 0;
}

#if IS_VAL_COMPLEX
static int
MatMultConj (oski_index_t M, oski_index_t N,
	     const oski_index_t* restrict brow,
	     const oski_index_t* restrict bcol,
	     const oski_index_t* restrict ptr,
	     const oski_index_t* restrict ind,
	     const oski_index_t* restrict valptr,
	     const oski_value_t* restrict val,
	     oski_value_t alpha,
	     const oski_value_t* restrict x, oski_index_t incx,
	     oski_value_t* restrict y, oski_index_t incy)
{
  if (incx == 1)
    {
      oski_index_t I;
      for (I = 0; I < M; I++)
	{
	  oski_index_t R = brow[I+1] - brow[I];
	  oski_index_t k;
	  for (k = ptr[I]; k < ptr[I+1]; k++)
	    {
	      oski_index_t K = ind[k];
	      oski_index_t j0 = bcol[K];
	      oski_index_t C = bcol[K+1] - j0;
	      const oski_value_t* vp = val + valptr[k];
	      oski_value_t* yp = y + brow[I]*incy;
	      oski_index_t di;
	      for (di = 0; di < R; di++, yp += incy)
		{
		  oski_value_t _y0 = MAKE_VAL_COMPLEX(0, 0);
		  const oski_value_t* xp = x + j0;
		  oski_index_t dj;
		  for (dj = 0; dj < C; dj++, xp++)
		    {
		      VAL_MAC (_y0, vp[0], xp[0]);
		      vp++;
		    }
		  VAL_MAC (yp[0], alpha, _y0);
		}
	    }
	}
    }
  else /* incx != 1 */
    {
      oski_index_t I;
      for (I = 0; I < M; I++)
	{
	  oski_index_t R = brow[I+1] - brow[I];
	  oski_index_t k;
	  for (k = ptr[I]; k < ptr[I+1]; k++)
	    {
	      oski_index_t K = ind[k];
	      oski_index_t j0 = bcol[K];
	      oski_index_t C = bcol[K+1] - j0;
	      const oski_value_t* vp = val + valptr[k];
	      oski_value_t* yp = y + brow[I]*incy;
	      oski_index_t di;
	      for (di = 0; di < R; di++, yp += incy)
		{
		  oski_value_t _y0 = MAKE_VAL_COMPLEX(0, 0);
		  const oski_value_t* xp = x + j0*incx;
		  oski_index_t dj;
		  for (dj = 0; dj < C; dj++, xp += incx)
		    {
		      VAL_MAC_CONJ (_y0, vp[0], xp[0]);
		      vp++;
		    }
		  VAL_MAC (yp[0], alpha, _y0);
		}
	    }
	}
    }
  return 0;
}
#endif

static int
MatMultTrans (oski_index_t M, oski_index_t N,
	     const oski_index_t* restrict brow,
	     const oski_index_t* restrict bcol,
	     const oski_index_t* restrict ptr,
	     const oski_index_t* restrict ind,
	     const oski_index_t* restrict valptr,
	     const oski_value_t* restrict val,
	     oski_value_t alpha,
	     const oski_value_t* restrict x, oski_index_t incx,
	     oski_value_t* restrict y, oski_index_t incy)
{
  if (incy == 1)
    {
      oski_index_t I;
      for (I = 0; I < M; I++)
	{
	  oski_index_t R = brow[I+1] - brow[I];
	  oski_index_t k;
	  for (k = ptr[I]; k < ptr[I+1]; k++)
	    {
	      oski_index_t K = ind[k];
	      oski_index_t j0 = bcol[K];
	      oski_index_t C = bcol[K+1] - j0;
	      const oski_value_t* vp = val + valptr[k];
	      const oski_value_t* xp = x + brow[I]*incx;
	      oski_index_t di;
	      for (di = 0; di < R; di++, xp += incx)
		{
		  oski_value_t _x0;
		  oski_index_t dj;
		  oski_value_t* yp = y + j0;
		  VAL_MUL (_x0, alpha, xp[0]);
		  for (dj = 0; dj < C; dj++, yp++)
		    {
		      VAL_MAC (yp[0], vp[0], _x0);
		      vp++;
		    }
		}
	    }
	}
    }
  else /* incy != 1 */
    {
      oski_index_t I;
      for (I = 0; I < M; I++)
	{
	  oski_index_t R = brow[I+1] - brow[I];
	  oski_index_t k;
	  for (k = ptr[I]; k < ptr[I+1]; k++)
	    {
	      oski_index_t K = ind[k];
	      oski_index_t j0 = bcol[K];
	      oski_index_t C = bcol[K+1] - j0;
	      const oski_value_t* vp = val + valptr[k];
	      const oski_value_t* xp = x + brow[I]*incx;
	      oski_index_t di;
	      for (di = 0; di < R; di++, xp += incx)
		{
		  oski_value_t _x0;
		  oski_index_t dj;
		  oski_value_t* yp = y + j0*incy;
		  VAL_MUL (_x0, alpha, xp[0]);
		  for (dj = 0; dj < C; dj++, yp += incy)
		    {
		      VAL_MAC (yp[0], vp[0], _x0);
		      vp++;
		    }
		}
	    }
	}
    }
  return 0;
}

#if IS_VAL_COMPLEX
static int
MatMultConjTrans (oski_index_t M, oski_index_t N,
		  const oski_index_t* restrict brow,
		  const oski_index_t* restrict bcol,
		  const oski_index_t* restrict ptr,
		  const oski_index_t* restrict ind,
		  const oski_index_t* restrict valptr,
		  const oski_value_t* restrict val,
		  oski_value_t alpha,
		  const oski_value_t* restrict x, oski_index_t incx,
		  oski_value_t* restrict y, oski_index_t incy)
{
  if (incy == 1)
    {
      oski_index_t I;
      for (I = 0; I < M; I++)
	{
	  oski_index_t R = brow[I+1] - brow[I];
	  oski_index_t k;
	  for (k = ptr[I]; k < ptr[I+1]; k++)
	    {
	      oski_index_t K = ind[k];
	      oski_index_t j0 = bcol[K];
	      oski_index_t C = bcol[K+1] - j0;
	      const oski_value_t* vp = val + valptr[k];
	      const oski_value_t* xp = x + brow[I]*incx;
	      oski_index_t di;
	      for (di = 0; di < R; di++, xp += incx)
		{
		  oski_value_t _x0;
		  oski_index_t dj;
		  oski_value_t* yp = y + j0;
		  VAL_MUL (_x0, alpha, xp[0]);
		  for (dj = 0; dj < C; dj++, yp++)
		    {
		      VAL_MAC_CONJ (yp[0], vp[0], _x0);
		      vp++;
		    }
		}
	    }
	}
    }
  else /* incy != 1 */
    {
      oski_index_t I;
      for (I = 0; I < M; I++)
	{
	  oski_index_t R = brow[I+1] - brow[I];
	  oski_index_t k;
	  for (k = ptr[I]; k < ptr[I+1]; k++)
	    {
	      oski_index_t K = ind[k];
	      oski_index_t j0 = bcol[K];
	      oski_index_t C = bcol[K+1] - j0;
	      const oski_value_t* vp = val + valptr[k];
	      const oski_value_t* xp = x + brow[I]*incx;
	      oski_index_t di;
	      for (di = 0; di < R; di++, xp += incx)
		{
		  oski_value_t _x0;
		  oski_index_t dj;
		  oski_value_t* yp = y + j0*incy;
		  VAL_MUL (_x0, alpha, xp[0]);
		  for (dj = 0; dj < C; dj++, yp += incy)
		    {
		      VAL_MAC_CONJ (yp[0], vp[0], _x0);
		      vp++;
		    }
		}
	    }
	}
    }
  return 0;
}
#endif

static int
MatMult (oski_index_t M, oski_index_t N,
	 const oski_index_t* restrict brow, const oski_index_t* restrict bcol,
	 const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	 const oski_index_t* restrict valptr, const oski_value_t* restrict val,
	 oski_matop_t opA,
	 oski_value_t alpha, const oski_value_t* restrict x,
	 oski_index_t incx, oski_value_t* restrict y, oski_index_t incy)
{
  switch (opA)
    {
      case OP_CONJ:
#if IS_VAL_COMPLEX
      return MatMultConj (M, N, brow, bcol, ptr, ind, valptr, val,
			  alpha, x, incx, y, incy);
      break;
#endif
    case OP_NORMAL:
      return MatMultNormal (M, N, brow, bcol, ptr, ind, valptr, val,
			    alpha, x, incx, y, incy);
      break;

    case OP_CONJ_TRANS:
#if IS_VAL_COMPLEX
      return MatMultConjTrans (M, N, brow, bcol, ptr, ind, valptr, val,
			       alpha, x, incx, y, incy);
      break;
#endif
    case OP_TRANS:
      return MatMultTrans (M, N, brow, bcol, ptr, ind, valptr, val,
			   alpha, x, incx, y, incy);
      break;
    default:
      return ERR_NOT_IMPLEMENTED;
    }
}

int
oski_MatReprMult (const void *pA, const oski_matcommon_t * props,
		  oski_matop_t opA,
		  oski_value_t alpha, const oski_vecview_t x_view,
		  oski_value_t beta, oski_vecview_t y_view)
{
  const oski_matVBR_t* A = (const oski_matVBR_t *)pA;

  assert (A != NULL);
  assert (props != NULL);

  oski_ScaleVecView (y_view, beta);
  if (IS_VAL_ZERO(alpha))
    return 0;
  else
    {
      oski_index_t k;
      const oski_value_t* xp = x_view->val;
      oski_value_t* yp = y_view->val;
      for (k = 0; k < x_view->num_cols; k++)
	{
	  int err = MatMult (A->mb, A->nb, A->brow, A->bcol,
			     A->ptr, A->ind, A->valptr, A->val, opA,
			     alpha, xp, x_view->rowinc, yp, y_view->rowinc);
	  if (err) return err;
	  xp += x_view->colinc;
	  yp += y_view->colinc;
	}
    }
  return 0;
}

/* eof */
