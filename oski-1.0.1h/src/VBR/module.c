/**
 *  \file src/VBR/module.c
 *  \ingroup MATTYPE_VBR
 *  \brief Variable block row (VBR) implementation.
 */

#include <oski/config.h>

#include <stdarg.h>

#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/VBR/format.h>
#include <oski/VBR/module.h>
#include <oski/CSR/module.h>

const char *
oski_GetShortDesc (void)
{
  return "Variable block row (VBR) implementation";
}

const char *
oski_GetLongDesc (void)
{
  return oski_GetShortDesc ();
}

void
oski_InitModule (void)
{
  oski_PrintDebugMessage (2, "Loading VBR(ind_t=%s, val_t=%s)",
			  MACRO_TO_STRING (IND_T), MACRO_TO_STRING (VAL_T));
  oski_PrintDebugMessage (3, oski_GetShortDesc ());
}

void
oski_CloseModule (void)
{
  oski_PrintDebugMessage (2, "Shutting down VBR(%s, %s) module",
			  MACRO_TO_STRING (IND_T), MACRO_TO_STRING (VAL_T));
}

/* eof */
