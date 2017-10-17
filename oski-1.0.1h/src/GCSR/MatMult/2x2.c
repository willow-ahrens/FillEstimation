/**
 *  \file src/GCSR/MatMult/2x2.c
 *  \brief The \f$2\times 2\f$ GCSR implementation
 *  of simultaneous multiplication by \f$A\f$ and \f$\mathrm{op}(A)\f$.
 *  \ingroup MATTYPE_GCSR
 *
 *  Automatically generated by ./gen.sh on Sun Jun 12 21:42:58 PDT 2005.
 */

#include <assert.h>

#include <oski/config.h> /* for 'restrict' keyword */
#include <oski/common.h>
#include <oski/mangle.h>
#include <oski/vecview.h>
#include <oski/GCSR/format.h>
#include <oski/GCSR/module.h>

#if IS_VAL_COMPLEX
	/** Complex-valued, so do not use explicit 'register' keyword. */
	#define REGISTER
#else
	/** Real-valued, so use explicit 'register' keyword. */
	#define REGISTER register
#endif

#if defined(DO_NAME_MANGLING)
/** Mangled name for GCSR_MatMult_v1_aX_b1_xs1_ysX. */
#define GCSR_MatMult_v1_aX_b1_xs1_ysX MANGLE_MOD_(GCSR_MatMult_v1_aX_b1_xs1_ysX_2x2)
#endif

/**
 *  \brief The \f$2\times 2\f$ GCSR implementation
 *  of \f$A\cdot x\f$, where x and y vectors have
 *  unit-stride and general-stride, respectively.
 */
void
GCSR_MatMult_v1_aX_b1_xs1_ysX(
	oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict rind,
	const oski_index_t* restrict cind, const oski_value_t* restrict val,
	oski_value_t alpha, const oski_value_t* restrict x,
	oski_value_t* restrict y, oski_index_t incy )
{
	oski_index_t I;
	for( I = 0; I < M; I++ )
	{
		oski_index_t i0 = rind[I];
		oski_index_t K;
		REGISTER oski_value_t _y0;
		REGISTER oski_value_t _y1;
		oski_value_t* yp = y + i0*incy;

		VAL_SET_ZERO( _y0 );
		VAL_SET_ZERO( _y1 );

		for( K = ptr[I]; K < ptr[I+1]; K++, val += 2*2 )
		{
			oski_index_t j0 = cind[K];
			const oski_value_t* xp = x + j0;
			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;

			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1] );
			VAL_MAC( _y0, val[0], _x0 );
			VAL_MAC( _y1, val[2], _x0 );
			VAL_MAC( _y0, val[1], _x1 );
			VAL_MAC( _y1, val[3], _x1 );

		} /* K */
		VAL_MAC( yp[0], alpha, _y0 );
		VAL_MAC( yp[1*incy], alpha, _y1 );

	} /* I */
} /* GCSR_MatMult_v1_aX_b1_xs1_ysX */

#if defined(DO_NAME_MANGLING)
/** Mangled name for GCSR_MatMult_v1_aX_b1_xsX_ysX. */
#define GCSR_MatMult_v1_aX_b1_xsX_ysX MANGLE_MOD_(GCSR_MatMult_v1_aX_b1_xsX_ysX_2x2)
#endif

/**
 *  \brief The \f$2\times 2\f$ GCSR implementation
 *  of \f$A\cdot x\f$, where x and y vectors have
 *  general-stride and general-stride, respectively.
 */
void
GCSR_MatMult_v1_aX_b1_xsX_ysX(
	oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict rind,
	const oski_index_t* restrict cind, const oski_value_t* restrict val,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y, oski_index_t incy )
{
	oski_index_t I;
	for( I = 0; I < M; I++ )
	{
		oski_index_t i0 = rind[I];
		oski_index_t K;
		REGISTER oski_value_t _y0;
		REGISTER oski_value_t _y1;
		oski_value_t* yp = y + i0*incy;

		VAL_SET_ZERO( _y0 );
		VAL_SET_ZERO( _y1 );

		for( K = ptr[I]; K < ptr[I+1]; K++, val += 2*2 )
		{
			oski_index_t j0 = cind[K];
			const oski_value_t* xp = x + j0*incx;
			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;

			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1*incx] );
			VAL_MAC( _y0, val[0], _x0 );
			VAL_MAC( _y1, val[2], _x0 );
			VAL_MAC( _y0, val[1], _x1 );
			VAL_MAC( _y1, val[3], _x1 );

		} /* K */
		VAL_MAC( yp[0], alpha, _y0 );
		VAL_MAC( yp[1*incy], alpha, _y1 );

	} /* I */
} /* GCSR_MatMult_v1_aX_b1_xsX_ysX */


