/**
 *  \defgroup MATTYPE_DENSE Dense (DENSE) Format
 *  \ingroup MATTYPES
 *
 *  Dense matrix in unpacked column major format.
 *
 *  For a detailed description of the data structure and its fields,
 *  see #oski_matDENSE_t.
 */

/**
 *  \file oski/DENSE/format.h
 *  \ingroup MATTYPE_DENSE
 *  \brief Generalized compressed sparse row data structure.
 *
 *  For an overview, see \ref MATTYPE_DENSE.
 */

/** \ingroup MATTYPE_DENSE */
/*@{*/

#if !defined(INC_OSKI_DENSE_FORMAT_H)
/** oski/DENSE/format.h included. */
#define INC_OSKI_DENSE_FORMAT_H

#include <oski/common.h>
#include <oski/matrix.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
/** DENSE matrix representation */
#define oski_matDENSE_t     MANGLE_(oski_matDENSE_t)
/*@}*/
#endif

/**
 *  \brief Dense column major format.
 */
typedef struct {
  oski_index_t lead_dim; /**< Leading dimension */
  oski_value_t* val; /**< Stores matrix elements in column major order. */
}
oski_matDENSE_t;

/*@}*/
#endif

/* eof */
