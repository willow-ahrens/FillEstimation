/**
 *  \file src/modloader.c
 *  \ingroup AUXIL
 *  \brief Shared library module loader and related utilities.
 *
 *  This module implements a basic shared library module loader,
 *  and utilities to load routines from shared libraries. The
 *  implementation is basically just a wrapper around LTDL, with
 *  BeBOP style memory allocation and error handling.
 *
 *  Our use of LTDL allows easy switching between pure shared
 *  and pure static compilation modes.
 *
 *  A module loaded using oski_LoadModule may choose to define
 *  initialization and clean-up routines named oski_InitModule
 *  and oski_CloseModule. However, these names should be mangled
 *  using the modloader macro, MANGLE_MOD_.
 *
 *  The library checks for an environment variable, whose name is
 *  defined by the compile-time macro OSKI_LIB_PATH, for a shared
 *  library load path.
 *
 *  NOTE: This file must be compiled with the macro
 *  OSKI_LIB_PATH_DEFAULT set to the default dynamic library
 *  search path.
 */

#include <oski/config.h>

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <oski/common.h>
#include <oski/modloader.h>

#if !defined(OSKI_LIB_PATH)
/** Default environment variable name to hold the shared library path. */
#define OSKI_LIB_PATH "OSKI_LD_LIBRARY_PATH"
#endif

#if !defined(OSKI_LIB_PATH_DEFAULT)
#error "ERROR: Must define default library search path, 'OSKI_LIB_PATH_DEFAULT'"
#endif

/**
 *  \brief
 *
 *  \returns 1 on success, 0 on error.
 *  \post The module loader routines to load modules and lookup
 *  functions may be called after this routine returns 1.
 */
int
oski_InitModuleLoader (void)
{
  int err;
  const char *path = NULL;

  oski_PrintDebugMessage (2, "Initializing shared library module loader");
  LTDL_SET_PRELOADED_SYMBOLS ();

  err = lt_dlinit ();
  if (!err)
    {
      oski_PrintDebugMessage (3,
			      "Checking for environment variable '%s'",
			      OSKI_LIB_PATH);

      path = getenv (OSKI_LIB_PATH);
      if (path == NULL || strlen (path) == 0)
	path = OSKI_LIB_PATH_DEFAULT;

      if (path != NULL)
	{
	  oski_PrintDebugMessage (3, "Setting search path to '%s'", path);
	  err = lt_dlsetsearchpath (path);
	}
    }

  if (err)
    {
      const char *error_message = lt_dlerror ();
      if (error_message != NULL)
	{
	  oski_HandleError (ERR_INIT_SHAREDLIB,
			    "Can't initialize shared library loader",
			    __FILE__, __LINE__,
			    "Search path: %s",
			    path == NULL ? "(default)" : path);
	}
      return 0;
    }
  else
    return 1;
}

void
oski_CloseModuleLoader (void)
{
#if USING_CYGWIN
  ;	  /** \todo Determine why lt_dlexit() causes a segfault under cygwin */
#else
  lt_dlexit ();
#endif
}

/**
 *  \brief
 *
 *  \param[in] modname Name of shared library module to load.
 *  \returns A handle to the loaded module, or NULL on error.
 */
oski_modhandle_t
oski_LoadModuleOptional (const char *modname)
{
  oski_modhandle_t module;

  oski_PrintDebugMessage (3, "Checking for shared library module '%s'",
			  modname);

  module = lt_dlopenext (modname);
  if (module == NULL)
    oski_PrintDebugMessage (4, "(Not found.)");
  else
    {
      oski_PrintDebugMessage (4, "(Found: %p. Initializing...)", module);
      oski_CallModuleInit (module);
    }
  return module;
}

/**
 *  \brief
 *
 *  \param[in] modname Name of shared library module to load.
 *  \returns A handle to the loaded module, or NULL on error.
 */
oski_modhandle_t
oski_LoadModule (const char *modname)
{
  oski_modhandle_t module = oski_LoadModuleOptional (modname);
  if (module == NULL)
    {
      const char *error_message = lt_dlerror ();
      oski_HandleError (ERR_LOAD_MOD,
			"Could not load shared library module", __FILE__,
			__LINE__, "Error '%s' for module '%s'", error_message,
			modname);
    }
  return module;
}

/**
*  \name Default base names for shared methods.
*/
/*@{*/
/**
 *  \brief Pointer to a function that takes no arguments and returns
 *  nothing.
 *
 *  \ingroup MATTYPES_INTERFACE
 */
