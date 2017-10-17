/**
 *  \file src/MBCSR/MatTransMatMult/7x3.c
 *  \brief The \f$7\times 3\f$ MBCSR implementation
 *  of \f$A^TA\cdot x\f$ and \f$A^HA\cdot x\f$.
 *  \ingroup MATTYPE_MBCSR
 *
 *  Automatically generated by ./gen_ata.sh on Wed Jun  8 16:02:31 PDT 2005.
 */

#include <assert.h>

#include <oski/config.h> /* for 'restrict' keyword */
#include <oski/common.h>
#include <oski/mangle.h>
#include <oski/vecview.h>
#include <oski/MBCSR/format.h>
#include <oski/MBCSR/module.h>

#if IS_VAL_COMPLEX
	/** Complex-valued, so do not use explicit 'register' keyword. */
	#define REGISTER
#else
	/** Real-valued, so use explicit 'register' keyword. */
	#define REGISTER register
#endif

#if defined(DO_NAME_MANGLING)
/** Mangled name for MBCSR_MatTransMatMult_v1_aX_b1_xs1_ys1. */
#define MBCSR_MatTransMatMult_v1_aX_b1_xs1_ys1 \
	MANGLE_MOD_(MBCSR_MatTransMatMult_v1_aX_b1_xs1_ys1_7x3)
#endif

/**
 *  \brief The \f$7\times 3\f$ MBCSR implementation
 *  of \f$A^TA\cdot x\f$, where the source and destination
 *  vectors have unit-stride and unit-stride,
 *  respectively.
 *
 *  Parameter 'tp' may be NULL if no intermediate vector output
 *  is desired.
 */
