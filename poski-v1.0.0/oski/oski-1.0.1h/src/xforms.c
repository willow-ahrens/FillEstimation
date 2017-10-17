/**
 *  \file src/xforms.c
 *  \brief Save/restore tuning transformations.
 *  \ingroup TUNING
 */

#include <string.h>

#include <oski/common.h>
#include <oski/mangle.h>
#include <oski/mattypes_internal.h>
#include <oski/tune.h>
#include <oski/xforms.h>
#include <oski/xforms_internal.h>

/**
 *  \brief
 *
 *  \param[in] A_tunable Valid matrix handle.
 *  \returns A newly allocated string representing the
 *  transformation/data structure that has been applied to \f$A\f$.
 *  \note The caller must free the returned string.
 */
char *
oski_GetMatTransforms (const oski_matrix_t A_tunable)
{
  char *xforms;

  if (A_tunable == INVALID_MAT)
    {
      OSKI_ERR_BAD_MAT (oski_GetMatTransforms, 1);
      return NULL;
    }

  if (A_tunable->tuned_xforms != NULL)
    xforms = oski_DuplicateString (A_tunable->tuned_xforms);
  else				/* No transformations */
    xforms = oski_DuplicateString ("return InputMat;");

  /* xforms == NULL only if an error occurred. */
  if (xforms == NULL)
    OSKI_ERR (oski_GetMatTransforms, ERR_OUT_OF_MEMORY);

  return xforms;
}

/* ----------------------------------------------------------------- */

/**
 *  \brief Print the type and, if possible, value of the element
 *  at the given position in the Lua stack.
 */
static void
DumpLuaArg (lua_State * L, int arg)
{
  int t;
  int n_args = lua_gettop (L);

  t = lua_type (L, arg);
  switch (t)
    {
    case LUA_TSTRING:
      oski_PrintDebugMessage (3, "[OLUA] (%d/%d) String: '%s'",
			      arg, n_args, lua_tostring (L, arg));
      break;

    case LUA_TBOOLEAN:
      oski_PrintDebugMessage (3, "[OLUA] (%d/%d) Boolean: %s",
			      arg, n_args, lua_toboolean (L,
							  arg) ? "true" :
			      "false");
      break;

    case LUA_TNUMBER:
      oski_PrintDebugMessage (3, "[OLUA] (%d/%d) Number: %g",
			      arg, n_args, lua_tonumber (L, arg));
      break;

    default:
      oski_PrintDebugMessage (3, "[OLUA] (%d/%d) %s",
			      arg, n_args, lua_typename (L, t));
      break;
    }
}

/** \brief Implements a call to Lua's "require()" function. */
static int
RequireLua (lua_State * L, const char *modname)
{
  lua_getglobal (L, "require");
  lua_pushstring (L, modname);

  if (lua_pcall (L, 1, 0, 0) != 0)
    {
      const char *errmsg = lua_tostring (L, -1);

      oski_HandleError (ERR_OLUA_STARTUP,
			"Unable to load a required OSKI-Lua module",
			__FILE__, __LINE__,
			"Could not load interpreter module, '%s': '%s'."
			" (Check environment variable, OSKI_LUA_PATH.)",
			modname, errmsg);
      return ERR_OLUA_STARTUP;
    }

  return 0;
}

/**
 *  \brief Traverse the list of available matrix types, and create
 *  globally visible Lua matrix types in the given Lua state.
 *
 *  \param[in,out] L Lua transformation engine state.
 */
static void
RegisterLuaMatTypes (lua_State * L)
{
  size_t len = oski_GetNumMatTypes ();
  oski_id_t id;

  for (id = 1; id <= len; id++)
    {
      const oski_mattype_t *rec = oski_FindMatTypeRecord (id);
      if (rec == NULL)
	continue;

      oski_PrintDebugMessage (3, "[OLUA] Registering OSKI-Lua type...");
      oski_PrintDebugMessage (4, "Type: %s", rec->desc);
      oski_PrintDebugMessage (4, "Module: %s", rec->modname);

      RequireLua (L, rec->name);
    }
}

/**
 *  \brief Leaves a newly instantiated OLUA_MAT object on the top
 *  of L's stack.
 *
 *  This routine implements 'Mat.new( [base_type_name] )'.
 */
