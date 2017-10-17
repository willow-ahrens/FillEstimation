/**
 *  \file oski/getset.h
 *  \brief Get/set value routines.
 *
 *  This module implements the routines that allow the user
 *  to modify the non-zero values of the matrix one at a time,
 *  a clique at a time, or a diagonal at a time.
 */

#if !defined(INC_OSKI_GETSET_H) && !defined(OSKI_UNBIND)
/** bebop/getset.h included. */
#define INC_OSKI_GETSET_H

#include <oski/matrix.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_GetMatEntry MANGLE_(oski_GetMatEntry)
#define oski_SetMatEntry MANGLE_(oski_SetMatEntry)
#define oski_GetMatClique MANGLE_(oski_GetMatClique)
#define oski_SetMatClique MANGLE_(oski_SetMatClique)
#define oski_GetMatDiagValues MANGLE_(oski_GetMatDiagValues)
#define oski_SetMatDiagValues MANGLE_(oski_SetMatDiagValues)

#define oski_GetMatReprEntry_funcpt \
		MANGLE_(oski_GetMatReprEntry_funcpt)
#define oski_SetMatReprEntry_funcpt \
		MANGLE_(oski_SetMatReprEntry_funcpt)
#define oski_GetMatReprClique_funcpt \
		MANGLE_(oski_GetMatReprClique_funcpt)
#define oski_SetMatReprClique_funcpt \
		MANGLE_(oski_SetMatReprClique_funcpt)
#define oski_GetMatReprDiagValues_funcpt \
		MANGLE_(oski_GetMatReprDiagValues_funcpt)
#define oski_SetMatReprDiagValues_funcpt \
		MANGLE_(oski_SetMatReprDiagValues_funcpt)

#define oski_GetMatReprEntry \
		MANGLE_MOD_(oski_GetMatReprEntry)
#define oski_SetMatReprEntry \
		MANGLE_MOD_(oski_SetMatReprEntry)
#define oski_GetMatReprClique \
		MANGLE_MOD_(oski_GetMatReprClique)
#define oski_SetMatReprClique \
		MANGLE_MOD_(oski_SetMatReprClique)
#define oski_GetMatReprDiagValues \
		MANGLE_MOD_(oski_GetMatReprDiagValues)
#define oski_SetMatReprDiagValues \
		MANGLE_MOD_(oski_SetMatReprDiagValues)
/*@}*/
#endif

/**
 *  \brief Returns the value of a matrix element.
 *  \ingroup PUBLIC
 */
oski_value_t oski_GetMatEntry (const oski_matrix_t A_tunable,
			       oski_index_t row, oski_index_t col);

/**
 *  \brief Changes the value of the specified matrix element.
 *  \ingroup PUBLIC
 */
int oski_SetMatEntry (oski_matrix_t A_tunable,
		      oski_index_t row, oski_index_t col,
		      oski_value_t new_val);

/**
 *  \brief Matrix type-specific implementation of oski_GetMatEntry().
 *  \ingroup MATTYPES_INTERFACE
 */
typedef int (*oski_GetMatReprEntry_funcpt) (const void *mat,
					    const oski_matcommon_t * props,
					    oski_index_t row,
					    oski_index_t col,
					    oski_value_t * p_value);

/**
 *  \brief Matrix type-specific implementation of oski_SetMatEntry().
 *  \ingroup MATTYPES_INTERFACE
 */
typedef int (*oski_SetMatReprEntry_funcpt) (void *mat,
					    const oski_matcommon_t * props,
					    oski_index_t row,
					    oski_index_t col,
					    oski_value_t new_val);

/**
 *  \brief Returns a block of values, defined by a clique, from a matrix.
 *  \ingroup PUBLIC
 */
int oski_GetMatClique (const oski_matrix_t A_tunable,
		       const oski_index_t * rows, oski_index_t num_rows,
		       const oski_index_t * cols, oski_index_t num_cols,
		       oski_vecview_t vals);

/**
 *  \brief Changes a block of values, defined by a clique, in a matrix.
 *  \ingroup PUBLIC
 */
int oski_SetMatClique (oski_matrix_t A_tunable,
		       const oski_index_t * rows, oski_index_t num_rows,
		       const oski_index_t * cols, oski_index_t num_cols,
		       const oski_vecview_t vals);

/**
 *  \brief Matrix type-specific implementation of oski_GetMatClique().
 *  \ingroup MATTYPES_INTERFACE
 */
typedef int (*oski_GetMatReprClique_funcpt) (const void *mat,
					     const oski_matcommon_t * props,
					     const oski_index_t * rows,
					     oski_index_t num_rows,
					     const oski_index_t * cols,
					     oski_index_t num_cols,
					     oski_vecview_t vals);

/**
 *  \brief Matrix type-specific implementation of oski_SetMatClique().
 *  \ingroup MATTYPES_INTERFACE
 */
typedef int (*oski_SetMatReprClique_funcpt) (void *mat,
					     const oski_matcommon_t * props,
					     const oski_index_t * rows,
					     oski_index_t num_rows,
					     const oski_index_t * cols,
					     oski_index_t num_cols,
					     const oski_vecview_t vals);

/**
 *  \brief Extract the diagonal \f$d\f$ from \f$A\f$, that is,
 *  all entries \f$A(i,j)\f$ such that \f$j-i = d\f$.
 *  \ingroup PUBLIC
 */
int oski_GetMatDiagValues (const oski_matrix_t A_tunable,
			   oski_index_t diag_num, oski_vecview_t vals);


/**
 *  \brief Sets the values along diagonal \f$d\f$ from \f$A\f$, that is,
 *  all entries \f$A(i,j)\f$ such that \f$j-i = d\f$.
 *  \ingroup PUBLIC
 */
int oski_SetMatDiagValues (oski_matrix_t A_tunable,
			   oski_index_t diag_num, const oski_vecview_t vals);

/**
 *  \brief Matrix type-specific implementation of oski_GetMatDiagValues().
 *  \ingroup MATTYPES_INTERFACE
 */
typedef int (*oski_GetMatReprDiagValues_funcpt) (const void *mat,
						 const oski_matcommon_t *
						 props, oski_index_t diag_num,
						 oski_vecview_t vals);

/**
 *  \brief Matrix type-specific implementation of oski_SetMatDiagValues().
 *  \ingroup MATTYPES_INTERFACE
 */
typedef int (*oski_SetMatReprDiagValues_funcpt) (void *mat,
						 const oski_matcommon_t *
						 props, oski_index_t diag_num,
						 const oski_vecview_t vals);

#endif /* !defined(INC_OSKI_GETSET_H) */

#if defined(OSKI_UNBIND)
#  include <oski/matrix.h>
#  undef INC_OSKI_GETSET_H
#  undef oski_GetMatEntry
#  undef oski_SetMatEntry
#  undef oski_GetMatClique
#  undef oski_SetMatClique
#  undef oski_GetMatDiagValues
#  undef oski_SetMatDiagValues
#  undef oski_GetMatReprEntry_funcpt
#  undef oski_SetMatReprEntry_funcpt
#  undef oski_GetMatReprClique_funcpt
#  undef oski_SetMatReprClique_funcpt
#  undef oski_GetMatReprDiagValues_funcpt
#  undef oski_SetMatReprDiagValues_funcpt
#  undef oski_GetMatReprEntry
#  undef oski_SetMatReprEntry
#  undef oski_GetMatReprClique
#  undef oski_SetMatReprClique
#  undef oski_GetMatReprDiagValues
#  undef oski_SetMatReprDiagValues
#endif

/* eof */
