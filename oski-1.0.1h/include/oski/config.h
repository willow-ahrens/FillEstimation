/**
 *  \file oski/config.h
 *  \ingroup AUXIL
 *  \brief Contains macros and definitions that depend on the specific
 *  system on which the library is being compiled.
 *
 *  This file includes the configure-generated 'config.h' file
 *  and takes actions based on the system build-time results.
 *  If at all possible, this file should only be used at library
 *  build time, i.e., no installed BeBOP library headers should
 *  include this file.
 */

#if !defined(INC_OSKI_CONFIG_H)
#define INC_OSKI_CONFIG_H

#if HAVE_CONFIG_H
#include "config/config.h"
#endif

#endif

/* eof */