static int
NewLuaMat (lua_State * L)
{
  oski_matlua_t *A = oski_PushEmptyLuaMat (L);
  int n_args;

  if (A == NULL)
    return 0;

  n_args = lua_gettop (L);
  if (n_args >= 2)
    {
      A->mattype_name = oski_DuplicateString (lua_tostring (L, 1));
    }
  else
    A->mattype_name = NULL;

  luaL_getmetatable (L, OLUA_MATTAB);
  lua_setmetatable (L, -2);	/* setmetatable(A, OLUA_MATTAB); */
  return 1;
}

/**
 *  \brief Examines the first item (position 1) on the stack
 *  and returns it if it is a valid OLUA_MATTAB object.
 *  Returns NULL otherwise.
 */
static oski_matlua_t *
CheckLuaMat (lua_State * L)
{
  void *ud = (void *) oski_GetStackLuaMat (L, 1);
  luaL_argcheck (L, ud != NULL, 1, "'" OLUA_MATTAB "' expected");
  return (oski_matlua_t *) ud;
}

/**
 *  \brief A.__tostring(): Returns a string representation of the
 *  specified LuaMat object.
 */
static int
ToStringLuaMat (lua_State * L)
{
  const oski_matlua_t *A = CheckLuaMat (L);

  if (A != NULL)
    {
      if (A->mat.type_id != INVALID_ID)
	{
	  const oski_mattype_t *info =
	    oski_FindMatTypeRecord (A->mat.type_id);

	  if (info != NULL)
	    {
	      lua_pushfstring (L,
			       "%s(%dx%d, %d nz, type=%s", OLUA_MAT,
			       A->props->num_rows, A->props->num_cols,
			       A->props->num_nonzeros, info->desc);
	      return 1;
	    }
	}
      else if (A->mattype_name != NULL)
	{
	  lua_pushfstring (L, "%s<i=%d, v=%d>(nil)",
			   A->mattype_name, A->ind_type, A->val_type);
	  return 1;
	}
    }

  lua_pushfstring (L, "(Invalid %s)", OLUA_MAT);
  return 1;
}

/**
 *  \brief A.__gc(x): Called when the system garbage collects x.
 */
static int
GCLuaMat (lua_State * L)
{
  oski_matlua_t *A = CheckLuaMat (L);
  if (A != NULL && A->mattype_name != NULL)
    oski_Free (A->mattype_name);
  return 0;
}

/**
 *  \brief Returns a newly created string method name for a
 *  Lua-callback matrix type conversion routine.
 */
static char *
MakeLuaConvMethodName (const oski_matlua_t * A_source,
		       const oski_matlua_t * A_dest)
{
  char *name;
  size_t len;

  if (A_source == NULL || A_dest == NULL)
    return NULL;

  if (A_dest->ind_type != A_source->ind_type
      || A_dest->val_type != A_source->val_type)
    {
      oski_HandleError (ERR_OLUA_TYPE_MISMATCH,
			"Matrix type mismatch", __FILE__, __LINE__,
			"Can't convert from %s<ind=%s, val=%s> to %s<ind=%s, val=%s>",
			A_source->mattype_name,
			oski_GetScalarIndexName (A_source->ind_type),
			oski_GetScalarValueName (A_source->val_type),
			A_dest->mattype_name,
			oski_GetScalarIndexName (A_dest->ind_type),
			oski_GetScalarValueName (A_dest->val_type));
      return NULL;
    }

  len = strlen ("oski_CreateLuaMatReprFrom")
    + strlen (A_source->mattype_name);
  name = oski_Malloc (char, len + 1);
  if (name == NULL)
    {
      OSKI_ERR (MakeLuaConvMethodName, ERR_OUT_OF_MEMORY);
      return NULL;
    }

  sprintf (name, "oski_CreateLuaMatReprFrom%s", A_source->mattype_name);
  name[len] = 0;
  return name;
}

/**
 *  \brief Converts the Lua matrix at position 'index' on the
 *  stack to CSR format, and leaves the new matrix at the top
 *  of the stack. Returns a pointer to the matrix object.
 *
 *  If an error occurs, this routine leaves the stack unchanged
 *  and returns NULL.
 */
