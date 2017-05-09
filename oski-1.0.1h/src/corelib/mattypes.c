/**
 *  \file src/mattypes.c
 *  \ingroup MATTYPES
 *  \brief Matrix type management routines.
 *
 *  This module implements the internally managed tables of available
 *  matrix types and kernel implementations.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <oski/common.h>

#include <oski/simplelist.h>
#include <oski/modloader.h>
#include <oski/mattypes_internal.h>

/* ----------------------------------------------------------------
 *  Private routines
 */

/**
 *  \brief Global (but private to this module) list of available
 *  matrix types.
 */
static simplelist_t *g_avail_mattypes = NULL;

/**
 *  \brief Frees all memory associated with a given record, and unloads
 *  any associated loaded modules.
 */
static void
FreeMatTypeRecord (oski_mattype_t * record)
{
  if (record == NULL)
    return;

  if (record->module != NULL)
    {
      /* Execute the module's CLOSE function, if available */
      oski_CloseModule (record->module);
      oski_UnloadModule (record->module);
    }
  if (record->desc != NULL)
    oski_Free (record->desc);
  if (record->modname != NULL)
    oski_Free (record->modname);
  if (record->name != NULL)
    oski_Free ((void *) (record->name));

  /* Without a trace... */
  record->module = NULL;
  record->id = INVALID_ID;
  record->desc = NULL;

  oski_Free (record);
}

/* ----------------------------------------------------------------
 *  Public routines
 */

/**
 *  \brief
 *
 *  If this type is already registered, this routine returns the
 *  id of the previously registered instance.
 *
 *  \param[in] type_name   Short, unique string identifier. The name should
 *  match the regular expression, "[0-9a-zA-Z_]+".
 *  \param[in] id_ind_type Integer index type (see \ref oski/scalinfo.h).
 *  \param[in] id_val_type Non-zero value type (see \ref oski/scalinfo.h).
 *
 *  \returns A unique positive integer identifying this type, or
 *  INVALID_ID on error.
 *  \see #oski_LookupMatTypeId
 *
 *  \post This routine creates a new registration record and fills in
 *  all of its fields, including loading its main shared library module.
 */
oski_id_t
oski_RegisterMatType (const char *type_name,
		      oski_id_t id_ind_type, oski_id_t id_val_type)
{
  oski_mattype_t *new_record;
  oski_id_t id;

  if (g_avail_mattypes == NULL)
    return INVALID_ID;

  /* Does this registration request duplicates an existing record? */
  id = oski_LookupMatTypeId (type_name, id_ind_type, id_val_type);
  if (id != INVALID_ID)
    return id;

  /* Create new record and fill in basic data. */
  new_record = oski_Malloc (oski_mattype_t, 1);
  if (!new_record)
    return INVALID_ID;

  new_record->name = oski_DuplicateString (type_name);
  new_record->id_index_type = id_ind_type;
  new_record->id_value_type = id_val_type;

  /* Cache some descriptive strings */
  new_record->desc = oski_MakeModuleDesc (type_name,
					  id_ind_type, id_val_type);

  /* Load the module */
  new_record->modname = oski_MakeModuleName ("mat", type_name,
					     id_ind_type, id_val_type);
  new_record->module = oski_LoadModuleOptional (new_record->modname);

  if (new_record->module == NULL)
    {
      oski_Free (new_record->modname);
      oski_Free (new_record->desc);
      oski_Free (new_record->name);
      oski_Free (new_record);
      return INVALID_ID;
    }

  /* Insert this record into the global table of available types */
  new_record->id = (oski_id_t) simplelist_Append (g_avail_mattypes,
						  new_record);

  if (new_record->id == INVALID_ID)
    {
      FreeMatTypeRecord (new_record);
      return INVALID_ID;
    }

  oski_PrintDebugMessage (2, "Registered new matrix type: %s (ID#%d)",
			  new_record->desc, new_record->id);

  return new_record->id;
}

/**
 *  \brief
 *
 *  \param[in] type_name String identifier of the general matrix type
 *  being sought.
 *  \param[in] id_ind_type Integer index type.
 *  \param[in] id_val_type Non-zero value type.
 *  \returns Returns an id number that uniquely identifies the given
 *  matrix type, specialized using particular integer index and non-zero
 *  value types, if the type has been registered in the system. Otherwise,
 *  returns INVALID_ID.
 *  \see #oski_RegisterMatType
 */
oski_id_t
oski_LookupMatTypeId (const char *type_name,
		      oski_id_t id_ind_type, oski_id_t id_val_type)
{
  oski_id_t id_found = INVALID_ID;
  size_t i;
  size_t len;

  len = oski_GetNumMatTypes ();

  for (i = 1; i <= len; i++)
    {
      const oski_mattype_t *record;

      record = simplelist_GetElem (g_avail_mattypes, i);
      if (record == NULL)
	continue;

      if ((strcmp (type_name, record->name) == 0)
	  && (id_ind_type == record->id_index_type)
	  && (id_val_type == record->id_value_type))
	{
	  id_found = record->id;
	  break;
	}
    }

  return id_found;
}

