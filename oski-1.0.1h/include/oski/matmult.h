/**
 *  \file oski/matmult.h
 *  \ingroup KERNELS
 *  \brief Sparse matrix-vector multiply implementation.
 */

#if !defined(INC_OSKI_MATMULT_H) && !defined(OSKI_UNBIND)
/** oski/matmul.h included. */
#define INC_OSKI_MATMULT_H

#include <oski/mangle.h>
#include <oski/matrix.h>
#include <oski/vecview.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_MatMult_funcpt  MANGLE_(oski_MatMult_funcpt)
#define oski_MatReprMult_funcpt MANGLE_(oski_MatReprMult_funcpt)
#define oski_MatMult           MANGLE_(oski_MatMult)
#define oski_CheckArgsMatMult  MANGLE_(oski_CheckArgsMatMult)
#define oski_traceargs_MatMult_t MANGLE_(oski_traceargs_MatMult_t)
#define oski_MakeArglistMatMult MANGLE_(oski_MakeArglistMatMult)
/*@}*/
#endif

/** \brief Returns 1 <==> multiply dimensions are compatible.
 *
 *  Given a desired matrix multiply operation,
 *  \f$C \leftarrow A \times B\f$,
 *  returns 1 <==> the dimensions of all three matrices are
 *  legal and compatible.
 *
 *  By 'legal,' we mean according to OSKI_CHECK_MAT_DIMS, which
 *  this routine also calls.
 *
 *  \param[in] m_A Number of rows in A.
 *  \param[in] k_A Number of columns in A.
 *  \param[in] k_B Number of rows in B.
 *  \param[in] n_B Number of columns in B.
 *  \param[in] m_C Number of rows in C.
 *  \param[in] n_C Number of columns in C.
 *
 *  \see OSKI_CHECK_MAT_DIMS, OSKI_MATTRANSMULT_COMPAT_DIMS
 */
#define OSKI_MATMULT_COMPAT_DIMS( m_A, k_A, k_B, n_B, m_C, n_C ) \
    ( OSKI_CHECK_MAT_DIMS(m_A, k_A) && OSKI_CHECK_MAT_DIMS(k_B, n_B) \
	      && OSKI_CHECK_MAT_DIMS(m_C, n_C) \
		      && ((m_A) == (m_C)) && ((k_A) == (k_B)) && ((n_B) == (n_C)) )

/** \brief Returns 1 <==> transpose-multiply dimensions are compatible.
 *
 *  Given a desired matrix multiply operation,
 *  \f$C \leftarrow op(A) \times B\f$, where \f$op(A) \in \{A^T, A^H\}\f$,
 *  returns 1 <==> the dimensions of all three matrices are
 *  legal and compatible.
 *
 *  By 'legal,' we mean according to OSKI_CHECK_MAT_DIMS, which
 *  this routine also calls.
 *
 *  \param[in] m_A Number of rows in A.
 *  \param[in] k_A Number of columns in A.
 *  \param[in] k_B Number of rows in B.
 *  \param[in] n_B Number of columns in B.
 *  \param[in] m_C Number of rows in C.
 *  \param[in] n_C Number of columns in C.
 *
 *  \see OSKI_CHECK_MAT_DIMS, OSKI_MATMULT_COMPAT_DIMS
 */
#define OSKI_MATTRANSMULT_COMPAT_DIMS( m_A, k_A, k_B, n_B, m_C, n_C ) \
    ( OSKI_CHECK_MAT_DIMS(m_A, k_A) && OSKI_CHECK_MAT_DIMS(k_B, n_B) \
	      && OSKI_CHECK_MAT_DIMS(m_C, n_C) \
		      && ((k_A) == (m_C)) && ((m_A) == (k_B)) && ((n_B) == (n_C)) )

/**
 *  \brief Returns 1 <==> multiply dimensions are compatible.
 *
 *  This routine is a high-level object wrapper around an equivalent
 *  call to #OSKI_CHECK_MAT_DIMS(). Specifically, this routine
 *  assumes the operands A, B, and C are pointers to structs
 *  containing 'num_rows' and 'num_cols' fields.
 *
 *  A must be a valid object. However, either B or C may be NULL,
 *  in which case the missing argument is assumed to have the
 *  correct dimensions. B and C must not both be NULL, however.
 */
#define OSKI_MATMULT_COMPAT_OBJ_DIMS( A, B, C ) \
	OSKI_MATMULT_COMPAT_DIMS( (A)->num_rows, (A)->num_cols, \
		((B) == INVALID_VEC) \
		|| ((B) == SYMBOLIC_VEC) || ((B) == SYMBOLIC_MULTIVEC) \
			? (A)->num_cols : (B)->num_rows, \
		((B) == INVALID_VEC) \
		|| ((B) == SYMBOLIC_VEC) || ((B) == SYMBOLIC_MULTIVEC) \
			? (C)->num_cols : (B)->num_cols, \
		((C) == INVALID_VEC) \
		|| ((C) == SYMBOLIC_VEC) || ((C) == SYMBOLIC_MULTIVEC) \
			? (A)->num_rows : (C)->num_rows, \
		((C) == INVALID_VEC) \
		|| ((C) == SYMBOLIC_VEC) || ((C) == SYMBOLIC_MULTIVEC) \
			? (B)->num_cols : (C)->num_cols \
	)