static oski_matlua_t *
ConvertLuaMatToCSR (lua_State * L, int index)
{
  const oski_matlua_t *A_source = oski_GetStackLuaMat (L, index);
  oski_matCSR_t *mat_csr;
  oski_matlua_t *A_csr;

  oski_ConvertMatReprToCSR_funcpt func_Convert;

  if (A_source == NULL || A_source->mat.type_id == INVALID_ID)
    return NULL;

  func_Convert = (oski_ConvertMatReprToCSR_funcpt)
    oski_LookupMatTypeMethod (A_source->mattype_name,
			      A_source->ind_type, A_source->val_type,
			      "oski_ConvertMatReprToCSR");
  if (func_Convert == NULL)
    {
      oski_HandleError (ERR_NOT_IMPLEMENTED,
			"Conversion-to-CSR routine not implemented.",
			__FILE__, __LINE__,
			"%s matrix type does not implement a conversion to CSR routine.",
			A_source->mattype_name);
      return NULL;
    }

  mat_csr = (*func_Convert) (A_source->mat.repr, A_source->props);
  if (mat_csr == NULL)
    return NULL;

  /* Create new matrix object on stack */
  A_csr = oski_PushEmptyLuaMat (L);
  if (A_csr != NULL)
    {
      A_csr->props = A_source->props;
      A_csr->mattype_name = oski_DuplicateString ("CSR");
      A_csr->ind_type = A_source->ind_type;
      A_csr->val_type = A_source->val_type;
      A_csr->mat.repr = mat_csr;
      A_csr->mat.type_id = oski_LookupMatTypeId ("CSR",
						 A_source->ind_type,
						 A_source->val_type);
      return A_csr;
    }
  else
    {
      lua_pop (L, 1);		/* error occurred; get rid of A_csr */
      return NULL;
    }
}

/**
 *  \brief Returns 1 if the input OSKI-Lua matrices have the same
 *  type (e.g., are both BCSR(int, double)).
 */
static int
AreSameLuaMatTypes (const oski_matlua_t * A, const oski_matlua_t * B)
{
  return strcmp (B->mattype_name, A->mattype_name) == 0
    && B->ind_type == A->ind_type && B->val_type == A->val_type;
}

/**
 *  \brief Mat.__call( A, ... ) --> A( ... ): Implements data structure
 *  conversion.
 *
 *  Assumed contents of the Lua stack:
 *    - 1       Destination matrix type
 *    - 2       Source matrix, in a compatible CSR format
 *    - 3, ...  Any destination type-specific arguments.
 */
