/**
 * \file include/poski/poski_tunematcommon.h
 * \brief
 *
 */
#include <stdarg.h>

#ifndef _POSKI_TUNEMATCOMMON_H
#define _POSKI_TUNEMATCOMMON_H

#include "poski_config.h"

#if defined( HAVE_OPENMP_H )
#	include <omp.h>
#endif
#if defined( HAVE_PTHREAD_H )
#	include <pthread.h>
#endif

#include "poski_tunemattype.h"
#include "poski_matrixtype.h"	
#include "poski_vectortype.h"

/* User interface */
#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

int poski_TuneMat_run(poski_mat_t A_tunable);

int poski_TuneHint_Structure_run(poski_mat_t A_tunable, poski_tunehint_t hint, int k, int *r, int *c);
int poski_TuneHint_MatMult_run(poski_mat_t A_tunable, poski_operation_t op, poski_value_t alpha, poski_vec_t x_view, poski_value_t beta, poski_vec_t y_view, int num_calls);

char *poski_GetMatTransforms (const poski_matstruct_t A_tunable);
void poski_TuneMat_status(const poski_matstruct_t A_tunable, int pid, int err);
//int poski_TuneSubMat (poski_matstruct_t A_tunable);
void *oski_CreateMatReprFromCSR (void *mat1, const oski_matcommon_t * props, ...);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_TUNEMATCOMMON_H