/**
 *  \brief Returns 1 <==> transpose-multiply dimensions are compatible.
 *
 *  This routine is a high-level object wrapper around an equivalent
 *  call to #OSKI_CHECK_MAT_DIMS(). Specifically, this routine
 *  assumes the operands A, B, and C are pointers to structs
 *  containing 'num_rows' and 'num_cols' fields.
 *
 *  A must be a valid object. However, either B or C may be NULL
 *  or symbolic, in which case the missing argument is assumed to
 *  have the correct dimensions. B and C must not both be NULL (though
 *  they may both be symbolic), however.
 */
#define OSKI_MATTRANSMULT_COMPAT_OBJ_DIMS( A, B, C ) \
	OSKI_MATTRANSMULT_COMPAT_DIMS( (A)->num_rows, (A)->num_cols, \
		((B) == INVALID_VEC) \
		|| ((B) == SYMBOLIC_VEC) || ((B) == SYMBOLIC_MULTIVEC) \
			? (A)->num_rows : (B)->num_rows, \
		((B) == INVALID_VEC) \
		|| ((B) == SYMBOLIC_VEC) || ((B) == SYMBOLIC_MULTIVEC) \
			? (C)->num_cols : (B)->num_cols, \
		((C) == INVALID_VEC) \
		|| ((C) == SYMBOLIC_VEC) || ((C) == SYMBOLIC_MULTIVEC) \
			? (A)->num_cols : (C)->num_rows, \
		((C) == INVALID_VEC) \
		|| ((C) == SYMBOLIC_VEC) || ((C) == SYMBOLIC_MULTIVEC) \
			? (B)->num_cols : (C)->num_cols \
	)


/**
 *  \brief Computes
 *  \f$y \leftarrow \alpha\cdot\mathrm{op}(A)x + \beta\cdot y\f$,
 *  where \f$\mathrm{op}(A) \in \{A, A^T, A^H\}\f$.
 *
 *  \ingroup PUBKERNELS
 */
int
oski_MatMult (const oski_matrix_t A_tunable, oski_matop_t opA,
	      oski_value_t alpha, const oski_vecview_t x_view,
	      oski_value_t beta, oski_vecview_t y_view);

/**
 *  \brief Checks validity of arguments to a routine with a matrix
 *  vector multiply signature.
 */
int
oski_CheckArgsMatMult (const oski_matrix_t A_tunable,
		       oski_matop_t opA,
		       oski_value_t alpha, const oski_vecview_t x_view,
		       oski_value_t beta, const oski_vecview_t y_view,
		       const char *caller);


/**
 *  \brief Function pointer type for oski_MatMult().
 */
typedef int (*oski_MatMult_funcpt) (const oski_matrix_t A_tunable,
				    oski_matop_t opA, oski_value_t alpha,
				    const oski_vecview_t x_view,
				    oski_value_t beta, oski_vecview_t y_view);

/**
 *  \brief Function pointer type for a matrix type-specific
 *  implementation of oski_MatMult().
 */
typedef int (*oski_MatReprMult_funcpt) (const void *A,
					const oski_matcommon_t * props,
					oski_matop_t opA, oski_value_t alpha,
					const oski_vecview_t x_view,
					oski_value_t beta,
					oski_vecview_t y_view);

/**
 *  \name Tracing.
 */
/*@{*/
/**
 *  \brief Trace argument signature for oski_MatMult().
 */
typedef struct
{
	/** For argument descriptions, see oski_MatMult(). */
  /*@{ */
  oski_matop_t opA;
  oski_index_t num_vecs;
  oski_value_t alpha;
  oski_storage_t x_orient;
  oski_value_t beta;
  oski_storage_t y_orient;
  /*@} */
}
oski_traceargs_MatMult_t;

/** \brief Initialize a static argument signature. */
void oski_MakeArglistMatMult (oski_matop_t opA,
			      oski_value_t alpha, const oski_vecview_t x_view,
			      oski_value_t beta, const oski_vecview_t y_view,
			      oski_traceargs_MatMult_t * args);

/** \brief Compute the "flop factor." */
#define GET_XFLOP_MatMult(args) \
	(2.0*((const oski_traceargs_MatMult_t *)(args))->num_vecs)

/*@}*/

#endif /* !defined(INC_OSKI_MATMULT_H) */

#if defined(OSKI_UNBIND)
#  include <oski/mangle.h>
#  include <oski/matrix.h>
#  include <oski/vecview.h>
#  undef INC_OSKI_MATMULT_H
#  undef oski_MatMult_funcpt
#  undef oski_MatReprMult_funcpt
#  undef oski_MatMult
#  undef oski_CheckArgsMatMult
#  undef oski_traceargs_MatMult_t
#  undef oski_MakeArglistMatMult
#  undef OSKI_MATMULT_COMPAT_DIMS
#  undef OSKI_MATTRANSMULT_COMPAT_DIMS
#  undef OSKI_MATMULT_COMPAT_OBJ_DIMS
#  undef OSKI_MATTRANSMULT_COMPAT_OBJ_DIMS
#  undef GET_XFLOP_MatMult
#endif

/* eof */
