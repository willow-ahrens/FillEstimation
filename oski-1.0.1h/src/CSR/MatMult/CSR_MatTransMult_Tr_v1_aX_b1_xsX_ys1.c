#include "CSR_HEADER.c"
void
CSR_MatTransMult_v1_aX_b1_xsX_ys1( oski_index_t A_M,
    oski_index_t A_N, const oski_index_t* restrict A_ptr, const
    oski_index_t* restrict A_ind, const oski_value_t* restrict
    A_val, oski_value_t alpha, const oski_value_t* restrict x,
    oski_index_t xstride, oski_value_t* restrict y )
{
	oski_index_t i;
	
	
	for( i = 0; i < A_M; i++ )
	{
		register oski_value_t _y0;
		oski_index_t j;
		const oski_value_t* p;
		
		_y0 = alpha * x[xstride * i];
		
		for( j = A_ptr[i], p = A_val + A_ptr[i]; j < A_ptr[i +
		    1]; j++, p++ )
		{
			oski_index_t k;
			register oski_value_t _z0;
			
			k = A_ind[j];
			_z0 = 0;
			
			_z0 += p[0] * _y0;
			
			y[k] += _z0;
		}
	}
}

/* finished on Tue Nov 23 15:06:04 PST 2004 */
/* eof */