/**
 *  \brief Exported module wrapper for the \f$2\times 2\f$
 *  implementation of \f$A\cdot x\f$.
 */
static int
MatMult( const oski_matGCSR_t* A,
	oski_value_t alpha, const oski_vecview_t x, oski_vecview_t y )
{
	oski_index_t j;
	const oski_value_t* restrict xp = x->val;
	oski_value_t* restrict yp = y->val;

	assert( A != NULL );
#if 0
	assert( A->r == 2 );
	assert( A->c == 2 );
#endif

	for( j = 0; j < x->num_cols; j++ )
	{
		if( x->rowinc == 1 )
			GCSR_MatMult_v1_aX_b1_xs1_ysX(
				A->num_stored_rows,
				A->ptr, A->rind, A->cind, A->val,
				alpha, xp, yp, y->rowinc );
		else
			GCSR_MatMult_v1_aX_b1_xsX_ysX(
				A->num_stored_rows,
				A->ptr, A->rind, A->cind, A->val,
				alpha, xp, x->rowinc, yp, y->rowinc );

		xp += x->colinc;
		yp += y->colinc;
	}
	return 0;
} /* MatMult */

#if IS_VAL_COMPLEX
#if defined(DO_NAME_MANGLING)
/** Mangled name for GCSR_MatConjMult_v1_aX_b1_xs1_ysX. */
#define GCSR_MatConjMult_v1_aX_b1_xs1_ysX MANGLE_MOD_(GCSR_MatConjMult_v1_aX_b1_xs1_ysX_2x2)
#endif

/**
 *  \brief The \f$2\times 2\f$ GCSR implementation
 *  of \f$\bar{A}\cdot x\f$, where x and y vectors have
 *  unit-stride and general-stride, respectively.
 */
void
GCSR_MatConjMult_v1_aX_b1_xs1_ysX(
	oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict rind,
	const oski_index_t* restrict cind, const oski_value_t* restrict val,
	oski_value_t alpha, const oski_value_t* restrict x,
	oski_value_t* restrict y, oski_index_t incy )
{
	oski_index_t I;
	for( I = 0; I < M; I++ )
	{
		oski_index_t i0 = rind[I];
		oski_index_t K;
		REGISTER oski_value_t _y0;
		REGISTER oski_value_t _y1;
		oski_value_t* yp = y + i0*incy;

		VAL_SET_ZERO( _y0 );
		VAL_SET_ZERO( _y1 );

		for( K = ptr[I]; K < ptr[I+1]; K++, val += 2*2 )
		{
			oski_index_t j0 = cind[K];
			const oski_value_t* xp = x + j0;
			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;

			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1] );
			VAL_MAC_CONJ( _y0, val[0], _x0 );
			VAL_MAC_CONJ( _y1, val[2], _x0 );
			VAL_MAC_CONJ( _y0, val[1], _x1 );
			VAL_MAC_CONJ( _y1, val[3], _x1 );

		} /* K */
		VAL_MAC( yp[0], alpha, _y0 );
		VAL_MAC( yp[1*incy], alpha, _y1 );

	} /* I */
} /* GCSR_MatConjMult_v1_aX_b1_xs1_ysX */

#endif /* IS_VAL_COMPLEX */
#if IS_VAL_COMPLEX
#if defined(DO_NAME_MANGLING)
/** Mangled name for GCSR_MatConjMult_v1_aX_b1_xsX_ysX. */
#define GCSR_MatConjMult_v1_aX_b1_xsX_ysX MANGLE_MOD_(GCSR_MatConjMult_v1_aX_b1_xsX_ysX_2x2)
#endif

/**
 *  \brief The \f$2\times 2\f$ GCSR implementation
 *  of \f$\bar{A}\cdot x\f$, where x and y vectors have
 *  general-stride and general-stride, respectively.
 */
