/**
 *  \file oski/matcreate.h
 *  \brief Matrix creation routines.
 */

#if !defined(INC_OSKI_MATCREATE_H) && !defined(OSKI_UNBIND)
/** oski/matcreate.h has been included. */
#define INC_OSKI_MATCREATE_H

#include <oski/matrix.h>
#include <oski/inmatprop.h>

#if defined(DO_NAME_MANGLING)
/**
 *  \name Names, defined in this module, to be mangled.
 */
/*@{*/
#define oski_CreateMatCSR   MANGLE_(oski_CreateMatCSR)
#define oski_CreateMatCSC   MANGLE_(oski_CreateMatCSC)
#define oski_CreateMatCSR_va   MANGLE_(oski_CreateMatCSR_va)
#define oski_CreateMatCSC_va   MANGLE_(oski_CreateMatCSC_va)
#define oski_CreateMatCSR_arr   MANGLE_(oski_CreateMatCSR_arr)
#define oski_CreateMatCSC_arr   MANGLE_(oski_CreateMatCSC_arr)
#define oski_CopyMat        MANGLE_(oski_CopyMat)
#define oski_DestroyMat     MANGLE_(oski_DestroyMat)

#define oski_CalcMat1Norm   MANGLE_(oski_CalcMat1Norm)
#define oski_GetMatProps    MANGLE_(oski_GetMatProps)
/*@}*/
#endif

/**
 *  \brief Creates and returns a valid tunable matrix object from a
 *  compressed sparse row (CSR) representation.
 *
 *  \ingroup PUBLIC
 */
oski_matrix_t oski_CreateMatCSR (oski_index_t * Aptr, oski_index_t * Aind,
				 oski_value_t * Aval, oski_index_t num_rows,
				 oski_index_t num_cols, oski_copymode_t mode,
				 int k, ...);

/**
 *  \brief Explicit variable-argument version of oski_CreateMatCSR().
 */
oski_matrix_t oski_CreateMatCSR_va (oski_index_t* Aptr, oski_index_t* Aind,
                                    oski_value_t* Aval,
                                    oski_index_t num_rows,
                                    oski_index_t num_cols,
                                    oski_copymode_t mode,
                                    int k, va_list ap);

/**
 *  \brief Explicit property-array version of oski_CreateMatCSR().
 */
oski_matrix_t oski_CreateMatCSR_arr (oski_index_t* Aptr, oski_index_t* Aind,
				     oski_value_t* Aval,
				     oski_index_t num_rows,
				     oski_index_t num_cols,
				     oski_copymode_t mode,
				     int k, const oski_inmatprop_t* in_props);

/**
 *  \brief Creates and returns a valid tunable matrix object from a
 *  compressed sparse column (CSC) representation.
 *
 *  \ingroup PUBLIC
 */
oski_matrix_t oski_CreateMatCSC (oski_index_t * Aptr, oski_index_t * Aind,
				 oski_value_t * Aval, oski_index_t num_rows,
				 oski_index_t num_cols, oski_copymode_t mode,
				 int k, ...);

/**
 *  \brief Variable-argument version of oski_CreateMatCSC.
 */
oski_matrix_t oski_CreateMatCSC_va (oski_index_t * Aptr, oski_index_t * Aind,
                                    oski_value_t * Aval, oski_index_t num_rows,
                                    oski_index_t num_cols, oski_copymode_t mode,
                                    int k, va_list ap);

/**
 *  \brief Explicit property-array version of oski_CreateMatCSC.
 */
oski_matrix_t oski_CreateMatCSC_arr (oski_index_t * Aptr, oski_index_t * Aind,
				     oski_value_t * Aval, oski_index_t num_rows,
				     oski_index_t num_cols, oski_copymode_t mode,
				     int k, const oski_inmatprop_t* in_props);

/**
 *  \brief Creates a copy of a matrix object.
 *
 *  \ingroup PUBLIC
 */
oski_matrix_t oski_CopyMat (const oski_matrix_t A_tunable);

/**
 *  \brief Frees object memory associated with a given matrix object.
 *
 *  \ingroup PUBLIC
 */
int oski_DestroyMat (oski_matrix_t A_tunable);

/**
 *  \brief Computes the 1-norm (maximum absolute row sum) of the
 *  specified matrix.
 */
double oski_CalcMat1Norm (const oski_matrix_t A_tunable);

/**
 *  \brief Returns a matrix's type-independent properties.
 */
const oski_matcommon_t *oski_GetMatProps (const oski_matrix_t A);

#endif /* !defined(INC_OSKI_MATCREATE_H) */

#if defined(OSKI_UNBIND)
#  include <oski/matrix.h>
#  undef INC_OSKI_MATCREATE_H
#  undef oski_CreateMatCSR
#  undef oski_CreateMatCSC
#  undef oski_CopyMat
#  undef oski_DestroyMat
#  undef oski_CalcMat1Norm
#  undef oski_GetMatProps
#endif

/* eof */