void
MBCSR_MatTransMatMult_v1_aX_b1_xs1_ys1( oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val, const oski_value_t* restrict diag,
	oski_value_t alpha, const oski_value_t* restrict x,
	oski_value_t* restrict y,
	oski_value_t* restrict t, oski_index_t inct )
{
	oski_index_t I;
	oski_value_t* tp = t;

	for( I = 0; I < M; I++ )
	{
		oski_index_t K;
		const oski_value_t* vp;
		REGISTER oski_value_t _t0 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t1 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t2 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t3 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t4 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t5 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t6 = MAKE_VAL_COMPLEX(0.0, 0.0);

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 7*3 )
		{
			oski_index_t j0 = ind[K];
			const oski_value_t* xp = x + j0;
			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;
			REGISTER oski_value_t _x2;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1] );
			VAL_ASSIGN( _x2, xp[2] );
			VAL_MAC( _t0, vp[0], _x0 );
			VAL_MAC( _t1, vp[3], _x0 );
			VAL_MAC( _t2, vp[6], _x0 );
			VAL_MAC( _t3, vp[9], _x0 );
			VAL_MAC( _t4, vp[12], _x0 );
			VAL_MAC( _t5, vp[15], _x0 );
			VAL_MAC( _t6, vp[18], _x0 );
			VAL_MAC( _t0, vp[1], _x1 );
			VAL_MAC( _t1, vp[4], _x1 );
			VAL_MAC( _t2, vp[7], _x1 );
			VAL_MAC( _t3, vp[10], _x1 );
			VAL_MAC( _t4, vp[13], _x1 );
			VAL_MAC( _t5, vp[16], _x1 );
			VAL_MAC( _t6, vp[19], _x1 );
			VAL_MAC( _t0, vp[2], _x2 );
			VAL_MAC( _t1, vp[5], _x2 );
			VAL_MAC( _t2, vp[8], _x2 );
			VAL_MAC( _t3, vp[11], _x2 );
			VAL_MAC( _t4, vp[14], _x2 );
			VAL_MAC( _t5, vp[17], _x2 );
			VAL_MAC( _t6, vp[20], _x2 );
		}

		/* Diag block multiply */
		{
			const oski_value_t* xp = x + I*7;
			const oski_value_t* dp = diag;
			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;
			REGISTER oski_value_t _x2;
			REGISTER oski_value_t _x3;
			REGISTER oski_value_t _x4;
			REGISTER oski_value_t _x5;
			REGISTER oski_value_t _x6;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1] );
			VAL_ASSIGN( _x2, xp[2] );
			VAL_ASSIGN( _x3, xp[3] );
			VAL_ASSIGN( _x4, xp[4] );
			VAL_ASSIGN( _x5, xp[5] );
			VAL_ASSIGN( _x6, xp[6] );
			VAL_MAC( _t0, dp[0], _x0 );
			VAL_MAC( _t1, dp[7], _x0 );
			VAL_MAC( _t2, dp[14], _x0 );
			VAL_MAC( _t3, dp[21], _x0 );
			VAL_MAC( _t4, dp[28], _x0 );
			VAL_MAC( _t5, dp[35], _x0 );
			VAL_MAC( _t6, dp[42], _x0 );
			VAL_MAC( _t0, dp[1], _x1 );
			VAL_MAC( _t1, dp[8], _x1 );
			VAL_MAC( _t2, dp[15], _x1 );
			VAL_MAC( _t3, dp[22], _x1 );
			VAL_MAC( _t4, dp[29], _x1 );
			VAL_MAC( _t5, dp[36], _x1 );
			VAL_MAC( _t6, dp[43], _x1 );
			VAL_MAC( _t0, dp[2], _x2 );
			VAL_MAC( _t1, dp[9], _x2 );
			VAL_MAC( _t2, dp[16], _x2 );
			VAL_MAC( _t3, dp[23], _x2 );
			VAL_MAC( _t4, dp[30], _x2 );
			VAL_MAC( _t5, dp[37], _x2 );
			VAL_MAC( _t6, dp[44], _x2 );
			VAL_MAC( _t0, dp[3], _x3 );
			VAL_MAC( _t1, dp[10], _x3 );
			VAL_MAC( _t2, dp[17], _x3 );
			VAL_MAC( _t3, dp[24], _x3 );
			VAL_MAC( _t4, dp[31], _x3 );
			VAL_MAC( _t5, dp[38], _x3 );
			VAL_MAC( _t6, dp[45], _x3 );
			VAL_MAC( _t0, dp[4], _x4 );
			VAL_MAC( _t1, dp[11], _x4 );
			VAL_MAC( _t2, dp[18], _x4 );
			VAL_MAC( _t3, dp[25], _x4 );
			VAL_MAC( _t4, dp[32], _x4 );
			VAL_MAC( _t5, dp[39], _x4 );
			VAL_MAC( _t6, dp[46], _x4 );
			VAL_MAC( _t0, dp[5], _x5 );
			VAL_MAC( _t1, dp[12], _x5 );
			VAL_MAC( _t2, dp[19], _x5 );
			VAL_MAC( _t3, dp[26], _x5 );
			VAL_MAC( _t4, dp[33], _x5 );
			VAL_MAC( _t5, dp[40], _x5 );
			VAL_MAC( _t6, dp[47], _x5 );
			VAL_MAC( _t0, dp[6], _x6 );
			VAL_MAC( _t1, dp[13], _x6 );
			VAL_MAC( _t2, dp[20], _x6 );
			VAL_MAC( _t3, dp[27], _x6 );
			VAL_MAC( _t4, dp[34], _x6 );
			VAL_MAC( _t5, dp[41], _x6 );
			VAL_MAC( _t6, dp[48], _x6 );
		}

		if( tp != NULL )
		{
			VAL_ASSIGN( tp[0], _t0 );
			VAL_ASSIGN( tp[1*inct], _t1 );
			VAL_ASSIGN( tp[2*inct], _t2 );
			VAL_ASSIGN( tp[3*inct], _t3 );
			VAL_ASSIGN( tp[4*inct], _t4 );
			VAL_ASSIGN( tp[5*inct], _t5 );
			VAL_ASSIGN( tp[6*inct], _t6 );
			tp += 7*inct;
		}

		VAL_SCALE( _t0, alpha );
		VAL_SCALE( _t1, alpha );
		VAL_SCALE( _t2, alpha );
		VAL_SCALE( _t3, alpha );
		VAL_SCALE( _t4, alpha );
		VAL_SCALE( _t5, alpha );
		VAL_SCALE( _t6, alpha );

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 7*3 )
		{
			oski_index_t j0 = ind[K];
			oski_value_t* yp = y + j0;
			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;
			REGISTER oski_value_t _y2;
			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_SET_ZERO( _y2 );
			VAL_MAC( _y0, vp[0], _t0 );
			VAL_MAC( _y1, vp[1], _t0 );
			VAL_MAC( _y2, vp[2], _t0 );
			VAL_MAC( _y0, vp[3], _t1 );
			VAL_MAC( _y1, vp[4], _t1 );
			VAL_MAC( _y2, vp[5], _t1 );
			VAL_MAC( _y0, vp[6], _t2 );
			VAL_MAC( _y1, vp[7], _t2 );
			VAL_MAC( _y2, vp[8], _t2 );
			VAL_MAC( _y0, vp[9], _t3 );
			VAL_MAC( _y1, vp[10], _t3 );
			VAL_MAC( _y2, vp[11], _t3 );
			VAL_MAC( _y0, vp[12], _t4 );
			VAL_MAC( _y1, vp[13], _t4 );
			VAL_MAC( _y2, vp[14], _t4 );
			VAL_MAC( _y0, vp[15], _t5 );
			VAL_MAC( _y1, vp[16], _t5 );
			VAL_MAC( _y2, vp[17], _t5 );
			VAL_MAC( _y0, vp[18], _t6 );
			VAL_MAC( _y1, vp[19], _t6 );
			VAL_MAC( _y2, vp[20], _t6 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1], _y1 );
			VAL_INC( yp[2], _y2 );
		}

		/* Diag block transpose-multiply */
		{
			oski_value_t* yp = y + I*7;
			const oski_value_t* dp = diag;
			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;
			REGISTER oski_value_t _y2;
			REGISTER oski_value_t _y3;
			REGISTER oski_value_t _y4;
			REGISTER oski_value_t _y5;
			REGISTER oski_value_t _y6;
			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_SET_ZERO( _y2 );
			VAL_SET_ZERO( _y3 );
			VAL_SET_ZERO( _y4 );
			VAL_SET_ZERO( _y5 );
			VAL_SET_ZERO( _y6 );
			VAL_MAC( _y0, dp[0], _t0 );
			VAL_MAC( _y1, dp[1], _t0 );
			VAL_MAC( _y2, dp[2], _t0 );
			VAL_MAC( _y3, dp[3], _t0 );
			VAL_MAC( _y4, dp[4], _t0 );
			VAL_MAC( _y5, dp[5], _t0 );
			VAL_MAC( _y6, dp[6], _t0 );
			VAL_MAC( _y0, dp[7], _t1 );
			VAL_MAC( _y1, dp[8], _t1 );
			VAL_MAC( _y2, dp[9], _t1 );
			VAL_MAC( _y3, dp[10], _t1 );
			VAL_MAC( _y4, dp[11], _t1 );
			VAL_MAC( _y5, dp[12], _t1 );
			VAL_MAC( _y6, dp[13], _t1 );
			VAL_MAC( _y0, dp[14], _t2 );
			VAL_MAC( _y1, dp[15], _t2 );
			VAL_MAC( _y2, dp[16], _t2 );
			VAL_MAC( _y3, dp[17], _t2 );
			VAL_MAC( _y4, dp[18], _t2 );
			VAL_MAC( _y5, dp[19], _t2 );
			VAL_MAC( _y6, dp[20], _t2 );
			VAL_MAC( _y0, dp[21], _t3 );
			VAL_MAC( _y1, dp[22], _t3 );
			VAL_MAC( _y2, dp[23], _t3 );
			VAL_MAC( _y3, dp[24], _t3 );
			VAL_MAC( _y4, dp[25], _t3 );
			VAL_MAC( _y5, dp[26], _t3 );
			VAL_MAC( _y6, dp[27], _t3 );
			VAL_MAC( _y0, dp[28], _t4 );
			VAL_MAC( _y1, dp[29], _t4 );
			VAL_MAC( _y2, dp[30], _t4 );
			VAL_MAC( _y3, dp[31], _t4 );
			VAL_MAC( _y4, dp[32], _t4 );
			VAL_MAC( _y5, dp[33], _t4 );
			VAL_MAC( _y6, dp[34], _t4 );
			VAL_MAC( _y0, dp[35], _t5 );
			VAL_MAC( _y1, dp[36], _t5 );
			VAL_MAC( _y2, dp[37], _t5 );
			VAL_MAC( _y3, dp[38], _t5 );
			VAL_MAC( _y4, dp[39], _t5 );
			VAL_MAC( _y5, dp[40], _t5 );
			VAL_MAC( _y6, dp[41], _t5 );
			VAL_MAC( _y0, dp[42], _t6 );
			VAL_MAC( _y1, dp[43], _t6 );
			VAL_MAC( _y2, dp[44], _t6 );
			VAL_MAC( _y3, dp[45], _t6 );
			VAL_MAC( _y4, dp[46], _t6 );
			VAL_MAC( _y5, dp[47], _t6 );
			VAL_MAC( _y6, dp[48], _t6 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1], _y1 );
			VAL_INC( yp[2], _y2 );
			VAL_INC( yp[3], _y3 );
			VAL_INC( yp[4], _y4 );
			VAL_INC( yp[5], _y5 );
			VAL_INC( yp[6], _y6 );
		}

		val += (ptr[I+1]-ptr[I]) * 21;
		diag += 7*7;
	}
} /* MBCSR_MatTransMatMult_v1_aX_b1_xs1_ys1 */

#if defined(DO_NAME_MANGLING)
/** Mangled name for MBCSR_MatTransMatMult_v1_aX_b1_xsX_ysX. */
#define MBCSR_MatTransMatMult_v1_aX_b1_xsX_ysX \
	MANGLE_MOD_(MBCSR_MatTransMatMult_v1_aX_b1_xsX_ysX_7x3)
#endif

/**
 *  \brief The \f$7\times 3\f$ MBCSR implementation
 *  of \f$A^TA\cdot x\f$, where the source and destination
 *  vectors have general-stride and general-stride,
 *  respectively.
 *
 *  Parameter 'tp' may be NULL if no intermediate vector output
 *  is desired.
 */
