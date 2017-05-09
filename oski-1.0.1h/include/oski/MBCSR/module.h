/**
 *  \file oski/MBCSR/module.h
 *  \ingroup MATTYPE_MBCSR
 *  \brief Modified block compressed sparse row (MBCSR) implementation.
 */

#if !defined(INC_OSKI_MBCSR_MODULE_H)
/** oski/MBCSR/module.h included. */
#define INC_OSKI_MBCSR_MODULE_H

#include <oski/matmodexport.h>
#include <oski/MBCSR/format.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define MBCSR_GetSubmatKernel MANGLE_(MBCSR_GetSubmatKernel)
#define MBCSR_InitSubVecView MANGLE_(MBCSR_InitSubVecView)

#define MBCSR_SubmatReprMult_funcpt MANGLE_(MBCSR_SubmatReprMult_funcpt)

#define oski_EnableMBCSRMatMultAndMatMult  \
	MANGLE_(oski_EnableMBCSRMatMultAndMatMult)
#define oski_EnableMBCSRMatMultAndMatTransMult  \
	MANGLE_(oski_EnableMBCSRMatMultAndMatTransMult)
#define oski_EnableMBCSRMatTransMatMult  \
	MANGLE_(oski_EnableMBCSRMatTransMatMult)
#define oski_EnableMBCSRMatPowMult  \
	MANGLE_(oski_EnableMBCSRMatPowMult)
#define oski_EnableMBCSRMatTransPowMult  \
	MANGLE_(oski_EnableMBCSRMatTransPowMult)
/*@}*/
#endif

/** Function pointer to a core MBCSR matrix-vector multiply kernel */
typedef int (*MBCSR_SubmatReprMult_funcpt) (const oski_submatMBCSR_t * A,
					    oski_matop_t opA,
					    oski_value_t alpha,
					    const oski_vecview_t x_view,
					    oski_vecview_t y_view);

/** Function pointer to a core MBCSR A^T*A*x kernel */
typedef int (*MBCSR_SubmatReprTransSubmatReprMult_funcpt) (const
							   oski_submatMBCSR_t
							   * A,
							   oski_value_t alpha,
							   const
							   oski_vecview_t x,
							   oski_vecview_t y,
							   oski_vecview_t t);

/** Function pointer to a core MBCSR symmetric/Hermitian mat-vec */
typedef int (*MBCSR_SymmSubmatReprMult_funcpt) (const oski_submatMBCSR_t * A,
						int is_herm, oski_matop_t opA,
						oski_value_t alpha,
						const oski_vecview_t x_view,
						oski_vecview_t y_view);

/** Function pointer to a core MBCSR sparse triangular solve routine */
typedef int (*MBCSR_SubmatReprTrisolve_funcpt) (const oski_submatMBCSR_t * T,
						int is_lower,
						oski_matop_t opT,
						oski_value_t alpha,
						oski_vecview_t x);

/**
 *  Function pointer to a core MBCSR \f$A\f$ and \f$\mathrm{op}(A)\f$
 *  multiply routine.
 */
typedef int (*MBCSR_SubmatReprMultAndSubmatReprTransMult_funcpt) (const
								  oski_submatMBCSR_t
								  * A,
								  oski_value_t
								  alpha,
								  const
								  oski_vecview_t
								  x,
								  oski_vecview_t
								  y,
								  oski_matop_t
								  opA,
								  oski_value_t
								  omega,
								  const
								  oski_vecview_t
								  w,
								  oski_vecview_t
								  z);

/** Returns a pointer to the requested submodule method. */
void *MBCSR_GetSubmatKernel (const oski_submatMBCSR_t * A, const char *name);

/** Returns a subset of the rows of a vector view. */
void MBCSR_InitSubVecView (const oski_vecview_t x,
			   oski_index_t row_start, oski_index_t len,
			   oski_vecview_t x_sub);

/** \name Routines to enable/disable individual kernel implementations. */
/*@{*/
void oski_EnableMBCSRMatTransMatMult (oski_matMBCSR_t * A, int v);
void oski_EnableMBCSRMatMultAndMatMult (oski_matMBCSR_t * A, int v);
void oski_EnableMBCSRMatMultAndMatTransMult (oski_matMBCSR_t * A, int v);
void oski_EnableMBCSRMatPowMult (oski_matMBCSR_t * A, int v);
void oski_EnableMBCSRMatTransPowMult (oski_matMBCSR_t * A, int v);
/*@}*/

#endif

/* eof */
