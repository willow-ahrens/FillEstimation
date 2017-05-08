/**
 *  \file oski/ata.h
 *  \ingroup KERNELS
 *  \brief Sparse \f$A^TA\cdot x\f$ implementation.
 */

#if !defined(INC_OSKI_ATA_H) && !defined(OSKI_UNBIND)
/** oski/ata.h included. */
#define INC_OSKI_ATA_H

#include <oski/matmult.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_MatTransMatMult_funcpt  MANGLE_(oski_MatTransMatMult_funcpt)
#define oski_MatReprTransMatReprMult_funcpt \
	MANGLE_(oski_MatReprTransMatReprMult_funcpt)

#define oski_MatTransMatMult          \
		MANGLE_(oski_MatTransMatMult)
#define oski_CheckArgsMatTransMatMult \
		MANGLE_(oski_CheckArgsMatTransMatMult)

#define oski_traceargs_MatTransMatMult_t \
		MANGLE_(oski_traceargs_MatTransMatMult_t)
#define oski_MakeArglistMatTransMatMult \
		MANGLE_(oski_MakeArglistMatTransMatMult)
/*@}*/
#endif

/**
 *  \brief Computes
 *  \f$y \leftarrow \alpha\cdot\mathrm{op}(A)x + \beta\cdot y\f$,
 *  where \f$\mathrm{op}(A) \in \{AA^T, A^TA, AA^H, A^HA\}\f$.
 *
 *  \ingroup PUBKERNELS
 */
int oski_MatTransMatMult (const oski_matrix_t A_tunable,
			  oski_ataop_t op,
			  oski_value_t alpha, const oski_vecview_t x_view,
			  oski_value_t beta, oski_vecview_t y_view,
			  oski_vecview_t t_view);

/**
 *  \brief Check input arguments for a routine with a signature
 *  like oski_MatTransMatMult().
 */
int
oski_CheckArgsMatTransMatMult (const oski_matrix_t A_tunable,
			       oski_ataop_t op,
			       oski_value_t alpha,
			       const oski_vecview_t x_view, oski_value_t beta,
			       const oski_vecview_t y_view,
			       const oski_vecview_t t_view,
			       const char *caller);


/**
 *  \brief Function pointer type for oski_MatTransMatMult().
 */
typedef int (*oski_MatTransMatMult_funcpt) (const oski_matrix_t A_tunable,
					    oski_ataop_t op,
					    oski_value_t alpha,
					    const oski_vecview_t x_view,
					    oski_value_t beta,
					    oski_vecview_t y_view,
					    oski_vecview_t t_view);

/**
 *  \brief Function pointer type for a matrix type-specific
 *  implementation of oski_MatTransMatMult().
 */
typedef int (*oski_MatReprTransMatReprMult_funcpt) (const void *A,
						    const oski_matcommon_t *
						    props, oski_ataop_t op,
						    oski_value_t alpha,
						    const oski_vecview_t
						    x_view, oski_value_t beta,
						    oski_vecview_t y_view,
						    oski_vecview_t t_view);

/**
 *  \name Tracing.
 */
/*@{*/
/**
 *  \brief Trace argument signature for oski_MatTransMatMult().
 */
typedef struct
{
	/** For argument descriptions, see oski_MatTransMatMult(). */
  /*@{ */
  oski_ataop_t op;
  oski_index_t num_vecs;
  oski_value_t alpha;
  oski_storage_t x_orient;
  oski_value_t beta;
  oski_storage_t y_orient;
  oski_storage_t t_orient;
  /*@} */
}
oski_traceargs_MatTransMatMult_t;

/**
 *  \brief Initialize a static argument signature for
 *  \ref oski_MatTransMatMult.
 */
void oski_MakeArglistMatTransMatMult (oski_ataop_t opA,
				      oski_value_t alpha,
				      const oski_vecview_t x_view,
				      oski_value_t beta,
				      const oski_vecview_t y_view,
				      const oski_vecview_t t_view,
				      oski_traceargs_MatTransMatMult_t *
				      args);

/** \brief Compute the "flop factor." */
#define GET_XFLOP_MatTransMatMult( args ) \
	(4.0*((const oski_traceargs_MatTransMatMult_t *)(args))->num_vecs)

/*@}*/

#endif /* !defined(INC_OSKI_ATA_H) */

#if defined(OSKI_UNBIND)
#  include <oski/matmult.h>
#  undef INC_OSKI_ATA_H
#  undef oski_MatTransMatMult_funcpt
#  undef oski_MatReprTransMatReprMult_funcpt
#  undef oski_MatTransMatMult
#  undef oski_CheckArgsMatTransMatMult
#  undef oski_traceargs_MatTransMatMult_t
#  undef oski_MakeArglistMatTransMatMult
#  undef GET_XFLOP_MatTransMatMult
#endif

/* eof */
