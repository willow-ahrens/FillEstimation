/**
 *  \file include/oski/oski.h
 *  \brief Single header file that includes all the module headers
 *  that end-users should need.
 *  \ingroup PUBLIC
 */

#if !defined(INC_OSKI_H)

/** \name Library build parameters. */
/*@{*/
#if !defined(OSKI_CC)
#  define OSKI_CC "icc"
#endif
#if !defined(OSKI_CFLAGS)
#  define OSKI_CFLAGS "-O3 -std=c99"
#endif
#if !defined(OSKI_LDFLAGS)
#  define OSKI_LDFLAGS " -lm"
#endif
#if !defined(OSKI_BUILD_HOST)
#  define OSKI_BUILD_HOST "i686-pc-linux-gnu"
#endif
#if !defined(OSKI_PREFIX)
#  define OSKI_PREFIX "/home/vuduc2/projects/bebop/oski/src-1.0/_install-icc-Tld_only"
#endif
#if !defined(OSKI_VERSION)
#  define OSKI_VERSION "1.0.1h"
#endif
/*@}*/

#if !defined(DO_NAME_MANGLING)
/** Enable name mangling by default. */
#  define DO_NAME_MANGLING
#endif

#if !defined(DEF_IND_TYPE)
/**
 *  \brief Default integer index type.
 *
 *  The options for integer indices:
 *    -# int
 *    -# long
 */
#  define DEF_IND_TYPE 1
#endif

#if !defined(DEF_VAL_TYPE)
/**
 *  \brief Default non-zero value type.
 *
 *  The options for non-zero values:
 *    -# float
 *    -# double
 *    -# complex float
 *    -# complex double
 */
#  define DEF_VAL_TYPE 2
#endif

#undef IND_TAG_CHAR
#if DEF_IND_TYPE == 1
#  define IND_TAG_CHAR 'i' /**< int */
#elif DEF_IND_TYPE == 2
#  define IND_TAG_CHAR 'l' /**< long */
#else
#  error "Unknown value for DEF_IND_TYPE"
#endif

#undef VAL_TAG_CHAR
#if DEF_VAL_TYPE == 1
#  define VAL_TAG_CHAR 's' /**< float */
#elif DEF_VAL_TYPE == 2
#  define VAL_TAG_CHAR 'd' /**< double */
#elif DEF_VAL_TYPE == 3
#  define VAL_TAG_CHAR 'c' /**< complex_t */
#elif DEF_VAL_TYPE == 4
#  define VAL_TAG_CHAR 'z' /**< doublecomplex_t */
#else
#  error "Unknown value for DEF_VAL_TYPE"
#endif

#endif /* !defined(INC_OSKI_H) */

#if !defined(INC_OSKI_H) || defined(OSKI_UNBIND)
/** include/oski/oski.h included */
#define INC_OSKI_H

#include <oski/common.h>

/* Basics */
#include <oski/matrix.h>
#include <oski/vecview.h>
#include <oski/matcreate.h>

#include <oski/getset.h>

/* Kernels */
#include <oski/matmult.h>
#include <oski/trisolve.h>
#include <oski/ata.h>
#include <oski/a_and_at.h>
#include <oski/matpow.h>

/* Tuning support */
#include <oski/hint.h>
#include <oski/tune.h>

/* Permutations */
#include <oski/perm.h>

#endif /* !defined(INC_OSKI_H) || defined(OSKI_UNBIND) */

#if defined(OSKI_UNBIND)
#  undef INC_OSKI_H
#  undef DEF_IND_TYPE
#  undef DEF_VAL_TYPE
#  undef IND_TAG_CHAR
#  undef VAL_TAG_CHAR
#endif

/* eof */
