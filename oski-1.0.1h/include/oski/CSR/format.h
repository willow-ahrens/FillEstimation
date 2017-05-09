/**
 *  \defgroup MATTYPE_CSR Compressed Sparse Row (CSR) Format
 *  \ingroup MATTYPES
 *
 *  BeBOP-OSKI supports a general form of compressed sparse row,
 *  with the following "features":
 *    - Either 0-based or 1-based indexing, for C and Fortran compatibility.
 *    - Symmetric/Hermitian storage.
 *    - Implicit unit diagonal.
 *    - Sorted or unsorted indices.
 *
 *  For a detailed description of the data structure and its fields,
 *  see #oski_matCSR_t.
 */

/**
 *  \file oski/CSR/format.h
 *  \ingroup MATTYPE_CSR
 *  \brief Compressed sparse row data structure.
 *
 *  For an overview, see \ref MATTYPE_CSR.
 */

/** \ingroup MATTYPE_CSR */
/*@{*/

#if !defined(INC_OSKI_CSR_FORMAT_H) && !defined(OSKI_UNBIND)
/** oski/CSR/format.h included. */
#define INC_OSKI_CSR_FORMAT_H

#include <oski/mangle.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
/** CSR matrix representation */
#define oski_matCSR_t     MANGLE_(oski_matCSR_t)

#define oski_WrapCSR_funcpt MANGLE_(oski_wrapCSR_funcpt)
#define oski_CheckCSR_funcpt MANGLE_(oski_checkCSR_funcpt)
#define oski_StreamMatRepr_funcpt MANGLE_(oski_StreamMatRepr_funcpt)
#define oski_SortIndices_funcpt MANGLE_(oski_SortIndices_funcpt)
#define oski_ExpandCSRToFull_funcpt MANGLE_(oski_ExpandCSRToFull_funcpt)
#define oski_ConditionallyExpandCSRToFull_funcpt \
	MANGLE_(oski_ConditionallyExpandCSRToFull_funcpt)
#define oski_ChangeIndexBase_funcpt  MANGLE_(oski_ChangeIndexBase_funcpt)
#define oski_TransposeCSR_funcpt MANGLE_(oski_TransposeCSR_funcpt)
#define oski_CountZeroRowsCSR_funcpt MANGLE_(oski_CountZeroRowsCSR_funcpt)
/*@}*/
#endif

/**
 *  \brief Compressed sparse row (CSR) format.
 *
 *  An \f$m\times n\f$ matrix \f$A\f$ is stored CSR format using
 *  three arrays, \f$(P, J, V)\f$, and a constant base index
 *  \f$b \in \{0,1\}\f$, such that
 *
 *    -# \f$P\f$ is an integer array of <em>row pointers</em>,
 *    of length at least \f$m+1\f$.
 *    -# \f$J\f$ is an integer array of <em>column indices</em>,
 *    of length at least \f$P[m]-b\f$.
 *    -# \f$V\f$ is an array of real/complex <em>stored values</em>,
 *    of length at least \f$P[m]-b\f$.
 *
 *  If the full non-zero pattern is stored, then the matrix elements
 *  \f$A(i,j)\f$ are computed as follows:
 *    - Let \f$K_{i,j} = \{k | J[k] = j, P[i] \leq k < P[i+1]\f$.
 *    Then \f$A(i,j) = \sum_{k\in K_{i,j}} V[k]\f$.
 *
 *  If the matrix is symmetric or Hermitian and the upper triangle is
 *  stored, then \f$A(i,j)\f$ are computed as above only for
 *  \f$i \leq j\f$. If instead the lower triangle is stored, then
 *  \f$A(i,j)\f$ are computed as above only for \f$i \geq j\f$.
 *
 *  If all the diagonal elements are identically 1 but not explicitly
 *  stored, then \f$K_{i,i} = \emptyset\f$. Otherwise, any non-zero
 *  diagonal elements <em>must</em> be stored explicitly.
 *
 *  If the matrix is logically upper triangular, then <em>only</em>
 *  elements \f$A(i,j)\f$ such that \f$j \geq i\f$ are stored.
 *
 *  If instead the matrix is logically lower triangular, then
 *  <em>only</em> elements \f$A(i,j)\f$ such that \f$j \geq i\f$
 *  are stored.
 *
 *  \todo Add a flag to indicate whether the matrix has a full
 *  (all non-zero) diagonal so that the triangular solve kernel
 *  does not have to check this condition explicitly.
 */
