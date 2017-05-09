/**
 *  \defgroup MATTYPE_VBR Variable Block Row (VBR) Format
 *  \ingroup MATTYPES
 *
 *  Variable block row (VBR) format logically partitions the rows
 *  and columns, and stores all non-zero blocks block-row-wise.
 */

/**
 *  \file oski/VBR/format.h
 *  \ingroup MATTYPE_VBR
 *  \brief Generalized compressed sparse row data structure.
 *
 *  For an overview, see \ref MATTYPE_VBR.
 */

/** \ingroup MATTYPE_VBR */
/*@{*/

#if !defined(INC_OSKI_VBR_FORMAT_H)
/** oski/VBR/format.h included. */
#define INC_OSKI_VBR_FORMAT_H

#include <oski/common.h>
#include <oski/matrix.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
/** VBR matrix representation */
#define oski_matVBR_t     MANGLE_(oski_matVBR_t)
/*@}*/
#endif

/**
 *  \brief Variable block row (VBR) format.
 *
 *  Variable block row (VBR) format generalizes block compressed
 *  sparse row (BCSR) by allowing block rows and columns to have
 *  variable sizes.
 *
 *  An \f$m\times n\f$ matrix \f$A\f$ stored in VBR format is
 *  partitioned into \f$M\f$ block rows and \f$N\f$ block columns and
 *  then stored into six arrays:
 *
 *  - brow[0:\f$M\f$] stores the starting positions in \f$A\f$ of each
 *  block row. More specifically, the \f$I^{\mathrm{th}}\f$ block row
 *  starts at row brow[\f$I\f$] of \f$A\f$, ends at brow[\f$I+1\f$]-1,
 *  and brow[\f$M\f$] = \f$m\f$.
 *  - bcol[0:\f$N\f$] stores the block column positions.
 *  - val[0:\f$k-1\f$] stores the non-zero values, block-by-block, with
 *  blocks laid out by row.
 *  - val_ptr[0:\f$K\f$] stores starting offsets of each block within
 *  val. The \f$b^{\mathrm{th}}\f$ block starts at position
 *  val_ptr[\f$b\f$] in the array val. The last element
 *  val_ptr[0:\f$K\f$] = \f$k\f$.
 *  - ind[0:\f$K-1\f$] stores the block column indices. The
 *  \f$b^{\mathrm{th}}\f$ block begins at column bcol[ind[\f$b]].
 *  - ptr[0:\f$M\f$] stores the starting offsets of each block row
 *  within ind. The \f$I^{\mathrm{th}}\f$ block row starts at position
 *  ptr[\f$I\f$] in ind.
 */
typedef struct
{
  oski_index_t mb; /**< No. of block rows, \f$M\f$. */
  oski_index_t nb; /**< No. of block columns, \f$N\f$. */

  oski_index_t* brow; /**< Block row starting indices */
  oski_index_t* bcol; /**< Block column starting indices */
  oski_value_t* val; /**< Non-zero values */
  oski_index_t* valptr; /**< Block value pointers */
  oski_index_t* ind; /**< Block column indices */
  oski_index_t* ptr; /**< Block row pointers */
}
oski_matVBR_t;

/*@}*/
#endif

/* eof */
