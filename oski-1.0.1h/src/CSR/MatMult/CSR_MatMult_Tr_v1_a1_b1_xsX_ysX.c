#include "CSR_HEADER.c"
void
CSR_MatMult_v1_a1_b1_xsX_ysX( oski_index_t A_M, oski_index_t
    A_N, const oski_index_t* restrict A_ptr, const oski_index_t*
    restrict A_ind, const oski_value_t* restrict A_val, const
    oski_value_t* restrict x, oski_index_t xstride, oski_value_t*
    restrict y, oski_index_t ystride )
{
	oski_index_t i;
	oski_value_t* p;

	for( i = 0, p = y; i < A_M; i++, p += ystride )
	{
		const oski_value_t* pa;
		oski_index_t j;
		register oski_value_t _y0;
		
		_y0 = 0;
		
		for( j = A_ptr[i], pa = A_val + A_ptr[i]; j < A_ptr[i +
		    1]; j++, pa++ )
		{
			register oski_value_t _x0;
			oski_index_t k;
			
			k = A_ind[j];
			_x0 = x[xstride * k];
			
			_y0 += pa[0] * _x0;
		}
		p[0] = p[0] + _y0;
	}
}

/* finished on Tue Nov 23 15:06:03 PST 2004 */
/* eof */
