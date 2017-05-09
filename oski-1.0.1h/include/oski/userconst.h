/**
 *  \file oski/userconst.h
 *  \brief Defines the constants available to the user in the
 *  official interface.
 *
 *  This header contains only those constants which are
 *  not related to errors and are defined in the official
 *  interface: http://bebop.cs.berkeley.edu/oski/oski-interface.pdf
 */

#if !defined(INC_OSKI_USERCONST_H)
/** oski/userconst.h has been included. */
#define INC_OSKI_USERCONST_H

/**
 *  Invalid (NULL) matrix handle.
 *
 *  \ingroup PUBLIC
 */
#define INVALID_MAT ((oski_matrix_t)NULL)

/**
 *  Invalid (NULL) vector view handle.
 *
 *  \ingroup PUBLIC
 */
#define INVALID_VEC ((oski_vecview_t)NULL)

/**
 *  \brief Input matrix copy modes.
 *
 *  On matrix handle creation, the library copies the user's input
 *  matrix arrays in one of two modes:
 *
 *      - Copy mode (COPY_INPUTMAT): The library makes an explicit,
 *        deep copy of the input matrix. Thus, the user may free the
 *        input matrix arrays immediately on return from the creation
 *        routine call.\n
 *        \n
 *      - Shared mode (SHARE_INPUTMAT): The library shares the input
 *        matrix arrays with the user. Specifically, the library and
 *        the user agree to the following conditions:\n
 *        \n
 *            -# The user promises that the input matrix arrays
 *               will not be freed or reallocated before destroying
 *               the matrix handle.
 *            -# The user promises not to modify any of the values
 *               in these arrays, except through the BeBOP
 *               interface (e.g., changing non-zero values through
 *               the interface's set-value routines).
 *            -# The library promises not to modify the arrays
 *               containing non-zero pattern data.
 *            -# The library promises to keep the input matrix
 *               and its tuned copy (if any) synchronized.
 *
 *  \ingroup PUBLIC
 */
typedef enum
{
  COPY_INPUTMAT,	 /**< Copy mode */
  SHARE_INPUTMAT	 /**< Share mode */
}
oski_copymode_t;

/** \brief Returns 1 <==> the specified copy mode x is legal.
 *
 *  \ingroup PUBLIC
 */
#define OSKI_CHECK_COPYMODE(x) \
	IS_VAL_IN_RANGE(x, COPY_INPUTMAT, SHARE_INPUTMAT)

/**
 *  \brief Dense multiple-vector storage layout.
 *
 *  On creation of a dense multiple-vector (i.e., dense matrix),
 *  the layout parameter specifies how the data is stored. Let
 *  \f$A\f$ be a logical \f$m\times n\f$ matrix stored in a
 *  dense physical array X with stride \f$s\f$. Then,
 *
 *      - Column major layout (LAYOUT_COLMAJ):
 *        Logical element \f$a_{ij}\f$ is stored in physical array
 *        element X[\f$(i-1) + (j-1)\cdot s\f$], where
 *        \f$1 \leq i \leq m \leq s\f$.
 *        This layout is the Fortran default for statically declared
 *        2-D arrays, and is also the usual BLAS convention.\n
 *        \n
 *      - Row major layout (LAYOUT_ROWMAJ):
 *        Logical element \f$a_{ij}\f$ is stored in physical array
 *        element X[\f$(i-1)\cdot s + (j-1) s\f$], where
 *        \f$1 \leq j \leq n \leq s\f$.
 *        This layout is the C default for statically declared
 *        2-D arrays.\n
 *        \n
 *
 *  Following the dense BLAS convention, we sometimes also refer to
 *  the stride parameter \f$s\f$ as the leading dimension.
 *
 *  \ingroup PUBLIC
 */
typedef enum
{
  LAYOUT_COLMAJ,	  /**< Column major layout. */
  LAYOUT_ROWMAJ		  /**< Row major layout. */
}
oski_storage_t;