typedef void (*oski_simple_funcpt) (void);

#define OSKI_METHOD_INIT "oski_InitModule"
#define OSKI_METHOD_CLOSE "oski_CloseModule"
#define OSKI_METHOD_GETDESC "oski_GetShortDesc"
#define OSKI_METHOD_GETLONGDESC "oski_GetLongDesc"
/*@}*/

/**
 *  \brief
 *
 *  If module == NULL, this routine returns without taking any
 *  action.
 */
void
oski_CallModuleInit (oski_modhandle_t module)
{
  oski_simple_funcpt init_routine;

  oski_PrintDebugMessage (3, "Trying to execute %s() in module %p",
			  OSKI_METHOD_INIT, module);

  if (module == NULL)
    return;

  init_routine = (oski_simple_funcpt) lt_dlsym (module, OSKI_METHOD_INIT);
  if (init_routine != NULL)
    (*init_routine) ();
  else
    oski_PrintDebugMessage (4,
			    "Module %p does not have a %s() routine",
			    module, OSKI_METHOD_INIT);
}

/**
 *  \brief
 *
 *  If module == NULL, this routine returns without taking any
 *  action.
 */
void
oski_CloseModule (oski_modhandle_t module)
{
  oski_simple_funcpt close_routine;

  if (module == NULL)
    return;

  close_routine = (oski_simple_funcpt) lt_dlsym (module, OSKI_METHOD_CLOSE);
  if (close_routine != NULL)
    (*close_routine) ();
  else
    oski_PrintDebugMessage (4,
			    "Module %p does not have a %s() routine",
			    module, OSKI_METHOD_CLOSE);
}

/**
 *  \brief
 *
 *  \returns 1 if module was unloaded successfully or module == NULL,
 *  and 0 otherwise.
 */
int
oski_UnloadModule (oski_modhandle_t module)
{
#if DISABLE_SHLIB_CLOSE		/* Workaround on Darwin */
  return 1;
#else
  int err;

  oski_PrintDebugMessage (4, "Unloading module %p", (void *) module);

  if (module == NULL)
    return 1;

  err = lt_dlclose (module);

  if (err)
    {
      const char *error_message = lt_dlerror ();
      oski_HandleError (ERR_UNLOAD_MOD,
			"Can't unload shared library module", __FILE__,
			__LINE__, "%s", error_message);
      return 1;
    }
  else
    return 0;
#endif
}

/**
 *  \brief
 *
 *  This routine does not call the error handler on error to
 *  keep the execution time of this routine as small as
 *  possible.
 *
 *  \param[in] module Module pointer.
 *  \param[in] method_name Name of the method (routine) to find.
 *  \returns A pointer to the routine, or NULL if not found.
 */
void *
oski_LookupModuleMethod (const oski_modhandle_t module,
			 const char *method_name)
{
  lt_ptr method;

  if (module == NULL)
    return NULL;

  /* NOTE: Cast removes 'const' qualifier. */
  method = lt_dlsym ((lt_dlhandle) module, method_name);
  return (void *) method;
}

/**
 *  \brief
 *  
 *  The caller must free the returned string.
 *
 *  \returns NULL on error.
 */
char *
oski_MakeModuleDesc (const char *name, oski_id_t ind, oski_id_t val)
{
  const char *ind_name;
  const char *val_name;

  ind_name = oski_GetScalarIndexName (ind);
  val_name = oski_GetScalarValueName (val);

  if (ind_name == NULL)
    ind_name = "UNKNOWN";
  if (val_name == NULL)
    val_name = "UNKNOWN";

  /* Build string of the form, "[NAME](index=[INDEX_T], value=[VAL_T])\0" */
  return oski_StringPrintf ("%s(index=%s, value=%s)",
			    name, ind_name, val_name);
}

/**
 *  \brief
 *
 *  The caller must free the returned string.
 *
 *  \returns NULL on error.
 */
char *
oski_MakeModuleName (const char *mod_type_name, const char *name,
		     oski_id_t ind, oski_id_t val)
{
  char ind_tag = oski_GetScalarIndexTag (ind);
  char val_tag = oski_GetScalarValueTag (val);

  if (ind_tag == (char) NULL)
    ind_tag = '?';
  if (val_tag == (char) NULL)
    val_tag = '?';

  /* Build string of the form: "liboski_<TYPE>_<NAME>_T<IND><VAL>\0" */
  return oski_StringPrintf ("liboski_%s_%s_T%c%c", mod_type_name,
			    name, ind_tag, val_tag);
}

