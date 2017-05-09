/**
 *  \file oski/common.h
 *  \brief A maximal set of system-independent prototypes and
 *  definitions widely useful to all of the BeBOP library's
 *  sub-modules.
 */

#if !defined(INC_OSKI_COMMON_H)
/** \brief oski/common.h has been included. */
#define INC_OSKI_COMMON_H

/**
 *  \brief Define macros to help prevent name-mangling when
 *  using a C++ compiler. 
 *
 *  A region of code enclosed in #DECL_C_BEGIN and #DECL_C_END macros
 *  are declared to be external C symbols.
 */
#if defined(__cplusplus)	/* Using a C++ compiler? */
#if !defined(DECL_C_BEGIN)
		/** Start of a region of C symbols. */
#define DECL_C_BEGIN extern "C" {
#endif

#if !defined(DECL_C_END)
		/** End of a region of C symbols. */
#define DECL_C_END }
#endif
#else
	/** Start of a region of C symbols. */
#define DECL_C_BEGIN
	/** End of a region of C symbols. */
#define DECL_C_END
#endif

/** "Stringify" argument */
#define MAKE_STRING(x)  #x

/** Expand argument macro, then stringify */
#define MACRO_TO_STRING(x) MAKE_STRING(x)

/** Min of a pair of arguments */
#define OSKI_MIN(a, b)  ((a) < (b) ? (a) : (b))

#include <stdlib.h>		/* for size_t */

#include <oski/malloc.h>
#include <oski/memcpy.h>
#include <oski/sprintf.h>
#include <oski/error.h>
#include <oski/debug.h>

/** \brief Integer identification number type. */
typedef size_t oski_id_t;

/** \brief Invalid id number. */
#define INVALID_ID  ((oski_id_t)0)

/**
 *  \brief Make a function pointer type name of the form,
 *  oski_[name]_funcpt.
 */
#define OSKI_MAKENAME_FUNCPT(name) oski_ ## name ## _funcpt


#include <oski/userconst.h>

#include <oski/scalinfo.h>
#include <oski/kerinfo.h>
#include <oski/mattypes.h>

#include <oski/init.h>

#include <oski/structhint.h>

#include <oski/timer.h>
#include <oski/heur.h>

/**
 *  \brief Returns the ceiling of a / b, where a and b are integer types.
 *
 *  This macro doesn't work if a and b are not integers.
 */
#define INT_CEIL_DIV(a, b)  ((a) / (b)) + (((a) % (b)) > 0)

#endif /* !defined(INC_OSKI_COMMON_H) */

/* eof */
