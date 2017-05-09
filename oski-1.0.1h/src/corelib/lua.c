/**
 *  \file src/corelib/lua.c
 *  \brief OSKI-Lua stack support.
 */

#include <oski/lua.h>

oski_matlua_t *
oski_GetStackLuaMat (lua_State * L, int stack_pos)
{
  return (oski_matlua_t *) luaL_checkudata (L, stack_pos, OLUA_MATTAB);
}

/**
 *  \brief
 *
 *  Returns a pointer to the newly created object, or NULL on
 *  error.
 */
oski_matlua_t *
oski_PushEmptyLuaMat (lua_State * L)
{
  oski_matlua_t *A = OLUA_NEWUSERDATA (L, oski_matlua_t);

  if (A != NULL)
    {
      A->props = NULL;
      A->mat.type_id = INVALID_ID;
      A->mat.repr = NULL;

      /* Default type information */
      A->ind_type = OSKI_IND_ID;
      A->val_type = OSKI_VAL_ID;
      A->mattype_name = NULL;

      luaL_getmetatable (L, OLUA_MATTAB);
      lua_setmetatable (L, -2);	/* setmetatable(A, OLUA_MATTAB); */
    }

  return A;
}

/* eof */
