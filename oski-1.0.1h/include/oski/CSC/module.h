/**
 *  \file oski/CSC/module.h
 *  \ingroup MATTYPE_CSC
 *  \brief Compressed sparse column implementation.
 */

#if !defined(INC_OSKI_CSC_MODULE_H)
/** oski/CSC/module.h included. */
#define INC_OSKI_CSC_MODULE_H

#include <oski/matmodexport.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_WrapCSC_funcpt  MANGLE_(oski_WrapCSC_funcpt)
#define oski_WrapCSC  MANGLE_MOD_(oski_WrapCSC)
/*@}*/
#endif

#include <oski/CSC/format.h>
#include <oski/inmatprop.h>

/**
 *  \brief Create a new CSC-specific wrapper around a raw
 *  CSC array representation.
 */
oski_matCSC_t *oski_WrapCSC (oski_matcommon_t * out_props,
			     oski_index_t * Aptr, oski_index_t * Aind,
			     oski_value_t * Aval, oski_index_t num_rows,
			     oski_index_t num_cols,
			     oski_inmatpropset_t * props,
			     oski_copymode_t mode);

/**
 *  \brief Function pointer type for oski_WrapCSC().
 */
typedef oski_matCSC_t *(*oski_WrapCSC_funcpt) (oski_matcommon_t * out_props,
					       oski_index_t * Aptr,
					       oski_index_t * Aind,
					       oski_value_t * Aval,
					       oski_index_t num_rows,
					       oski_index_t num_cols,
					       oski_inmatpropset_t * props,
					       oski_copymode_t mode);

#endif

/* eof */
