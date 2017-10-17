#ifndef _POSKI_LOADMATTYPE_H
#define _POSKI_LOADMATTYPE_H

#include "poski_commontype.h"

#define CacheLineSizeInDoubles 8

#define CRT_DONE "\033[1;32;40m"
#define CRT_ERROR "\033[5;31;40m"
#define CRT_RESET "\033[0m"

typedef enum
{
	MM = 0,
	HB = 1
} poski_MatFormat_t;

/* \brief nonzeronode handle type\ */
struct NonZeroNode_struct
{
	poski_index_t Row;
	poski_index_t Col;
#ifdef READ_VALUES
	poski_value_t value;
#endif
	struct NonZeroNode_struct *next;
};
typedef struct NonZeroNode_struct NonZeroNode; 

#endif // _POSKI_LOADMATTYPE_H