static int
ConvertLuaMat (lua_State * L)
{
  oski_matlua_t *A_desttype = CheckLuaMat (L);
  const oski_matlua_t *A_source = oski_GetStackLuaMat (L, 2);

  char *method_name;
  lua_CFunction func_Convert;

  oski_matlua_t *A_csr;
  oski_DestroyMatRepr_funcpt func_DestroyCSR;

  int num_ret;

  if (A_desttype == NULL || A_source == NULL)
    return 0;

  /* If converting between the same types, just create a copy. */
  if (AreSameLuaMatTypes (A_desttype, A_source))
    {
      oski_CopyMatRepr_funcpt func_Copy;
      void *mat_copy;
      oski_matlua_t *lua_copy;

      oski_PrintDebugMessage (3,
			      "[OLUA] Duplicating a matrix in '%s' format...",
			      A_source->mattype_name);

      func_Copy = (oski_CopyMatRepr_funcpt)
	oski_LookupMatTypeMethod (A_source->mattype_name,
				  A_source->ind_type, A_source->val_type,
				  "oski_CopyMatRepr");
      if (func_Copy == NULL)
	{
	  OSKI_ERR_NOT_IMPLEMENTED (CopyMatRepr, A_source->mattype_name);
	  return 0;
	}

      mat_copy = (*func_Copy) (A_source->mat.repr, A_source->props);
      if (mat_copy == NULL)
	return 0;

      lua_copy = oski_PushEmptyLuaMat (L);
      if (lua_copy == NULL)
	return 0;

      lua_copy->props = A_source->props;
      lua_copy->mat.type_id = A_source->mat.type_id;
      lua_copy->mat.repr = mat_copy;
      lua_copy->mattype_name = oski_DuplicateString (A_source->mattype_name);
      lua_copy->ind_type = A_source->ind_type;
      lua_copy->val_type = A_source->val_type;

      return 1;
    }
  /* ELSE not a simple copy operation.
   * Instead, we need to convert between types.
   */

  oski_PrintDebugMessage (3,
			  "[OLUA] Converting from '%s' to '%s' format...",
			  A_source->mattype_name, A_desttype->mattype_name);

  /* Determine if there is a native conversion routine. */
  method_name = MakeLuaConvMethodName (A_source, A_desttype);
  if (method_name == NULL)
    return 0;

  func_Convert =
    (lua_CFunction) oski_LookupMatTypeMethod (A_desttype->mattype_name,
					      A_desttype->ind_type,
					      A_desttype->val_type,
					      method_name);
  oski_Free (method_name);
  if (func_Convert != NULL)
      return (*func_Convert) (L);

  /* If we reached this point and the source matrix type is CSR,
   * then the conversion fails because the destination matrix type
   * did not implement a create-from-CSR routine.
   */
  if (strcmp (A_source->mattype_name, "CSR") == 0)
    {
      oski_HandleError (ERR_NOT_IMPLEMENTED,
			"Create-from-CSR routine not implemented.",
			__FILE__, __LINE__,
			"%s matrix type does not implement a method"
			" to create a %s matrix from %s format.",
			A_desttype->mattype_name, A_desttype->mattype_name,
			A_source->mattype_name);
      return 0;
    }

  /* CSR is the assumed intermediate format, so let's convert
   * to that first.
   */
  A_csr = ConvertLuaMatToCSR (L, 2);
  if (A_csr == NULL)		/* <A_csr> = CSR(A_source) */
    return 0;

  /* If the target format was CSR anyhow, we can return now. */
  if (AreSameLuaMatTypes (A_csr, A_desttype))
    return 1;

  /*  Otherwise, call A_dest's CSR conversion routine
   *  (MUST exist!).
   */
  lua_replace (L, 2);		/* A_source = A_csr */

  /* Stack == [ A_dest A_csr ... ] */
  num_ret = ConvertLuaMat (L);

  /* Clean-up temporary CSR matrix */
  func_DestroyCSR = (oski_DestroyMatRepr_funcpt)
    oski_LookupMatTypeIdMethod (A_csr->mat.type_id, "oski_DestroyMatRepr");
  if (func_DestroyCSR)
    (*func_DestroyCSR) (A_csr->mat.repr);

  return num_ret;
}

/** OLUA_MAT static members */
static const struct luaL_reg g_luamatlib_f[] = {
  {"new", NewLuaMat},
  {NULL, NULL}
};

/** OLUA_MAT object methods */
static const struct luaL_reg g_luamatlib_m[] = {
  {"__tostring", ToStringLuaMat},
  {"__gc", GCLuaMat},
  {"__call", ConvertLuaMat},
  {NULL, NULL}
};

/** \brief Load OSKI-Lua matrix module and metatable */
static int
OpenLuaMatMod (lua_State * L)
{
  /* Create metatable for the OLUA_MAT type */
  luaL_newmetatable (L, OLUA_MATTAB);
  lua_pushstring (L, "__index");
  lua_pushvalue (L, -2);	/* push OLUA_MATTAB */
  lua_settable (L, -3);		/* OLUA_MATTAB.__index = OLUA_MATTAB */

  /* Register metatable methods */
  luaL_openlib (L, NULL, g_luamatlib_m, 0);

  /* Register "regular" methods */
  luaL_openlib (L, OLUA_MAT, g_luamatlib_f, 0);

  return 1;
}

/**
 *  \brief Start a OSKI-Lua process.
 *
 *  Calls the error handler on error and returns NULL. Otherwise,
 *  returns a pointer to valid OSKI-Lua state.
 */
