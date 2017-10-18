/** 
 * \file include/poski/poski_malloc.h
 * \brief
 *
 */

#ifndef _POSKI_MALLOC_H
#define _POSKI_MALLOC_H

#include <string.h>
#include "poski_commontype.h"

#define poski_malloc(type, size, pid) (type *)malloc(sizeof(type)*size) 
	
#define poski_free(ptr) \
{	\
	if (ptr!=NULL) free(ptr);	\
}

#define poski_CopyMem( dest, src, elem_type, num_elems ) \
	memcpy((void *)(dest), (const void *)(src), \
		(num_elems) * sizeof(elem_type))

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

void poski_ZeroMem(void *buf, size_t num_bytes);
char *poski_DuplicateString (const char *s);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_MALLOC_H

