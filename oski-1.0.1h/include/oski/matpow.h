/**
 *  \file oski/matpow.h
 *  \ingroup KERNELS
 *  \brief Sparse \f$A^k\cdot x\f$ implementation.
 */

#if !defined(INC_OSKI_MATPOW_H) && !defined(OSKI_UNBIND)
/** oski/matpow.h included. */
#define INC_OSKI_MATPOW_H

#include <oski/matmult.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_MatPowMult_funcpt  MANGLE_(oski_MatPowMult_funcpt)
#define oski_MatReprPowMult_funcpt \
	MANGLE_(oski_MatReprPowMult_funcpt)

#define oski_CheckArgsMatPowMult MANGLE_(oski_CheckArgsMatPowMult)
#define oski_MatPowMult         MANGLE_(oski_MatPowMult)

#define oski_traceargs_MatPowMult_t \
	MANGLE_(oski_traceargs_MatPowMult_t)
#define oski_MakeArglistMatPowMult \
	MANGLE_(oski_MakeArglistMatPowMult)
/*@}*/
#endif

/**
 *  \brief Computes a power of a matrix times a vector, or
 *  \f$y \leftarrow \alpha\cdot\mathrm{op}(A)^\rho x + \beta\cdot y\f$.
 *
 *  \ingroup PUBKERNELS
 */
int oski_MatPowMult (const oski_matrix_t A_tunable,
		     oski_matop_t opA, int power,
		     oski_value_t alpha, const oski_vecview_t x_view,
		     oski_value_t beta, oski_vecview_t y_view,
		     oski_vecview_t T_view);

/**
 *  \brief Check arguments passed to \ref oski_MatPowMult().
 */
int oski_CheckArgsMatPowMult (const oski_matrix_t A_tunable,
			      oski_matop_t opA, int power,
			      oski_value_t alpha, const oski_vecview_t x_view,
			      oski_value_t beta, oski_vecview_t y_view,
			      oski_vecview_t T_view, const char *caller);

/**
 *  \brief Function pointer type for oski_MatPowMult().
 */
typedef int (*oski_MatPowMult_funcpt) (const oski_matrix_t A_tunable,
				       oski_matop_t opA, int power,
				       oski_value_t alpha,
				       const oski_vecview_t x_view,
				       oski_value_t beta,
				       oski_vecview_t y_view,
				       oski_vecview_t T_view);

/**
 *  \brief Function pointer type for a matrix type-specific
 *  implementation of oski_MatPowMult().
 */
typedef int (*oski_MatReprPowMult_funcpt) (const void *A,
					   const oski_matcommon_t * props,
					   oski_matop_t opA, int power,
					   oski_value_t alpha,
					   const oski_vecview_t x_view,
					   oski_value_t beta,
					   oski_vecview_t y_view,
					   oski_vecview_t T_view);

/**
 *  \name Tracing.
 */
/*@{*/
/**
 *  \brief Trace argument signature for oski_MatPowMult().
 */
typedef struct
{
	/** For argument descriptions, see oski_MatPowMult(). */
  /*@{ */
  oski_matop_t opA;
  int power;
  oski_index_t num_vecs;
  oski_value_t alpha;
  oski_storage_t x_orient;
  oski_value_t beta;
  oski_storage_t y_orient;
  oski_storage_t t_orient;
  /*@} */
}
oski_traceargs_MatPowMult_t;
/*@}*/

/**
 *  \brief Initialize a static argument signature for a call to
 *  \ref oski_MatPowMult().
 */
void
oski_MakeArglistMatPowMult (oski_matop_t op, int power,
			    oski_value_t alpha, const oski_vecview_t x_view,
			    oski_value_t beta, const oski_vecview_t y_view,
			    const oski_vecview_t T_view,
			    oski_traceargs_MatPowMult_t * args);

/** \brief Compute the "flop factor." */
#define GET_XFLOP_MatPowMult( args ) \
	(2.0 * ((const oski_traceargs_MatPowMult_t *)(args))->num_vecs \
	     * ((const oski_traceargs_MatPowMult_t *)(args))->power)

#endif /* !defined(INC_OSKI_MATPOW_H) */

#if defined(OSKI_UNBIND)
#  include <oski/matmult.h>
#  undef INC_OSKI_MATPOW_H
#  undef GET_XFLOP_MatPowMult
#  undef oski_MatPowMult_funcpt
#  undef oski_MatReprPowMult_funcpt
#  undef oski_CheckArgsMatPowMult
#  undef oski_MatPowMult
#  undef oski_traceargs_MatPowMult_t
#  undef oski_MakeArglistMatPowMult
#endif


/* eof */
