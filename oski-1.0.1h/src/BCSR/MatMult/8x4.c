/**
 *  \file BCSR_8x4.c
 *  \brief BCSR 8x4 SpMV implementation, for all transpose options.
 *  \ingroup MATTYPE_BCSR
 *
 *  Automatically generated by ./gen_bcsr2.sh on Tue Jun  7 13:29:01 PDT 2005.
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
/** Complex-valued, so do not use explicit 'register' keyword */
#define REGISTER
#else
/** Real-valued, so use explicit 'register' keyword */
#define REGISTER register
#endif



#if defined(DO_NAME_MANGLING)
/** Mangled name for BCSR_MatMult_v1_aX_b1_xs1_ysX */
#define BCSR_MatMult_v1_aX_b1_xs1_ysX MANGLE_MOD_(BCSR_MatMult_v1_aX_b1_xs1_ysX_8x4)
#endif

/**
 *  \brief BCSR implementation of
 *  \f$y \leftarrow y + \alpha\cdot A\cdot x\f$.
 */
void
BCSR_MatMult_v1_aX_b1_xs1_ysX( oski_index_t M,
	const oski_index_t* restrict bptr, const oski_index_t* restrict bind,
	const oski_value_t* restrict bval,
	oski_value_t alpha, const oski_value_t* restrict x,
	oski_value_t* restrict y, oski_index_t incy )
{
	oski_value_t* yp;
	oski_index_t I;

	for( I = 0, yp = y; I < M; I++, yp += 8*incy )
	{
		REGISTER oski_value_t _y0;
		REGISTER oski_value_t _y1;
		REGISTER oski_value_t _y2;
		REGISTER oski_value_t _y3;
		REGISTER oski_value_t _y4;
		REGISTER oski_value_t _y5;
		REGISTER oski_value_t _y6;
		REGISTER oski_value_t _y7;
		oski_index_t K;

		VAL_SET_ZERO( _y0 );
		VAL_SET_ZERO( _y1 );
		VAL_SET_ZERO( _y2 );
		VAL_SET_ZERO( _y3 );
		VAL_SET_ZERO( _y4 );
		VAL_SET_ZERO( _y5 );
		VAL_SET_ZERO( _y6 );
		VAL_SET_ZERO( _y7 );

		for( K = bptr[I]; K < bptr[I+1]; K++, bind++, bval += 8*4 )
		{
			oski_index_t j0 = bind[0]; /* block's leftmost col index */
			const oski_value_t* xp = x + j0;

			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;
			REGISTER oski_value_t _x2;
			REGISTER oski_value_t _x3;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1] );
			VAL_ASSIGN( _x2, xp[2] );
			VAL_ASSIGN( _x3, xp[3] );
			VAL_MAC( _y0, bval[0], _x0 );
			VAL_MAC( _y1, bval[4], _x0 );
			VAL_MAC( _y2, bval[8], _x0 );
			VAL_MAC( _y3, bval[12], _x0 );
			VAL_MAC( _y4, bval[16], _x0 );
			VAL_MAC( _y5, bval[20], _x0 );
			VAL_MAC( _y6, bval[24], _x0 );
			VAL_MAC( _y7, bval[28], _x0 );
			VAL_MAC( _y0, bval[1], _x1 );
			VAL_MAC( _y1, bval[5], _x1 );
			VAL_MAC( _y2, bval[9], _x1 );
			VAL_MAC( _y3, bval[13], _x1 );
			VAL_MAC( _y4, bval[17], _x1 );
			VAL_MAC( _y5, bval[21], _x1 );
			VAL_MAC( _y6, bval[25], _x1 );
			VAL_MAC( _y7, bval[29], _x1 );
			VAL_MAC( _y0, bval[2], _x2 );
			VAL_MAC( _y1, bval[6], _x2 );
			VAL_MAC( _y2, bval[10], _x2 );
			VAL_MAC( _y3, bval[14], _x2 );
			VAL_MAC( _y4, bval[18], _x2 );
			VAL_MAC( _y5, bval[22], _x2 );
			VAL_MAC( _y6, bval[26], _x2 );
			VAL_MAC( _y7, bval[30], _x2 );
			VAL_MAC( _y0, bval[3], _x3 );
			VAL_MAC( _y1, bval[7], _x3 );
			VAL_MAC( _y2, bval[11], _x3 );
			VAL_MAC( _y3, bval[15], _x3 );
			VAL_MAC( _y4, bval[19], _x3 );
			VAL_MAC( _y5, bval[23], _x3 );
			VAL_MAC( _y6, bval[27], _x3 );
			VAL_MAC( _y7, bval[31], _x3 );
		}

		VAL_MAC( yp[0], alpha, _y0 );
		VAL_MAC( yp[1*incy], alpha, _y1 );
		VAL_MAC( yp[2*incy], alpha, _y2 );
		VAL_MAC( yp[3*incy], alpha, _y3 );
		VAL_MAC( yp[4*incy], alpha, _y4 );
		VAL_MAC( yp[5*incy], alpha, _y5 );
		VAL_MAC( yp[6*incy], alpha, _y6 );
		VAL_MAC( yp[7*incy], alpha, _y7 );
	}
}


#if defined(DO_NAME_MANGLING)
/** Mangled name for BCSR_MatMult_v1_aX_b1_xsX_ysX */
#define BCSR_MatMult_v1_aX_b1_xsX_ysX MANGLE_MOD_(BCSR_MatMult_v1_aX_b1_xsX_ysX_8x4)
#endif

/**
 *  \brief BCSR implementation of
 *  \f$y \leftarrow y + \alpha\cdot A\cdot x\f$.
 */