void
GCSR_MatConjMult_v1_aX_b1_xsX_ysX(
	oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict rind,
	const oski_index_t* restrict cind, const oski_value_t* restrict val,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y, oski_index_t incy )
{
	oski_index_t I;
	for( I = 0; I < M; I++ )
	{
		oski_index_t i0 = rind[I];
		oski_index_t K;
		REGISTER oski_value_t _y0;
		REGISTER oski_value_t _y1;
		oski_value_t* yp = y + i0*incy;

		VAL_SET_ZERO( _y0 );
		VAL_SET_ZERO( _y1 );

		for( K = ptr[I]; K < ptr[I+1]; K++, val += 2*2 )
		{
			oski_index_t j0 = cind[K];
			const oski_value_t* xp = x + j0*incx;
			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;

			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1*incx] );
			VAL_MAC_CONJ( _y0, val[0], _x0 );
			VAL_MAC_CONJ( _y1, val[2], _x0 );
			VAL_MAC_CONJ( _y0, val[1], _x1 );
			VAL_MAC_CONJ( _y1, val[3], _x1 );

		} /* K */
		VAL_MAC( yp[0], alpha, _y0 );
		VAL_MAC( yp[1*incy], alpha, _y1 );

	} /* I */
} /* GCSR_MatConjMult_v1_aX_b1_xsX_ysX */

#endif /* IS_VAL_COMPLEX */

/**
 *  \brief Exported module wrapper for the \f$2\times 2\f$
 *  implementation of \f$\bar{A}\cdot x\f$.
 */
static int
MatConjMult( const oski_matGCSR_t* A,
	oski_value_t alpha, const oski_vecview_t x, oski_vecview_t y )
{
#if !IS_VAL_COMPLEX
	/* Call equivalent real kernel */
	return MatMult( A, alpha, x, y );
#else /* IS_VAL_COMPLEX */

	oski_index_t j;
	const oski_value_t* restrict xp = x->val;
	oski_value_t* restrict yp = y->val;

	assert( A != NULL );
#if 0
	assert( A->r == 2 );
	assert( A->c == 2 );
#endif

	for( j = 0; j < x->num_cols; j++ )
	{
		if( x->rowinc == 1 )
			GCSR_MatConjMult_v1_aX_b1_xs1_ysX(
				A->num_stored_rows,
				A->ptr, A->rind, A->cind, A->val,
				alpha, xp, yp, y->rowinc );
		else
			GCSR_MatConjMult_v1_aX_b1_xsX_ysX(
				A->num_stored_rows,
				A->ptr, A->rind, A->cind, A->val,
				alpha, xp, x->rowinc, yp, y->rowinc );

		xp += x->colinc;
		yp += y->colinc;
	}
	return 0;
#endif /* IS_VAL_COMPLEX */
} /* MatConjMult */

#if defined(DO_NAME_MANGLING)
/** Mangled name for GCSR_MatTransMult_v1_aX_b1_xsX_ys1. */
#define GCSR_MatTransMult_v1_aX_b1_xsX_ys1 MANGLE_MOD_(GCSR_MatTransMult_v1_aX_b1_xsX_ys1_2x2)
#endif

/**
 *  \brief The \f$2\times 2\f$ GCSR implementation
 *  of \f$A^T\cdot x\f$, where x and y vectors have
 *  general-stride and unit-stride, respectively.
 */
void
GCSR_MatTransMult_v1_aX_b1_xsX_ys1(
	oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict rind,
	const oski_index_t* restrict cind, const oski_value_t* restrict val,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y )
{
	oski_index_t I;
	for( I = 0; I < M; I++ )
	{
		oski_index_t i0 = rind[I];
		oski_index_t K;
		REGISTER oski_value_t _x0;
		REGISTER oski_value_t _x1;
		const oski_value_t* xp = x + i0*incx;

		VAL_MUL( _x0, alpha, xp[0] );
		VAL_MUL( _x1, alpha, xp[1*incx] );

		for( K = ptr[I]; K < ptr[I+1]; K++, val += 2*2 )
		{
			oski_index_t j0 = cind[K];
			oski_value_t* yp = y + j0;
			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;

			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_MAC( _y0, val[0], _x0 );
			VAL_MAC( _y1, val[1], _x0 );
			VAL_MAC( _y0, val[2], _x1 );
			VAL_MAC( _y1, val[3], _x1 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1], _y1 );
		} /* K */
	} /* I */
} /* GCSR_MatTransMult_v1_aX_b1_xsX_ys1 */

#if defined(DO_NAME_MANGLING)
/** Mangled name for GCSR_MatTransMult_v1_aX_b1_xsX_ysX. */
#define GCSR_MatTransMult_v1_aX_b1_xsX_ysX MANGLE_MOD_(GCSR_MatTransMult_v1_aX_b1_xsX_ysX_2x2)
#endif

