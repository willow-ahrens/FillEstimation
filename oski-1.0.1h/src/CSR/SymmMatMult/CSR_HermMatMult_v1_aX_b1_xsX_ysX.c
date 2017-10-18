
/**
 *  \file CSR_HermMatMult_v1_aX_b1_xsX_ysX.c
 *  \f$y \leftarrow y + \alpha\cdot op(A)\cdot x\f$,
 *  where \f$A\f$ is Hermitian (i.e., \f$A = A^H\f$), \f$op(A) = A\f$, \f$\alpha = \f$ (a general value),
 *  x is general-stride accessible, and y is general-stride accessible.
 *  \ingroup MATTYPE_CSR
 */

#include "CSR_HEADER.c"

/**
 *  \brief Computes
 *  \f$y \leftarrow y + \alpha\cdot op(A)\cdot x\f$,
 *  where \f$A\f$ is Hermitian (i.e., \f$A = A^H\f$), \f$op(A) = A\f$, \f$\alpha = \f$ (a general value),
 *  x is general-stride accessible, and y is general-stride accessible.
 *  \ingroup MATTYPE_CSR
 *
 *  \pre Column indices must be sorted in ascending order.
 *  \pre Unlike non-symmetric case, the input pointers should
 *  not be index-adjusted yet!
 */
void
CSR_HermMatMult_v1_aX_b1_xsX_ysX( oski_index_t m, oski_index_t n,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val, oski_index_t index_base
	, oski_value_t alpha, const oski_value_t* restrict x , oski_index_t incx,
	oski_value_t* restrict y , oski_index_t incy )
{
	oski_index_t i;
	const oski_value_t* px;
	oski_value_t* py;

	for( i = 0, py = y, px = x; i < m; i++, px += incx, py += incy )
	{
		oski_index_t k;
		oski_index_t nnz_i = ptr[i+1] - ptr[i]; /* nnz in row i */

		/* alpha * x(i) */
		register oski_value_t _x0;
		register oski_value_t _y0;
		oski_value_t _y_diag; /* Stores 'alpha * a(i,i) * x(i)' */

		if( nnz_i == 0 ) continue;

		/* assert( nnz_i >= 1 ); */

		VAL_MUL( _x0, alpha, px[0] );
		VAL_SET_ZERO( _y0 );
		VAL_SET_ZERO( _y_diag );

		/* check for diagonal element if upper triangular */
		k = ptr[i] - index_base;
		if( ind[k] == (i+index_base) )
		{
			VAL_MUL( _y_diag, val[k], _x0 );
				/* _y_diag = opc(val[k]) * _x0; */

			if( nnz_i == 1 ) /* if that was the only non-zero in row i */
			{
				VAL_INC( py[0], _y_diag ); /* py[0] += _y_diag; */
				continue;
			}
			else
				k++;  /* assert( nnz_i >= 2 ); */
		}

		/* assert( k < ptr[i+1]-index_base ); */

		for( ; k < ptr[i+1]-index_base-1; k++ )
		{
			oski_index_t j = ind[k] - index_base;  /* 0-based col index */
			register oski_value_t a_ij = val[k];

			/* y(i) += opc(A(i, j)) * x(j) */
			VAL_MAC( _y0, a_ij, x[j * incx] );
				/* _y0 += a_ij * x[j * incx]; */

			/* y(j) += a_ij * alpha * x(i) */
			VAL_MAC_CONJ( y[j * incy], a_ij, _x0 );
				/* y[j * incy] += opc(a_ij) * _x0; */
		}

		/* assert( k < ptr[i+1]-index_base ); */

		/* check for diagonal element if lower triangular */
		if( ind[k] == (i+index_base) )
		{
			VAL_MUL( _y_diag, val[k], _x0 );
				/* _y_diag = opc(val[k]) * _x0; */
		}
		else
		{
			oski_index_t j = ind[k] - index_base;  /* 0-based col index */
			register oski_value_t a_ij = val[k];

			/* y(i) += opc(A(i, j)) * x(j) */
			VAL_MAC( _y0, a_ij, x[j * incx] );
				/* _y0 += opc(a_ij) * x[j * incx]; */

			/* y(j) += opc(a_ij) * alpha * x(i) */
			VAL_MAC_CONJ( y[j * incy], a_ij, _x0 );
				/* y[j * incy] += opc(a_ij) * _x0; */
		}

		/* store result for y(i) */
		VAL_MAC( _y_diag, alpha, _y0 );
		VAL_INC( py[0], _y_diag );
			/* py[0] += _y_diag + alpha * _y0; */
	}
}

/* finished on: Fri Feb 25 13:16:19 PST 2005 */
/* eof */