void
BCSR_MatMult_v1_aX_b1_xsX_ysX( oski_index_t M,
	const oski_index_t* restrict bptr, const oski_index_t* restrict bind,
	const oski_value_t* restrict bval,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y, oski_index_t incy )
{
	oski_value_t* yp;
	oski_index_t I;

	for( I = 0, yp = y; I < M; I++, yp += 8*incy )
	{
		REGISTER oski_value_t _y0;
		REGISTER oski_value_t _y1;
		REGISTER oski_value_t _y2;
		REGISTER oski_value_t _y3;
		REGISTER oski_value_t _y4;
		REGISTER oski_value_t _y5;
		REGISTER oski_value_t _y6;
		REGISTER oski_value_t _y7;
		oski_index_t K;

		VAL_SET_ZERO( _y0 );
		VAL_SET_ZERO( _y1 );
		VAL_SET_ZERO( _y2 );
		VAL_SET_ZERO( _y3 );
		VAL_SET_ZERO( _y4 );
		VAL_SET_ZERO( _y5 );
		VAL_SET_ZERO( _y6 );
		VAL_SET_ZERO( _y7 );

		for( K = bptr[I]; K < bptr[I+1]; K++, bind++, bval += 8*4 )
		{
			oski_index_t j0 = bind[0]; /* block's leftmost col index */
			const oski_value_t* xp = x + j0*incx;

			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;
			REGISTER oski_value_t _x2;
			REGISTER oski_value_t _x3;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1*incx] );
			VAL_ASSIGN( _x2, xp[2*incx] );
			VAL_ASSIGN( _x3, xp[3*incx] );
			VAL_MAC( _y0, bval[0], _x0 );
			VAL_MAC( _y1, bval[4], _x0 );
			VAL_MAC( _y2, bval[8], _x0 );
			VAL_MAC( _y3, bval[12], _x0 );
			VAL_MAC( _y4, bval[16], _x0 );
			VAL_MAC( _y5, bval[20], _x0 );
			VAL_MAC( _y6, bval[24], _x0 );
			VAL_MAC( _y7, bval[28], _x0 );
			VAL_MAC( _y0, bval[1], _x1 );
			VAL_MAC( _y1, bval[5], _x1 );
			VAL_MAC( _y2, bval[9], _x1 );
			VAL_MAC( _y3, bval[13], _x1 );
			VAL_MAC( _y4, bval[17], _x1 );
			VAL_MAC( _y5, bval[21], _x1 );
			VAL_MAC( _y6, bval[25], _x1 );
			VAL_MAC( _y7, bval[29], _x1 );
			VAL_MAC( _y0, bval[2], _x2 );
			VAL_MAC( _y1, bval[6], _x2 );
			VAL_MAC( _y2, bval[10], _x2 );
			VAL_MAC( _y3, bval[14], _x2 );
			VAL_MAC( _y4, bval[18], _x2 );
			VAL_MAC( _y5, bval[22], _x2 );
			VAL_MAC( _y6, bval[26], _x2 );
			VAL_MAC( _y7, bval[30], _x2 );
			VAL_MAC( _y0, bval[3], _x3 );
			VAL_MAC( _y1, bval[7], _x3 );
			VAL_MAC( _y2, bval[11], _x3 );
			VAL_MAC( _y3, bval[15], _x3 );
			VAL_MAC( _y4, bval[19], _x3 );
			VAL_MAC( _y5, bval[23], _x3 );
			VAL_MAC( _y6, bval[27], _x3 );
			VAL_MAC( _y7, bval[31], _x3 );
		}

		VAL_MAC( yp[0], alpha, _y0 );
		VAL_MAC( yp[1*incy], alpha, _y1 );
		VAL_MAC( yp[2*incy], alpha, _y2 );
		VAL_MAC( yp[3*incy], alpha, _y3 );
		VAL_MAC( yp[4*incy], alpha, _y4 );
		VAL_MAC( yp[5*incy], alpha, _y5 );
		VAL_MAC( yp[6*incy], alpha, _y6 );
		VAL_MAC( yp[7*incy], alpha, _y7 );
	}
}


/**
 *  \brief Matrix times single-vector multiply in the normal case.
 */
static void
MatMult_v1( oski_index_t M,
	const oski_index_t* bptr, const oski_index_t* bind,
	const oski_value_t* bval,
	oski_value_t alpha, const oski_value_t* x, oski_index_t incx,
	oski_value_t* y, oski_index_t incy )
{
	if( incx == 1 )
		BCSR_MatMult_v1_aX_b1_xs1_ysX( M,
			bptr, bind, bval, alpha, x, y, incy );
	else
		BCSR_MatMult_v1_aX_b1_xsX_ysX( M,
			bptr, bind, bval, alpha, x, incx, y, incy );
}


/**
 *  \brief Computes
 *  \f$y \leftarrow y + \alpha\cdot\mathrm{op}(A)\cdot x\f$,
 *  where \f$\mathrm{op}(A) = A\f$, on the fully blocked
 *  portion of \f$A\f$.
 */
static int
MatMult( const oski_matBCSR_t* A,
	oski_value_t alpha, const oski_vecview_t x_view,
	oski_vecview_t y_view )
{
	oski_index_t j; /* column number */
	const oski_value_t* xpj; /* X(:, j) */
	oski_value_t* ypj; /* Y(:, j) */

	assert( A->row_block_size == 8 );
	assert( A->col_block_size == 4 );

	for( j = 0, xpj = x_view->val, ypj = y_view->val;
		j < x_view->num_cols;
		j++, xpj += x_view->colinc, ypj += y_view->colinc )
	{
		MatMult_v1( A->num_block_rows,
			A->bptr, A->bind, A->bval,
			alpha, xpj, x_view->rowinc, ypj, y_view->rowinc );
	}

	return 0;
}


#if defined(DO_NAME_MANGLING)
/** Mangled name for BCSR_MatTransMult_v1_aX_b1_xsX_ys1 */
#define BCSR_MatTransMult_v1_aX_b1_xsX_ys1 MANGLE_MOD_(BCSR_MatTransMult_v1_aX_b1_xsX_ys1_8x4)
#endif

/**
 *  \brief BCSR implementation of
 *  \f$y \leftarrow y + \alpha\cdot A^T\cdot x\f$.
 */