/**
 *  \brief The \f$2\times 2\f$ GCSR implementation
 *  of \f$A^T\cdot x\f$, where x and y vectors have
 *  general-stride and general-stride, respectively.
 */
void
GCSR_MatTransMult_v1_aX_b1_xsX_ysX(
	oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict rind,
	const oski_index_t* restrict cind, const oski_value_t* restrict val,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y, oski_index_t incy )
{
	oski_index_t I;
	for( I = 0; I < M; I++ )
	{
		oski_index_t i0 = rind[I];
		oski_index_t K;
		REGISTER oski_value_t _x0;
		REGISTER oski_value_t _x1;
		const oski_value_t* xp = x + i0*incx;

		VAL_MUL( _x0, alpha, xp[0] );
		VAL_MUL( _x1, alpha, xp[1*incx] );

		for( K = ptr[I]; K < ptr[I+1]; K++, val += 2*2 )
		{
			oski_index_t j0 = cind[K];
			oski_value_t* yp = y + j0*incy;
			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;

			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_MAC( _y0, val[0], _x0 );
			VAL_MAC( _y1, val[1], _x0 );
			VAL_MAC( _y0, val[2], _x1 );
			VAL_MAC( _y1, val[3], _x1 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1*incy], _y1 );
		} /* K */
	} /* I */
} /* GCSR_MatTransMult_v1_aX_b1_xsX_ysX */


/**
 *  \brief Exported module wrapper for the \f$2\times 2\f$
 *  implementation of \f$A^T\cdot x\f$.
 */
static int
MatTransMult( const oski_matGCSR_t* A,
	oski_value_t alpha, const oski_vecview_t x, oski_vecview_t y )
{
	oski_index_t j;
	const oski_value_t* restrict xp = x->val;
	oski_value_t* restrict yp = y->val;

	assert( A != NULL );
#if 0
	assert( A->r == 2 );
	assert( A->c == 2 );
#endif

	for( j = 0; j < x->num_cols; j++ )
	{
		if( y->rowinc == 1 )
			GCSR_MatTransMult_v1_aX_b1_xsX_ys1(
				A->num_stored_rows,
				A->ptr, A->rind, A->cind, A->val,
				alpha, xp, x->rowinc, yp );
		else
			GCSR_MatTransMult_v1_aX_b1_xsX_ysX(
				A->num_stored_rows,
				A->ptr, A->rind, A->cind, A->val,
				alpha, xp, x->rowinc, yp, y->rowinc );

		xp += x->colinc;
		yp += y->colinc;
	}
	return 0;
} /* MatTransMult */

#if IS_VAL_COMPLEX
#if defined(DO_NAME_MANGLING)
/** Mangled name for GCSR_MatHermMult_v1_aX_b1_xsX_ys1. */
#define GCSR_MatHermMult_v1_aX_b1_xsX_ys1 MANGLE_MOD_(GCSR_MatHermMult_v1_aX_b1_xsX_ys1_2x2)
#endif

/**
 *  \brief The \f$2\times 2\f$ GCSR implementation
 *  of \f$\bar{A}^T\cdot x\f$, where x and y vectors have
 *  general-stride and unit-stride, respectively.
 */
void
GCSR_MatHermMult_v1_aX_b1_xsX_ys1(
	oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict rind,
	const oski_index_t* restrict cind, const oski_value_t* restrict val,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y )
{
	oski_index_t I;
	for( I = 0; I < M; I++ )
	{
		oski_index_t i0 = rind[I];
		oski_index_t K;
		REGISTER oski_value_t _x0;
		REGISTER oski_value_t _x1;
		const oski_value_t* xp = x + i0*incx;

		VAL_MUL( _x0, alpha, xp[0] );
		VAL_MUL( _x1, alpha, xp[1*incx] );

		for( K = ptr[I]; K < ptr[I+1]; K++, val += 2*2 )
		{
			oski_index_t j0 = cind[K];
			oski_value_t* yp = y + j0;
			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;

			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_MAC_CONJ( _y0, val[0], _x0 );
			VAL_MAC_CONJ( _y1, val[1], _x0 );
			VAL_MAC_CONJ( _y0, val[2], _x1 );
			VAL_MAC_CONJ( _y1, val[3], _x1 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1], _y1 );
		} /* K */
	} /* I */
} /* GCSR_MatHermMult_v1_aX_b1_xsX_ys1 */

