/**
 *  \file oski/blas.h
 *  \ingroup AUXIL
 *  \brief BeBOP wrappers around the dense BLAS routines.
 */

#if !defined(INC_OSKI_BLAS_H)
/** oski/blas.h included. */
#define INC_OSKI_BLAS_H

#include <oski/vecview.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_ZeroVecView   MANGLE_(oski_ZeroVecView)
#define oski_ConjVecView   MANGLE_(oski_ConjVecView)
#define oski_ScaleVecView  MANGLE_(oski_ScaleVecView)
#define oski_AXPYVecView   MANGLE_(oski_AXPYVecView)
#define oski_RectScaledIdentityMult  MANGLE_(oski_RectScaledIdentityMult)
/*@}*/
#endif

/** Returns 1 <==> the given vector views have equal logical dimensions */
#define ARE_VECVIEW_DIMS_EQUAL( x, y ) \
	(((x)->num_rows == (y)->num_rows) && ((x)->num_cols == (y)->num_cols))

/**
 *  \brief Set all elements of a vector view \f$x\f$ to zero.
 */
int oski_ZeroVecView (oski_vecview_t x);

/**
 *  \brief Computes \f$x \leftarrow \alpha\cdot x\f$.
 */
int oski_ScaleVecView (oski_vecview_t x, oski_value_t alpha);

/**
 *  \brief Computes the complex conjugate of a vector view,
 *  i.e., \f$x \leftarrow \bar{x}\f$.
 */
int oski_ConjVecView (oski_vecview_t x);

/**
 *  \brief Computes \f$y \leftarrow y + \alpha\cdot x\f$.
 */
int oski_AXPYVecView (const oski_vecview_t x, oski_value_t alpha,
		      oski_vecview_t y);

/**
 *  \brief Computes \f$y \leftarrow y + \alpha I_{m\times n}\cdot x\f$,
 *  where \f$I_{m\times n}\f$ is an \f$m\times n\f$ matrix such that
 *  \f$I(k,k) = 1\f$ for all \f$1 \leq k \leq \min(m,n)\f$.
 */
int oski_RectScaledIdentityMult (oski_value_t alpha,
				 const oski_vecview_t x, oski_vecview_t y);

/**
 *  \brief Computes
 *  \f$y \leftarrow \beta\cdot y + \alpha\cdot \mathrm{op}(A)\cdot x\f$,
 *  where \f$A, x, y\f$ are all dense matrices (multivector views).
 */
int oski_DenseMatMult (const oski_vecview_t A, oski_matop_t opA,
		       oski_value_t alpha, const oski_vecview_t x,
		       oski_value_t beta, const oski_vecview_t y);

#endif

/* eof */
