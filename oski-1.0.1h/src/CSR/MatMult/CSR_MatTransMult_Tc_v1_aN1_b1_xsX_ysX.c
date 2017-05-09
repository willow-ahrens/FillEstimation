#include "CSR_HEADER.c"
void
CSR_MatTransMult_v1_aN1_b1_xsX_ysX( oski_index_t A_M,
    oski_index_t A_N, const oski_index_t* restrict A_ptr, const
    oski_index_t* restrict A_ind, const oski_value_t* restrict
    A_val, const oski_value_t* restrict x, oski_index_t xstride,
    oski_value_t* restrict y, oski_index_t ystride )
{
	oski_index_t i;
	
	
	for( i = 0; i < A_M; i++ )
	{
		oski_index_t j;
		const oski_value_t* p;
		oski_value_t _y0;
		
		_RE(_y0) = -_RE(x[xstride * i]);
		_IM(_y0) = -_IM(x[xstride * i]);
		
		for( j = A_ptr[i], p = A_val + A_ptr[i]; j < A_ptr[i +
		    1]; j++, p++ )
		{
			oski_value_t _z0;
			oski_index_t k;
			
			k = A_ind[j];
			_RE(_z0) = 0;
			_IM(_z0) = 0;
			
			_RE(_z0) = _RE(_z0) + _RE(p[0]) * _RE(_y0) -
			    _IM(p[0]) * _IM(_y0);
			_IM(_z0) = _IM(_z0) + _RE(p[0]) * _IM(_y0) +
			    _IM(p[0]) * _RE(_y0);
			
			_RE(y[ystride * k]) = _RE(y[ystride * k]) + _RE(_z0);
			_IM(y[ystride * k]) = _IM(y[ystride * k]) + _IM(_z0);
		}
	}
}

/* finished on Tue Nov 23 15:06:07 PST 2004 */
/* eof */
