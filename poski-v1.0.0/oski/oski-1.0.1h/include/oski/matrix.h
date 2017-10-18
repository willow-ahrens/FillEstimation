/**
 *  \file oski/matrix.h
 *  \brief Matrix handle.
 */

#if !defined(INC_OSKI_MATRIX_H) && !defined(OSKI_UNBIND)
/** matrix.h has been included. */
#define INC_OSKI_MATRIX_H

#include <oski/common.h>
#include <oski/matcommon.h>
#include <oski/structhint.h>
#include <oski/trace.h>

#if defined(DO_NAME_MANGLING)
/** \name Mangled names. */
/*@{*/
#define oski_matstruct_t    MANGLE_(oski_matstruct_t)
#define oski_matrix_t       MANGLE_(oski_matrix_t)
#define const_oski_matrix_t      MANGLE_(const_oski_matrix_t)

#define oski_CreateMatStruct  MANGLE_(oski_CreateMatStruct)
#define oski_DestroyMatStruct MANGLE_(oski_DestroyMatStruct)
/*@}*/
#endif

/**
 *  \brief Generic matrix representation.
 *
 *  This structure is a wrapper around the library's representation
 *  of the matrix comprising these components:
 *    - A set of properties independent of the matrix type
 *    (e.g., logical matrix dimensions).
 *    - An input matrix, initialized on handle creation.
 *    - A tuned matrix, created during a call to the tune routine.
 *    - A trace, used to monitor the frequency of different kernel calls.
 *    - Structural and workload hints provided by the user.
 *    - A pre-initialized timer, for internal profiling.
 *    - The time to stream through the matrix, collected at
 *    handle-creation time.
 *    - A string corresponding to the transformations applied to
 *    the tuned matrix.
 */
typedef struct
{
  oski_matcommon_t props;	/**< Type-independent properties */
  oski_matspecific_t input_mat;	       /**< Input matrix representation */
  int is_shared;	/**< 0 <==> input matrix not shared with user */

  double time_stream;	     /**< Time to stream input matrix */

  oski_structhint_t structhints;	/**< Stores structural hints */
  oski_trace_t workhints;	       /**< Stores workload hints */

  oski_trace_t trace;		   /**< Stores kernel call trace */
  oski_timer_t timer;		   /**< Preallocated stopwatch */

  oski_matspecific_t tuned_mat;	       /**< Tuned matrix representation */
  char *tuned_xforms;	    /**< Tuning transformation string */
}
oski_matstruct_t;

/**
 *  \brief Sparse matrix handle type.
 */
typedef oski_matstruct_t *oski_matrix_t;

/**
 *  \brief Read-only (const) sparse matrix handle type,
 *  for function return values.
 */
typedef const oski_matstruct_t *const_oski_matrix_t;

/**
 *  \brief Returns a newly allocated matrix structure.
 */
oski_matstruct_t *oski_CreateMatStruct (void);

/**
 *  \brief Free a matrix structure (but not its child data structures).
 */
void oski_DestroyMatStruct (oski_matstruct_t * A);

/* -------------------------------------------------------
 *  Diagnostics.
 */

/** \brief Returns 1 <==> specified dimensions are legal */
#define OSKI_CHECK_MAT_DIMS( m, n )   (((m) >= 0) && ((n) >= 0))

/** \brief Returns 1 <==> dimensions are equal */
#define OSKI_IS_SQUARE_MAT( m, n )  ((m) == (n))

/* -------------------------------------------------------
 *  The matrix type module interface.
 */

#include <oski/CSR/format.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling of conversion routines. */
/*@{*/
#define oski_DestroyMatRepr_funcpt \
	MANGLE_(oski_DestroyMatRepr_funcpt)
#define oski_CreateMatReprFromCSR_funcpt \
	MANGLE_(oski_CreateMatReprFromCSR_funcpt)