void
MBCSR_MatTransMatMult_v1_aX_b1_xsX_ysX( oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val, const oski_value_t* restrict diag,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y, oski_index_t incy,
	oski_value_t* restrict t, oski_index_t inct )
{
	oski_index_t I;
	oski_value_t* tp = t;

	for( I = 0; I < M; I++ )
	{
		oski_index_t K;
		const oski_value_t* vp;
		REGISTER oski_value_t _t0 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t1 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t2 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t3 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t4 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t5 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t6 = MAKE_VAL_COMPLEX(0.0, 0.0);

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 7*3 )
		{
			oski_index_t j0 = ind[K];
			const oski_value_t* xp = x + j0*incx;
			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;
			REGISTER oski_value_t _x2;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1*incx] );
			VAL_ASSIGN( _x2, xp[2*incx] );
			VAL_MAC( _t0, vp[0], _x0 );
			VAL_MAC( _t1, vp[3], _x0 );
			VAL_MAC( _t2, vp[6], _x0 );
			VAL_MAC( _t3, vp[9], _x0 );
			VAL_MAC( _t4, vp[12], _x0 );
			VAL_MAC( _t5, vp[15], _x0 );
			VAL_MAC( _t6, vp[18], _x0 );
			VAL_MAC( _t0, vp[1], _x1 );
			VAL_MAC( _t1, vp[4], _x1 );
			VAL_MAC( _t2, vp[7], _x1 );
			VAL_MAC( _t3, vp[10], _x1 );
			VAL_MAC( _t4, vp[13], _x1 );
			VAL_MAC( _t5, vp[16], _x1 );
			VAL_MAC( _t6, vp[19], _x1 );
			VAL_MAC( _t0, vp[2], _x2 );
			VAL_MAC( _t1, vp[5], _x2 );
			VAL_MAC( _t2, vp[8], _x2 );
			VAL_MAC( _t3, vp[11], _x2 );
			VAL_MAC( _t4, vp[14], _x2 );
			VAL_MAC( _t5, vp[17], _x2 );
			VAL_MAC( _t6, vp[20], _x2 );
		}

		/* Diag block multiply */
		{
			const oski_value_t* xp = x + I*7*incx;
			const oski_value_t* dp = diag;
			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;
			REGISTER oski_value_t _x2;
			REGISTER oski_value_t _x3;
			REGISTER oski_value_t _x4;
			REGISTER oski_value_t _x5;
			REGISTER oski_value_t _x6;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1*incx] );
			VAL_ASSIGN( _x2, xp[2*incx] );
			VAL_ASSIGN( _x3, xp[3*incx] );
			VAL_ASSIGN( _x4, xp[4*incx] );
			VAL_ASSIGN( _x5, xp[5*incx] );
			VAL_ASSIGN( _x6, xp[6*incx] );
			VAL_MAC( _t0, dp[0], _x0 );
			VAL_MAC( _t1, dp[7], _x0 );
			VAL_MAC( _t2, dp[14], _x0 );
			VAL_MAC( _t3, dp[21], _x0 );
			VAL_MAC( _t4, dp[28], _x0 );
			VAL_MAC( _t5, dp[35], _x0 );
			VAL_MAC( _t6, dp[42], _x0 );
			VAL_MAC( _t0, dp[1], _x1 );
			VAL_MAC( _t1, dp[8], _x1 );
			VAL_MAC( _t2, dp[15], _x1 );
			VAL_MAC( _t3, dp[22], _x1 );
			VAL_MAC( _t4, dp[29], _x1 );
			VAL_MAC( _t5, dp[36], _x1 );
			VAL_MAC( _t6, dp[43], _x1 );
			VAL_MAC( _t0, dp[2], _x2 );
			VAL_MAC( _t1, dp[9], _x2 );
			VAL_MAC( _t2, dp[16], _x2 );
			VAL_MAC( _t3, dp[23], _x2 );
			VAL_MAC( _t4, dp[30], _x2 );
			VAL_MAC( _t5, dp[37], _x2 );
			VAL_MAC( _t6, dp[44], _x2 );
			VAL_MAC( _t0, dp[3], _x3 );
			VAL_MAC( _t1, dp[10], _x3 );
			VAL_MAC( _t2, dp[17], _x3 );
			VAL_MAC( _t3, dp[24], _x3 );
			VAL_MAC( _t4, dp[31], _x3 );
			VAL_MAC( _t5, dp[38], _x3 );
			VAL_MAC( _t6, dp[45], _x3 );
			VAL_MAC( _t0, dp[4], _x4 );
			VAL_MAC( _t1, dp[11], _x4 );
			VAL_MAC( _t2, dp[18], _x4 );
			VAL_MAC( _t3, dp[25], _x4 );
			VAL_MAC( _t4, dp[32], _x4 );
			VAL_MAC( _t5, dp[39], _x4 );
			VAL_MAC( _t6, dp[46], _x4 );
			VAL_MAC( _t0, dp[5], _x5 );
			VAL_MAC( _t1, dp[12], _x5 );
			VAL_MAC( _t2, dp[19], _x5 );
			VAL_MAC( _t3, dp[26], _x5 );
			VAL_MAC( _t4, dp[33], _x5 );
			VAL_MAC( _t5, dp[40], _x5 );
			VAL_MAC( _t6, dp[47], _x5 );
			VAL_MAC( _t0, dp[6], _x6 );
			VAL_MAC( _t1, dp[13], _x6 );
			VAL_MAC( _t2, dp[20], _x6 );
			VAL_MAC( _t3, dp[27], _x6 );
			VAL_MAC( _t4, dp[34], _x6 );
			VAL_MAC( _t5, dp[41], _x6 );
			VAL_MAC( _t6, dp[48], _x6 );
		}

		if( tp != NULL )
		{
			VAL_ASSIGN( tp[0], _t0 );
			VAL_ASSIGN( tp[1*inct], _t1 );
			VAL_ASSIGN( tp[2*inct], _t2 );
			VAL_ASSIGN( tp[3*inct], _t3 );
			VAL_ASSIGN( tp[4*inct], _t4 );
			VAL_ASSIGN( tp[5*inct], _t5 );
			VAL_ASSIGN( tp[6*inct], _t6 );
			tp += 7*inct;
		}

		VAL_SCALE( _t0, alpha );
		VAL_SCALE( _t1, alpha );
		VAL_SCALE( _t2, alpha );
		VAL_SCALE( _t3, alpha );
		VAL_SCALE( _t4, alpha );
		VAL_SCALE( _t5, alpha );
		VAL_SCALE( _t6, alpha );

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 7*3 )
		{
			oski_index_t j0 = ind[K];
			oski_value_t* yp = y + j0*incy;
			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;
			REGISTER oski_value_t _y2;
			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_SET_ZERO( _y2 );
			VAL_MAC( _y0, vp[0], _t0 );
			VAL_MAC( _y1, vp[1], _t0 );
			VAL_MAC( _y2, vp[2], _t0 );
			VAL_MAC( _y0, vp[3], _t1 );
			VAL_MAC( _y1, vp[4], _t1 );
			VAL_MAC( _y2, vp[5], _t1 );
			VAL_MAC( _y0, vp[6], _t2 );
			VAL_MAC( _y1, vp[7], _t2 );
			VAL_MAC( _y2, vp[8], _t2 );
			VAL_MAC( _y0, vp[9], _t3 );
			VAL_MAC( _y1, vp[10], _t3 );
			VAL_MAC( _y2, vp[11], _t3 );
			VAL_MAC( _y0, vp[12], _t4 );
			VAL_MAC( _y1, vp[13], _t4 );
			VAL_MAC( _y2, vp[14], _t4 );
			VAL_MAC( _y0, vp[15], _t5 );
			VAL_MAC( _y1, vp[16], _t5 );
			VAL_MAC( _y2, vp[17], _t5 );
			VAL_MAC( _y0, vp[18], _t6 );
			VAL_MAC( _y1, vp[19], _t6 );
			VAL_MAC( _y2, vp[20], _t6 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1*incy], _y1 );
			VAL_INC( yp[2*incy], _y2 );
		}

		/* Diag block transpose-multiply */
		{
			oski_value_t* yp = y + I*7*incy;
			const oski_value_t* dp = diag;
			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;
			REGISTER oski_value_t _y2;
			REGISTER oski_value_t _y3;
			REGISTER oski_value_t _y4;
			REGISTER oski_value_t _y5;
			REGISTER oski_value_t _y6;
			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_SET_ZERO( _y2 );
			VAL_SET_ZERO( _y3 );
			VAL_SET_ZERO( _y4 );
			VAL_SET_ZERO( _y5 );
			VAL_SET_ZERO( _y6 );
			VAL_MAC( _y0, dp[0], _t0 );
			VAL_MAC( _y1, dp[1], _t0 );
			VAL_MAC( _y2, dp[2], _t0 );
			VAL_MAC( _y3, dp[3], _t0 );
			VAL_MAC( _y4, dp[4], _t0 );
			VAL_MAC( _y5, dp[5], _t0 );
			VAL_MAC( _y6, dp[6], _t0 );
			VAL_MAC( _y0, dp[7], _t1 );
			VAL_MAC( _y1, dp[8], _t1 );
			VAL_MAC( _y2, dp[9], _t1 );
			VAL_MAC( _y3, dp[10], _t1 );
			VAL_MAC( _y4, dp[11], _t1 );
			VAL_MAC( _y5, dp[12], _t1 );
			VAL_MAC( _y6, dp[13], _t1 );
			VAL_MAC( _y0, dp[14], _t2 );
			VAL_MAC( _y1, dp[15], _t2 );
			VAL_MAC( _y2, dp[16], _t2 );
			VAL_MAC( _y3, dp[17], _t2 );
			VAL_MAC( _y4, dp[18], _t2 );
			VAL_MAC( _y5, dp[19], _t2 );
			VAL_MAC( _y6, dp[20], _t2 );
			VAL_MAC( _y0, dp[21], _t3 );
			VAL_MAC( _y1, dp[22], _t3 );
			VAL_MAC( _y2, dp[23], _t3 );
			VAL_MAC( _y3, dp[24], _t3 );
			VAL_MAC( _y4, dp[25], _t3 );
			VAL_MAC( _y5, dp[26], _t3 );
			VAL_MAC( _y6, dp[27], _t3 );
			VAL_MAC( _y0, dp[28], _t4 );
			VAL_MAC( _y1, dp[29], _t4 );
			VAL_MAC( _y2, dp[30], _t4 );
			VAL_MAC( _y3, dp[31], _t4 );
			VAL_MAC( _y4, dp[32], _t4 );
			VAL_MAC( _y5, dp[33], _t4 );
			VAL_MAC( _y6, dp[34], _t4 );
			VAL_MAC( _y0, dp[35], _t5 );
			VAL_MAC( _y1, dp[36], _t5 );
			VAL_MAC( _y2, dp[37], _t5 );
			VAL_MAC( _y3, dp[38], _t5 );
			VAL_MAC( _y4, dp[39], _t5 );
			VAL_MAC( _y5, dp[40], _t5 );
			VAL_MAC( _y6, dp[41], _t5 );
			VAL_MAC( _y0, dp[42], _t6 );
			VAL_MAC( _y1, dp[43], _t6 );
			VAL_MAC( _y2, dp[44], _t6 );
			VAL_MAC( _y3, dp[45], _t6 );
			VAL_MAC( _y4, dp[46], _t6 );
			VAL_MAC( _y5, dp[47], _t6 );
			VAL_MAC( _y6, dp[48], _t6 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1*incy], _y1 );
			VAL_INC( yp[2*incy], _y2 );
			VAL_INC( yp[3*incy], _y3 );
			VAL_INC( yp[4*incy], _y4 );
			VAL_INC( yp[5*incy], _y5 );
			VAL_INC( yp[6*incy], _y6 );
		}

		val += (ptr[I+1]-ptr[I]) * 21;
		diag += 7*7;
	}
} /* MBCSR_MatTransMatMult_v1_aX_b1_xsX_ysX */


