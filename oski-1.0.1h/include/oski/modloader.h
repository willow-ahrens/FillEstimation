/**
 *  \file oski/modloader.h
 *  \ingroup AUXIL
 *  \brief Shared library module loader.
 *
 *  To prevent our users' dependence on our use of libltdl
 *  this file must be explicitly included by a source file
 *  when needed.
 */

#if !defined(INC_OSKI_MODLOADER_H)
/* oski/modloader.h has been included. */
#define INC_OSKI_MODLOADER_H

#include <libltdl/ltdl.h>

/** Handle to a shared library module */
typedef lt_dlhandle oski_modhandle_t;

/**
 *  \brief Initialize the dynamic/shared library loader.
 */
int oski_InitModuleLoader (void);

/**
 *  \brief Shutdown the dynamic/shared library loader.
 */
void oski_CloseModuleLoader (void);

/**
 *  \brief Load a library module.
 */
oski_modhandle_t oski_LoadModule (const char *name);

/**
 *  \brief Load a library module, but do not call the error
 *  handler if the module does not exist.
 */
oski_modhandle_t oski_LoadModuleOptional (const char *name);

/**
 *  \brief Execute a loaded module's initialization code, if any.
 */
void oski_CallModuleInit (oski_modhandle_t module);

/**
 *  \brief Execute a loaded module's shutdown code, if any.
 */
void oski_CloseModule (oski_modhandle_t module);

/**
 *  \brief Unload a library module.
 */
int oski_UnloadModule (oski_modhandle_t module);

/**
 *  \brief Find a routine within a shared library module.
 */
void *oski_LookupModuleMethod (const oski_modhandle_t module,
			       const char *method_name);

/**
 *  \brief Returns a newly allocated NULL-terminated string describing
 *  the matrix type, used primarily for debugging purposes.
 */
char *oski_MakeModuleDesc (const char *name, oski_id_t ind, oski_id_t val);

/**
 *  \brief Returns a newly allocated NULL-terminated string specifying
 *  the shared library module name for the indicated matrix type.
 */
char *oski_MakeModuleName (const char *mod_type_name, const char *name,
			   oski_id_t ind, oski_id_t val);

/**
 *  \brief Module registration routine.
 */
typedef oski_id_t (*oski_register_funcpt) (const char *, oski_id_t, oski_id_t);

/**
 *  \brief Load site modules.
 */
int oski_RegisterSiteModules (const char* file, const char* mod_type_name,
			      oski_register_funcpt func);

#endif

/* eof */