void
BCSR_MatTransMult_v1_aX_b1_xsX_ys1( oski_index_t M,
	const oski_index_t* restrict bptr, const oski_index_t* restrict bind,
	const oski_value_t* restrict bval,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y )
{
	const oski_value_t* xp;
	oski_index_t I;

	for( I = 0, xp = x; I < M; I++, xp += 8*incx )
	{
		REGISTER oski_value_t _x0;
		REGISTER oski_value_t _x1;
		REGISTER oski_value_t _x2;
		REGISTER oski_value_t _x3;
		REGISTER oski_value_t _x4;
		REGISTER oski_value_t _x5;
		REGISTER oski_value_t _x6;
		REGISTER oski_value_t _x7;
		oski_index_t K;

		VAL_MUL( _x0, alpha, xp[0] );
		VAL_MUL( _x1, alpha, xp[1*incx] );
		VAL_MUL( _x2, alpha, xp[2*incx] );
		VAL_MUL( _x3, alpha, xp[3*incx] );
		VAL_MUL( _x4, alpha, xp[4*incx] );
		VAL_MUL( _x5, alpha, xp[5*incx] );
		VAL_MUL( _x6, alpha, xp[6*incx] );
		VAL_MUL( _x7, alpha, xp[7*incx] );

		for( K = bptr[I]; K < bptr[I+1]; K++, bind++, bval += 8*4 )
		{
			oski_index_t j0 = bind[0]; /* block's leftmost col index */
			oski_value_t* yp = y + j0;

			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;
			REGISTER oski_value_t _y2;
			REGISTER oski_value_t _y3;
			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_SET_ZERO( _y2 );
			VAL_SET_ZERO( _y3 );
			VAL_MAC( _y0, bval[0], _x0 );
			VAL_MAC( _y1, bval[1], _x0 );
			VAL_MAC( _y2, bval[2], _x0 );
			VAL_MAC( _y3, bval[3], _x0 );
			VAL_MAC( _y0, bval[4], _x1 );
			VAL_MAC( _y1, bval[5], _x1 );
			VAL_MAC( _y2, bval[6], _x1 );
			VAL_MAC( _y3, bval[7], _x1 );
			VAL_MAC( _y0, bval[8], _x2 );
			VAL_MAC( _y1, bval[9], _x2 );
			VAL_MAC( _y2, bval[10], _x2 );
			VAL_MAC( _y3, bval[11], _x2 );
			VAL_MAC( _y0, bval[12], _x3 );
			VAL_MAC( _y1, bval[13], _x3 );
			VAL_MAC( _y2, bval[14], _x3 );
			VAL_MAC( _y3, bval[15], _x3 );
			VAL_MAC( _y0, bval[16], _x4 );
			VAL_MAC( _y1, bval[17], _x4 );
			VAL_MAC( _y2, bval[18], _x4 );
			VAL_MAC( _y3, bval[19], _x4 );
			VAL_MAC( _y0, bval[20], _x5 );
			VAL_MAC( _y1, bval[21], _x5 );
			VAL_MAC( _y2, bval[22], _x5 );
			VAL_MAC( _y3, bval[23], _x5 );
			VAL_MAC( _y0, bval[24], _x6 );
			VAL_MAC( _y1, bval[25], _x6 );
			VAL_MAC( _y2, bval[26], _x6 );
			VAL_MAC( _y3, bval[27], _x6 );
			VAL_MAC( _y0, bval[28], _x7 );
			VAL_MAC( _y1, bval[29], _x7 );
			VAL_MAC( _y2, bval[30], _x7 );
			VAL_MAC( _y3, bval[31], _x7 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1], _y1 );
			VAL_INC( yp[2], _y2 );
			VAL_INC( yp[3], _y3 );
		}

	}
}


#if defined(DO_NAME_MANGLING)
/** Mangled name for BCSR_MatTransMult_v1_aX_b1_xsX_ysX */
#define BCSR_MatTransMult_v1_aX_b1_xsX_ysX MANGLE_MOD_(BCSR_MatTransMult_v1_aX_b1_xsX_ysX_8x4)
#endif

/**
 *  \brief BCSR implementation of
 *  \f$y \leftarrow y + \alpha\cdot A^T\cdot x\f$.
 */
void
BCSR_MatTransMult_v1_aX_b1_xsX_ysX( oski_index_t M,
	const oski_index_t* restrict bptr, const oski_index_t* restrict bind,
	const oski_value_t* restrict bval,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y, oski_index_t incy )
{
	const oski_value_t* xp;
	oski_index_t I;

	for( I = 0, xp = x; I < M; I++, xp += 8*incx )
	{
		REGISTER oski_value_t _x0;
		REGISTER oski_value_t _x1;
		REGISTER oski_value_t _x2;
		REGISTER oski_value_t _x3;
		REGISTER oski_value_t _x4;
		REGISTER oski_value_t _x5;
		REGISTER oski_value_t _x6;
		REGISTER oski_value_t _x7;
		oski_index_t K;

		VAL_MUL( _x0, alpha, xp[0] );
		VAL_MUL( _x1, alpha, xp[1*incx] );
		VAL_MUL( _x2, alpha, xp[2*incx] );
		VAL_MUL( _x3, alpha, xp[3*incx] );
		VAL_MUL( _x4, alpha, xp[4*incx] );
		VAL_MUL( _x5, alpha, xp[5*incx] );
		VAL_MUL( _x6, alpha, xp[6*incx] );
		VAL_MUL( _x7, alpha, xp[7*incx] );

		for( K = bptr[I]; K < bptr[I+1]; K++, bind++, bval += 8*4 )
		{
			oski_index_t j0 = bind[0]; /* block's leftmost col index */
			oski_value_t* yp = y + j0*incy;

			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;
			REGISTER oski_value_t _y2;
			REGISTER oski_value_t _y3;
			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_SET_ZERO( _y2 );
			VAL_SET_ZERO( _y3 );
			VAL_MAC( _y0, bval[0], _x0 );
			VAL_MAC( _y1, bval[1], _x0 );
			VAL_MAC( _y2, bval[2], _x0 );
			VAL_MAC( _y3, bval[3], _x0 );
			VAL_MAC( _y0, bval[4], _x1 );
			VAL_MAC( _y1, bval[5], _x1 );
			VAL_MAC( _y2, bval[6], _x1 );
			VAL_MAC( _y3, bval[7], _x1 );
			VAL_MAC( _y0, bval[8], _x2 );
			VAL_MAC( _y1, bval[9], _x2 );
			VAL_MAC( _y2, bval[10], _x2 );
			VAL_MAC( _y3, bval[11], _x2 );
			VAL_MAC( _y0, bval[12], _x3 );
			VAL_MAC( _y1, bval[13], _x3 );
			VAL_MAC( _y2, bval[14], _x3 );
			VAL_MAC( _y3, bval[15], _x3 );
			VAL_MAC( _y0, bval[16], _x4 );
			VAL_MAC( _y1, bval[17], _x4 );
			VAL_MAC( _y2, bval[18], _x4 );
			VAL_MAC( _y3, bval[19], _x4 );
			VAL_MAC( _y0, bval[20], _x5 );
			VAL_MAC( _y1, bval[21], _x5 );
			VAL_MAC( _y2, bval[22], _x5 );
			VAL_MAC( _y3, bval[23], _x5 );
			VAL_MAC( _y0, bval[24], _x6 );
			VAL_MAC( _y1, bval[25], _x6 );
			VAL_MAC( _y2, bval[26], _x6 );
			VAL_MAC( _y3, bval[27], _x6 );
			VAL_MAC( _y0, bval[28], _x7 );
			VAL_MAC( _y1, bval[29], _x7 );
			VAL_MAC( _y2, bval[30], _x7 );
			VAL_MAC( _y3, bval[31], _x7 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1*incy], _y1 );
			VAL_INC( yp[2*incy], _y2 );
			VAL_INC( yp[3*incy], _y3 );
		}

	}
}