#if defined(DO_NAME_MANGLING)
/** Mangled name for primary exportable symbol, SubmatReprTransSubmatReprMult */
#define SubmatReprTransSubmatReprMult \
	MANGLE_MOD_(SubmatReprTransSubmatReprMult_7x3)
#endif

/**
 *  \brief Exported module wrapper for the \f$7\times 3\f$
 *  implementation of \f$A^TA\cdot x\f$.
 */
int
SubmatReprTransSubmatReprMult( const oski_submatMBCSR_t* A,
	oski_value_t alpha, const oski_vecview_t x,
	oski_vecview_t y, oski_vecview_t t )
{
	oski_index_t j;
	const oski_value_t* xp = x->val;
	oski_value_t* yp = y->val;
	oski_value_t* tp;
	oski_index_t inct;

	assert( A != NULL );
	assert( A->r == 7 );
	assert( A->c == 3 );

	if( t == INVALID_VEC || t->num_rows == 0 || t->num_cols == 0 ) {
		inct = 0;
		tp = NULL;
	} else {
		inct = t->rowinc;
		tp = t->val + A->offset * inct;
	}

	for( j = 0; j < x->num_cols; j++ )
	{
		if( x->rowinc == 1 && y->rowinc == 1 )
			MBCSR_MatTransMatMult_v1_aX_b1_xs1_ys1(
				A->num_block_rows, A->bptr, A->bind, A->bval, A->bdiag,
				alpha, xp, yp, tp, inct );
		else
			MBCSR_MatTransMatMult_v1_aX_b1_xsX_ysX(
				A->num_block_rows, A->bptr, A->bind, A->bval, A->bdiag,
				alpha, xp, x->rowinc, yp, y->rowinc, tp, inct );

		xp += x->colinc;
		yp += y->colinc;
		if( tp != NULL ) tp += t->colinc;
	}
	return 0;
}

#if defined(DO_NAME_MANGLING)
/** Mangled name for MBCSR_MatHermMatMult_v1_aX_b1_xs1_ys1. */
#define MBCSR_MatHermMatMult_v1_aX_b1_xs1_ys1 \
	MANGLE_MOD_(MBCSR_MatHermMatMult_v1_aX_b1_xs1_ys1_7x3)
#endif

