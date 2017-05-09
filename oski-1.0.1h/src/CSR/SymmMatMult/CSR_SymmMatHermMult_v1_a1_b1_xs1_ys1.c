
/**
 *  \file CSR_SymmMatHermMult_v1_a1_b1_xs1_ys1.c
 *  \f$y \leftarrow y + \alpha\cdot op(A)\cdot x\f$,
 *  where \f$A\f$ is symmetric (i.e., \f$A = A^T\f$), \f$op(A) = A^H\f$, \f$\alpha = 1\f$,
 *  x is unit-stride accessible, and y is unit-stride accessible.
 *  \ingroup MATTYPE_CSR
 */

#include "CSR_HEADER.c"

/**
 *  \brief Computes
 *  \f$y \leftarrow y + \alpha\cdot op(A)\cdot x\f$,
 *  where \f$A\f$ is symmetric (i.e., \f$A = A^T\f$), \f$op(A) = A^H\f$, \f$\alpha = 1\f$,
 *  x is unit-stride accessible, and y is unit-stride accessible.
 *  \ingroup MATTYPE_CSR
 *
 *  \pre Column indices must be sorted in ascending order.
 *  \pre Unlike non-symmetric case, the input pointers should
 *  not be index-adjusted yet!
 */
void
CSR_SymmMatHermMult_v1_a1_b1_xs1_ys1( oski_index_t m, oski_index_t n,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val, oski_index_t index_base
	, const oski_value_t* restrict x ,
	oski_value_t* restrict y  )
{
	oski_index_t i;
	const oski_value_t* px;
	oski_value_t* py;

	for( i = 0, py = y, px = x; i < m; i++, px++, py++ )
	{
		oski_index_t k;
		oski_index_t nnz_i = ptr[i+1] - ptr[i]; /* nnz in row i */

		/* x(i) */
		register oski_value_t _x0;
		register oski_value_t _y0;
		oski_value_t _y_diag; /* Stores 'a(i,i) * x(i)' */

		if( nnz_i == 0 ) continue;

		/* assert( nnz_i >= 1 ); */

		VAL_ASSIGN(_x0, px[0]);
		VAL_SET_ZERO( _y0 );
		VAL_SET_ZERO( _y_diag );

		/* check for diagonal element if upper triangular */
		k = ptr[i] - index_base;
		if( ind[k] == (i+index_base) )
		{
			VAL_MUL_CONJ( _y_diag, val[k], _x0 );
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
			VAL_MAC_CONJ( _y0, a_ij, x[j] );
				/* _y0 += a_ij * x[j]; */

			/* y(j) += a_ij * x(i) */
			VAL_MAC_CONJ( y[j], a_ij, _x0 );
				/* y[j] += opc(a_ij) * _x0; */
		}

		/* assert( k < ptr[i+1]-index_base ); */

		/* check for diagonal element if lower triangular */
		if( ind[k] == (i+index_base) )
		{
			VAL_MUL_CONJ( _y_diag, val[k], _x0 );
				/* _y_diag = opc(val[k]) * _x0; */
		}
		else
		{
			oski_index_t j = ind[k] - index_base;  /* 0-based col index */
			register oski_value_t a_ij = val[k];

			/* y(i) += opc(A(i, j)) * x(j) */
			VAL_MAC_CONJ( _y0, a_ij, x[j] );
				/* _y0 += opc(a_ij) * x[j]; */

			/* y(j) += opc(a_ij) * x(i) */
			VAL_MAC_CONJ( y[j], a_ij, _x0 );
				/* y[j] += opc(a_ij) * _x0; */
		}

		/* store result for y(i) */
		VAL_INC(_y_diag, _y0);
		VAL_INC( py[0], _y_diag );
			/* py[0] += _y_diag + _y0; */
	}
}

/* finished on: Fri Feb 25 13:16:18 PST 2005 */
/* eof */