/**
 *  \brief Matrix times single-vector multiply in the trans case.
 */
static void
MatTransMult_v1( oski_index_t M,
	const oski_index_t* bptr, const oski_index_t* bind,
	const oski_value_t* bval,
	oski_value_t alpha, const oski_value_t* x, oski_index_t incx,
	oski_value_t* y, oski_index_t incy )
{
	if( incy == 1 )
		BCSR_MatTransMult_v1_aX_b1_xsX_ys1( M,
			bptr, bind, bval, alpha, x, incx, y );
	else
		BCSR_MatTransMult_v1_aX_b1_xsX_ysX( M,
			bptr, bind, bval, alpha, x, incx, y, incy );
}


/**
 *  \brief Computes
 *  \f$y \leftarrow y + \alpha\cdot\mathrm{op}(A)\cdot x\f$,
 *  where \f$\mathrm{op}(A) = A^T\f$, on the fully blocked
 *  portion of \f$A\f$.
 */
static int
MatTransMult( const oski_matBCSR_t* A,
	oski_value_t alpha, const oski_vecview_t x_view,
	oski_vecview_t y_view )
{
	oski_index_t j; /* column number */
	const oski_value_t* xpj; /* X(:, j) */
	oski_value_t* ypj; /* Y(:, j) */

	assert( A->row_block_size == 8 );
	assert( A->col_block_size == 4 );

	for( j = 0, xpj = x_view->val, ypj = y_view->val;
		j < x_view->num_cols;
		j++, xpj += x_view->colinc, ypj += y_view->colinc )
	{
		MatTransMult_v1( A->num_block_rows,
			A->bptr, A->bind, A->bval,
			alpha, xpj, x_view->rowinc, ypj, y_view->rowinc );
	}

	return 0;
}


#if !IS_VAL_COMPLEX

#if defined(DO_NAME_MANGLING)
/** See BCSR_MatMult_v1_aX_b1_xs1_ysX(). */
#define BCSR_MatConjMult_v1_aX_b1_xs1_ysX BCSR_MatMult_v1_aX_b1_xs1_ysX
#endif

#else /* IS_VAL_COMPLEX */



#if defined(DO_NAME_MANGLING)
/** Mangled name for BCSR_MatConjMult_v1_aX_b1_xs1_ysX */
#define BCSR_MatConjMult_v1_aX_b1_xs1_ysX MANGLE_MOD_(BCSR_MatConjMult_v1_aX_b1_xs1_ysX_8x4)
#endif

/**
 *  \brief BCSR implementation of
 *  \f$y \leftarrow y + \alpha\cdot \bar{A}\cdot x\f$.
 */