static lua_State *
OpenLua (void)
{
  lua_State *L;

  oski_PrintDebugMessage (2, "[OLUA] Starting transformation engine...");

  L = lua_open ();
  if (L == NULL)
    {
      oski_HandleError (ERR_LOAD_MOD,
			"Can't load OSKI-Lua transformation engine",
			__FILE__, __LINE__, "");
      return NULL;
    }

  /* Load standard libraries */
  luaopen_base (L);
  luaopen_io (L);
  luaopen_string (L);
  luaopen_math (L);
  luaopen_table (L);

  /* Load standard OSKI-Lua libraries */
  if (RequireLua (L, "oski") != 0)
    {
      lua_close (L);
      return NULL;
    }

  /* Create Lua matrix metatype */
  OpenLuaMatMod (L);

  /**
   *  \todo Create global OSKI-Lua matrix types
   *  for all registered types.
   */
  RegisterLuaMatTypes (L);

  return L;
}

/**
 *  \brief Shutdown a OSKI-Lua process.
 */
static void
CloseLua (lua_State * L)
{
  oski_PrintDebugMessage (2, "[OLUA] Closing transformation engine...");
  lua_close (L);
  oski_PrintDebugMessage (3, "(closed)");
}

/**
 *  \brief Execute a OSKI-Lua program to transform a matrix data
 *  structure.
 *
 *  \param[in] L Lua engine in which to run the code.
 *  \param[in] xforms String program representing the transformation
 *  to execute.
 *  \returns 0 on success, or an error code.
 *
 *  If an error occurs while trying to execute the transformation,
 *  converts the error code to its equivalent BeBOP error code,
 *  and leaves the Lua error message on L's stack.

Available OSKI-Lua globals:
- InputMat (SparseMat)
- indtype, valtype (strings)

OSKI-Lua syntax examples:
\code
-- Program 1: Convert to 4x2 blocked format
return BCSR( InputMat, 4, 2 );
\endcode

Lua-native module code:
\code
\endcode

Make the following C-implemented routines available in Lua:
\code
loadmattype( type_name, ind_type, val_type )
\endcode

*/
static int
RunLua (lua_State * L, const char *xforms)
{
  int err_lua;
  int err_oski;

  /* Try to execute the transformation. */
  err_lua = luaL_loadbuffer (L, xforms, strlen (xforms), xforms)
    || lua_pcall (L, 0, 1, 0);

  /* Convert Lua error code to a BeBOP error code. */
  if (!err_lua)
    {
      /* Check for valid return matrix. */
      oski_matlua_t *A = oski_GetStackLuaMat (L, -1);
      if (A == NULL)
	{
	  lua_pushstring (L,
			  "Transformation program did not return"
			  " a valid matrix object.");
	  err_oski = ERR_OLUA_NORET;
	}
      else
	err_oski = 0;
    }
  else
    {
      switch (err_lua)
	{
	case LUA_ERRSYNTAX:
	  err_oski = ERR_BAD_SYNTAX;
	  break;

	case LUA_ERRMEM:
	  err_oski = ERR_OUT_OF_MEMORY;
	  break;

	default:
	  err_oski = ERR_CANT_CONVERT;
	  break;
	}
    }

  return err_oski;
}

/**
 *  \brief Instantiates new OLUA_MAT object wrapper around the
 *  matrix (props, mat) and assigns it to the global variable
 *  'name'. The new object is left at the top of the stack.
 *
 *  \returns 0 on success, or an error code on error.
 */
static int
CreateLuaMatObj (lua_State * L, const oski_matcommon_t * props,
		 oski_matspecific_t * mat, const char *name)
{
  /* <A> = OLUA_MAT.new() */
  oski_matlua_t *A = oski_PushEmptyLuaMat (L);

  /* Initialize private fields */
  if (A != NULL)
    {
      const oski_mattype_t *rec;

      A->props = props;
      A->mat.type_id = mat->type_id;
      A->mat.repr = mat->repr;

      /* Cache some matrix type data */
      rec = oski_FindMatTypeRecord (mat->type_id);
      if (rec != NULL)
	{
	  A->mattype_name = oski_DuplicateString (rec->name);
	  A->ind_type = rec->id_index_type;
	  A->val_type = rec->id_value_type;
	}
      lua_setglobal (L, name);	/* name = <A> */
      return 0;
    }

  return ERR_OUT_OF_MEMORY;
}