void *
oski_LookupMatTypeIdMethod (oski_id_t id, const char *method_name)
{
  const oski_mattype_t *record;

  record = oski_FindMatTypeRecord (id);
  if (record == NULL)
    return NULL;

  return oski_LookupModuleMethod (record->module, method_name);
}

void *
oski_LookupMatTypeMethod (const char *type_name,
			  oski_id_t id_ind_type, oski_id_t id_val_type,
			  const char *method_name)
{
  oski_id_t id;

  id = oski_LookupMatTypeId (type_name, id_ind_type, id_val_type);
  if (id == INVALID_ID)		/* Attempt to load the module */
    id = oski_RegisterMatType (type_name, id_ind_type, id_val_type);

  if (id == INVALID_ID)		/* Still didn't work */
    return NULL;
  else
    return oski_LookupMatTypeIdMethod (id, method_name);
}

/**
 *  \brief
 *
 *  \param[in] id  Matrix type id.
 *  \returns Returns a pointer to the type record if registered,
 *  or NULL otherwise. Returns NULL immediately if id == INVALID_ID.
 */
const oski_mattype_t *
oski_FindMatTypeRecord (oski_id_t id)
{
  size_t i;
  size_t len;

  if (id == INVALID_ID)
    return NULL;

  len = oski_GetNumMatTypes ();

  for (i = 1; i <= len; i++)
    {
      const oski_mattype_t *record = simplelist_GetElem (g_avail_mattypes, i);

      if (record == NULL)
	continue;

      if (record->id == id)
	return record;
    }

  return NULL;
}

/**
 *  \brief
 *
 *  \param[in] record  Record to display.
 *  \param[in,out] fp   File pointer for debugging message.
 */
void
oski_DisplayMatTypeInfo (const oski_mattype_t * record, FILE * fp)
{
  const oski_scalinfo_t *info;
  oski_id_t id;
  const char *desc;

  info = oski_LookupScalarIndexInfo (record->id_index_type);
  if (info != NULL)
    {
      id = info->id;
      desc = (info->name == NULL) ? "null" : info->name;
    }
  else
    {
      id = INVALID_ID;
      desc = "undefined";
    }
  info = oski_LookupScalarValueInfo (record->id_value_type);
  if (info != NULL)
    {
      id = info->id;
      desc = (info->name == NULL) ? "null" : info->name;
    }
  else
    {
      id = INVALID_ID;
      desc = "undefined";
    }
}

/** Register default matrix types */
static void
RegisterDefaultMatTypes (void)
{
  if (!oski_RegisterSiteModules ("site-modules-mats.txt", "mat",
				 oski_RegisterMatType))
    {
      /* Couldn't find site module file, so try loading all possible
	 CSR/CSC types */
      oski_id_t i;
      for (i = 1; i <= oski_GetNumScalarIndexTypes(); i++)
	{
	  oski_id_t j;
	  for (j = 1; j <= oski_GetNumScalarValueTypes(); j++)
	    oski_RegisterMatType ("CSR", i, j);
	  for (j = 1; j <= oski_GetNumScalarValueTypes(); j++)
	    oski_RegisterMatType ("CSC", i, j);
	}
    }
}


/**
 *  \brief
 *
 *  \returns Initializes the list of available matrix types.
 *
 *  \todo Should call this routine during library initialization.
 */
void
oski_InitMatTypeManager (void)
{
  if (g_avail_mattypes == NULL)
    {
      oski_PrintDebugMessage (2, "Initializing matrix type manager");
      g_avail_mattypes = simplelist_Create ();
      RegisterDefaultMatTypes();
    }
}

/**
 *  \brief
 *
 *  \returns Destroys the list of available matrix types.
 *
 *  \post g_avail_mattypes == NULL.
 */
void
oski_CloseMatTypeManager (void)
{
  if (g_avail_mattypes != NULL)
    {
      size_t i;
      size_t len;

      oski_PrintDebugMessage (2, "Shutting down matrix type manager");

      len = oski_GetNumMatTypes ();

      /* Free individual matrix type records. */
      for (i = 1; i <= len; i++)
	{
	  /* Get data element, but throw away 'const' qualifier */
	  oski_mattype_t *record =
	    (oski_mattype_t *) simplelist_GetElem (g_avail_mattypes, i);

	  if (record != NULL)
	    FreeMatTypeRecord (record);
	}

      /* Destroy the list. */
      simplelist_Destroy (g_avail_mattypes);
      g_avail_mattypes = NULL;
    }
}

size_t
oski_GetNumMatTypes (void)
{
  return simplelist_GetLength (g_avail_mattypes);
}

/* eof */