void
BCSR_MatConjMult_v1_aX_b1_xs1_ysX( oski_index_t M,
	const oski_index_t* restrict bptr, const oski_index_t* restrict bind,
	const oski_value_t* restrict bval,
	oski_value_t alpha, const oski_value_t* restrict x,
	oski_value_t* restrict y, oski_index_t incy )
{
	oski_value_t* yp;
	oski_index_t I;

	for( I = 0, yp = y; I < M; I++, yp += 8*incy )
	{
		REGISTER oski_value_t _y0;
		REGISTER oski_value_t _y1;
		REGISTER oski_value_t _y2;
		REGISTER oski_value_t _y3;
		REGISTER oski_value_t _y4;
		REGISTER oski_value_t _y5;
		REGISTER oski_value_t _y6;
		REGISTER oski_value_t _y7;
		oski_index_t K;

		VAL_SET_ZERO( _y0 );
		VAL_SET_ZERO( _y1 );
		VAL_SET_ZERO( _y2 );
		VAL_SET_ZERO( _y3 );
		VAL_SET_ZERO( _y4 );
		VAL_SET_ZERO( _y5 );
		VAL_SET_ZERO( _y6 );
		VAL_SET_ZERO( _y7 );

		for( K = bptr[I]; K < bptr[I+1]; K++, bind++, bval += 8*4 )
		{
			oski_index_t j0 = bind[0]; /* block's leftmost col index */
			const oski_value_t* xp = x + j0;

			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;
			REGISTER oski_value_t _x2;
			REGISTER oski_value_t _x3;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1] );
			VAL_ASSIGN( _x2, xp[2] );
			VAL_ASSIGN( _x3, xp[3] );
			VAL_MAC_CONJ( _y0, bval[0], _x0 );
			VAL_MAC_CONJ( _y1, bval[4], _x0 );
			VAL_MAC_CONJ( _y2, bval[8], _x0 );
			VAL_MAC_CONJ( _y3, bval[12], _x0 );
			VAL_MAC_CONJ( _y4, bval[16], _x0 );
			VAL_MAC_CONJ( _y5, bval[20], _x0 );
			VAL_MAC_CONJ( _y6, bval[24], _x0 );
			VAL_MAC_CONJ( _y7, bval[28], _x0 );
			VAL_MAC_CONJ( _y0, bval[1], _x1 );
			VAL_MAC_CONJ( _y1, bval[5], _x1 );
			VAL_MAC_CONJ( _y2, bval[9], _x1 );
			VAL_MAC_CONJ( _y3, bval[13], _x1 );
			VAL_MAC_CONJ( _y4, bval[17], _x1 );
			VAL_MAC_CONJ( _y5, bval[21], _x1 );
			VAL_MAC_CONJ( _y6, bval[25], _x1 );
			VAL_MAC_CONJ( _y7, bval[29], _x1 );
			VAL_MAC_CONJ( _y0, bval[2], _x2 );
			VAL_MAC_CONJ( _y1, bval[6], _x2 );
			VAL_MAC_CONJ( _y2, bval[10], _x2 );
			VAL_MAC_CONJ( _y3, bval[14], _x2 );
			VAL_MAC_CONJ( _y4, bval[18], _x2 );
			VAL_MAC_CONJ( _y5, bval[22], _x2 );
			VAL_MAC_CONJ( _y6, bval[26], _x2 );
			VAL_MAC_CONJ( _y7, bval[30], _x2 );
			VAL_MAC_CONJ( _y0, bval[3], _x3 );
			VAL_MAC_CONJ( _y1, bval[7], _x3 );
			VAL_MAC_CONJ( _y2, bval[11], _x3 );
			VAL_MAC_CONJ( _y3, bval[15], _x3 );
			VAL_MAC_CONJ( _y4, bval[19], _x3 );
			VAL_MAC_CONJ( _y5, bval[23], _x3 );
			VAL_MAC_CONJ( _y6, bval[27], _x3 );
			VAL_MAC_CONJ( _y7, bval[31], _x3 );
		}

		VAL_MAC( yp[0], alpha, _y0 );
		VAL_MAC( yp[1*incy], alpha, _y1 );
		VAL_MAC( yp[2*incy], alpha, _y2 );
		VAL_MAC( yp[3*incy], alpha, _y3 );
		VAL_MAC( yp[4*incy], alpha, _y4 );
		VAL_MAC( yp[5*incy], alpha, _y5 );
		VAL_MAC( yp[6*incy], alpha, _y6 );
		VAL_MAC( yp[7*incy], alpha, _y7 );
	}
}


#endif /* !IS_VAL_COMPLEX */


#if !IS_VAL_COMPLEX

#if defined(DO_NAME_MANGLING)
/** See BCSR_MatMult_v1_aX_b1_xsX_ysX(). */
#define BCSR_MatConjMult_v1_aX_b1_xsX_ysX BCSR_MatMult_v1_aX_b1_xsX_ysX
#endif

#else /* IS_VAL_COMPLEX */



#if defined(DO_NAME_MANGLING)
/** Mangled name for BCSR_MatConjMult_v1_aX_b1_xsX_ysX */
#define BCSR_MatConjMult_v1_aX_b1_xsX_ysX MANGLE_MOD_(BCSR_MatConjMult_v1_aX_b1_xsX_ysX_8x4)
#endif

/**
 *  \brief BCSR implementation of
 *  \f$y \leftarrow y + \alpha\cdot \bar{A}\cdot x\f$.
 */
void
BCSR_MatConjMult_v1_aX_b1_xsX_ysX( oski_index_t M,
	const oski_index_t* restrict bptr, const oski_index_t* restrict bind,
	const oski_value_t* restrict bval,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y, oski_index_t incy )
{
	oski_value_t* yp;
	oski_index_t I;

	for( I = 0, yp = y; I < M; I++, yp += 8*incy )
	{
		REGISTER oski_value_t _y0;
		REGISTER oski_value_t _y1;
		REGISTER oski_value_t _y2;
		REGISTER oski_value_t _y3;
		REGISTER oski_value_t _y4;
		REGISTER oski_value_t _y5;
		REGISTER oski_value_t _y6;
		REGISTER oski_value_t _y7;
		oski_index_t K;

		VAL_SET_ZERO( _y0 );
		VAL_SET_ZERO( _y1 );
		VAL_SET_ZERO( _y2 );
		VAL_SET_ZERO( _y3 );
		VAL_SET_ZERO( _y4 );
		VAL_SET_ZERO( _y5 );
		VAL_SET_ZERO( _y6 );
		VAL_SET_ZERO( _y7 );

		for( K = bptr[I]; K < bptr[I+1]; K++, bind++, bval += 8*4 )
		{
			oski_index_t j0 = bind[0]; /* block's leftmost col index */
			const oski_value_t* xp = x + j0*incx;

			REGISTER oski_value_t _x0;
			REGISTER oski_value_t _x1;
			REGISTER oski_value_t _x2;
			REGISTER oski_value_t _x3;
			VAL_ASSIGN( _x0, xp[0] );
			VAL_ASSIGN( _x1, xp[1*incx] );
			VAL_ASSIGN( _x2, xp[2*incx] );
			VAL_ASSIGN( _x3, xp[3*incx] );
			VAL_MAC_CONJ( _y0, bval[0], _x0 );
			VAL_MAC_CONJ( _y1, bval[4], _x0 );
			VAL_MAC_CONJ( _y2, bval[8], _x0 );
			VAL_MAC_CONJ( _y3, bval[12], _x0 );
			VAL_MAC_CONJ( _y4, bval[16], _x0 );
			VAL_MAC_CONJ( _y5, bval[20], _x0 );
			VAL_MAC_CONJ( _y6, bval[24], _x0 );
			VAL_MAC_CONJ( _y7, bval[28], _x0 );
			VAL_MAC_CONJ( _y0, bval[1], _x1 );
			VAL_MAC_CONJ( _y1, bval[5], _x1 );
			VAL_MAC_CONJ( _y2, bval[9], _x1 );
			VAL_MAC_CONJ( _y3, bval[13], _x1 );
			VAL_MAC_CONJ( _y4, bval[17], _x1 );
			VAL_MAC_CONJ( _y5, bval[21], _x1 );
			VAL_MAC_CONJ( _y6, bval[25], _x1 );
			VAL_MAC_CONJ( _y7, bval[29], _x1 );
			VAL_MAC_CONJ( _y0, bval[2], _x2 );
			VAL_MAC_CONJ( _y1, bval[6], _x2 );
			VAL_MAC_CONJ( _y2, bval[10], _x2 );
			VAL_MAC_CONJ( _y3, bval[14], _x2 );
			VAL_MAC_CONJ( _y4, bval[18], _x2 );
			VAL_MAC_CONJ( _y5, bval[22], _x2 );
			VAL_MAC_CONJ( _y6, bval[26], _x2 );
			VAL_MAC_CONJ( _y7, bval[30], _x2 );
			VAL_MAC_CONJ( _y0, bval[3], _x3 );
			VAL_MAC_CONJ( _y1, bval[7], _x3 );
			VAL_MAC_CONJ( _y2, bval[11], _x3 );
			VAL_MAC_CONJ( _y3, bval[15], _x3 );
			VAL_MAC_CONJ( _y4, bval[19], _x3 );
			VAL_MAC_CONJ( _y5, bval[23], _x3 );
			VAL_MAC_CONJ( _y6, bval[27], _x3 );
			VAL_MAC_CONJ( _y7, bval[31], _x3 );
		}

		VAL_MAC( yp[0], alpha, _y0 );
		VAL_MAC( yp[1*incy], alpha, _y1 );
		VAL_MAC( yp[2*incy], alpha, _y2 );
		VAL_MAC( yp[3*incy], alpha, _y3 );
		VAL_MAC( yp[4*incy], alpha, _y4 );
		VAL_MAC( yp[5*incy], alpha, _y5 );
		VAL_MAC( yp[6*incy], alpha, _y6 );
		VAL_MAC( yp[7*incy], alpha, _y7 );
	}
}


