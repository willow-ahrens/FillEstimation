#include "CSR_HEADER.c"
void
CSR_MatMult_v1_aX_b1_xsX_ysX( oski_index_t A_M, oski_index_t
    A_N, const oski_index_t* restrict A_ptr, const oski_index_t*
    restrict A_ind, const oski_value_t* restrict A_val,
    oski_value_t alpha, const oski_value_t* restrict x,
    oski_index_t xstride, oski_value_t* restrict y, oski_index_t
    ystride )
{
	oski_value_t* p;
	oski_index_t i;
	
	
	for( i = 0, p = y; i < A_M; i++, p += ystride )
	{
		register oski_value_t _y0;
		oski_index_t j;
		const oski_value_t* pa;
		
		_y0 = 0;
		
		for( j = A_ptr[i], pa = A_val + A_ptr[i]; j < A_ptr[i +
		    1]; j++, pa++ )
		{
			oski_index_t k;
			register oski_value_t _x0;
			
			k = A_ind[j];
			_x0 = x[xstride * k];
			
			_y0 += pa[0] * _x0;
			
		}
		p[0] = p[0] + alpha * _y0;
	}
}

/* finished on Tue Nov 23 15:06:03 PST 2004 */
/* eof */
