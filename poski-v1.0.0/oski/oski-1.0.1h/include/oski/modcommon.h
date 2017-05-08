/**
 *  \file oski/modcommon.h
 *  \ingroup AUXIL
 *  \brief Definitions and structures common to dynamically shared modules.
 */

#if !defined(INC_OSKI_MODCOMMON_H)
/** oski/modcommon.h included. */
#define INC_OSKI_MODCOMMON_H

#include <oski/mangle.h>

#if defined(DO_NAME_MANGLING)
/** \name Mangled names. */
/*@{*/
#define oski_InitModule    MANGLE_MOD_(oski_InitModule)
#define oski_CloseModule   MANGLE_MOD_(oski_CloseModule)
#define oski_GetShortDesc  MANGLE_MOD_(oski_GetShortDesc)
#define oski_GetLongDesc   MANGLE_MOD_(oski_GetLongDesc)
/*@}*/
#endif

/**
 *  \brief Method: Returns a short string description of the module.
 */
const char *oski_GetShortDesc (void);

/**
 *  \brief Method: Returns a "long" string description of the module.
 */
const char *oski_GetLongDesc (void);

/**
 *  \brief Method: Initialize the module.
 */
void oski_InitModule (void);

/**
 *  \brief Method: Shutdown the module.
 */
void oski_CloseModule (void);

#endif

/* eof */