#endif /* !IS_VAL_COMPLEX */


#if !IS_VAL_COMPLEX
/**
 *  \brief Matrix times single-vector multiply in the conj case;
 *  see MatMult_v1().
 */
#define MatConjMult_v1 MatMult_v1

#else /* IS_VAL_COMPLEX */


/**
 *  \brief Matrix times single-vector multiply in the conj case.
 */
static void
MatConjMult_v1( oski_index_t M,
	const oski_index_t* bptr, const oski_index_t* bind,
	const oski_value_t* bval,
	oski_value_t alpha, const oski_value_t* x, oski_index_t incx,
	oski_value_t* y, oski_index_t incy )
{
	if( incx == 1 )
		BCSR_MatConjMult_v1_aX_b1_xs1_ysX( M,
			bptr, bind, bval, alpha, x, y, incy );
	else
		BCSR_MatConjMult_v1_aX_b1_xsX_ysX( M,
			bptr, bind, bval, alpha, x, incx, y, incy );
}


#endif /* !IS_VAL_COMPLEX */


#if !IS_VAL_COMPLEX
/**
 *  \brief See MatMult().
 */
#define MatConjMult MatMult

#else /* IS_VAL_COMPLEX */



/**
 *  \brief Computes
 *  \f$y \leftarrow y + \alpha\cdot\mathrm{op}(A)\cdot x\f$,
 *  where \f$\mathrm{op}(A) = \bar{A}\f$, on the fully blocked
 *  portion of \f$A\f$.
 */
static int
MatConjMult( const oski_matBCSR_t* A,
	oski_value_t alpha, const oski_vecview_t x_view,
	oski_vecview_t y_view )
{
	oski_index_t j; /* column number */
	const oski_value_t* xpj; /* X(:, j) */
	oski_value_t* ypj; /* Y(:, j) */

	assert( A->row_block_size == 8 );
	assert( A->col_block_size == 4 );

	for( j = 0, xpj = x_view->val, ypj = y_view->val;
		j < x_view->num_cols;
		j++, xpj += x_view->colinc, ypj += y_view->colinc )
	{
		MatConjMult_v1( A->num_block_rows,
			A->bptr, A->bind, A->bval,
			alpha, xpj, x_view->rowinc, ypj, y_view->rowinc );
	}

	return 0;
}


#endif /* !IS_VAL_COMPLEX */


#if !IS_VAL_COMPLEX

#if defined(DO_NAME_MANGLING)
/** See BCSR_MatTransMult_v1_aX_b1_xsX_ys1(). */
#define BCSR_MatHermMult_v1_aX_b1_xsX_ys1 BCSR_MatTransMult_v1_aX_b1_xsX_ys1
#endif

#else /* IS_VAL_COMPLEX */



#if defined(DO_NAME_MANGLING)
/** Mangled name for BCSR_MatHermMult_v1_aX_b1_xsX_ys1 */
#define BCSR_MatHermMult_v1_aX_b1_xsX_ys1 MANGLE_MOD_(BCSR_MatHermMult_v1_aX_b1_xsX_ys1_8x4)
#endif

/**
 *  \brief BCSR implementation of
 *  \f$y \leftarrow y + \alpha\cdot \bar{A}^T\cdot x\f$.
 */
