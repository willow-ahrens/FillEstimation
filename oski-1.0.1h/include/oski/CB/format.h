/**
 *  \defgroup MATTYPE_CB  Cache Blocked (CB) Format
 *  \ingroup MATTYPES
 *
 *  Cache blocking implementation in which each submatrix is
 *  stored as a complete matrix handle to an arbitrary matrix
 *  type.
 */

/**
 *  \file oski/CB/format.h
 *  \ingroup MATTYPES_CB
 *  \brief Cache blocking data structure.
 *
 *  For an overview, see \ref MATTYPE_CB.
 */

/** \ingroup MATTYPE_CB */
/*@{*/
#if !defined(INC_OSKI_CB_FORMAT_H)
/** oski/CB/format.h included */
#define INC_OSKI_CB_FORMAT_H

#include <oski/common.h>
#include <oski/matrix.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
/** CB matrix representation */
#define oski_matCB_t MANGLE_(oski_matCB_t)
/*@}*/
#endif

/**
 *  \brief Cache block.
 *
 *  This data structure defines a single cache block, which
 *  is an \f$R\times C\f$ submatrix beginning at position
 *  \f$(i_0, j_0)\f$ (0-based indices) of a larger matrix.
 */
typedef struct
{
  oski_index_t row;	   /**< Starting row offset, \f$i_0\f$. */
  oski_index_t col;	   /**< Starting column offset, \f$j_0\f$. */
  oski_index_t num_rows;       /**< Number of rows. */
  oski_index_t num_cols;       /**< Number of columns. */
  oski_index_t num_zero_rows;	    /**< Number of zero rows. */
  oski_index_t nnz;	  /**< Number of non-zeros. */
  oski_matrix_t mat;	    /**< Pointer to a matrix handle. */
} oski_submatCB_t;

/**
 *  \brief Cache block format.
 *
 *  This implementation of cache block format stores a matrix as
 *  simply a list of matrix handles.
 */
typedef struct
{
  simplelist_t *cache_blocks;	    /**< List of cache blocks */
} oski_matCB_t;

#endif
/*@}*/

/* eof */
