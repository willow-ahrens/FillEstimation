/**
 *  \file oski/xforms_internal.h
 *  \brief This header file isolates the Lua-dependent aspects of
 *  the tuning transform implementation from the core OSKI code.
 *
 *  "Lua-dependent" includes dependencies on Lua-specific data
 *  structures (e.g., lua_State), for instance.
 */

#if !defined(INC_OSKI_XFORMS_INTERNAL_H)
/** \brief oski/xforms_internal.h included. */
#define INC_OSKI_XFORMS_INTERNAL_H

#include <oski/matrix.h>
#include <oski/xforms.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_CreateLuaMatReprGenericFromCSR \
	MANGLE_(oski_CreateLuaMatReprGenericFromCSR)
#define oski_CreateLuaMatReprGeneric2IndexFromCSR \
	MANGLE_(oski_CreateLuaMatReprGeneric2IndexFromCSR)
#define oski_CreateLuaMatReprFromCSR \
	MANGLE_MOD_(oski_CreateLuaMatReprFromCSR)
/*@}*/
#endif

/**
 *  \brief Routine to create a Lua matrix representation from a CSR
 *  representation for a simple matrix type.
 */
int oski_CreateLuaMatReprGenericFromCSR (lua_State * L, const char *mattype);

/**
 *  \brief Routine to create a Lua matrix representation from a CSR
 *  representation for a matrix type that, on creation, expects two
 *  integer index arguments.
 */
int oski_CreateLuaMatReprGeneric2IndexFromCSR (lua_State * L,
					       const char *mattype);

/**
 *  \brief Matrix-type specific method to convert from a CSR matrix,
 *  with arguments passed on the Lua stack.
 */
int oski_CreateLuaMatReprFromCSR (lua_State * L);

#endif

/* eof */
