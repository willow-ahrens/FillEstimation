#ifndef _POSKI_THREADCOMMON_H
#define _POSKI_THREADCOMMON_H

#include "poski_threadtype.h"
#include "poski_threadnuma.h"

/* Internal */

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

int poski_Copy_threadargs(poski_threadarg_t *dest, poski_threadarg_t *src);
int poski_Check_threadargs(poski_threadarg_t *threadargs);
int poski_Check_numthreads(poski_int_t nthreads);
int poski_Check_threadingmodeltype(poski_threadtype_t ttype);

char *poski_findthreadtype(poski_threadtype_t ttype);

poski_threadpool_t *poski_CreateThreadPool(poski_int_t nthreads);
int poski_Destroy_threadpool(poski_threadpool_t *thread);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_THREADCOMMON_H
