/**
 *  \defgroup MATTYPE_BCSR Block Compressed Sparse Row (BCSR) Format
 *  \ingroup MATTYPES
 */

/**
 *  \file oski/BCSR/format.h
 *  \ingroup MATTYPES_BCSR
 *  \brief Block compressed sparse row data structure.
 *
 *  For an overview, see \ref MATTYPE_BCSR.
 */

/** \ingroup MATTYPE_BCSR */
/*@{*/
#if !defined(INC_OSKI_BCSR_FORMAT_H)
/** BCSR/format.h included. */
#define INC_OSKI_BCSR_FORMAT_H

#include <oski/common.h>
#include <oski/matrix.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/

/** BCSR matrix representation */
#define oski_matBCSR_t  MANGLE_(oski_matBCSR_t)

/*@}*/
#endif

/**
 *  \brief Block compressed sparse row (BCSR) format.
 *
 *  An \f$m\times n\f$ matrix \f$A\f$ is stored in
 *  \f$r\times c\f$ BCSR format using six arrays,
 *  \f$(P, J, V, \hat{P}, \hat{J}, \hat{V})\f$.
 *  The triplet \f$(P, J, V)\f$ stores rows 1 through
 *  \f$M = \lfloor\frac{m}{r}\rfloor\f$ using uniformly
 *  aligned \f$r\times c\f$ blocks, and the triplet
 *  \f$(\hat{P}, \hat{J}, \hat{V})\f$ stores any
 *  remaining \f$r' = m\bmod r\f$ rows using uniformly
 *  aligned \f$r'\times c\f$ blocks.
 *
 *  -# \f$P\f$ is an integer array of <em>block-row pointers</em>,
 *  of length at least \f$M+1\f$.
 *  -# \f$J\f$ is an integer array of <em>block column indices</em>,
 *  of length at least \f$P[M]\f$.
 *  -# \f$V\f$ is an array of <em>non-zero block values</em>,
 *  of length at least \f$P[M]\cdot r\cdot c\f$.
 *
 *  Define block-row \f$I\f$ to be rows \f$1 + (I-1)\cdot r\f$
 *  through \f$I\cdot r\f$ of \f$A\f$, where \f$1 \leq I \leq M\f$.
 *  For each \f$k\f$ such that \f$P[I-1] \leq k \leq P[I]\f$,
 *  \f$j_0=J[k]+1\f$ is the column index \f$(1,1)\f$ entry of an
 *  explicitly stored non-zero block whose values are laid out in
 *  row-major order in the subarray \f$V[krc : (k+1)rc-1]\f$.
 *
 *  These blocks are <em>uniformly aligned</em>, meaning that
 *  \f$(j_0-1)\bmod c = 0\f$. However, there is one exception:
 *  if \f$c\f$ does not divide \f$n\f$, then each block row may
 *  contain one block with \f$j_0 = n - c + 1\f$. If such a block
 *  overlaps with another block starting at column index
 *  \f$j_0'\f$, then the initial columns of the block at
 *  \f$j_0\f$ are set to zero.
 *
 *  If \f$r\f$ does not divide \f$m\f$, then the remaining rows
 *  are stored in \f$(\hat{P},\hat{J},\hat{V})\f$ as a single
 *  block row with \f$r'\times c\f$ blocks. These arrays follow
 *  the same conventions as \f$(P,J,V)\f$.
 */
typedef struct tagBebop_matBCSR_t
{
  int has_unit_diag_implicit;	     /**< 1 <==> Diagonal == 1 and is
									implicitly stored */

  oski_index_t row_block_size;	      /**< Row block size */
  oski_index_t col_block_size;	      /**< Column block size */

  oski_index_t num_block_rows;	      /**< # of full block rows */
  oski_index_t num_block_cols;	      /**< # of full block columns */

  oski_index_t *bptr;	     /**< Block row pointers. */
  oski_index_t *bind;	     /**< Block column indices. */
  oski_value_t *bval;	     /**< Non-zero blocks. */

	/** Number of "leftover" rows (i.e., \f$\mathrm{mod}(m, r)\f$). */
  oski_index_t num_rows_leftover;

	/** Stores leftover rows, if any. (NULL otherwise) */
  struct tagBebop_matBCSR_t *leftover;

	/** Module name containing this rxc implementation */
  char *mod_name;
  void *mod_cached;	  /**< Cached module pointer */
}
oski_matBCSR_t;

#endif
/*@}*/
/* eof */