#define oski_CreateSubmatReprFromCSR_funcpt \
	MANGLE_(oski_CreateSubmatReprFromCSR_funcpt)
#define oski_CopyMatRepr_funcpt \
	MANGLE_(oski_CopyMatRepr_funcpt)
#define oski_CalcMatRepr1Norm_funcpt \
	MANGLE_(oski_CalcMatRepr1Norm_funcpt)
#define oski_ConvertMatReprToCSR_funcpt \
        MANGLE_(oski_ConvertMatReprToCSR_funcpt)

#define tagOski_submat_t MANGLE_(tagOski_submat_t)
#define oski_submat_t    MANGLE_(oski_submat_t)
/*@}*/
#endif

/**
 *  \brief Defines a rectangular submatrix consisting of a given
 *  number of consecutive rows and columns, starting at a given
 *  offset. The base index is implicitly defined.
 */
typedef struct tagOski_submat_t
{
  oski_index_t i0;	  /**< Starting row index. */
  oski_index_t j0;	  /**< Starting column index. */
  oski_index_t num_rows;	/**< Number of rows. */
  oski_index_t num_cols;	/**< Number of columns. */
}
oski_submat_t;

/**
 *  \brief Method type: Destroy matrix type-specific representation.
 */
typedef void (*oski_DestroyMatRepr_funcpt) (void *mat);

/**
 *  \brief Method type: Instantiate from an existing CSR representation.
 */
#if 1
typedef void *(*oski_CreateMatReprFromCSR_funcpt) (const oski_matCSR_t * mat,
						   const oski_matcommon_t *
						   props, ...);
#endif

/**
 *  \brief Method type: Instantiate from a submatrix of an existing
 *  CSR representation.
 *
 *  \todo Future functionality: implement submatrix instantiation.
 */
typedef void *(*oski_CreateSubmatReprFromCSR_funcpt) (const oski_matCSR_t *
						      mat,
						      const oski_matcommon_t *
						      props,
						      const oski_submat_t *
						      sub, ...);

/**
 *  \brief Method type: Convert to CSR format.
 */
typedef oski_matCSR_t *(*oski_ConvertMatReprToCSR_funcpt) (const void *,
							   const
							   oski_matcommon_t *
							   props);

/**
 *  \brief Method type: Duplicate a matrix representation.
 */
typedef void *(*oski_CopyMatRepr_funcpt) (const void *,
					  const oski_matcommon_t * props);

/**
 *  \brief Methyod type: Compute the 1-norm.
 */
typedef double (*oski_CalcMatRepr1Norm_funcpt) (const void *mat,
						const oski_matcommon_t *
						props);

/* ----- Generic (matrix type-independent) kernel implementations ----- */

/**
*  \defgroup PUBKERNELS Kernels (matrix type-independent implementations).
*  \ingroup PUBLIC
*/

#include <oski/matmult.h>
#include <oski/ata.h>
#include <oski/trisolve.h>
#include <oski/a_and_at.h>
#include <oski/matpow.h>

/*@}*/

#endif /* !defined(INC_OSKI_MATRIX_H) */

#if defined(OSKI_UNBIND)
#  include <oski/matcommon.h>
#  include <oski/trace.h>
#  include <oski/CSR/format.h>
#  undef INC_OSKI_MATRIX_H
#  undef oski_matstruct_t
#  undef oski_matrix_t
#  undef const_oski_matrix_t
#  undef oski_CreateMatStruct
#  undef oski_DestroyMatStruct
#  undef OSKI_CHECK_MAT_DIMS
#  undef OSKI_IS_SQUARE_MAT
#  undef oski_DestroyMatRepr_funcpt
#  undef oski_CreateMatReprFromCSR_funcpt
#  undef oski_CreateSubmatReprFromCSR_funcpt
#  undef oski_CopyMatRepr_funcpt
#  undef oski_CalcMatRepr1Norm_funcpt
#  undef tagOski_submat_t
#  undef oski_submat_t
#endif

/* eof */