/**
 *  \brief If the given matrix has a valid tuned representation and
 *  does not share its input representation, then this routine
 *  destroys the input matrix representation.
 *
 *  \returns An error code on error. Otherwise, this function returns
 *  0, regardless of whether the input matrix representation could
 *  be legally destroyed.
 */
int
oski_FreeInputMatRepr (oski_matrix_t A)
{
  oski_DestroyMatRepr_funcpt func_DestroyMatRepr;

  if (A == INVALID_MAT)
    return ERR_BAD_MAT;

  if (A->tuned_mat.type_id == INVALID_ID || A->is_shared)
    return 0;

  oski_PrintDebugMessage (3,
			  "[OLUA] Destroying unshared input matrix representation");

  func_DestroyMatRepr = OSKI_MATTYPEID_METHOD (A->input_mat.type_id,
					       DestroyMatRepr);
  if (func_DestroyMatRepr != NULL)
    (*func_DestroyMatRepr) (A->input_mat.repr);

  A->input_mat.type_id = INVALID_ID;
  A->input_mat.repr = NULL; /** \todo Possible leak? */

  return 0;
}

/**
 *  \brief
 *
 *  \param[in,out] A Matrix whose tuned representation is to be changed.
 *  \param[in] new_mat The new matrix representation.
 *  \param[in] xforms The corresponding transformation string used to
 *  create 'new_mat'.
 *
 *  \returns 0 on success, or an error code. Does not call the error
 *  handler.
 *
 *  If new_mat points to A's tuned matrix representation, this
 *  function immediately returns 0.
 *
 *  If A is invalid, this function returns ERR_BAD_MAT.
 */
int
oski_ReplaceTunedMatRepr (oski_matrix_t A, oski_matspecific_t * new_mat,
			  const char *xforms)
{
  oski_DestroyMatRepr_funcpt func_DestroyMatRepr;

  if (A == INVALID_MAT)
    return ERR_BAD_MAT;
  if (&(A->tuned_mat) == new_mat)
    return 0;

  /* Out with the old tuned matrix, if any */
  func_DestroyMatRepr = OSKI_MATTYPEID_METHOD (A->tuned_mat.type_id,
					       DestroyMatRepr);
  if (func_DestroyMatRepr != NULL)
    (*func_DestroyMatRepr) (A->tuned_mat.repr);

  /* In with the new... */
  if (new_mat != NULL)
    {
      oski_PrintDebugMessage (3,
			      "[OLUA] Replacing tuned matrix representation...");

      A->tuned_mat.type_id = new_mat->type_id;
      A->tuned_mat.repr = new_mat->repr;

      /* Save transform information */
      if (A->tuned_xforms)
	oski_Free (A->tuned_xforms);

      if (xforms != NULL)
	A->tuned_xforms = oski_DuplicateString (xforms);
      else
	A->tuned_xforms = NULL;
    }
  else
    {
      A->tuned_mat.type_id = INVALID_ID;
      A->tuned_mat.repr = NULL;
    }

  return 0;
}

/**
 *  \brief
 *
 *  \param[in] A_tunable A valid matrix object.
 *  \param[in] xforms A valid OSKI-Lua program describing the
 *  transformation to apply to the matrix.
 *  \returns 0 if the transformation was applied successfully.
 */
