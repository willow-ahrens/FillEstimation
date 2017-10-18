/**
 *  \file oski/memcpy.h
 *  \brief Macros for typed block memory copy operations.
 */

#if !defined(INC_OSKI_MEMCPY_H)
/** oski/memcpy.h included. */
#define INC_OSKI_MEMCPY_H

#include <string.h>

/**
 *  \brief Copy a typed block of consecutive memory elements from one
 *  buffer to another.
 *
 *  This implementation is simply a wrapper around memcpy.
 *
 *  \param[out] dest Buffer into which to copy.
 *  \param[in] src Buffer from which to copy.
 *  \param[in] num_elems Integer number of logical elements.
 *  \param[in] elem_type Type of each logical element.
 *  \returns A typed pointer to the destination buffer.
 */
#define oski_CopyMem( dest, src, elem_type, num_elems ) \
	memcpy((void *)(dest), (const void *)(src), \
		(num_elems) * sizeof(elem_type))

/**
 *  \brief Returns a newly allocated copy of the NULL-terminated
 *  string 's'.
 */
char *oski_DuplicateString (const char *s);

#endif

/* eof */
