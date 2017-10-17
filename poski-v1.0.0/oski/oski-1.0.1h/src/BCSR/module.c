/**
 *  \file src/BCSR/module.c
 *  \ingroup MATTYPE_BCSR
 *  \brief Block compressed sparse row (BCSR) module.
 */

#include <oski/config.h>
#include <oski/common.h>
#include <oski/modloader.h>
#include <oski/BCSR/format.h>
#include <oski/BCSR/module.h>

const char *
oski_GetShortDesc (void)
{
  return "Block compressed sparse row (BCSR) module";
}

const char *
oski_GetLongDesc (void)
{
  return
    "This module implements aligned block compressed sparse row (BCSR) format.";
}

void
oski_InitModule (void)
{
  oski_PrintDebugMessage (2, "Loading BCSR(ind_t=%s, val_t=%s)",
			  MACRO_TO_STRING (IND_T), MACRO_TO_STRING (VAL_T));
  oski_PrintDebugMessage (3, oski_GetShortDesc ());
}

void
oski_CloseModule (void)
{
  oski_PrintDebugMessage (2, "Shutting down BCSR(%s, %s) module",
			  MACRO_TO_STRING (IND_T), MACRO_TO_STRING (VAL_T));
}

/**
 *  \brief
 *
 *  This routine loads the requested method if it exists, and
 *  caches the \f$r\times c\f$ module if it has not yet been
 *  loaded.
 *
 *  \param[in] A An \f$r\times c\f$ BCSR matrix representation.
 *  \param[in] name Name of the method to load from the
 *  \f$r\times c\f$ submodule.
 *
 *  \returns A pointer to the requested method, or NULL if none
 *  could be found.
 */
void *
BCSR_GetKernel (const oski_matBCSR_t * A, const char *name)
{
  oski_modhandle_t mod;

  if (A == NULL)
    return NULL;

  /* Cache module */
  if (A->mod_cached)
    mod = (oski_modhandle_t) A->mod_cached;
  else
    mod = ((oski_matBCSR_t *) A)->mod_cached = oski_LoadModule (A->mod_name);

  if (mod != NULL)
    {
      /* Load and return method */
      char *method_name = oski_StringPrintf ("%s_%dx%d", name,
					     A->row_block_size,
					     A->col_block_size);
      if (method_name != NULL)
	{
	  void *func = oski_LookupModuleMethod (mod, method_name);
	  oski_Free (method_name);
	  return func;
	}
    }
  return NULL;
}

/* eof */
