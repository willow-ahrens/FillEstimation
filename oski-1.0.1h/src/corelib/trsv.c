/**
 *  \file trsv.c
 *  \brief OSKI wrapper around the dense BLAS routine, xTRSV.
 *  \ingroup AUXIL
 */

#include <assert.h>
#include <ctype.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/mangle.h>
#include <oski/blas.h>
#include <oski/blas_names.h>

#if !HAVE_BLAS

/**
 *  This implementation translated from the reference BLAS
 *  code available at http://netlib.org/blas/?trsv.f
 */
static void
xTRSV (char uplo, char trans, char diag, oski_index_t n,
       const oski_value_t* restrict A, oski_index_t lda,
       oski_value_t* restrict x, oski_index_t incx)
{
  int nounit = tolower(diag) == 'n';

  /* Set up the start point in X if increment is not unity.
   * This will be (n-1)*incx too small for descending loops.
   */
  oski_index_t kx = (incx <= 0) ? (-(n-1)*incx) : 0;

  /* Quick return if possible. */
  if (n == 0) return;

  /* Start the operations. In this version the elements of A are
   * accessed sequentially with one pass through A.
   */
  if (tolower(trans) == 'n')
    {
      /* Form  x := inv( A )*x. */
      if (tolower(uplo) == 'u')
	{
	  if (incx == 1)
	    {
	      oski_index_t j;
	      for (j = n-1; j >= 0; j--)
		{
		  if (!IS_VAL_ZERO(x[j]))
		    {
		      oski_value_t temp;
		      oski_index_t i;
		      const oski_value_t* Ap = A + j*lda;
		      VAL_ASSIGN (temp, x[j]);
		      if (nounit)
			{
			  oski_value_t ajj;
			  VAL_ASSIGN (ajj, Ap[j]);
			  VAL_DIVEQ (temp, ajj);
			  VAL_ASSIGN (x[j], temp);
			}
		      for (i = j-1; i >= 0; i--)
			{
			  oski_value_t aij;
			  VAL_ASSIGN (aij, Ap[i]);
			  VAL_MSUB (x[i], temp, aij);
			}
		    }
		} /* j */
	    }
	  else /* incx != 1 */
	    {
	      oski_index_t jx = kx + (n-1)*incx;
	      oski_index_t j;
	      for (j = n-1; j >= 0; j--)
		{
		  if (!IS_VAL_ZERO(x[jx]))
		    {
		      oski_value_t temp;
		      oski_index_t ix = jx;
		      oski_index_t i;
		      const oski_value_t* Ap = A + j*lda;
		      VAL_ASSIGN (temp, x[jx]);
		      if (nounit)
			{
			  oski_value_t ajj;
			  VAL_ASSIGN (ajj, Ap[j]);
			  VAL_DIVEQ (temp, ajj);
			  VAL_ASSIGN (x[jx], temp);
			}
		      for (i = j-1; i >= 0; i--)
			{
			  oski_value_t aij;
			  VAL_ASSIGN (aij, Ap[i]);
			  ix -= incx;
			  VAL_MSUB (x[ix], temp, aij);
			} /* i */
		    }
		  jx -= incx;
		} /* j */
	    }
	}
      else /* tolower(uplo) == 'l' */
	{
	  if (incx == 1)
	    {
	      oski_index_t j;
	      for (j = 0; j < n; j++)
		{
		  oski_value_t temp;
		  const oski_value_t* Ap = A + j*lda;
		  VAL_ASSIGN (temp, x[j]);
		  if (!IS_VAL_ZERO(temp))
		    {
		      oski_index_t i;
		      if (nounit)
			{
			  oski_value_t ajj;
			  VAL_ASSIGN (ajj, Ap[j]);
			  VAL_DIVEQ (temp, ajj);
			  VAL_ASSIGN (x[j], temp);
			}
		       for (i = j+1; i < n; i++)
			 {
			   oski_value_t aij;
			   VAL_ASSIGN (aij, Ap[i]);
			   VAL_MSUB (x[i], temp, aij);
			 } /* i */
		    }
		}
	    }
	  else /* incx != 1 */
	    {
	      oski_index_t j, jx;
	      for (j = 0, jx = kx; j < n; j++, jx += incx)
		{
		  oski_value_t temp;
		  VAL_ASSIGN (temp, x[jx]);
		  if (!IS_VAL_ZERO(temp))
		    {
		      const oski_value_t* Ap = A + j*lda;
		      oski_index_t i, ix;
		      if (nounit)
			{
			  oski_value_t ajj;
			  VAL_ASSIGN (ajj, Ap[j]);
			  VAL_DIVEQ (temp, ajj);
			  VAL_ASSIGN (x[jx], temp);
			}
		      for (i = j+1, ix = jx + incx; i < n; i++, ix += incx)
			{
			  oski_value_t aij;
			  VAL_ASSIGN (aij, Ap[i]);
			  VAL_MSUB (x[ix], aij, temp);
			} /* i */
		    }
		} /* j */
	    }
	}
    }
  else /* tolower(trans) == 't' || tolower(trans) == 'c' */
    {
      /* Form  x := inv( A' )*x  or  x := inv( conjg( A' ) )*x. */

      int noconj = tolower(trans) == 't';
      if (tolower(uplo) == 'u')
	{
	  if (incx == 1)
	    {
	      oski_index_t j;
	      for (j = 0; j < n; j++)
		{
		  const oski_value_t* Ap = A + j*lda;
		  oski_value_t temp;
		  VAL_ASSIGN (temp, x[j]);
		  if (noconj)
		    {
		      oski_index_t i;
		      for (i = 0; i < j; i++)
			{
			  oski_value_t aij;
			  VAL_ASSIGN (aij, Ap[i]);
			  VAL_MSUB (temp, aij, x[i]);
			}
		      if (nounit)
			{
			  oski_value_t ajj;
			  VAL_ASSIGN (ajj, Ap[j]);
			  VAL_DIVEQ (temp, ajj);
			}
		    }
		  else /* conjugate */
		    {
		      oski_index_t i;
		      for (i = 0; i < j; i++)
			{
			  oski_value_t aij;
			  VAL_ASSIGN (aij, Ap[i]);
			  VAL_MSUB_CONJ (temp, aij, x[i]);
			}
		      if (nounit)
			{
			  oski_value_t ajj;
			  VAL_ASSIGN (ajj, Ap[j]);
			  VAL_DIVEQ_CONJ (temp, ajj);
			}
		    }
		  VAL_ASSIGN (x[j], temp);
		} /* j */
	    }
	  else /* incx != 1 */
	    {
	      oski_index_t jx = kx;
	      oski_index_t j;
	      for (j = 0; j < n; j++) /* 140 */
		{
		  const oski_value_t* Ap = A + j*lda;
		  oski_index_t ix = kx;
		  oski_value_t temp;
		  VAL_ASSIGN (temp, x[jx]);
		  if (noconj)
		    {
		      oski_index_t i;
		      for (i = 0; i < j; i++) /* 120 */
			{
			  oski_value_t aij;
			  VAL_ASSIGN (aij, Ap[i]);
			  VAL_MSUB (temp, aij, x[ix]);
			  ix += incx;
			} /* i, 120 */
		      if (nounit)
			{
			  oski_value_t ajj;
			  VAL_ASSIGN (ajj, Ap[j]);
			  VAL_DIVEQ (temp, ajj);
			}
		    }
		  else /* conj */
		    {
		      oski_index_t i;
		      for (i = 0; i < j; i++) /* 120 */
			{
			  oski_value_t aij;
			  VAL_ASSIGN (aij, Ap[i]);
			  VAL_MSUB_CONJ (temp, aij, x[ix]);
			  ix += incx;
			} /* i, 120 */
		      if (nounit)
			{
			  oski_value_t ajj;
			  VAL_ASSIGN (ajj, Ap[j]);
			  VAL_DIVEQ_CONJ (temp, ajj);
			}
		    }
		  VAL_ASSIGN (x[jx], temp);
		  jx += incx;
		} /* j, 140 */
	    }
	}
      else /* tolower(uplo) == 'l' */
	{
	  if (incx == 1)
	    {
	      oski_index_t j;
	      for (j = n-1; j >= 0; j--) /* 170 */
		{
		  const oski_value_t* Ap = A + j*lda;
		  oski_value_t temp;
		  VAL_ASSIGN (temp, x[j]);
		  if (noconj)
		    {
		      oski_index_t i;
		      for (i = n-1; i > j; i--) /* 150 */
			{
			  oski_value_t aij;
			  VAL_ASSIGN (aij, Ap[i]);
			  VAL_MSUB (temp, aij, x[i]);
			} /* i, 150 */
		      if (nounit)
			{
			  oski_value_t ajj;
			  VAL_ASSIGN (ajj, Ap[j]);
			  VAL_DIVEQ (temp, ajj);
			}
		    }
		  else /* conj */
		    {
		      oski_index_t i;
		      for (i = n-1; i > j; i--)
			{
			  oski_value_t aij;
			  VAL_ASSIGN (aij, Ap[i]);
			  VAL_MSUB_CONJ (temp, aij, x[i]);
			}
		      if (nounit)
			{
			  oski_value_t ajj;
			  VAL_ASSIGN (ajj, Ap[j]);
			  VAL_DIVEQ_CONJ (temp, ajj);
			}
		    }
		  VAL_ASSIGN (x[j], temp);
		} /* j, 170 */
	    }
	  else /* incx != 1 */
	    {
	      oski_index_t j, jx;
	      kx += (n-1) * incx;
	      jx = kx;
	      for (j = n-1; j >= 0; j--) /* 200 */
		{
		  const oski_value_t* Ap = A + j*lda;
		  oski_index_t ix = kx;
		  oski_value_t temp;
		  VAL_ASSIGN (temp, x[jx]);
		  if (noconj)
		    {
		      oski_index_t i;
		      for (i = n-1; i > j; i--) /* 180 */
			{
			  oski_value_t aij;
			  VAL_ASSIGN (aij, Ap[i]);
			  VAL_MSUB (temp, aij, x[ix]);
			  ix -= incx;
			} /* i, 180 */
		      if (nounit)
			{
			  oski_value_t ajj;
			  VAL_ASSIGN (ajj, Ap[j]);
			  VAL_DIVEQ (temp, ajj);
			}
		    }
		  else /* conj */
		    {
		      oski_index_t i;
		      for (i = n-1; i > j; i--) /* 190 */
			{
			  oski_value_t aij;
			  VAL_ASSIGN (aij, Ap[i]);
			  VAL_MSUB_CONJ (temp, aij, x[ix]);
			  ix -= incx;
			} /* i, 190 */
		      if (nounit)
			{
			  oski_value_t ajj;
			  VAL_ASSIGN (ajj, Ap[j]);
			  VAL_DIVEQ_CONJ (temp, ajj);
			}
		    }
		  VAL_ASSIGN (x[jx], temp);
		  jx -= incx;
		}
	    }
	}
    }
}

void
BLAS_xTRSV (const char* uplo, const char* trans, const char* diag,
	    const oski_index_t* n,
	    const oski_value_t* A, const oski_index_t* lda,
	    oski_value_t* x, const oski_index_t* incx)
{
  xTRSV (uplo[0], trans[0], diag[0], *n, A, *lda, x, *incx);
}

#endif

/* eof */
