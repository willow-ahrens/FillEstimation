/**
 *  \defgroup MATTYPE_CSC Compressed Sparse Column (CSC) Format
 *  \ingroup MATTYPES
 *
 *  The CSC implementation reuses most of the CSR (\ref MATTYPE_CSR)
 *  infrastructure, since the arrays representing a matrix \f$A\f$
 *  in CSC format are the same as the arrays that would be used to
 *  store \f$A^T\f$ in CSR format. Therefore, the #oski_matCSC_t
 *  data structure for storing CSC matrices is simply a wrapper
 *  around #oski_matCSR_t.
 */

/**
 *  \file oski/CSC/format.h
 *  \ingroup MATTYPE_CSC
 *  \brief Compressed sparse column data structure.
 *
 *  For an overview, see \ref MATTYPE_CSC.
 */

/** \ingroup MATTYPE_CSC */
/*@{*/
#if !defined(INC_OSKI_CSC_FORMAT_H)
/** oski/CSC/format.h included. */
#define INC_OSKI_CSC_FORMAT_H

#include <oski/CSR/format.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
/** CSC matrix representation */
#define oski_matCSC_t  MANGLE_(oski_matCSC_t)
/*@}*/
#endif

/**
 *  \brief Compressed sparse column (CSC) format.
 *
 *  An \f$m\times n\f$ matrix \f$A\f$ is stored in CSC format
 *  by equivalently storing the \f$n\times m\f$ matrix
 *  \f$A^T\f$ in CSR format.
 */
typedef struct
{
  oski_matCSR_t *mat_trans;	   /**< Stores \f$A^T\f$. */
}
oski_matCSC_t;

#endif
/*@}*/
/* eof */