const char *
GetDirFromPath (const char* path)
{
  if (path == NULL)
    return NULL;
  else /* path != NULL */
    {
      const char* end = path;
      while (end[0] != '\0' && end[0] != ';')
	end++;
      /* assert (end[0] == '\0' || end[0] == ';') */
      return end;
    }
}

/**
 *  \brief Searches for a text file in a path, and returns an
 *  open file handle to it.
 */
FILE *
FindFile (const char* filebase, const char* path, const char* mode)
{
  size_t len_filebase = strlen (filebase);
  size_t len_path = strlen (path);
  char* buf = oski_Malloc (char, len_path + len_filebase + 2);

  const char* dir_begin = path;
  FILE* fp = NULL;

  if (buf == NULL) /* insufficient memory */
    return NULL;

  while (fp == NULL && dir_begin != NULL && dir_begin[0] != '\0')
    {
      const char* dir_end = GetDirFromPath (dir_begin);
      size_t len_dir = (dir_end > dir_begin) ? (dir_end-dir_begin) : 0;

      if (dir_end == NULL)
	break;
      if (len_dir != 0) /* well-formed directory, as far as we know */
	{
	  strncpy (buf, dir_begin, len_dir);
	  buf[len_dir] = '/';
	  buf[len_dir+1] = '\0';
	  strcat (buf, filebase);
	  fp = fopen (buf, mode);
	}
      if (dir_end[0] != '\0')
	dir_begin = dir_end + 1; /* skip delimiter */
      else
	dir_begin = dir_end;
    }

  oski_Free (buf);
  return fp;
}

/**
 *  \brief Loads a set of site modules from the specified file and
 *  returns a non-zero value on success, or 0 on error.
 *
 *  \pre All arguments are non-NULL.
 */
int
RegisterSiteModules (FILE* fp, const char* mod_type_name,
		     oski_register_funcpt func)
{
  int num_registered = 0;
  assert (fp != NULL && mod_type_name != NULL && func != NULL);
  while (!feof(fp))
    {
#define BUFLEN 2048
      char buf[BUFLEN];
      char* begin_type = buf;
      int id_int = (int)INVALID_ID;
      int id_val = (int)INVALID_ID;
      if (fgets (buf, (BUFLEN-1)*sizeof(char), fp) == NULL) /* eof */
	break;

      /* Parse each line as "%s %d %d" */
      while (begin_type[0] != '\0' && !isspace(begin_type[0]))
	begin_type++;
      if (begin_type[0] != '\0')
	  begin_type[0] = '\0';
      begin_type++;
      sscanf (begin_type, "%d %d", &id_int, &id_val);

      /* Register the parsed module */
      if ((*func) (buf, (oski_id_t)id_int, (oski_id_t)id_val))
        {
          oski_PrintDebugMessage (3, "Registered: '%s' (%d, %d)",
                                  buf, (int)id_int, (int)id_val);
          num_registered++;
        }
    }
  return num_registered;
}

/**
 *  \brief
 *
 *  \returns 0 on error, or a non-zero value.
 */
int
oski_RegisterSiteModules (const char* filebase, const char* mod_type_name,
			  oski_register_funcpt func)
{
  oski_PrintDebugMessage (2, "Site module registration");
  if (filebase == NULL || mod_type_name == NULL || func == NULL)
    return 0;
  else
    {
      const char* path = getenv (OSKI_LIB_PATH);
      FILE* fp;
      int num_registered = 0;

      if (path == NULL)
	path = OSKI_LIB_PATH_DEFAULT;

      oski_PrintDebugMessage (3, "filebase = '%s'", filebase);
      oski_PrintDebugMessage (3, "mod_type_name = '%s'", mod_type_name);
      oski_PrintDebugMessage (3, "func = %p", func);
      oski_PrintDebugMessage (3, "path = '%s'", path);
      
      fp = FindFile (filebase, path, "rt");
      if (fp != NULL)
	{
	  num_registered = RegisterSiteModules (fp, mod_type_name, func);
	  fclose (fp);
	}
      oski_PrintDebugMessage (4, "(Registered %d modules.)",
			      num_registered);
      return num_registered;
    }
}

/* eof */
