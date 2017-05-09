#include "CSR_HEADER.c"
void
CSR_MatConjMult_v1_aX_b1_xs1_ysX( oski_index_t A_M,
    oski_index_t A_N, const oski_index_t* restrict A_ptr, const
    oski_index_t* restrict A_ind, const oski_value_t* restrict
    A_val, oski_value_t alpha, const oski_value_t* restrict x,
    oski_value_t* restrict y, oski_index_t ystride )
{
	oski_value_t* p;
	oski_index_t i;
	
	
	for( i = 0, p = y; i < A_M; i++, p += ystride )
	{
		const oski_value_t* pa;
		oski_index_t j;
		oski_value_t _y0;
		
		_RE(_y0) = 0;
		_IM(_y0) = 0;
		
		for( j = A_ptr[i], pa = A_val + A_ptr[i]; j < A_ptr[i +
		    1]; j++, pa++ )
		{
			oski_value_t _x0;
			oski_index_t k;
			
			k = A_ind[j];
			_RE(_x0) = _RE(x[k]);
			_IM(_x0) = _IM(x[k]);
			
			_RE(_y0) = _RE(_y0) + _RE(pa[0]) * _RE(_x0) +
			    _IM(pa[0]) * _IM(_x0);
			_IM(_y0) = _IM(_y0) + _RE(pa[0]) * _IM(_x0) -
			    _IM(pa[0]) * _RE(_x0);
			
		}
		_RE(p[0]) = _RE(p[0]) + _RE(alpha) * _RE(_y0) -
		    _IM(alpha) * _IM(_y0);
		_IM(p[0]) = _IM(p[0]) + _RE(alpha) * _IM(_y0) +
		    _IM(alpha) * _RE(_y0);
	}
}

/* finished on Tue Nov 23 15:06:12 PST 2004 */
/* eof */
