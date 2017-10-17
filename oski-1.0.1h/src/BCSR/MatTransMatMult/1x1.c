/**
 *  \file src/BCSR/MatTransMatMult/BCSR_1x1.c
 *  \brief The \f$1\times 1\f$ BCSR implementation
 *  of \f$A^TA\cdot x\f$ and \f$A^HA\cdot x\f$.
 *  \ingroup MATTYPE_BCSR
 *
 *  Automatically generated by ./gen_ata.sh on Tue Jun  7 13:30:53 PDT 2005.
 */

#if HAVE_CONFIG_H
#include <config/config.h> /* for 'restrict' keyword */
#endif

#include <assert.h>

#include <oski/config.h>
#include <oski/common.h>
#include <oski/mangle.h>
#include <oski/vecview.h>
#include <oski/BCSR/format.h>
#include <oski/BCSR/module.h>

#if IS_VAL_COMPLEX
	/** Complex-valued, so do not use explicit 'register' keyword. */
	#define REGISTER
#else
	/** Real-valued, so use explicit 'register' keyword. */
	#define REGISTER register
#endif

#if defined(DO_NAME_MANGLING)
/** Mangled name for BCSR_MatTransMatMult_v1_aX_b1_xs1_ys1. */
#define BCSR_MatTransMatMult_v1_aX_b1_xs1_ys1 \
	MANGLE_MOD_(BCSR_MatTransMatMult_v1_aX_b1_xs1_ys1_1x1)
#endif

/**
 *  \brief The \f$1\times 1\f$ BCSR implementation
 *  of \f$A^TA\cdot x\f$, where the source and destination
 *  vectors have unit-stride and unit-stride,
 *  respectively.
 *
 *  Parameter 'tp' may be NULL if no intermediate vector output
 *  is desired.
 */
void
BCSR_MatTransMatMult_v1_aX_b1_xs1_ys1( oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val,
	oski_value_t alpha, const oski_value_t* restrict x,
	oski_value_t* restrict y,
	oski_value_t* restrict t, oski_index_t inct )
{
	oski_index_t I;
	oski_value_t* tp = t;

	for( I = 0; I < M; I++ )
	{
		oski_index_t K;
		REGISTER oski_value_t _t0 = MAKE_VAL_COMPLEX(0.0, 0.0);
		const oski_value_t* vp;

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 1*1 )
		{
			oski_index_t j0 = ind[K];
			const oski_value_t* xp = x + j0;
			REGISTER oski_value_t _x0;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_MAC( _t0, vp[0], _x0 );
		}

		if( tp != NULL )
		{
			VAL_ASSIGN( tp[0], _t0 );
			tp += 1*inct;
		}

		VAL_SCALE( _t0, alpha );

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 1*1 )
		{
			oski_index_t j0 = ind[K];
			oski_value_t* yp = y + j0;
			REGISTER oski_value_t _y0;
			VAL_SET_ZERO( _y0 );
			VAL_MAC( _y0, vp[0], _t0 );
			VAL_INC( yp[0], _y0 );
		}

		val += (ptr[I+1]-ptr[I]);
	}
} /* BCSR_MatTransMatMult_v1_aX_b1_xs1_ys1 */

#if defined(DO_NAME_MANGLING)
/** Mangled name for BCSR_MatTransMatMult_v1_aX_b1_xsX_ysX. */
#define BCSR_MatTransMatMult_v1_aX_b1_xsX_ysX \
	MANGLE_MOD_(BCSR_MatTransMatMult_v1_aX_b1_xsX_ysX_1x1)
#endif

/**
 *  \brief The \f$1\times 1\f$ BCSR implementation
 *  of \f$A^TA\cdot x\f$, where the source and destination
 *  vectors have general-stride and general-stride,
 *  respectively.
 *
 *  Parameter 'tp' may be NULL if no intermediate vector output
 *  is desired.
 */