/** \brief Returns 1 <==> the specified storage mode is legal */
#define OSKI_CHECK_STORAGE(x) \
	IS_VAL_IN_RANGE(x, LAYOUT_COLMAJ, LAYOUT_ROWMAJ)

/**
 *  \brief Matrix transpose options.
 *
 *  The user may apply the transpose or conjugate transpose of a
 *  matrix \f$A\f$.
 *
 *  \see OSKI_CHECK_MATOP
 *  \see oski_MatMult
 *  \see oski_MatMultAndMatTransMult
 *  \see oski_MatTrisolve
 *  \see oski_MatPowMult
 *
 *  \ingroup PUBLIC
 */
typedef enum
{
  OP_NORMAL,	     /**< Apply \f$A\f$. */
  OP_CONJ,	     /**< Applies the complex conjugate, \f$\bar{A}\f$ */
  OP_TRANS,	     /**< Apply the transpose, \f$A^T\f$. */
  OP_CONJ_TRANS	       /**< Apply the complex conjugate transpose,
					 \f$A^H=\bar{A}^T\f$. */
}
oski_matop_t;

/** \brief Returns 1 <==> the specified op is a legal matrix op type */
#define OSKI_CHECK_MATOP(op) \
	IS_VAL_IN_RANGE(op, OP_NORMAL, OP_CONJ_TRANS)

/**
 *  \brief Transpose options for matrix-transpose-times-matrix
 *  (\f$A^T\cdot A\f$) kernel.
 *
 *  \see OSKI_CHECK_ATAOP
 *  \see oski_MatTransMatMult
 *
 *  \ingroup PUBLIC
 */
typedef enum
{
  OP_AT_A,	   /**< Apply \f$A^T\cdot A\f$. */
  OP_AH_A,	   /**< Apply \f$A^H\cdot A\f$. */
  OP_A_AT,	   /**< Apply \f$A\cdot A^T\f$. */
  OP_A_AH	   /**< Apply \f$A\cdot A^H\f$. */
}
oski_ataop_t;

/** \brief Returns 1 <==> the specified op is a legal A^T*A op */
#define OSKI_CHECK_ATAOP(op) \
	IS_VAL_IN_RANGE(op, OP_AT_A, OP_A_AH)

/**
 *  \brief Input matrix properties.
 *
 *  Properties are divided into subcategories:
 *    - Pattern properties: MAT_GENERAL (default),
 *      MAT_TRI_UPPER, MAT_TRI_LOWER,
 *      MAT_SYMM_UPPER, MAT_SYMM_LOWER, MAT_SYMM_FULL,
 *      MAT_HERM_UPPER, MAT_HERM_LOWER, MAT_HERM_FULL
 *    - Diagonal properties: MAT_DIAG_EXPLICIT (default),
 *      MAT_UNIT_DIAG_IMPLICIT
 *    - Index base: INDEX_ONE_BASED (default), INDEX_ZERO_BASED
 *    - Index ordering: INDEX_UNSORTED (default), INDEX_SORTED
 *    - Index repetition: INDEX_REPEATED (default), INDEX_UNIQUE
 *
 *  Properties within a given subcategory are mutually exclusive,
 *  with default values as marked.
 *
 *  \note If modifying these properties, please also make the
 *  corresponding modifications in inmatprop.h
 *
 *  \ingroup PUBLIC
 */