#endif /* IS_VAL_COMPLEX */
#if IS_VAL_COMPLEX
#if defined(DO_NAME_MANGLING)
/** Mangled name for GCSR_MatHermMult_v1_aX_b1_xsX_ysX. */
#define GCSR_MatHermMult_v1_aX_b1_xsX_ysX MANGLE_MOD_(GCSR_MatHermMult_v1_aX_b1_xsX_ysX_2x2)
#endif

/**
 *  \brief The \f$2\times 2\f$ GCSR implementation
 *  of \f$\bar{A}^T\cdot x\f$, where x and y vectors have
 *  general-stride and general-stride, respectively.
 */
void
GCSR_MatHermMult_v1_aX_b1_xsX_ysX(
	oski_index_t M,
	const oski_index_t* restrict ptr, const oski_index_t* restrict rind,
	const oski_index_t* restrict cind, const oski_value_t* restrict val,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y, oski_index_t incy )
{
	oski_index_t I;
	for( I = 0; I < M; I++ )
	{
		oski_index_t i0 = rind[I];
		oski_index_t K;
		REGISTER oski_value_t _x0;
		REGISTER oski_value_t _x1;
		const oski_value_t* xp = x + i0*incx;

		VAL_MUL( _x0, alpha, xp[0] );
		VAL_MUL( _x1, alpha, xp[1*incx] );

		for( K = ptr[I]; K < ptr[I+1]; K++, val += 2*2 )
		{
			oski_index_t j0 = cind[K];
			oski_value_t* yp = y + j0*incy;
			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;

			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_MAC_CONJ( _y0, val[0], _x0 );
			VAL_MAC_CONJ( _y1, val[1], _x0 );
			VAL_MAC_CONJ( _y0, val[2], _x1 );
			VAL_MAC_CONJ( _y1, val[3], _x1 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1*incy], _y1 );
		} /* K */
	} /* I */
} /* GCSR_MatHermMult_v1_aX_b1_xsX_ysX */

#endif /* IS_VAL_COMPLEX */

/**
 *  \brief Exported module wrapper for the \f$2\times 2\f$
 *  implementation of \f$\bar{A}^T\cdot x\f$.
 */
static int
MatHermMult( const oski_matGCSR_t* A,
	oski_value_t alpha, const oski_vecview_t x, oski_vecview_t y )
{
#if !IS_VAL_COMPLEX
	/* Call equivalent real kernel */
	return MatTransMult( A, alpha, x, y );
#else /* IS_VAL_COMPLEX */

	oski_index_t j;
	const oski_value_t* restrict xp = x->val;
	oski_value_t* restrict yp = y->val;

	assert( A != NULL );
#if 0
	assert( A->r == 2 );
	assert( A->c == 2 );
#endif

	for( j = 0; j < x->num_cols; j++ )
	{
		if( y->rowinc == 1 )
			GCSR_MatHermMult_v1_aX_b1_xsX_ys1(
				A->num_stored_rows,
				A->ptr, A->rind, A->cind, A->val,
				alpha, xp, x->rowinc, yp );
		else
			GCSR_MatHermMult_v1_aX_b1_xsX_ysX(
				A->num_stored_rows,
				A->ptr, A->rind, A->cind, A->val,
				alpha, xp, x->rowinc, yp, y->rowinc );

		xp += x->colinc;
		yp += y->colinc;
	}
	return 0;
#endif /* IS_VAL_COMPLEX */
} /* MatHermMult */


#if defined(DO_NAME_MANGLING)
/** Mangled name for primary exportable symbol */
#define MatReprMult MANGLE_MOD_(MatReprMult_2x2)
#endif

/**
 *  \brief Entry point to the 2x2 kernel that implements
 *  simultaneous multiplication by sparse \f$A\f$ and
 *  \f$\mathrm{op}(A)\f$.
 */
int
MatReprMult(
	const oski_matGCSR_t* A, oski_matop_t opA,
	oski_value_t alpha, const oski_vecview_t x, oski_vecview_t y )
{
	int err = 0;
	switch( opA ) {
		case OP_NORMAL:
			err = MatMult( A, alpha, x, y );
			break;
		case OP_TRANS:
			err = MatTransMult( A, alpha, x, y );
			break;
		case OP_CONJ:
			err = MatConjMult( A, alpha, x, y );
			break;
		case OP_CONJ_TRANS:
			err = MatHermMult( A, alpha, x, y );
			break;
		default:
			assert( 0 );
	}
	return err;
}

/* eof */