void
BCSR_MatHermMult_v1_aX_b1_xsX_ys1( oski_index_t M,
	const oski_index_t* restrict bptr, const oski_index_t* restrict bind,
	const oski_value_t* restrict bval,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y )
{
	const oski_value_t* xp;
	oski_index_t I;

	for( I = 0, xp = x; I < M; I++, xp += 8*incx )
	{
		REGISTER oski_value_t _x0;
		REGISTER oski_value_t _x1;
		REGISTER oski_value_t _x2;
		REGISTER oski_value_t _x3;
		REGISTER oski_value_t _x4;
		REGISTER oski_value_t _x5;
		REGISTER oski_value_t _x6;
		REGISTER oski_value_t _x7;
		oski_index_t K;

		VAL_MUL( _x0, alpha, xp[0] );
		VAL_MUL( _x1, alpha, xp[1*incx] );
		VAL_MUL( _x2, alpha, xp[2*incx] );
		VAL_MUL( _x3, alpha, xp[3*incx] );
		VAL_MUL( _x4, alpha, xp[4*incx] );
		VAL_MUL( _x5, alpha, xp[5*incx] );
		VAL_MUL( _x6, alpha, xp[6*incx] );
		VAL_MUL( _x7, alpha, xp[7*incx] );

		for( K = bptr[I]; K < bptr[I+1]; K++, bind++, bval += 8*4 )
		{
			oski_index_t j0 = bind[0]; /* block's leftmost col index */
			oski_value_t* yp = y + j0;

			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;
			REGISTER oski_value_t _y2;
			REGISTER oski_value_t _y3;
			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_SET_ZERO( _y2 );
			VAL_SET_ZERO( _y3 );
			VAL_MAC_CONJ( _y0, bval[0], _x0 );
			VAL_MAC_CONJ( _y1, bval[1], _x0 );
			VAL_MAC_CONJ( _y2, bval[2], _x0 );
			VAL_MAC_CONJ( _y3, bval[3], _x0 );
			VAL_MAC_CONJ( _y0, bval[4], _x1 );
			VAL_MAC_CONJ( _y1, bval[5], _x1 );
			VAL_MAC_CONJ( _y2, bval[6], _x1 );
			VAL_MAC_CONJ( _y3, bval[7], _x1 );
			VAL_MAC_CONJ( _y0, bval[8], _x2 );
			VAL_MAC_CONJ( _y1, bval[9], _x2 );
			VAL_MAC_CONJ( _y2, bval[10], _x2 );
			VAL_MAC_CONJ( _y3, bval[11], _x2 );
			VAL_MAC_CONJ( _y0, bval[12], _x3 );
			VAL_MAC_CONJ( _y1, bval[13], _x3 );
			VAL_MAC_CONJ( _y2, bval[14], _x3 );
			VAL_MAC_CONJ( _y3, bval[15], _x3 );
			VAL_MAC_CONJ( _y0, bval[16], _x4 );
			VAL_MAC_CONJ( _y1, bval[17], _x4 );
			VAL_MAC_CONJ( _y2, bval[18], _x4 );
			VAL_MAC_CONJ( _y3, bval[19], _x4 );
			VAL_MAC_CONJ( _y0, bval[20], _x5 );
			VAL_MAC_CONJ( _y1, bval[21], _x5 );
			VAL_MAC_CONJ( _y2, bval[22], _x5 );
			VAL_MAC_CONJ( _y3, bval[23], _x5 );
			VAL_MAC_CONJ( _y0, bval[24], _x6 );
			VAL_MAC_CONJ( _y1, bval[25], _x6 );
			VAL_MAC_CONJ( _y2, bval[26], _x6 );
			VAL_MAC_CONJ( _y3, bval[27], _x6 );
			VAL_MAC_CONJ( _y0, bval[28], _x7 );
			VAL_MAC_CONJ( _y1, bval[29], _x7 );
			VAL_MAC_CONJ( _y2, bval[30], _x7 );
			VAL_MAC_CONJ( _y3, bval[31], _x7 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1], _y1 );
			VAL_INC( yp[2], _y2 );
			VAL_INC( yp[3], _y3 );
		}

	}
}


#endif /* !IS_VAL_COMPLEX */


#if !IS_VAL_COMPLEX

#if defined(DO_NAME_MANGLING)
/** See BCSR_MatTransMult_v1_aX_b1_xsX_ysX(). */
#define BCSR_MatHermMult_v1_aX_b1_xsX_ysX BCSR_MatTransMult_v1_aX_b1_xsX_ysX
#endif

#else /* IS_VAL_COMPLEX */



#if defined(DO_NAME_MANGLING)
/** Mangled name for BCSR_MatHermMult_v1_aX_b1_xsX_ysX */
#define BCSR_MatHermMult_v1_aX_b1_xsX_ysX MANGLE_MOD_(BCSR_MatHermMult_v1_aX_b1_xsX_ysX_8x4)
#endif

/**
 *  \brief BCSR implementation of
 *  \f$y \leftarrow y + \alpha\cdot \bar{A}^T\cdot x\f$.
 */