void
BCSR_MatTransMatMult_v1_aX_b1_xsX_ysX( oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y, oski_index_t incy,
	oski_value_t* restrict t, oski_index_t inct )
{
	oski_index_t I;
	oski_value_t* tp = t;

	for( I = 0; I < M; I++ )
	{
		oski_index_t K;
		REGISTER oski_value_t _t0 = MAKE_VAL_COMPLEX(0.0, 0.0);
		const oski_value_t* vp;

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 1*1 )
		{
			oski_index_t j0 = ind[K];
			const oski_value_t* xp = x + j0*incx;
			REGISTER oski_value_t _x0;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_MAC( _t0, vp[0], _x0 );
		}

		if( tp != NULL )
		{
			VAL_ASSIGN( tp[0], _t0 );
			tp += 1*inct;
		}

		VAL_SCALE( _t0, alpha );

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 1*1 )
		{
			oski_index_t j0 = ind[K];
			oski_value_t* yp = y + j0*incy;
			REGISTER oski_value_t _y0;
			VAL_SET_ZERO( _y0 );
			VAL_MAC( _y0, vp[0], _t0 );
			VAL_INC( yp[0], _y0 );
		}

		val += (ptr[I+1]-ptr[I]);
	}
} /* BCSR_MatTransMatMult_v1_aX_b1_xsX_ysX */


#if defined(DO_NAME_MANGLING)
/** Mangled name for primary exportable symbol, MatReprTransMatReprMult. */
#define MatReprTransMatReprMult MANGLE_MOD_(MatReprTransMatReprMult_1x1)
#endif

/**
 *  \brief Exported module wrapper for the \f$1\times 1\f$
 *  implementation of \f$A^TA\cdot x\f$.
 */
int
MatReprTransMatReprMult( const oski_matBCSR_t* A,
	oski_value_t alpha, const oski_vecview_t x,
	oski_vecview_t y, oski_vecview_t t )
{
	oski_index_t j;
	const oski_value_t* xp = x->val;
	oski_value_t* yp = y->val;
	oski_value_t* tp;
	oski_index_t inct;

	if( t == INVALID_VEC )
	{
		tp = NULL;
		inct = 0;
	}
	else
	{
		tp = t->val;
		inct = t->rowinc;
	}

	for( j = 0; j < x->num_cols; j++ )
	{
		if( x->rowinc == 1 && y->rowinc == 1 )
			BCSR_MatTransMatMult_v1_aX_b1_xs1_ys1(
				A->num_block_rows, A->bptr, A->bind, A->bval,
				alpha, xp, yp, tp, inct );
		else
			BCSR_MatTransMatMult_v1_aX_b1_xsX_ysX(
				A->num_block_rows,
				A->bptr, A->bind, A->bval, alpha,
				xp, x->rowinc, yp, y->rowinc, tp, inct );

		xp += x->colinc;
		yp += y->colinc;
		if( tp != NULL ) tp += t->colinc;
	}
	return 0;
}

#if defined(DO_NAME_MANGLING)
/** Mangled name for BCSR_MatHermMatMult_v1_aX_b1_xs1_ys1. */
#define BCSR_MatHermMatMult_v1_aX_b1_xs1_ys1 \
	MANGLE_MOD_(BCSR_MatHermMatMult_v1_aX_b1_xs1_ys1_1x1)
#endif

/**
 *  \brief The \f$1\times 1\f$ BCSR implementation
 *  of \f$A^HA\cdot x\f$, where the source and destination
 *  vectors have unit-stride and unit-stride,
 *  respectively.
 *
 *  Parameter 'tp' may be NULL if no intermediate vector output
 *  is desired.
 */
void
BCSR_MatHermMatMult_v1_aX_b1_xs1_ys1( oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val,
	oski_value_t alpha, const oski_value_t* restrict x,
	oski_value_t* restrict y,
	oski_value_t* restrict t, oski_index_t inct )
{
	oski_index_t I;
	oski_value_t* tp = t;

	for( I = 0; I < M; I++ )
	{
		oski_index_t K;
		REGISTER oski_value_t _t0 = MAKE_VAL_COMPLEX(0.0, 0.0);
		const oski_value_t* vp;

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 1*1 )
		{
			oski_index_t j0 = ind[K];
			const oski_value_t* xp = x + j0;
			REGISTER oski_value_t _x0;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_MAC( _t0, vp[0], _x0 );
		}

		if( tp != NULL )
		{
			VAL_ASSIGN( tp[0], _t0 );
			tp += 1*inct;
		}

		VAL_SCALE( _t0, alpha );

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 1*1 )
		{
			oski_index_t j0 = ind[K];
			oski_value_t* yp = y + j0;
			REGISTER oski_value_t _y0;
			VAL_SET_ZERO( _y0 );
			VAL_MAC_CONJ( _y0, vp[0], _t0 );
			VAL_INC( yp[0], _y0 );
		}

		val += (ptr[I+1]-ptr[I]);
	}
} /* BCSR_MatHermMatMult_v1_aX_b1_xs1_ys1 */

