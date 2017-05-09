/**
 *  \file oski/BCSR/module.h
 *  \ingroup MATTYPE_BCSR
 *  \brief Block compressed sparse row (BCSR) implementation.
 */

#if !defined(INC_OSKI_BCSR_MODULE_H)
/** BCSR/module.h included. */
#define INC_OSKI_BCSR_MODULE_H

#include <oski/matmodexport.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_BCSR_MatMult_funcpt \
	MANGLE_(oski_BCSR_MatMult_funcpt)
#define oski_BCSR_MatTransMatMult_funcpt \
	MANGLE_(oski_BCSR_MatTransMatMult_funcpt)
#define BCSR_GetKernel \
	MANGLE_(BCSR_GetKernel)
/*@}*/
#endif

/** Pointer to a BCSR SpMV kernel implementation. */
typedef int (*oski_BCSR_MatMult_funcpt) (const oski_matBCSR_t * A,
					 oski_matop_t opA,
					 oski_value_t alpha,
					 const oski_vecview_t x_view,
					 oski_vecview_t y_view);

/** Pointer to a BCSR sparse \f$A^TAx\f$ implementation. */
typedef int (*oski_BCSR_MatTransMatMult_funcpt) (const oski_matBCSR_t * A,
						 oski_value_t alpha,
						 const oski_vecview_t x_view,
						 oski_vecview_t y_view,
						 oski_vecview_t t_view);

/** Returns a pointer to the requested submodule method. */
void *BCSR_GetKernel (const oski_matBCSR_t * A, const char *name);

#endif

/* eof */