void
BCSR_MatHermMult_v1_aX_b1_xsX_ysX( oski_index_t M,
	const oski_index_t* restrict bptr, const oski_index_t* restrict bind,
	const oski_value_t* restrict bval,
	oski_value_t alpha, const oski_value_t* restrict x, oski_index_t incx,
	oski_value_t* restrict y, oski_index_t incy )
{
	const oski_value_t* xp;
	oski_index_t I;

	for( I = 0, xp = x; I < M; I++, xp += 8*incx )
	{
		REGISTER oski_value_t _x0;
		REGISTER oski_value_t _x1;
		REGISTER oski_value_t _x2;
		REGISTER oski_value_t _x3;
		REGISTER oski_value_t _x4;
		REGISTER oski_value_t _x5;
		REGISTER oski_value_t _x6;
		REGISTER oski_value_t _x7;
		oski_index_t K;

		VAL_MUL( _x0, alpha, xp[0] );
		VAL_MUL( _x1, alpha, xp[1*incx] );
		VAL_MUL( _x2, alpha, xp[2*incx] );
		VAL_MUL( _x3, alpha, xp[3*incx] );
		VAL_MUL( _x4, alpha, xp[4*incx] );
		VAL_MUL( _x5, alpha, xp[5*incx] );
		VAL_MUL( _x6, alpha, xp[6*incx] );
		VAL_MUL( _x7, alpha, xp[7*incx] );

		for( K = bptr[I]; K < bptr[I+1]; K++, bind++, bval += 8*4 )
		{
			oski_index_t j0 = bind[0]; /* block's leftmost col index */
			oski_value_t* yp = y + j0*incy;

			REGISTER oski_value_t _y0;
			REGISTER oski_value_t _y1;
			REGISTER oski_value_t _y2;
			REGISTER oski_value_t _y3;
			VAL_SET_ZERO( _y0 );
			VAL_SET_ZERO( _y1 );
			VAL_SET_ZERO( _y2 );
			VAL_SET_ZERO( _y3 );
			VAL_MAC_CONJ( _y0, bval[0], _x0 );
			VAL_MAC_CONJ( _y1, bval[1], _x0 );
			VAL_MAC_CONJ( _y2, bval[2], _x0 );
			VAL_MAC_CONJ( _y3, bval[3], _x0 );
			VAL_MAC_CONJ( _y0, bval[4], _x1 );
			VAL_MAC_CONJ( _y1, bval[5], _x1 );
			VAL_MAC_CONJ( _y2, bval[6], _x1 );
			VAL_MAC_CONJ( _y3, bval[7], _x1 );
			VAL_MAC_CONJ( _y0, bval[8], _x2 );
			VAL_MAC_CONJ( _y1, bval[9], _x2 );
			VAL_MAC_CONJ( _y2, bval[10], _x2 );
			VAL_MAC_CONJ( _y3, bval[11], _x2 );
			VAL_MAC_CONJ( _y0, bval[12], _x3 );
			VAL_MAC_CONJ( _y1, bval[13], _x3 );
			VAL_MAC_CONJ( _y2, bval[14], _x3 );
			VAL_MAC_CONJ( _y3, bval[15], _x3 );
			VAL_MAC_CONJ( _y0, bval[16], _x4 );
			VAL_MAC_CONJ( _y1, bval[17], _x4 );
			VAL_MAC_CONJ( _y2, bval[18], _x4 );
			VAL_MAC_CONJ( _y3, bval[19], _x4 );
			VAL_MAC_CONJ( _y0, bval[20], _x5 );
			VAL_MAC_CONJ( _y1, bval[21], _x5 );
			VAL_MAC_CONJ( _y2, bval[22], _x5 );
			VAL_MAC_CONJ( _y3, bval[23], _x5 );
			VAL_MAC_CONJ( _y0, bval[24], _x6 );
			VAL_MAC_CONJ( _y1, bval[25], _x6 );
			VAL_MAC_CONJ( _y2, bval[26], _x6 );
			VAL_MAC_CONJ( _y3, bval[27], _x6 );
			VAL_MAC_CONJ( _y0, bval[28], _x7 );
			VAL_MAC_CONJ( _y1, bval[29], _x7 );
			VAL_MAC_CONJ( _y2, bval[30], _x7 );
			VAL_MAC_CONJ( _y3, bval[31], _x7 );
			VAL_INC( yp[0], _y0 );
			VAL_INC( yp[1*incy], _y1 );
			VAL_INC( yp[2*incy], _y2 );
			VAL_INC( yp[3*incy], _y3 );
		}

	}
}


#endif /* !IS_VAL_COMPLEX */


#if !IS_VAL_COMPLEX
/**
 *  \brief Matrix times single-vector multiply in the herm case;
 *  see MatTransMult_v1().
 */
#define MatHermMult_v1 MatTransMult_v1

#else /* IS_VAL_COMPLEX */


/**
 *  \brief Matrix times single-vector multiply in the herm case.
 */
static void
MatHermMult_v1( oski_index_t M,
	const oski_index_t* bptr, const oski_index_t* bind,
	const oski_value_t* bval,
	oski_value_t alpha, const oski_value_t* x, oski_index_t incx,
	oski_value_t* y, oski_index_t incy )
{
	if( incy == 1 )
		BCSR_MatHermMult_v1_aX_b1_xsX_ys1( M,
			bptr, bind, bval, alpha, x, incx, y );
	else
		BCSR_MatHermMult_v1_aX_b1_xsX_ysX( M,
			bptr, bind, bval, alpha, x, incx, y, incy );
}


#endif /* !IS_VAL_COMPLEX */


#if !IS_VAL_COMPLEX
/**
 *  \brief See MatTransMult().
 */
#define MatHermMult MatTransMult

#else /* IS_VAL_COMPLEX */



/**
 *  \brief Computes
 *  \f$y \leftarrow y + \alpha\cdot\mathrm{op}(A)\cdot x\f$,
 *  where \f$\mathrm{op}(A) = A^H = \bar{A}^T\f$, on the fully blocked
 *  portion of \f$A\f$.
 */
static int
MatHermMult( const oski_matBCSR_t* A,
	oski_value_t alpha, const oski_vecview_t x_view,
	oski_vecview_t y_view )
{
	oski_index_t j; /* column number */
	const oski_value_t* xpj; /* X(:, j) */
	oski_value_t* ypj; /* Y(:, j) */

	assert( A->row_block_size == 8 );
	assert( A->col_block_size == 4 );

	for( j = 0, xpj = x_view->val, ypj = y_view->val;
		j < x_view->num_cols;
		j++, xpj += x_view->colinc, ypj += y_view->colinc )
	{
		MatHermMult_v1( A->num_block_rows,
			A->bptr, A->bind, A->bval,
			alpha, xpj, x_view->rowinc, ypj, y_view->rowinc );
	}

	return 0;
}


#endif /* !IS_VAL_COMPLEX */


#if defined(DO_NAME_MANGLING)
/** Mangled name for primary exportable symbol */
#define MatReprMult MANGLE_MOD_(MatReprMult_8x4)
#endif

/**
 *  \brief Computes
 *  \f$y \leftarrow y + \alpha\cdot\mathrm{op}(A)\cdot x\f$,
 *  where \f$A\f$ is stored in 8x4 BCSR format.
 */
int
MatReprMult( const oski_matBCSR_t* A, oski_matop_t opA,
	oski_value_t alpha, const oski_vecview_t x_view,
	oski_vecview_t y_view )
{
	int err;

	switch( opA )
	{
		case OP_NORMAL:
			err = MatMult( A, alpha, x_view, y_view );
			break;
		case OP_CONJ:
			err = MatConjMult( A, alpha, x_view, y_view );
			break;
		case OP_TRANS:
			err = MatTransMult( A, alpha, x_view, y_view );
			break;
		case OP_CONJ_TRANS:
			err = MatHermMult( A, alpha, x_view, y_view );
			break;

		default:
			OSKI_ERR_BAD_MATOP( MatReprMult, 2, opA );
			err = ERR_BAD_ARG;
	}

	return err;
}


/* eof */
