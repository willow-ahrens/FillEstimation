/**
 *  \file oski/trisolve.h
 *  \ingroup KERNELS
 *  \brief Sparse triangular solve implementation.
 */

#if !defined(INC_OSKI_TRISOLVE_H) && !defined(OSKI_UNBIND)
/** oski/trisolve.h included. */
#define INC_OSKI_TRISOLVE_H

#include <oski/matmult.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_MatReprTrisolve_funcpt \
	MANGLE_(oski_MatReprTrisolve_funcpt)
#define oski_MatTrisolve         MANGLE_(oski_MatTrisolve)

#define oski_CheckArgsMatTrisolve \
	MANGLE_(oski_CheckArgsMatTrisolve)

#define oski_traceargs_MatTrisolve_t \
	MANGLE_(oski_traceargs_MatTrisovle_t)
#define oski_MakeArglistMatTrisolve \
	MANGLE_(oski_MakeArglistMatTrisolve)
/*@}*/
#endif

/**
 *  \brief Computes
 *  \f$x \leftarrow \alpha\cdot\mathrm{op}(T^{-1})x\f$,
 *  where \f$T\f$ is a triangular matrix.
 *
 *  \ingroup PUBKERNELS
 */
int oski_MatTrisolve (const oski_matrix_t T_tunable, oski_matop_t opT,
		      oski_value_t alpha, oski_vecview_t x_view);

/**
 *  \brief Check argument list for a call to \ref oski_MatTrisolve().
 */
int oski_CheckArgsMatTrisolve (const oski_matrix_t T_tunable,
			       oski_matop_t opT, oski_value_t alpha,
			       oski_vecview_t x_view, const char *caller);

/**
 *  \brief Function pointer type for a matrix type-specific
 *  implementation of oski_MatTrisolve().
 */
typedef int (*oski_MatReprTrisolve_funcpt) (const void *T,
					    const oski_matcommon_t * props,
					    oski_matop_t opT,
					    oski_value_t alpha,
					    oski_vecview_t x_view);

/**
 *  \name Tracing.
 */
/*@{*/
typedef struct
{
	/** For argument descriptions, see oski_MatTrisolve(). */
  /*@{ */
  oski_matop_t opT;
  oski_index_t num_vecs;
  oski_value_t alpha;
  oski_storage_t x_orient;
  /*@} */
}
oski_traceargs_MatTrisolve_t;

/**
 *  \brief Initialize a static argument signature for
 *  \ref oski_MatTrisolve().
 */
void oski_MakeArglistMatTrisolve (oski_matop_t op,
				  oski_value_t alpha,
				  const oski_vecview_t x_view,
				  oski_traceargs_MatTrisolve_t * args);

/** \brief Compute the "flop factor." */
#define GET_XFLOP_MatTrisolve( args ) \
	(2.0*((const oski_traceargs_MatTrisolve_t *)(args))->num_vecs)
/*@}*/

#endif /* !defined(INC_OSKI_TRISOLVE_H) */

#if defined(OSKI_UNBIND)
#  include <oski/matrix.h>
#  undef INC_OSKI_TRISOLVE_H
#  undef oski_MatReprTrisolve_funcpt
#  undef oski_MatTrisolve
#  undef oski_CheckArgsMatTrisolve
#  undef oski_traceargs_MatTrisolve_t
#  undef oski_MakeArglistMatTrisolve
#  undef GET_XFLOP_MatTrisolve
#endif

/* eof */