/**
 *  \brief The \f$7\times 3\f$ MBCSR implementation
 *  of \f$A^HA\cdot x\f$, where the source and destination
 *  vectors have unit-stride and unit-stride,
 *  respectively.
 *
 *  Parameter 'tp' may be NULL if no intermediate vector output
 *  is desired.
 */
void
MBCSR_MatHermMatMult_v1_aX_b1_xs1_ys1( oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val, const oski_value_t* restrict diag,
	oski_value_t alpha, const oski_value_t* restrict x,
	oski_value_t* restrict y,
	oski_value_t* restrict t, oski_index_t inct )
{
	oski_index_t I;
	oski_value_t* tp = t;

	for( I = 0; I < M; I++ )
	{
		oski_index_t K;
		const oski_value_t* vp;
		REGISTER oski_value_t _t0 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t1 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t2 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t3 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t4 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t5 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t6 = MAKE_VAL_COMPLEX(0.0, 0.0);

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 7*3 )
		{
			oski_index_t j0 = ind[K];
			const oski_value_t* xp = x + j0;
			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;
			REGISTER oski_value_t _x2;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1] );
			VAL_ASSIGN( _x2, xp[2] );
			VAL_MAC( _t0, vp[0], _x0 );
			VAL_MAC( _t1, vp[3], _x0 );
			VAL_MAC( _t2, vp[6], _x0 );
			VAL_MAC( _t3, vp[9], _x0 );
			VAL_MAC( _t4, vp[12], _x0 );
			VAL_MAC( _t5, vp[15], _x0 );
			VAL_MAC( _t6, vp[18], _x0 );
			VAL_MAC( _t0, vp[1], _x1 );
			VAL_MAC( _t1, vp[4], _x1 );
			VAL_MAC( _t2, vp[7], _x1 );
			VAL_MAC( _t3, vp[10], _x1 );
			VAL_MAC( _t4, vp[13], _x1 );
			VAL_MAC( _t5, vp[16], _x1 );
			VAL_MAC( _t6, vp[19], _x1 );
			VAL_MAC( _t0, vp[2], _x2 );
			VAL_MAC( _t1, vp[5], _x2 );
			VAL_MAC( _t2, vp[8], _x2 );
			VAL_MAC( _t3, vp[11], _x2 );
			VAL_MAC( _t4, vp[14], _x2 );
			VAL_MAC( _t5, vp[17], _x2 );
			VAL_MAC( _t6, vp[20], _x2 );
		}

		/* Diag block multiply */
		{
			const oski_value_t* xp = x + I*7;
			const oski_value_t* dp = diag;
			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;
			REGISTER oski_value_t _x2;
			REGISTER oski_value_t _x3;
			REGISTER oski_value_t _x4;
			REGISTER oski_value_t _x5;
			REGISTER oski_value_t _x6;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1] );
			VAL_ASSIGN( _x2, xp[2] );
			VAL_ASSIGN( _x3, xp[3] );
			VAL_ASSIGN( _x4, xp[4] );
			VAL_ASSIGN( _x5, xp[5] );
			VAL_ASSIGN( _x6, xp[6] );
			VAL_MAC( _t0, dp[0], _x0 );
			VAL_MAC( _t1, dp[7], _x0 );
			VAL_MAC( _t2, dp[14], _x0 );
			VAL_MAC( _t3, dp[21], _x0 );
			VAL_MAC( _t4, dp[28], _x0 );
			VAL_MAC( _t5, dp[35], _x0 );
			VAL_MAC( _t6, dp[42], _x0 );
			VAL_MAC( _t0, dp[1], _x1 );
			VAL_MAC( _t1, dp[8], _x1 );
			VAL_MAC( _t2, dp[15], _x1 );
			VAL_MAC( _t3, dp[22], _x1 );
			VAL_MAC( _t4, dp[29], _x1 );
			VAL_MAC( _t5, dp[36], _x1 );
			VAL_MAC( _t6, dp[43], _x1 );
			VAL_MAC( _t0, dp[2], _x2 );
			VAL_MAC( _t1, dp[9], _x2 );
			VAL_MAC( _t2, dp[16], _x2 );
			VAL_MAC( _t3, dp[23], _x2 );
			VAL_MAC( _t4, dp[30], _x2 );
			VAL_MAC( _t5, dp[37], _x2 );
			VAL_MAC( _t6, dp[44], _x2 );
			VAL_MAC( _t0, dp[3], _x3 );
			VAL_MAC( _t1, dp[10], _x3 );
			VAL_MAC( _t2, dp[17], _x3 );
			VAL_MAC( _t3, dp[24], _x3 );
			VAL_MAC( _t4, dp[31], _x3 );
			VAL_MAC( _t5, dp[38], _x3 );
			VAL_MAC( _t6, dp[45], _x3 );
			VAL_MAC( _t0, dp[4], _x4 );
			VAL_MAC( _t1, dp[11], _x4 );
			VAL_MAC( _t2, dp[18], _x4 );
			VAL_MAC( _t3, dp[25], _x4 );
			VAL_MAC( _t4, dp[32], _x4 );
			VAL_MAC( _t5, dp[39], _x4 );
			VAL_MAC( _t6, dp[46], _x4 );
			VAL_MAC( _t0, dp[5], _x5 );
			VAL_MAC( _t1, dp[12], _x5 );
			VAL_MAC( _t2, dp[19], _x5 );
			VAL_MAC( _t3, dp[26], _x5 );
			VAL_MAC( _t4, dp[33], _x5 );
			VAL_MAC( _t5, dp[40], _x5 );
			VAL_MAC( _t6, dp[47], _x5 );
			VAL_MAC( _t0, dp[6], _x6 );
			VAL_MAC( _t1, dp[13], _x6 );
			VAL_MAC( _t2, dp[20], _x6 );
			VAL_MAC( _t3, dp[27], _x6 );
			VAL_MAC( _t4, dp[34], _x6 );
			VAL_MAC( _t5, dp[41], _x6 );
			VAL_MAC( _t6, dp[48], _x6 );
		}

		if( tp != NULL )
		{
			VAL_ASSIGN( tp[0], _t0 );
			VAL_ASSIGN( tp[1*inct], _t1 );
			VAL_ASSIGN( tp[2*inct], _t2 );
			VAL_ASSIGN( tp[3*inct], _t3 );
			VAL_ASSIGN( tp[4*inct], _t4 );
			VAL_ASSIGN( tp[5*inct], _t5 );
			VAL_ASSIGN( tp[6*inct], _t6 );
			tp += 7*inct;
		}

		VAL_SCALE( _t0, alpha );
		VAL_SCALE( _t1, alpha );
		VAL_SCALE( _t2, alpha );
		VAL_SCALE( _t3, alpha );
		VAL_SCALE( _t4, alpha );
		VAL_SCALE( _t5, alpha );
		VAL_SCALE( _t6, alpha );

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 7*3 )
		{
			oski_index_t j0 = ind[K];
			oski_value_t* yp = y + j0;
			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;
			REGISTER oski_value_t _y2;
			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_SET_ZERO( _y2 );
			VAL_MAC_CONJ( _y0, vp[0], _t0 );
			VAL_MAC_CONJ( _y1, vp[1], _t0 );
			VAL_MAC_CONJ( _y2, vp[2], _t0 );
			VAL_MAC_CONJ( _y0, vp[3], _t1 );
			VAL_MAC_CONJ( _y1, vp[4], _t1 );
			VAL_MAC_CONJ( _y2, vp[5], _t1 );
			VAL_MAC_CONJ( _y0, vp[6], _t2 );
			VAL_MAC_CONJ( _y1, vp[7], _t2 );
			VAL_MAC_CONJ( _y2, vp[8], _t2 );
			VAL_MAC_CONJ( _y0, vp[9], _t3 );
			VAL_MAC_CONJ( _y1, vp[10], _t3 );
			VAL_MAC_CONJ( _y2, vp[11], _t3 );
			VAL_MAC_CONJ( _y0, vp[12], _t4 );
			VAL_MAC_CONJ( _y1, vp[13], _t4 );
			VAL_MAC_CONJ( _y2, vp[14], _t4 );
			VAL_MAC_CONJ( _y0, vp[15], _t5 );
			VAL_MAC_CONJ( _y1, vp[16], _t5 );
			VAL_MAC_CONJ( _y2, vp[17], _t5 );
			VAL_MAC_CONJ( _y0, vp[18], _t6 );
			VAL_MAC_CONJ( _y1, vp[19], _t6 );
			VAL_MAC_CONJ( _y2, vp[20], _t6 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1], _y1 );
			VAL_INC( yp[2], _y2 );
		}

		/* Diag block transpose-multiply */
		{
			oski_value_t* yp = y + I*7;
			const oski_value_t* dp = diag;
			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;
			REGISTER oski_value_t _y2;
			REGISTER oski_value_t _y3;
			REGISTER oski_value_t _y4;
			REGISTER oski_value_t _y5;
			REGISTER oski_value_t _y6;
			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_SET_ZERO( _y2 );
			VAL_SET_ZERO( _y3 );
			VAL_SET_ZERO( _y4 );
			VAL_SET_ZERO( _y5 );
			VAL_SET_ZERO( _y6 );
			VAL_MAC_CONJ( _y0, dp[0], _t0 );
			VAL_MAC_CONJ( _y1, dp[1], _t0 );
			VAL_MAC_CONJ( _y2, dp[2], _t0 );
			VAL_MAC_CONJ( _y3, dp[3], _t0 );
			VAL_MAC_CONJ( _y4, dp[4], _t0 );
			VAL_MAC_CONJ( _y5, dp[5], _t0 );
			VAL_MAC_CONJ( _y6, dp[6], _t0 );
			VAL_MAC_CONJ( _y0, dp[7], _t1 );
			VAL_MAC_CONJ( _y1, dp[8], _t1 );
			VAL_MAC_CONJ( _y2, dp[9], _t1 );
			VAL_MAC_CONJ( _y3, dp[10], _t1 );
			VAL_MAC_CONJ( _y4, dp[11], _t1 );
			VAL_MAC_CONJ( _y5, dp[12], _t1 );
			VAL_MAC_CONJ( _y6, dp[13], _t1 );
			VAL_MAC_CONJ( _y0, dp[14], _t2 );
			VAL_MAC_CONJ( _y1, dp[15], _t2 );
			VAL_MAC_CONJ( _y2, dp[16], _t2 );
			VAL_MAC_CONJ( _y3, dp[17], _t2 );
			VAL_MAC_CONJ( _y4, dp[18], _t2 );
			VAL_MAC_CONJ( _y5, dp[19], _t2 );
			VAL_MAC_CONJ( _y6, dp[20], _t2 );
			VAL_MAC_CONJ( _y0, dp[21], _t3 );
			VAL_MAC_CONJ( _y1, dp[22], _t3 );
			VAL_MAC_CONJ( _y2, dp[23], _t3 );
			VAL_MAC_CONJ( _y3, dp[24], _t3 );
			VAL_MAC_CONJ( _y4, dp[25], _t3 );
			VAL_MAC_CONJ( _y5, dp[26], _t3 );
			VAL_MAC_CONJ( _y6, dp[27], _t3 );
			VAL_MAC_CONJ( _y0, dp[28], _t4 );
			VAL_MAC_CONJ( _y1, dp[29], _t4 );
			VAL_MAC_CONJ( _y2, dp[30], _t4 );
			VAL_MAC_CONJ( _y3, dp[31], _t4 );
			VAL_MAC_CONJ( _y4, dp[32], _t4 );
			VAL_MAC_CONJ( _y5, dp[33], _t4 );
			VAL_MAC_CONJ( _y6, dp[34], _t4 );
			VAL_MAC_CONJ( _y0, dp[35], _t5 );
			VAL_MAC_CONJ( _y1, dp[36], _t5 );
			VAL_MAC_CONJ( _y2, dp[37], _t5 );
			VAL_MAC_CONJ( _y3, dp[38], _t5 );
			VAL_MAC_CONJ( _y4, dp[39], _t5 );
			VAL_MAC_CONJ( _y5, dp[40], _t5 );
			VAL_MAC_CONJ( _y6, dp[41], _t5 );
			VAL_MAC_CONJ( _y0, dp[42], _t6 );
			VAL_MAC_CONJ( _y1, dp[43], _t6 );
			VAL_MAC_CONJ( _y2, dp[44], _t6 );
			VAL_MAC_CONJ( _y3, dp[45], _t6 );
			VAL_MAC_CONJ( _y4, dp[46], _t6 );
			VAL_MAC_CONJ( _y5, dp[47], _t6 );
			VAL_MAC_CONJ( _y6, dp[48], _t6 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1], _y1 );
			VAL_INC( yp[2], _y2 );
			VAL_INC( yp[3], _y3 );
			VAL_INC( yp[4], _y4 );
			VAL_INC( yp[5], _y5 );
			VAL_INC( yp[6], _y6 );
		}

		val += (ptr[I+1]-ptr[I]) * 21;
		diag += 7*7;
	}
} /* MBCSR_MatHermMatMult_v1_aX_b1_xs1_ys1 */