#if defined(DO_NAME_MANGLING)
/** Mangled name for BCSR_MatHermMatMult_v1_aX_b1_xsX_ysX. */
#define BCSR_MatHermMatMult_v1_aX_b1_xsX_ysX \
	MANGLE_MOD_(BCSR_MatHermMatMult_v1_aX_b1_xsX_ysX_1x1)
#endif

/**
 *  \brief The \f$1\times 1\f$ BCSR implementation
 *  of \f$A^HA\cdot x\f$, where the source and destination
 *  vectors have general-stride and general-stride,
 *  respectively.
 *
 *  Parameter 'tp' may be NULL if no intermediate vector output
 *  is desired.
 */
void
BCSR_MatHermMatMult_v1_aX_b1_xsX_ysX( oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict ind,
	const oski_value_t* restrict val,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y, oski_index_t incy,
	oski_value_t* restrict t, oski_index_t inct )
{
	oski_index_t I;
	oski_value_t* tp = t;

	for( I = 0; I < M; I++ )
	{
		oski_index_t K;
		REGISTER oski_value_t _t0 = MAKE_VAL_COMPLEX(0.0, 0.0);
		const oski_value_t* vp;

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 1*1 )
		{
			oski_index_t j0 = ind[K];
			const oski_value_t* xp = x + j0*incx;
			REGISTER oski_value_t _x0;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_MAC( _t0, vp[0], _x0 );
		}

		if( tp != NULL )
		{
			VAL_ASSIGN( tp[0], _t0 );
			tp += 1*inct;
		}

		VAL_SCALE( _t0, alpha );

		for( K = ptr[I], vp = val; K < ptr[I+1]; K++, vp += 1*1 )
		{
			oski_index_t j0 = ind[K];
			oski_value_t* yp = y + j0*incy;
			REGISTER oski_value_t _y0;
			VAL_SET_ZERO( _y0 );
			VAL_MAC_CONJ( _y0, vp[0], _t0 );
			VAL_INC( yp[0], _y0 );
		}

		val += (ptr[I+1]-ptr[I]);
	}
} /* BCSR_MatHermMatMult_v1_aX_b1_xsX_ysX */


#if defined(DO_NAME_MANGLING)
/** Mangled name for primary exportable symbol, MatReprHermMatReprMult. */
#define MatReprHermMatReprMult MANGLE_MOD_(MatReprHermMatReprMult_1x1)
#endif

/**
 *  \brief Exported module wrapper for the \f$1\times 1\f$
 *  implementation of \f$A^HA\cdot x\f$.
 */
int
MatReprHermMatReprMult( const oski_matBCSR_t* A,
	oski_value_t alpha, const oski_vecview_t x,
	oski_vecview_t y, oski_vecview_t t )
{
	oski_index_t j;
	const oski_value_t* xp = x->val;
	oski_value_t* yp = y->val;
	oski_value_t* tp;
	oski_index_t inct;

	if( t == INVALID_VEC )
	{
		tp = NULL;
		inct = 0;
	}
	else
	{
		tp = t->val;
		inct = t->rowinc;
	}

	for( j = 0; j < x->num_cols; j++ )
	{
		if( x->rowinc == 1 && y->rowinc == 1 )
			BCSR_MatHermMatMult_v1_aX_b1_xs1_ys1(
				A->num_block_rows, A->bptr, A->bind, A->bval,
				alpha, xp, yp, tp, inct );
		else
			BCSR_MatHermMatMult_v1_aX_b1_xsX_ysX(
				A->num_block_rows,
				A->bptr, A->bind, A->bval, alpha,
				xp, x->rowinc, yp, y->rowinc, tp, inct );

		xp += x->colinc;
		yp += y->colinc;
		if( tp != NULL ) tp += t->colinc;
	}
	return 0;
}


/* eof */