int
oski_ApplyMatTransforms (oski_matrix_t A_tunable, const char *xforms)
{
  lua_State *L;
  int err;

  oski_PrintDebugMessage (1, "Applying transformation program: '%s'", xforms);

  L = OpenLua ();
  if (L == NULL)
    return ERR_LOAD_MOD;

  /* Create OLUA_INMAT */
  err = CreateLuaMatObj (L,
			 &A_tunable->props,
			 &A_tunable->input_mat, OLUA_INMAT);
  if (err)
    {
      oski_HandleError (err,
			"Can't create '" OLUA_INMAT "'", __FILE__, __LINE__,
			"");
      CloseLua (L);
      return err;
    }

  /* Execute user's code */
  err = RunLua (L, xforms);
  if (!err)			/* Save result of tuning */
    {
      oski_matlua_t *A = oski_GetStackLuaMat (L, -1);

      /* Transformations lead to a new representation */
      if (A == NULL || A->mat.type_id == INVALID_ID || A->mat.repr == NULL)
	{
	  oski_PrintDebugMessage (3,
				  "[OLUA] OSKI-Lua transformation program"
				  " did not return a valid matrix.");
	  err = ERR_OLUA_NORET;
	}
      else if (A_tunable->input_mat.repr == A->mat.repr)
	oski_PrintDebugMessage (3,
				"[OLUA] NOTE: Transformed matrix same as input.");
      else			/* Matrix changed */
	{
	  oski_ReplaceTunedMatRepr (A_tunable, &A->mat, xforms);
	  oski_FreeInputMatRepr (A_tunable);
	}
    }
  else				/* Error occurred while running transformation program */
    {
      const char *errmsg_lua = lua_tostring (L, -1);
      lua_pop (L, 1);		/* pop error message from stack */

      oski_HandleError (err,
			"Could not transform data structure", __FILE__,
			__LINE__, "%s(): '%s'",
			MACRO_TO_STRING (oski_ApplyMatTransforms),
			errmsg_lua);
    }

  CloseLua (L);
  return err;
}

/**
 *  \brief
 *
 *  This routine can be used for any 'simple' matrix type which
 *  expects no arguments on the Lua stack besides the matrix
 *  types:
 *    -# Destination matrix type (e.g., 'CSC', 'GCSR')
 *    -# Source matrix ('CSR')
 */
int
oski_CreateLuaMatReprGenericFromCSR (lua_State * L, const char *mattype)
{
  oski_matlua_t *B_desttype = oski_GetStackLuaMat (L, 1);
  oski_matlua_t *B_dest;	/* Lua matrix to return */
  void *B;			/* raw destination matrix */
  oski_CreateMatReprFromCSR_funcpt func_Convert;

  const oski_matlua_t *A_source = oski_GetStackLuaMat (L, 2);
  const oski_matCSR_t *A;	/* raw CSR matrix */

  oski_id_t id_CSR = OSKI_MATTYPE_ID ("CSR");
  oski_id_t id_B = OSKI_MATTYPE_ID (mattype);

  /* Check input arguments */
  if (B_desttype == NULL || A_source == NULL
      || A_source->mat.type_id != id_CSR
      || strcmp (B_desttype->mattype_name, mattype) != 0
      || B_desttype->ind_type != OSKI_IND_ID
      || B_desttype->val_type != OSKI_VAL_ID)
    {
      fprintf (stderr, "@@ id_CSR = %d, id_B = %d\n", id_CSR, id_B);
      fprintf (stderr, "OSKI_IND_ID = %d, OSKI_VAL_ID = %d\n", OSKI_IND_ID, OSKI_VAL_ID);
      fprintf (stderr, "mattype = '%s'\n", mattype);
      fprintf (stderr, "B_desttype = %p\n", B_desttype);
      fprintf (stderr, "A_source = %p\n", A_source);
      if (A_source != NULL)
	fprintf (stderr, "A_source->mat.type_id = %d\n",
		 A_source->mat.type_id);
      if (B_desttype != NULL)
	{
	  fprintf (stderr, "B_desttype->mattype_name = '%s'\n",
		   B_desttype->mattype_name);
	  fprintf (stderr, "B_desttype->ind_type = %d\n",
		   B_desttype->ind_type);
	  fprintf (stderr, "B_desttype->val_type = %d\n",
		   B_desttype->val_type);
	}

      oski_HandleError (ERR_BAD_ARG,
			"One of the conversion arguments is invalid.",
			__FILE__, __LINE__,
			"In call to %s( dest=%s<i=%d, v=%d>, source=%s<i=%d, v=%d> )",
			MACRO_TO_STRING (oski_CreateLuaMatReprGenericFromCSR),
			B_desttype->mattype_name,
			(int) B_desttype->ind_type,
			(int) B_desttype->val_type, A_source->mattype_name,
			(int) A_source->ind_type, (int) A_source->val_type);
      return 0;
    }

  /* Create new object */
  A = (const oski_matCSR_t *) A_source->mat.repr;
  B = NULL;
  func_Convert = OSKI_MATTYPE_METHOD (mattype, CreateMatReprFromCSR);
  if (func_Convert == NULL)
    return 0;
  else
    B = (*func_Convert) (A, A_source->props);
  if (B == NULL)
    return 0;

  B_dest = oski_PushEmptyLuaMat (L);
  if (B_dest == NULL)
    {
      oski_DestroyMatRepr_funcpt func_Destroy =
	OSKI_MATTYPE_METHOD (mattype, DestroyMatRepr);
      if (func_Destroy != NULL)
	(*func_Destroy) (B);
       /** \todo else: possible leak */
      return 0;
    }

  B_dest->props = A_source->props;
  B_dest->mattype_name = oski_DuplicateString (mattype);
  B_dest->ind_type = B_desttype->ind_type;
  B_dest->val_type = B_desttype->val_type;
  B_dest->mat.repr = (void *) B;
  B_dest->mat.type_id = id_B;
  return 1;
}

