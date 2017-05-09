/**
 *  \file oski/GCSR/module.h
 *  \ingroup MATTYPE_GCSR
 *  \brief Compressed sparse column implementation.
 */

#if !defined(INC_OSKI_GCSR_MODULE_H)
/** oski/GCSR/module.h included. */
#define INC_OSKI_GCSR_MODULE_H

#include <oski/matmodexport.h>
#include <oski/GCSR/format.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define GCSR_MatReprMult_1x1  MANGLE_MOD_(MatReprMult_1x1)
#define oski_FindRowGCSR  MANGLE_(oski_FindRowGCSR)
/*@}*/
#endif

/**
 *  \brief Returns the index in 'rind' of a given row (0-based index),
 *  or an integer less than 0 on error.
 */
oski_index_t oski_FindRowGCSR (const oski_matGCSR_t * A, oski_index_t i0);

/**
 *  \brief Sparse matrix-vector multiply for GCSR format.
 */
int GCSR_MatReprMult_1x1 (const oski_matGCSR_t * A, oski_matop_t opA,
			  oski_value_t alpha, const oski_vecview_t x,
			  oski_vecview_t y);

#endif

/* eof */