#if defined(DO_NAME_MANGLING)
/** Mangled name for MBCSR_MatHermMatMult_v1_aX_b1_xsX_ysX. */
#define MBCSR_MatHermMatMult_v1_aX_b1_xsX_ysX \
	MANGLE_MOD_(MBCSR_MatHermMatMult_v1_aX_b1_xsX_ysX_7x3)
#endif

/**
 *  \brief The \f$7\times 3\f$ MBCSR implementation
 *  of \f$A^HA\cdot x\f$, where the source and destination
 *  vectors have general-stride and general-stride,
 *  respectively.
 *
 *  Parameter 'tp' may be NULL if no intermediate vector output
 *  is desired.
 */
void
MBCSR_MatHermMatMult_v1_aX_b1_xsX_ysX( oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val, const oski_value_t* restrict diag,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y, oski_index_t incy,
	oski_value_t* restrict t, oski_index_t inct )
{
	oski_index_t I;
	oski_value_t* tp = t;

	for( I = 0; I < M; I++ )
	{
		oski_index_t K;
		const oski_value_t* vp;
		REGISTER oski_value_t _t0 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t1 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t2 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t3 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t4 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t5 = MAKE_VAL_COMPLEX(0.0, 0.0);
		REGISTER oski_value_t _t6 = MAKE_VAL_COMPLEX(0.0, 0.0);

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 7*3 )
		{
			oski_index_t j0 = ind[K];
			const oski_value_t* xp = x + j0*incx;
			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;
			REGISTER oski_value_t _x2;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1*incx] );
			VAL_ASSIGN( _x2, xp[2*incx] );
			VAL_MAC( _t0, vp[0], _x0 );
			VAL_MAC( _t1, vp[3], _x0 );
			VAL_MAC( _t2, vp[6], _x0 );
			VAL_MAC( _t3, vp[9], _x0 );
			VAL_MAC( _t4, vp[12], _x0 );
			VAL_MAC( _t5, vp[15], _x0 );
			VAL_MAC( _t6, vp[18], _x0 );
			VAL_MAC( _t0, vp[1], _x1 );
			VAL_MAC( _t1, vp[4], _x1 );
			VAL_MAC( _t2, vp[7], _x1 );
			VAL_MAC( _t3, vp[10], _x1 );
			VAL_MAC( _t4, vp[13], _x1 );
			VAL_MAC( _t5, vp[16], _x1 );
			VAL_MAC( _t6, vp[19], _x1 );
			VAL_MAC( _t0, vp[2], _x2 );
			VAL_MAC( _t1, vp[5], _x2 );
			VAL_MAC( _t2, vp[8], _x2 );
			VAL_MAC( _t3, vp[11], _x2 );
			VAL_MAC( _t4, vp[14], _x2 );
			VAL_MAC( _t5, vp[17], _x2 );
			VAL_MAC( _t6, vp[20], _x2 );
		}

		/* Diag block multiply */
		{
			const oski_value_t* xp = x + I*7*incx;
			const oski_value_t* dp = diag;
			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;
			REGISTER oski_value_t _x2;
			REGISTER oski_value_t _x3;
			REGISTER oski_value_t _x4;
			REGISTER oski_value_t _x5;
			REGISTER oski_value_t _x6;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1*incx] );
			VAL_ASSIGN( _x2, xp[2*incx] );
			VAL_ASSIGN( _x3, xp[3*incx] );
			VAL_ASSIGN( _x4, xp[4*incx] );
			VAL_ASSIGN( _x5, xp[5*incx] );
			VAL_ASSIGN( _x6, xp[6*incx] );
			VAL_MAC( _t0, dp[0], _x0 );
			VAL_MAC( _t1, dp[7], _x0 );
			VAL_MAC( _t2, dp[14], _x0 );
			VAL_MAC( _t3, dp[21], _x0 );
			VAL_MAC( _t4, dp[28], _x0 );
			VAL_MAC( _t5, dp[35], _x0 );
			VAL_MAC( _t6, dp[42], _x0 );
			VAL_MAC( _t0, dp[1], _x1 );
			VAL_MAC( _t1, dp[8], _x1 );
			VAL_MAC( _t2, dp[15], _x1 );
			VAL_MAC( _t3, dp[22], _x1 );
			VAL_MAC( _t4, dp[29], _x1 );
			VAL_MAC( _t5, dp[36], _x1 );
			VAL_MAC( _t6, dp[43], _x1 );
			VAL_MAC( _t0, dp[2], _x2 );
			VAL_MAC( _t1, dp[9], _x2 );
			VAL_MAC( _t2, dp[16], _x2 );
			VAL_MAC( _t3, dp[23], _x2 );
			VAL_MAC( _t4, dp[30], _x2 );
			VAL_MAC( _t5, dp[37], _x2 );
			VAL_MAC( _t6, dp[44], _x2 );
			VAL_MAC( _t0, dp[3], _x3 );
			VAL_MAC( _t1, dp[10], _x3 );
			VAL_MAC( _t2, dp[17], _x3 );
			VAL_MAC( _t3, dp[24], _x3 );
			VAL_MAC( _t4, dp[31], _x3 );
			VAL_MAC( _t5, dp[38], _x3 );
			VAL_MAC( _t6, dp[45], _x3 );
			VAL_MAC( _t0, dp[4], _x4 );
			VAL_MAC( _t1, dp[11], _x4 );
			VAL_MAC( _t2, dp[18], _x4 );
			VAL_MAC( _t3, dp[25], _x4 );
			VAL_MAC( _t4, dp[32], _x4 );
			VAL_MAC( _t5, dp[39], _x4 );
			VAL_MAC( _t6, dp[46], _x4 );
			VAL_MAC( _t0, dp[5], _x5 );
			VAL_MAC( _t1, dp[12], _x5 );
			VAL_MAC( _t2, dp[19], _x5 );
			VAL_MAC( _t3, dp[26], _x5 );
			VAL_MAC( _t4, dp[33], _x5 );
			VAL_MAC( _t5, dp[40], _x5 );
			VAL_MAC( _t6, dp[47], _x5 );
			VAL_MAC( _t0, dp[6], _x6 );
			VAL_MAC( _t1, dp[13], _x6 );
			VAL_MAC( _t2, dp[20], _x6 );
			VAL_MAC( _t3, dp[27], _x6 );
			VAL_MAC( _t4, dp[34], _x6 );
			VAL_MAC( _t5, dp[41], _x6 );
			VAL_MAC( _t6, dp[48], _x6 );
		}

		if( tp != NULL )
		{
			VAL_ASSIGN( tp[0], _t0 );
			VAL_ASSIGN( tp[1*inct], _t1 );
			VAL_ASSIGN( tp[2*inct], _t2 );
			VAL_ASSIGN( tp[3*inct], _t3 );
			VAL_ASSIGN( tp[4*inct], _t4 );
			VAL_ASSIGN( tp[5*inct], _t5 );
			VAL_ASSIGN( tp[6*inct], _t6 );
			tp += 7*inct;
		}

		VAL_SCALE( _t0, alpha );
		VAL_SCALE( _t1, alpha );
		VAL_SCALE( _t2, alpha );
		VAL_SCALE( _t3, alpha );
		VAL_SCALE( _t4, alpha );
		VAL_SCALE( _t5, alpha );
		VAL_SCALE( _t6, alpha );

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 7*3 )
		{
			oski_index_t j0 = ind[K];
			oski_value_t* yp = y + j0*incy;
			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;
			REGISTER oski_value_t _y2;
			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_SET_ZERO( _y2 );
			VAL_MAC_CONJ( _y0, vp[0], _t0 );
			VAL_MAC_CONJ( _y1, vp[1], _t0 );
			VAL_MAC_CONJ( _y2, vp[2], _t0 );
			VAL_MAC_CONJ( _y0, vp[3], _t1 );
			VAL_MAC_CONJ( _y1, vp[4], _t1 );
			VAL_MAC_CONJ( _y2, vp[5], _t1 );
			VAL_MAC_CONJ( _y0, vp[6], _t2 );
			VAL_MAC_CONJ( _y1, vp[7], _t2 );
			VAL_MAC_CONJ( _y2, vp[8], _t2 );
			VAL_MAC_CONJ( _y0, vp[9], _t3 );
			VAL_MAC_CONJ( _y1, vp[10], _t3 );
			VAL_MAC_CONJ( _y2, vp[11], _t3 );
			VAL_MAC_CONJ( _y0, vp[12], _t4 );
			VAL_MAC_CONJ( _y1, vp[13], _t4 );
			VAL_MAC_CONJ( _y2, vp[14], _t4 );
			VAL_MAC_CONJ( _y0, vp[15], _t5 );
			VAL_MAC_CONJ( _y1, vp[16], _t5 );
			VAL_MAC_CONJ( _y2, vp[17], _t5 );
			VAL_MAC_CONJ( _y0, vp[18], _t6 );
			VAL_MAC_CONJ( _y1, vp[19], _t6 );
			VAL_MAC_CONJ( _y2, vp[20], _t6 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1*incy], _y1 );
			VAL_INC( yp[2*incy], _y2 );
		}

		/* Diag block transpose-multiply */
		{
			oski_value_t* yp = y + I*7*incy;
			const oski_value_t* dp = diag;
			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;
			REGISTER oski_value_t _y2;
			REGISTER oski_value_t _y3;
			REGISTER oski_value_t _y4;
			REGISTER oski_value_t _y5;
			REGISTER oski_value_t _y6;
			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_SET_ZERO( _y2 );
			VAL_SET_ZERO( _y3 );
			VAL_SET_ZERO( _y4 );
			VAL_SET_ZERO( _y5 );
			VAL_SET_ZERO( _y6 );
			VAL_MAC_CONJ( _y0, dp[0], _t0 );
			VAL_MAC_CONJ( _y1, dp[1], _t0 );
			VAL_MAC_CONJ( _y2, dp[2], _t0 );
			VAL_MAC_CONJ( _y3, dp[3], _t0 );
			VAL_MAC_CONJ( _y4, dp[4], _t0 );
			VAL_MAC_CONJ( _y5, dp[5], _t0 );
			VAL_MAC_CONJ( _y6, dp[6], _t0 );
			VAL_MAC_CONJ( _y0, dp[7], _t1 );
			VAL_MAC_CONJ( _y1, dp[8], _t1 );
			VAL_MAC_CONJ( _y2, dp[9], _t1 );
			VAL_MAC_CONJ( _y3, dp[10], _t1 );
			VAL_MAC_CONJ( _y4, dp[11], _t1 );
			VAL_MAC_CONJ( _y5, dp[12], _t1 );
			VAL_MAC_CONJ( _y6, dp[13], _t1 );
			VAL_MAC_CONJ( _y0, dp[14], _t2 );
			VAL_MAC_CONJ( _y1, dp[15], _t2 );
			VAL_MAC_CONJ( _y2, dp[16], _t2 );
			VAL_MAC_CONJ( _y3, dp[17], _t2 );
			VAL_MAC_CONJ( _y4, dp[18], _t2 );
			VAL_MAC_CONJ( _y5, dp[19], _t2 );
			VAL_MAC_CONJ( _y6, dp[20], _t2 );
			VAL_MAC_CONJ( _y0, dp[21], _t3 );
			VAL_MAC_CONJ( _y1, dp[22], _t3 );
			VAL_MAC_CONJ( _y2, dp[23], _t3 );
			VAL_MAC_CONJ( _y3, dp[24], _t3 );
			VAL_MAC_CONJ( _y4, dp[25], _t3 );
			VAL_MAC_CONJ( _y5, dp[26], _t3 );
			VAL_MAC_CONJ( _y6, dp[27], _t3 );
			VAL_MAC_CONJ( _y0, dp[28], _t4 );
			VAL_MAC_CONJ( _y1, dp[29], _t4 );
			VAL_MAC_CONJ( _y2, dp[30], _t4 );
			VAL_MAC_CONJ( _y3, dp[31], _t4 );
			VAL_MAC_CONJ( _y4, dp[32], _t4 );
			VAL_MAC_CONJ( _y5, dp[33], _t4 );
			VAL_MAC_CONJ( _y6, dp[34], _t4 );
			VAL_MAC_CONJ( _y0, dp[35], _t5 );
			VAL_MAC_CONJ( _y1, dp[36], _t5 );
			VAL_MAC_CONJ( _y2, dp[37], _t5 );
			VAL_MAC_CONJ( _y3, dp[38], _t5 );
			VAL_MAC_CONJ( _y4, dp[39], _t5 );
			VAL_MAC_CONJ( _y5, dp[40], _t5 );
			VAL_MAC_CONJ( _y6, dp[41], _t5 );
			VAL_MAC_CONJ( _y0, dp[42], _t6 );
			VAL_MAC_CONJ( _y1, dp[43], _t6 );
			VAL_MAC_CONJ( _y2, dp[44], _t6 );
			VAL_MAC_CONJ( _y3, dp[45], _t6 );
			VAL_MAC_CONJ( _y4, dp[46], _t6 );
			VAL_MAC_CONJ( _y5, dp[47], _t6 );
			VAL_MAC_CONJ( _y6, dp[48], _t6 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1*incy], _y1 );
			VAL_INC( yp[2*incy], _y2 );
			VAL_INC( yp[3*incy], _y3 );
			VAL_INC( yp[4*incy], _y4 );
			VAL_INC( yp[5*incy], _y5 );
			VAL_INC( yp[6*incy], _y6 );
		}

		val += (ptr[I+1]-ptr[I]) * 21;
		diag += 7*7;
	}
} /* MBCSR_MatHermMatMult_v1_aX_b1_xsX_ysX */


