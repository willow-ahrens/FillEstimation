/**
 *  \file oski/matmodexport.h
 *  \ingroup MATTYPES_INTERFACE
 *  \brief Declares prototypes for the 'standard' set of dynamically
 *  exportable methods stored in matrix type modules.
 *
 *  All matrix types should include this file, which declares
 *  mangled module-specific prototypes for the common
 *  methods (matrix creation, type-specific kernels, etc.).
 *  In other words, think of this file as declaring the
 *  interface to a matrix type module.
 */

#if !defined(INC_OSKI_MATMODEXPORT_H)
/** oski/matmodexport.h included. */
#define INC_OSKI_MATMODEXPORT_H

#include <oski/modcommon.h>
#include <oski/matcommon.h>
#include <oski/vecview.h>
#include <oski/CSR/format.h>

#if defined(DO_NAME_MANGLING)
/** Name mangling. */
/*@{*/
#define oski_DestroyMatRepr  MANGLE_MOD_(oski_DestroyMatRepr)
#define oski_ConvertMatReprToCSR    MANGLE_MOD_(oski_ConvertMatReprToCSR)
#define oski_CreateMatReprFromCSR   MANGLE_MOD_(oski_CreateMatReprFromCSR)
#define oski_CopyMatRepr     MANGLE_MOD_(oski_CopyMatRepr)
#define oski_CalcMatRepr1Norm  MANGLE_MOD_(oski_CalcMatRepr1Norm)

#define oski_MatReprMult \
	MANGLE_MOD_(oski_MatReprMult)
#define oski_MatReprTrisolve \
	MANGLE_MOD_(oski_MatReprTrisolve)
#define oski_MatReprMultAndMatReprTransMult \
	MANGLE_MOD_(oski_MatReprMultAndMatReprTransMult)
#define oski_MatReprTransMatReprMult \
	MANGLE_MOD_(oski_MatReprTransMatReprMult)
#define oski_MatReprPowMult \
	MANGLE_MOD_(oski_MatReprPowMult)
/*@}*/
#endif

/**
 *  \brief Method: Destroy matrix type-specific representation.
 *  \ingroup MATTYPES_INTERFACE
 *
 *  Frees all the memory associated with a given matrix
 *  representation.
 *
 *  \param[in,out] mat Matrix representation object to destroy.
 *
 *  \see #oski_DestroyMat, #oski_DestroyMatRepr_funcpt
 */
void oski_DestroyMatRepr (void *mat);

/**
 *  \brief Method: Instantiate from an existing CSR representation.
 *  \ingroup MATTYPES_INTERFACE
 *
 *  \param[in] mat CSR representation.
 *  \param[in] props Additional global properties of the matrix, mat.
 *  \returns A pointer to a matrix type-specific representation in
 *  the current module's format.
 *
 *  The variable argument list specifies a matrix type-dependent
 *  sequence of arguments, allowing the final data structure to
 *  be customized accordingly.
 *
 *  \pre props != NULL
 *  \see #oski_CreateMatCSR, #oski_CreateMatCSC,
 *  #oski_ConvertMatReprFromCSR_funcpt
 */
void *oski_CreateMatReprFromCSR (const oski_matCSR_t * mat,
				 const oski_matcommon_t * props, ...);

/**
 *  \brief Method: Convert to CSR format.
 *  \ingroup MATTYPES_INTERFACE
 *
 *  \param[in] mat Matrix type-specific representation in the current
 *  module's format.
 *  \param[in] props Additional global properties of the matrix, mat.
 *  \returns A CSR representation.
 *
 *  \pre props != NULL
 *  \see #oski_CreateMatCSR, #oski_CreateMatCSC,
 *  #oski_ConvertMatReprToCSR_funcpt
 */
oski_matCSR_t *oski_ConvertMatReprToCSR (const void *mat,
					 const oski_matcommon_t * props);

/**
 *  \brief Method: Duplicate a matrix representation.
 *  \ingroup MATTYPES_INTERFACE
 *
 *  \param[in] mat Matrix type-specific representation in the current
 *  module's format.
 *  \param[in] props Additional global properties of the matrix, mat.
 *  \returns A copy of 'mat', or NULL on error.
 *
 *  \pre props != NULL
 *  \see #oski_CopyMat, #oski_CopyMatRepr_funcpt
 */
void *oski_CopyMatRepr (const void *mat, const oski_matcommon_t * props);

/**
 *  \brief Method: Compute the 1-norm.
 *
 *  \param[in] mat Matrix type-specific representation.
 *  \param[in] props Additional global properties of mat.
 *  \returns The 1-norm, or -1 on error.
 *
 *  \see #oski_CalcMat1Norm, #oski_CalcMatRepr1Norm_funcpt
 */
double oski_CalcMatRepr1Norm (const void *mat,
			      const oski_matcommon_t * props);

/**
 *  \brief Matrix type-specific implementation of
 *  sparse matrix-vector multiply.
 *  \ingroup MATTYPES_INTERFACE
 *
 *  \see #oski_MatMult, #oski_MatReprMult_funcpt
 */
int oski_MatReprMult (const void *A,
		      const oski_matcommon_t * props, oski_matop_t opA,
		      oski_value_t alpha, const oski_vecview_t x_view,
		      oski_value_t beta, oski_vecview_t y_view);

/**
 *  \brief Matrix type specific implementation of oski_MatTrisolve().
 *
 *  \ingroup MATTYPES_INTERFACE
 *  \see #oski_MatTrisolve, #oski_MatReprTrisolve_funcpt
 */
int oski_MatReprTrisolve (const void *T,
			  const oski_matcommon_t * props, oski_matop_t opT,
			  oski_value_t alpha, oski_vecview_t x_view);

/**
 *  \brief Matrix type specific implementation of oski_MatTransMatMult().
 *  \ingroup MATTYPES_INTERFACE
 *
 *  \see #oski_MatTransMatMult, #oski_MatReprTransMatReprMult_funcpt
 */
int oski_MatReprTransMatReprMult (const void *A,
				  const oski_matcommon_t * props,
				  oski_ataop_t op, oski_value_t alpha,
				  const oski_vecview_t x_view,
				  oski_value_t beta, oski_vecview_t y_view,
				  oski_vecview_t t_view);

/**
 *  \brief Matrix type specific implementation of
 *  oski_MatMultAndMatTransMult().
 *  \ingroup MATTYPES_INTERFACE
 *
 *  \see #oski_MatMultAndMatTransMult
 *  \see #oski_MatReprMultAndMatReprTransMult_funcpt
 */
int oski_MatReprMultAndMatReprTransMult (const void *A,
					 const oski_matcommon_t * props,
					 oski_value_t alpha,
					 const oski_vecview_t x_view,
					 oski_value_t beta,
					 oski_vecview_t y_view,
					 oski_matop_t opA, oski_value_t omega,
					 const oski_vecview_t w_view,
					 oski_value_t zeta,
					 oski_vecview_t z_view);

/**
 *  \brief Matrix type specific implementation of oski_MatPowMult().
 *  \ingroup MATTYPES_INTERFACE
 *
 *  \see #oski_MatPowMult, #oski_MatReprPowMult_funcpt
 */
int oski_MatReprPowMult (const void *A, const oski_matcommon_t * props,
			 oski_matop_t opA, int power,
			 oski_value_t alpha, const oski_vecview_t x_view,
			 oski_value_t beta, oski_vecview_t y_view,
			 oski_vecview_t T_view);

#endif

/* eof */
