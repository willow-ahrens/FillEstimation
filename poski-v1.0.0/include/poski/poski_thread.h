/**
 * \file include/poski/poski_thread.h
 * \brief
 *
 */

#ifndef _POSKI_THREAD_H
#define _POSKI_THREAD_H

#include "poski_threadtype.h"
#include "poski_matrixtype.h"

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

/* User Interface */
poski_threadarg_t *poski_InitThreads ();
int poski_DestroyThreads (poski_threadarg_t *args);
int poski_ThreadHints(poski_threadarg_t *threadargs, poski_mat_t A_tunable, poski_threadtype_t ttype, poski_int_t nthreads);

int poski_report_threadmodel(poski_threadarg_t *threadargs, poski_mat_t A);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_THREAD_H
