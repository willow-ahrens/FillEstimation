/**
 *  \file oski/xforms.h
 *  \brief OSKI-Lua transformation module.
 */

#if !defined(INC_OSKI_XFORMS_H)
/** oski/xforms.h included. */
#define INC_OSKI_XFORMS_H

#include <oski/matrix.h>
#include <oski/lua.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_ReplaceTunedMatRepr MANGLE_(oski_ReplaceTunedMatRepr)
#define oski_FreeInputMatRepr  MANGLE_(oski_FreeInputMatRepr)
/*@}*/
#endif

/**
 *  \brief Replace a matrix's tuned representation with a new one.
 */
int oski_ReplaceTunedMatRepr (oski_matrix_t A,
			      oski_matspecific_t * new_mat,
			      const char *xforms);

/**
 *  \brief If the given matrix has a valid tuned representation and
 *  does not share its input representation, then this routine
 *  destroys the input matrix representation.
 */
int oski_FreeInputMatRepr (oski_matrix_t A);

#endif

/* eof */