/**
 *  \brief
 *
 *  This conversion routine may be used with any matrix type
 *  that expects two positive integer index arguments on the
 *  Lua stack:
 *    -# Destination matrix type (e.g., 'MBCSR', 'BCSR', 'CB')
 *    -# Source matrix ('CSR')
 *    -# First index (e.g., the row block size 'r')
 *    -# Second index (e.g., the column block size 'c')
 */
int
oski_CreateLuaMatReprGeneric2IndexFromCSR (lua_State * L, const char *mattype)
{
  oski_matlua_t *B_desttype = oski_GetStackLuaMat (L, 1);
  oski_matlua_t *B_dest;
  void *B;
  oski_CreateMatReprFromCSR_funcpt func_Convert;
  oski_DestroyMatRepr_funcpt func_Destroy;

  const oski_matlua_t *A_source = oski_GetStackLuaMat (L, 2);
  const oski_matCSR_t *A;

  oski_index_t r = (oski_index_t) lua_tonumber (L, 3);
  oski_index_t c = (oski_index_t) lua_tonumber (L, 4);

  oski_id_t id_CSR = OSKI_MATTYPE_ID ("CSR");
  oski_id_t id_B = OSKI_MATTYPE_ID (mattype);

  /* Check input arguments */
  if (r <= 0 || c <= 0
      || B_desttype == NULL
      || A_source == NULL
      || A_source->mat.type_id != id_CSR
      || strcmp (B_desttype->mattype_name, mattype) != 0
      || B_desttype->ind_type != OSKI_IND_ID
      || B_desttype->val_type != OSKI_VAL_ID)
    {
      oski_HandleError (ERR_BAD_ARG,
			"One of the conversion arguments is invalid.",
			__FILE__, __LINE__,
			"In call to %s( dest=%s<i=%d, v=%d>, source=%s<i=%d, v=%d>"
			", r=%d, c=%d )",
			MACRO_TO_STRING (oski_CreateLuaMatReprFromCSR),
			B_desttype->mattype_name,
			(int) B_desttype->ind_type,
			(int) B_desttype->val_type, A_source->mattype_name,
			(int) A_source->ind_type, (int) A_source->val_type, r,
			c);
      return 0;
    }

  /* Create a new object */
  A = (const oski_matCSR_t *) A_source->mat.repr;
  func_Convert = OSKI_MATTYPE_METHOD (mattype, CreateMatReprFromCSR);
  func_Destroy = OSKI_MATTYPE_METHOD (mattype, DestroyMatRepr);
  if (func_Convert == NULL || func_Destroy == NULL)
    return 0;
  B = (*func_Convert) (A, A_source->props, r, c);
  if (B == NULL)
    return 0;

  B_dest = oski_PushEmptyLuaMat (L);
  if (B_dest == NULL)
    {
      (*func_Destroy) (B);
      return 0;
    }

  B_dest->props = A_source->props;
  B_dest->mattype_name = oski_DuplicateString (mattype);
  B_dest->ind_type = B_desttype->ind_type;
  B_dest->val_type = B_desttype->val_type;
  B_dest->mat.repr = (void *) B;
  B_dest->mat.type_id = id_B;
  return 1;
}

/* eof */
