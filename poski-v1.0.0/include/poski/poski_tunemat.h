/**
 * \file include/poski/poski_tunemat.h
 * \brief
 *
 */

#ifndef _POSKI_TUNEMAT_H
#define _POSKI_TUNEMAT_H

#include "poski_tunemattype.h"
#include "poski_matrixtype.h"	

/* User interface */
#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

int poski_TuneMat(poski_mat_t A_tunable);

int poski_TuneHint_Structure(poski_mat_t A_tunable, poski_tunehint_t hint, ...);
int poski_TuneHint_MatMult(poski_mat_t A_tunable, poski_operation_t op, poski_value_t alpha, poski_vec_t x_view, poski_value_t beta, poski_vec_t y_view, int num_calls);


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_TUNEMAT_H
