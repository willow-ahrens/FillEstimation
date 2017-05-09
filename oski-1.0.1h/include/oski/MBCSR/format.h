/**
 *  \defgroup MATTYPE_MBCSR Modified Block Compressed Sparse Row (MBCSR) Format
 *  \ingroup MATTYPES
 *
 *  Modified block compressed sparse row format stores the block diagonal
 *  elements separately from the off-diagonal elements. OSKI uses
 *  MBCSR to simplify the implementations of the symmetric matrix-vector
 *  multiply and sparse triangular solve kernels.
 *
 *  \todo MBCSR currently has an overly strong interdependence on the
 *  BCSR data structure as defined in include/oski/BCSR/format.h because
 *  MBCSR contains a pointer to a BCSR object, and moreover initializes
 *  the fields of the BCSR object explicitly. We should weaken this
 *  dependence by implementing the submatrix instantiation functionality
 *  (see the defined but unused structure, \ref oski_submat_t).
 */

/**
 *  \file oski/MBCSR/format.h
 *  \ingroup MATTYPES_MBCSR
 *  \brief Modified block compressed sparse row data structure.
 *
 *  For an overview, see \ref MATTYPE_MBCSR.
 */

/** \ingroup MATTYPE_MBCSR */
/*@{*/
#if !defined(INC_OSKI_MBCSR_FORMAT_H)
/** oski/MBCSR/format.h included. */
#define INC_OSKI_MBCSR_FORMAT_H

#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/BCSR/format.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/

/** MBCSR matrix representation */
#define oski_submatMBCSR_t  MANGLE_(oski_submatMBCSR_t)
#define oski_matMBCSR_t     MANGLE_(oski_matMBCSR_t)

#define oski_MBCSR_MatMult_funcpt  MANGLE_(oski_MBCSR_MatMult_funcpt)
/*@}*/
#endif

/**
 *  \brief Stores an MBCSR "submatrix".
 *
 *  An \f$m\times n\f$ matrix \f$B\f$ stored in \f$r\times c\f$
 *  MBCSR submatrix format must satisfy the following conditions:
 *    - \f$r\f$ divides \f$m\f$
 *    - \f$m \leq n\f$
 *
 *  Let \f$M = \frac{m}{r}\f$ be the number of block rows. Then
 *  \f$B\f$ is stored in 4 arrays, \f$(P, J, V, D)\f$, as follows:
 *    - \f$D\f$ is an array of length \f$Mr^2\f$ containing all the
 *      elements of \f$B\f$ that lie within the \f$M\f$ diagonal
 *      blocks beginning at element \f$B(1, \delta)\f$ where each
 *      block is a square \f$r\times r\f$ block. The blocks are
 *      stored consecutively, and each block is stored in row-major
 *      order. More precisely, for all \f$0 \leq I_0 < M\f$,
 *      \f$0 \leq i < r\f$, and \f$0 \leq j < r\f$, the array
 *      element \f$D[I_0\cdot r^2 + i\cdot r + j]\f$ stores the
 *      matrix element \f$B(1+I_0\cdot r + i, \delta+I_0\cdot r + j)\f$.
 *    - \f$P\f$ is an integer array of <em>block-row pointers</em>,
 *      of length at least \f$M+1\f$.
 *    - \f$J\f$ is an integer array of <em>block column indices</em>,
 *      of length at least \f$P[M]\f$.
 *    - \f$V\f$ is an array of <em>non-zero block values</em>, of
 *      length at least \f$P[M]\cdot r\cdot c\f$.
 *    - Define block-row \f$I\f$, where \f$0 \leq I < M\f$, as rows
 *      \f$1+(I-1)\cdot r\f$ through \f$I\cdot r\f$ of \f$B\f$. Then
 *      For each \f$k\f$ such that \f$P[I-1] \leq k \leq P[I]\f$,
 *      \f$j_0=J[k]+1\f$ is the column index of the \f$(1,1)\f$ entry
 *      of an explicitly stored non-zero block whose values are laid
 *      out in row-major order in the subarray \f$V[krc : (k+1)rc-1]\f$.
 *    - If any block in \f$V\f$ overlaps with a diagonal block in
 *      \f$D\f$, then the corresponding entries in \f$V\f$ are set to 0.
 *    - No block in \f$V\f$ is defined so that it extends beyond column
 *      \f$n\f$ of \f$B\f$. That is, if \f$c\f$ does not divide \f$n\f$,
 *      then each block row may contain one block with
 *      \f$j_0 = n - c + 1\f$. If such a block overlaps with another
 *      block starting at column index \f$j_0'\f$, then the initial
 *      columns of the block at \f$j_0\f$ are set to zero.
 */