#if defined(DO_NAME_MANGLING)
/** Mangled name for primary exportable symbol, SubmatReprHermSubmatReprMult */
#define SubmatReprHermSubmatReprMult \
	MANGLE_MOD_(SubmatReprHermSubmatReprMult_7x3)
#endif

/**
 *  \brief Exported module wrapper for the \f$7\times 3\f$
 *  implementation of \f$A^HA\cdot x\f$.
 */
int
SubmatReprHermSubmatReprMult( const oski_submatMBCSR_t* A,
	oski_value_t alpha, const oski_vecview_t x,
	oski_vecview_t y, oski_vecview_t t )
{
	oski_index_t j;
	const oski_value_t* xp = x->val;
	oski_value_t* yp = y->val;
	oski_value_t* tp;
	oski_index_t inct;

	assert( A != NULL );
	assert( A->r == 7 );
	assert( A->c == 3 );

	if( t == INVALID_VEC || t->num_rows == 0 || t->num_cols == 0 ) {
		inct = 0;
		tp = NULL;
	} else {
		inct = t->rowinc;
		tp = t->val + A->offset * inct;
	}

	for( j = 0; j < x->num_cols; j++ )
	{
		if( x->rowinc == 1 && y->rowinc == 1 )
			MBCSR_MatHermMatMult_v1_aX_b1_xs1_ys1(
				A->num_block_rows, A->bptr, A->bind, A->bval, A->bdiag,
				alpha, xp, yp, tp, inct );
		else
			MBCSR_MatHermMatMult_v1_aX_b1_xsX_ysX(
				A->num_block_rows, A->bptr, A->bind, A->bval, A->bdiag,
				alpha, xp, x->rowinc, yp, y->rowinc, tp, inct );

		xp += x->colinc;
		yp += y->colinc;
		if( tp != NULL ) tp += t->colinc;
	}
	return 0;
}


/* eof */