typedef enum
{
  INMATPROP_NULL = -1,	     /**< "Undefined" property */

  /* Pattern properties. */
  MAT_GENERAL = 0,	 /**< General pattern [DEFAULT]. */
  MAT_TRI_UPPER,	 /**< Upper triangular. */
  MAT_TRI_LOWER,	 /**< Lower triangular. */
  MAT_SYMM_UPPER,
		/**< Symmetric (\f$A=A^T\f$), but only upper triangle stored. */
  MAT_SYMM_LOWER,	 /**< Symmetric, but only lower triangle stored. */
  MAT_SYMM_FULL,	 /**< Symmetric but full pattern stored. */
  MAT_HERM_UPPER,
		/**< Hermitian (\f$A=A^H\f$), but only upper triangle stored. */
  MAT_HERM_LOWER,	 /**< Hermitian, but only lower triangle stored. */
  MAT_HERM_FULL,	 /**< Hermitian but full pattern stored. */

  /* Diagonal properties. */
  MAT_DIAG_EXPLICIT,
		/**< Diagonal elements, if any, stored explicitly [DEFAULT]. */
  MAT_UNIT_DIAG_IMPLICIT,
		/**< \f$A(i,i)=1\f$ for all diagonal elements. */

  /* Index base. */
  INDEX_ONE_BASED,
		/**< Array indices start at 1 (Fortran convention) [DEFAULT]. */
  INDEX_ZERO_BASED,
		/**< Array indices start at 0 (C convention). */

  /* Index ordering. */
  INDEX_UNSORTED,	/**< Indices are unsorted [DEFAULT]. */
  INDEX_SORTED,	       /**< Indices are sorted. */

  /* Index repetition. */
  INDEX_REPEATED,	/**< Values for repeated indices are summed [DEFAULT]. */
  INDEX_UNIQUE,		/**< Indices are unique. */

  INMATPROP_MAX /**< Maximum property value. */
}
oski_inmatprop_t;

/**
 *  \name Degree of tuning.
 *  \ingroup HINT
 */
/*@{*/
/** Use to force "moderate" tuning in calls to \ref HINT routines. */
#define ALWAYS_TUNE               ((int)(1000))
/** Use to force "aggressive" tuning in calls to \ref HINT routines. */
#define ALWAYS_TUNE_AGGRESSIVELY  ((int)(100000))
/*@}*/

/**
 *  \name Structural tuning hints.
 *  \ingroup HINT
 */
/*@{*/
/**
 *  The possible structural hints listed below are grouped as follows.
 *
 *    -# Dense block substructure
 *    -# Block alignment
 *    -# Symmetry/structural symmetry
 *    -# Random versus correlated pattern (for cache blocking)
 *    -# Diagonal structure
 */
typedef enum
{
  /* Group 1 */
  HINT_NO_BLOCKS = 1,	       /**< No dense block structure */
  HINT_SINGLE_BLOCKSIZE,       /**< Structure dominated by a single block size.
							Parameters: r, c. */
  HINT_MULTIPLE_BLOCKSIZES,	  /**< Structure has mixture of block sizes.
							Parameters: k, r_1, c_1, ..., r_k, c_k. */

  /* Group 2 */
  HINT_ALIGNED_BLOCKS,	     /**< Blocks are uniformly aligned. */
  HINT_UNALIGNED_BLOCKS,       /**< Blocks may have arbitrary alignment. */

  /* Group 3 */
  HINT_SYMM_PATTERN,	   /**< Matrix is structurally symmetric, or nearly so. */
  HINT_NONSYMM_PATTERN,	      /**< Matrix is not structurally symmetric. */

  /* Group 4 */
  HINT_RANDOM_PATTERN,	     /**< Matrix non-zero pattern is nearly uniformly
						   random. */
  HINT_CORREL_PATTERN,	     /**< Row and column indices of structural
						   non-zeros are highly correlated. */

  /* Group 5 */
  HINT_NO_DIAGS,       /**< No "consecutive diagonal" structure exists. */
  HINT_DIAGS,	    /**< Consecutive diagonal structure exists.
				Parameters: k, d_1, ..., d_k. */

  HINT_MAX	 /**< Dummy value, for bounds checking */
}
oski_tunehint_t;
/*@}*/

/** \brief Returns 1 <==> Specified structural tuning hint is valid. */
#define OSKI_CHECK_TUNEHINT(hint) \
	IS_VAL_IN_RANGE(hint, HINT_NO_BLOCKS, HINT_MAX-1)

/**
 *  \brief Use when not providing arguments with a structural hint
 *  (see \ref HINT).
 *  \ingroup HINT
 */
#define ARGS_NONE   ((int)0)

#endif

/* eof */