typedef struct tagOski_submatMBCSR_t
{
	/** # of full block rows; if 0, all other fields are invalid. */
  oski_index_t num_block_rows;

  oski_index_t offset;	     /**< Row/column index of first diagonal block */
  oski_index_t r;	 /**< Row block size */
  oski_index_t c;	 /**< Column block size */

  oski_index_t *bptr;	     /**< Block row pointers. */
  oski_index_t *bind;	     /**< Column indices. */
  oski_value_t *bval;	     /**< Non-zero value off-diagonal blocks */
  oski_value_t *bdiag;	     /**< Non-zero value diagonal blocks */

  char *mod_name;	/**< Name of module at this block size */
  void *mod_cached;	  /**< Cached implementation */
}
oski_submatMBCSR_t;

/**
 *  \brief Modified block compressed sparse row (MBCSR) format.
 *
 *  An \f$m\times n\f$ matrix \f$A\f$ stored in \f$r\times c\f$ MBCSR
 *  format is logically partitioned row-wise into 3 submatrices,
 *  \f$A = \left(\begin{array} {}A_1 \\ A_2 \\ A_3\end{array}\right)\f$,
 *  where \f$A\f$ is stored in \f$r\times c\f$ MBCSR format. This
 *  partitioning is a `canonical' format in which \f$A_1\f$, \f$A_2\f$,
 *  and \f$A_3\f$ have the following properties:
 *    - \f$A_1\f$ consists of all block rows and diagonal blocks
 *  of size \f$r\times c\f$ and \f$r\times r\f$, respectively.
 *    - \f$A_2\f$ contains the degenerate diagonal block of size
 *  \f$r'\times r'\f$ where \f$r' < r\f$, and the corresponding
 *  block row of size \f$r'\f$ that contains it.
 *    - \f$A_3\f$ contains no diagonal blocks (i.e., is simply
 *  stored in BCSR format).
 *
 *  The purpose of this format is to enable fast implementations
 *  of the triangular solve and \f$A^TA\cdot x\f$ kernels in
 *  which the `special case' code to handle degenerate diagonal
 *  block, if any, is stored separately in \f$A_2\f$, and
 *  multiplication by \f$A_1, A_2,\f$ and \f$A_3\f$ can execute
 *  at `full' speed. The canonical form provides a uniform way
 *  to  treat both square and rectangular matrices stored in
 *  MBCSR format.
 */
typedef struct tagBebop_matMBCSR_t
{
  oski_submatMBCSR_t A1;       /**< \f$A_1\f$ */
  oski_submatMBCSR_t A2;       /**< \f$A_2\f$ */
  oski_matBCSR_t *p_A3;	      /**< Pointer to \f$A_3\f$ */

	/**
	 *  \brief Mutually exclusive non-zero pattern properties.
	 *
	 *  These properties work in conjunction with the 'pattern'
	 *  properties in oski_matcommon_t. If the matrix is
	 *  symmetric or Hermitian, then one of these flags
	 *  indicates which non-zero values are actually stored.
	 *
	 *  is_upper == is_lower <==> full storage.
	 *
	 *  These flags apply to each of the individual submatrices,
	 *  \f$A_1\f$, \f$A_2\f$, and \f$A_3\f$.
	 *
	 *  If the matrix is triangular, these flags are ignored.
	 */
  struct
  {
    int is_upper;
    int is_lower;
  } stored;

	/**
	 *  \brief Flags to enable/disable individual kernel
	 *  implementations.
	 *
	 *  These are currently implemented as boolean flags,
	 *  but stored as general integers for future expansion.
	 *
	 *  \note We implicitly prefer to always conserve matrix
	 *  storage, and so 'symmetry' is always enabled.
	 */
  struct
  {
    int MatTransMatMult;	     /**< Specialized \f$A^TAx\f$ kernel */
    int MatMultAndMatMult;	       /**< Specialized \f$(Ax,Aw)\f$ kernel */
    int MatMultAndMatTransMult;		    /**< Specialized \f$(Ax,A^Tw)\f$ kernel */
    int MatPowMult;		/**< Specialized \f$A^kx\f$ kernel */
    int MatTransPowMult;	     /**< Specialized \f$(A^T)^kx\f$ kernel */
  } enabled;
}
oski_matMBCSR_t;

#endif
/*@}*/
/* eof */
