/**
 *  \defgroup MATTYPE_GCSR Generalized Compressed Sparse Row (GCSR) Format
 *  \ingroup MATTYPES
 *
 *  Generalized compressed sparse row (GCSR) format augments the
 *  traditional CSR with an optional list of row indices, allowing
 *  entire rows to be sparse. However, unlike the OSKI CSR
 *  implementation, only a subset of semantic features are
 *  supported:
 *    - Indices are 0-based.
 *    - Symmetric/Hermitian storage is not currently supported.
 *    - Indices are assumed to be unsorted.
 *
 *  A matrix with \f$m\f$ rows and \f$m_z\f$ structurally zero
 *  rows requires less total storage in GCSR than in CSR when
 *  \f$m_z > \frac{m}{2}\f$.
 *
 *  For a detailed description of the data structure and its fields,
 *  see #oski_matGCSR_t.
 */

/**
 *  \file oski/GCSR/format.h
 *  \ingroup MATTYPE_GCSR
 *  \brief Generalized compressed sparse row data structure.
 *
 *  For an overview, see \ref MATTYPE_GCSR.
 */

/** \ingroup MATTYPE_GCSR */
/*@{*/

#if !defined(INC_OSKI_GCSR_FORMAT_H)
/** oski/GCSR/format.h included. */
#define INC_OSKI_GCSR_FORMAT_H

#include <oski/common.h>
#include <oski/matrix.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
/** GCSR matrix representation */
#define oski_matGCSR_t     MANGLE_(oski_matGCSR_t)
/*@}*/
#endif

/**
 *  \brief Generalized compressed sparse row (GCSR) format.
 *
 *  This data structure is similar to CSR except that there
 *  is an additional array, rind, which stores row indices.
 */
typedef struct
{
  oski_index_t num_stored_rows;		/**< Number of stored rows */
  oski_index_t *ptr;	     /**< Row pointers. */
  oski_index_t *rind;	     /**< Row indices. */
  oski_index_t *cind;	     /**< Column indices. */
  oski_value_t *val;	     /**< Stored values. */
}
oski_matGCSR_t;

/*@}*/
#endif

/* eof */