typedef struct
{
	/**
	 *  \brief Integer index starting value (0 or 1).
	 */
  oski_index_t base_index;

	/**
	 *  \brief \f$=1 \Longleftrightarrow A(i,i)=1\f$ for
	 *  all diagonal elements and no diagonal elements are
	 *  explicitly stored; 0 otherwise.
	 */
  int has_unit_diag_implicit;

	/**
	 *  \brief \f$=1 \Longleftrightarrow\f$ within each row,
	 *  the column indices are sorted in increasing order;
	 *  0 otherwise.
	 */
  int has_sorted_indices;

	/**
	 *  \brief Mutually exclusive non-zero pattern properties.
	 *
	 *  These properties work in conjunction with the 'pattern'
	 *  properties in oski_matcommon_t. If the matrix is
	 *  symmetric or Hermitian, then one of these flags indicates
	 *  which non-zero values are actually stored.
	 *
	 *  is_upper == is_lower <==> full storage.
	 *
	 *  If the matrix is triangular, these flags are ignored.
	 */
  struct
  {
    int is_upper;	      /**< Upper triangle stored. */
    int is_lower;	      /**< Lower triangle stored. */
  } stored;

  oski_index_t *ptr;	   /**< Row pointers. */
  oski_index_t *ind;	   /**< Column indices. */
  oski_value_t *val;	   /**< Stored values. */
  int is_shared;       /**< Buffers ptr, ind, val are shared */
}
oski_matCSR_t;

/** \name Methods available to external modules */
/*@{*/

#include <oski/inmatprop.h>
#include <oski/matrix.h>

/**
 *  \brief Function pointer type corresponding to oski_StreamMatRepr().
 */
typedef double (*oski_StreamMatRepr_funcpt) (const void *mat,
					     const oski_matcommon_t * props);

/**
 *  \brief Function pointer type for oski_ExpandCSRToFull().
 */
typedef oski_matCSR_t *(*oski_ExpandCSRToFull_funcpt) (const oski_matCSR_t *
						       S,
						       const oski_matcommon_t
						       * props);

/**
 *  \brief Function pointer type for oski_ConditionallyExpandCSRToFull().
 */
typedef int (*oski_ConditionallyExpandCSRToFull_funcpt) (const oski_matCSR_t *
							 S,
							 const
							 oski_matcommon_t *
							 props,
							 oski_matCSR_t **
							 p_mat_full);

/** \brief Function pointer type for oski_ChangeIndexBase(). */
typedef void (*oski_ChangeIndexBase_funcpt) (oski_matCSR_t * A,
					     const oski_matcommon_t * props,
					     const oski_index_t new_base);

/**
 *  \brief Function pointer type for oski_SortIndices().
 */
typedef int (*oski_SortIndices_funcpt) (oski_matCSR_t * A, oski_index_t m);

/**
 *  \brief Function pointer type for oski_TransposeCSR.
 */
typedef oski_matCSR_t *(*oski_TransposeCSR_funcpt) (const oski_matCSR_t * A,
						    const oski_matcommon_t *
						    props);

/**
 *  \brief Function pointer type corresponding to oski_WrapCSR()
 *
 *  \see oski_WrapCSR()
 */
typedef oski_matCSR_t *(*oski_WrapCSR_funcpt) (oski_matcommon_t * out_props,
					       oski_index_t * Aptr,
					       oski_index_t * Aind,
					       oski_value_t * Aval,
					       oski_index_t num_rows,
					       oski_index_t num_cols,
					       oski_inmatpropset_t * props,
					       oski_copymode_t mode);

/**
 *  \brief Function pointer type corresponding to oski_CountZeroRowsCSR()
 *
 *  \see oski_CountZeroRowsCSR()
 */
typedef oski_index_t (*oski_CountZeroRowsCSR_funcpt) (const oski_matCSR_t * A,
						      const oski_matcommon_t *
						      props);
/*@}*/

#endif

#if defined(OSKI_UNBIND)
#  include <oski/mangle.h>
#  undef INC_OSKI_CSR_FORMAT_H
#  undef oski_matCSR_t
#  undef oski_WrapCSR_funcpt
#  undef oski_CheckCSR_funcpt
#  undef oski_StreamMatRepr_funcpt
#  undef oski_SortIndices_funcpt
#  undef oski_ExpandCSRToFull_funcpt
#  undef oski_ConditionallyExpandCSRToFull_funcpt
#  undef oski_ChangeIndexBase_funcpt
#  undef oski_TransposeCSR_funcpt
#  undef oski_CountZeroRowsCSR_funcpt
#endif

/* eof */
