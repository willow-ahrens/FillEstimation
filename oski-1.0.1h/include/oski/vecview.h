/**
 *  \file oski/vecview.h
 *  \brief Multivector view module.
 *
 *  This module defines the basic single- and multivector view
 *  data structures.
 */

#if !defined(INC_OSKI_VECVIEW_H) && !defined(OSKI_UNBIND)
/** oski/vecview.h included. */
#define INC_OSKI_VECVIEW_H

#include <oski/vector.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_vecview_t      MANGLE_(oski_vecview_t)
#define SYMBOLIC_VEC         MANGLE_(SYMBOLIC_VEC)
#define SYMBOLIC_MULTIVEC    MANGLE_(SYMBOLIC_MULTIVEC)

#define oski_CreateVecView       MANGLE_(oski_CreateVecView)
#define oski_CreateMultiVecView  MANGLE_(oski_CreateMultiVecView)
#define oski_DestroyVecView      MANGLE_(oski_DestroyVecView)
#define oski_CopyVecView         MANGLE_(oski_CopyVecView)

#define oski_PrintDebugVecView   MANGLE_(oski_PrintDebugVecView)
#define oski_CalcVecViewInfNorm  MANGLE_(oski_CalcVecViewInfNorm)
#define oski_CalcVecViewInfNormDiff  MANGLE_(oski_CalcVecViewInfNormDiff)

#define oski_InitSubVecView  MANGLE_(oski_InitSubVecView)
/*@}*/
#endif

/**
*  \defgroup VECVIEW Single- and multivector view creation.
*  \ingroup PUBLIC
*
*  Vector views are wrappers around user-declared dense array
*  matrix representations. These wrappers encapsulate information
*  about the stride (leading dimension) and data layout of
*  the matrix in its corresponding array. The following routines
*  create, destroy, and duplicate vector views.
*/
/*@{*/
/** \brief Symbolic constant for "unit stride" */

#define STRIDE_UNIT ((oski_index_t)1)

/**
 * Multivector view handle
 */
typedef oski_vecstruct_t *oski_vecview_t;

/**
 *  \brief Creates a valid view on a single dense column vector \f$x\f$.
 *  \ingroup PUBLIC
 */
oski_vecview_t
oski_CreateVecView (oski_value_t * x, oski_index_t length, oski_index_t inc);

/**
 *  \brief Creates a multivector view on \f$k\f$ dense column vectors
 *  \f$X = \left(\begin{array}{c} x_1 \cdots x_k \end{array}\right)\f$,
 *  stored as a submatrix in the user's data.
 */
oski_vecview_t
oski_CreateMultiVecView (oski_value_t * X,
			 oski_index_t length, oski_index_t num_vecs,
			 oski_storage_t orient, oski_index_t stride);

/**
 *  \brief Destroy an existing vector view.
 *  \ingroup PUBLIC
 */
int oski_DestroyVecView (oski_vecview_t x);

/**
 *  \brief Creates a copy of the given (multi)vector view.
 *  \ingroup PUBLIC
 */
oski_vecview_t oski_CopyVecView (const oski_vecview_t x);
/*@}*/


/**
 *  \brief Dump a vector view object to the debug log (FOR DEBUGGING ONLY).
 */
void oski_PrintDebugVecView (const oski_vecview_t x, const char *val_fmt);

/**
 *  \brief Compute the infinity norm of a specified column vector of
 *  a vector view.
 */
double oski_CalcVecViewInfNorm (const oski_vecview_t x, oski_index_t j);

/**
 *  \brief Compute the infinity norm of the difference between two vectors.
 */
double oski_CalcVecViewInfNormDiff (const oski_vecview_t x0, oski_index_t j0,
				    const oski_vecview_t x1, oski_index_t j1);

/**
 *  \brief Given an \f$m\times n\f$ multivector object \f$x\f$, returns
 *  a shallow copy submatrix \f$y = x(i_0:(i_0+R-1), j_0:(j_0+C-1)\f$.
 */
int oski_InitSubVecView (const oski_vecview_t x,
			 oski_index_t i0, oski_index_t j0, oski_index_t R,
			 oski_index_t C, oski_vecstruct_t * y);

/**
*  \name Symbolic vector views.
*  \ingroup VECVIEW
*
*  These constants are used by the hint routines (see \ref HINT).
*/
/*@{*/
/** Symbolic single-vector view. */
extern const oski_vecview_t SYMBOLIC_VEC;

/** Symbolic multiple-vector view. */
extern const oski_vecview_t SYMBOLIC_MULTIVEC;
/*@}*/

#endif /* !defined(INC_OSKI_VECVIEW_H) */

#if defined(OSKI_UNBIND)
#  include <oski/vector.h>
#  undef INC_OSKI_VECVIEW_H
#  undef oski_vecview_t
#  undef SYMBOLIC_VEC
#  undef SYMBOLIC_MULTIVEC
#  undef oski_CreateVecView
#  undef oski_CreateMultiVecView
#  undef oski_DestroyVecView
#  undef oski_CopyVecView
#  undef oski_PrintDebugVecView
#  undef oski_CalcVecViewInfNorm
#  undef oski_CalcVecViewInfNormDiff
#  undef oski_InitSubVecView
#  undef STRIDE_UNIT
#endif

/* eof */
