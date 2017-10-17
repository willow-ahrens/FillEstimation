/**
 *  \file ltdlstub.c
 *  \brief Stub file to define lt_preloaded_symbols in the case when
 *  we do not link using libtool.
 *
 *  This file is only used on MacOS X/Darwin when building liboski.la,
 *  because all external symbols are required.
 */

#include <libltdl/ltdl.h>

/**
 *  \brief Contains preloaded symbols (only needed if libtool is not used
 *  to link against BeBOP-OSKI).
 */
const lt_dlsymlist lt_preloaded_symbols[] = { { 0, 0 } };

/* eof */
