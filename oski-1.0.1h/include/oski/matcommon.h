/**
 *  \file oski/matcommon.h
 *  \brief Define properties common to all matrix types.
 */

#if !defined(INC_OSKI_MATCOMMON_H) && !defined(OSKI_UNBIND)
/** oski/matcommon.h included. */
#define INC_OSKI_MATCOMMON_H

#include <oski/common.h>
#include <oski/mangle.h>

#if defined(DO_NAME_MANGLING)
/** \name Mangled names. */
/*@{*/
#define oski_matcommon_t    MANGLE_(oski_matcommon_t)
#define oski_matspecific_t  MANGLE_(oski_matspecific_t)

#define oski_TransposePatternProp MANGLE_(oski_TransposePatternProp)
#define oski_TransposeProps  MANGLE_(oski_TransposeProps)
/*@}*/
#endif

/**
 *  \brief Matrix type-independent data.
 *
 *  This structure defines matrix attributes which are independent
 *  of the storage format.
 */
typedef struct
{
  oski_index_t num_rows;	    /**< Logical number of rows. */
  oski_index_t num_cols;	    /**< Logical number of columns. */
  oski_index_t num_nonzeros;	    /**< Number of logical non-zeros */

  int has_unit_diag;	   /**< \f$1 \Longleftrightarrow A(i,i)=1\f$ (else 0) */

	/** Mutually exclusive logical pattern properties */
  struct
  {
    int is_symm;	     /**< \f$=1 \Longleftrightarrow A = A^T\f$ (else 0) */
    int is_herm;	     /**< \f$=1 \Longleftrightarrow A = A^H\f$ (else 0) */
    int is_tri_upper;
			/**< \f$=1 \Longleftrightarrow \f$ matrix is upper triangular. */
    int is_tri_lower;
			/**< \f$=1 \Longleftrightarrow \f$ matrix is lower triangular. */
  } pattern;
}
oski_matcommon_t;

/**
 *  \brief Returns 1 if the given properties specify a square matrix,
 *  and 0 otherwise.
 */
#define OSKI_MATPROP_IS_SQUARE(props) ((props)->num_rows == (props)->num_cols)

/**
 *  \brief Returns 1 if the given properties specify a triangular matrix,
 *  and 0 otherwise.
 */
#define OSKI_MATPROP_IS_TRI(props) \
	((props)->pattern.is_tri_upper || (props)->pattern.is_tri_lower)

/**
 *  \brief Given a pattern type, return the corresponding transposed
 *  type. For example, if the input pattern is MAT_TRI_UPPER, the
 *  corresponding transpose pattern is MAT_TRI_LOWER. Similarly,
 *  the transpose of MAT_SYMM_UPPER is MAT_SYMM_LOWER.
 */
oski_inmatprop_t oski_TransposePatternProp (oski_inmatprop_t pattern);

/**  \brief Transpose common properties. */
void oski_TransposeProps (oski_matcommon_t * props);

/* ---------------------------------------------------------- */

/**
 *  \brief Matrix type-specific representation.
 *
 *  This structure defines generic attributes that specify the
 *  concrete storage format.
 */
typedef struct
{
  oski_id_t type_id;	     /**< Matrix type */
  void *repr;		      /**< Type-specific data representation */
}
oski_matspecific_t;

#endif /* !defined(INC_OSKI_MATCOMMON_H) */

#if defined(OSKI_UNBIND)
#  include <oski/common.h>
#  include <oski/mangle.h>
#  undef INC_OSKI_MATCOMMON_H
#  undef oski_matcommon_t
#  undef oski_matspecific_t
#  undef oski_TransposePatternProp
#  undef oski_TransposeProps
#  undef OSKI_MATPROP_IS_SQUARE
#  undef OSKI_MATPROP_IS_TRI
#endif

/* eof */
