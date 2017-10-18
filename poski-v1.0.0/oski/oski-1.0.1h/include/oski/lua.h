/**
 *  \file oski/lua.h
 *  \brief Basic OSKI-Lua support routines.
 */

#if !defined(INC_OSKI_LUA_H)
/** oski/lua.h included. */
#define INC_OSKI_LUA_H

#include <oski/common.h>
#include <oski/mangle.h>
#include <oski/matcommon.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_matlua_t MANGLE_(oski_matlua_t)
#define oski_PushEmptyLuaMat  MANGLE_(oski_PushEmptyLuaMat)
#define oski_GetStackLuaMat MANGLE_(oski_GetStackLuaMat)
/*@}*/
#endif

/**
 *  \brief OSKI-Lua userdata structure, wrapper around
 *  a generic matrix type.
 *
 *  Invariant: Let A be an instance of oski_matlua_t. If A.mat != NULL,
 *  then the information in A.{mattype_name, ind_type, val_type} should
 *  be consistent with it. Moreover, if A.mat == NULL, then A.mat may
 *  only be replaced by an instance of a matrix representation
 *  consistent with the type data, A.{mattype_name, ind_type, val_type}.
 */
typedef struct
{
  const oski_matcommon_t *props;  /**< Type-independent properties */
  oski_matspecific_t mat;  /**< Type-dependent properties */

  char *mattype_name; /**< Type of this instance */
  oski_id_t ind_type;	/**< Scalar integer index type */
  oski_id_t val_type;	/**< Scalar non-zero value type */
}
oski_matlua_t;

/** \brief Macro wrapper around lua_newuserdata(). */
#define OLUA_NEWUSERDATA(L, typ) (typ *)lua_newuserdata(L, sizeof(typ))

/** \brief Name of OSKI-Lua matrix type metatable. */
#define OLUA_MATTAB   "OskiLua.Mat"

/** \brief Name of OSKI-Lua matrix type. */
#define OLUA_MAT  "Mat"

/** \brief Name of OSKI-Lua global variable to store input matrix */
#define OLUA_INMAT "InputMat"

/* ----------- Lua-specific ---------- */
#include <lua/include/lua.h>
#include <lua/include/lualib.h>
#include <lua/include/lauxlib.h>

/**
 *  \brief Executes a OSKI-Lua call to OLUA_MAT.new(), and leaves
 *  the result on the stack.
 */
oski_matlua_t *oski_PushEmptyLuaMat (lua_State * L);

/**
 *  \brief Examines the item at the specified position on the stack,
 *  and returns the object there if it is a OLUA_MAT object, or
 *  returns NULL otherwise.
 */
oski_matlua_t *oski_GetStackLuaMat (lua_State * L, int stack_pos);

#endif

/* eof */
