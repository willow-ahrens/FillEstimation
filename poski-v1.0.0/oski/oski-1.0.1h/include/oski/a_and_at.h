/**
 *  \file oski/a_and_at.h
 *  \ingroup KERNELS
 *  \brief Sparse simultaneous multiplication by \f$A\f$ and \f$A^T\f$.
 */

#if !defined(INC_OSKI_A_AND_AT_H) && !defined(OSKI_UNBIND)
/** oski/a_and_at.h included. */
#define INC_OSKI_A_AND_AT_H

#include <oski/matmult.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_MatMultAndMatTransMult_funcpt  \
	MANGLE_(oski_MatMultAndMatTransMult_funcpt)
#define oski_MatReprMultAndMatReprTransMult_funcpt \
	MANGLE_(oski_MatReprMultAndMatReprTransMult_funcpt)

#define oski_MatMultAndMatTransMult         \
	MANGLE_(oski_MatMultAndMatTransMult)
#define oski_CheckArgsMatMultAndMatTransMult \
	MANGLE_(oski_CheckArgsMatMultAndMatTransMult)

#define oski_traceargs_MatMultAndMatTransMult_t \
	MANGLE_(oski_traceargs_MatMultAndMatTransMult_t)
#define oski_MakeArglistMatMultAndMatTransMult \
	MANGLE_(oski_MakeArglistMatMultAndMatTransMult)
/*@}*/
#endif

/**
 *  \brief Computes \f$y \leftarrow \alpha\cdot Ax + \beta\cdot y\f$
 *  and \f$z \leftarrow \omega\cdot\mathrm{op}(A)x + \zeta\cdot z\f$,
 *  where \f$\mathrm{op}(A) \in \{A, A^T, A^H\}\f$.
 *
 *  \ingroup PUBKERNELS
 */
int oski_MatMultAndMatTransMult (const oski_matrix_t A_tunable,
				 oski_value_t alpha,
				 const oski_vecview_t x_view,
				 oski_value_t beta, oski_vecview_t y_view,
				 oski_matop_t opA, oski_value_t omega,
				 const oski_vecview_t w_view,
				 oski_value_t zeta, oski_vecview_t z_view);

/**
 *  \brief Verify the arguments to a function with a calling
 *  signature like \ref oski_MatMultAndMatTransMult(). Allows
 *  for symbolic arguments.
 */
int oski_CheckArgsMatMultAndMatTransMult (const oski_matrix_t A_tunable,
					  oski_value_t alpha,
					  const oski_vecview_t x_view,
					  oski_value_t beta,
					  oski_vecview_t y_view,
					  oski_matop_t op, oski_value_t omega,
					  const oski_vecview_t w_view,
					  oski_value_t zeta,
					  oski_vecview_t z_view,
					  const char *caller);

/**
 *  \brief Function pointer type for oski_MatMultAndMatTransMult().
 */
typedef int (*oski_MatMultAndMatTransMult_funcpt) (const oski_matrix_t
						   A_tunable,
						   oski_value_t alpha,
						   const oski_vecview_t
						   x_view, oski_value_t beta,
						   oski_vecview_t y_view,
						   oski_matop_t opA,
						   oski_value_t omega,
						   const oski_vecview_t
						   w_view, oski_value_t zeta,
						   oski_vecview_t z_view);

/**
 *  \brief Function pointer type for a matrix type-specific
 *  implementation of oski_MatMultAndMatTransMult().
 */
typedef int (*oski_MatReprMultAndMatReprTransMult_funcpt) (const void *A,
							   const
							   oski_matcommon_t *
							   props,
							   oski_value_t alpha,
							   const
							   oski_vecview_t
							   x_view,
							   oski_value_t beta,
							   oski_vecview_t
							   y_view,
							   oski_matop_t opA,
							   oski_value_t omega,
							   const
							   oski_vecview_t
							   w_view,
							   oski_value_t zeta,
							   oski_vecview_t
							   z_view);

/**
 *  \name Tracing.
 */
/*@{*/
/** \brief Store features of the input arguments to the
 *  #oski_MatMultAndMatTransMult() kernel.
 */
typedef struct
{
	/** For argument descriptions, see oski_MatMultAndMatTransMult(). */
  /*@{ */
  oski_value_t alpha;
  oski_index_t num_vecs;
  oski_storage_t x_orient;
  oski_value_t beta;
  oski_storage_t y_orient;
  oski_matop_t opA;
  oski_value_t omega;
  oski_storage_t w_orient;
  oski_value_t zeta;
  oski_storage_t z_orient;
  /*@} */
}
oski_traceargs_MatMultAndMatTransMult_t;

/**
 *  \brief Initialize a static argument signature for a call to
 *  \ref oski_MatMultAndMatTransMult().
 */
void oski_MakeArglistMatMultAndMatTransMult (oski_value_t alpha,
					     const oski_vecview_t x_view,
					     oski_value_t beta,
					     const oski_vecview_t y_view,
					     oski_matop_t opA,
					     oski_value_t omega,
					     const oski_vecview_t w_view,
					     oski_value_t zeta,
					     const oski_vecview_t z_view,
					     oski_traceargs_MatMultAndMatTransMult_t
					     * args);

/** \brief Compute the "flop factor." */
#define GET_XFLOP_MatMultAndMatTransMult(args) \
	(4.0*((const oski_traceargs_MatMultAndMatTransMult_t *)(args))->num_vecs)
/*@}*/

#endif /* !defined(INC_OSKI_A_AND_AT_H) */

#if defined(OSKI_UNBIND)
#  include <oski/matmult.h>
#  undef INC_OSKI_A_AND_AT_H
#  undef oski_MatMultAndMatTransMult_funcpt
#  undef oski_MatReprMultAndMatReprTransMult_funcpt
#  undef oski_MatMultAndMatTransMult
#  undef oski_CheckArgsMatMultAndMatTransMult
#  undef oski_traceargs_MatMultAndMatTransMult_t
#  undef oski_MakeArglistMatMultAndMatTransMult
#  undef GET_XFLOP_MatMultAndMatTransMult
#endif

/* eof */
