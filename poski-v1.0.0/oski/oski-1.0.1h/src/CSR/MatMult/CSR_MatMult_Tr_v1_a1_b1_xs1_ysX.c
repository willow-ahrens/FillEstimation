#include "CSR_HEADER.c"
void
CSR_MatMult_v1_a1_b1_xs1_ysX( oski_index_t A_M, oski_index_t
    A_N, const oski_index_t* restrict A_ptr, const oski_index_t*
    restrict A_ind, const oski_value_t* restrict A_val, const
    oski_value_t* restrict x, oski_value_t* restrict y,
    oski_index_t ystride )
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
			_x0 = x[k];
			
			_y0 += pa[0] * _x0;
			
		}
		p[0] = p[0] + _y0;
	}
}

/* finished on Tue Nov 23 15:06:02 PST 2004 */
/* eof */
