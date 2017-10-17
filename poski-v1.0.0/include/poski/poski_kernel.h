/**
 * \file include/poski/poski_kernel.h
 * \brief
 *
 */

#ifndef _POSKI_KERNEL_H
#define _POSKI_KERNEL_H

#include "poski_kerneltype.h"
#include "poski_matrixtype.h"
#include "poski_vectortype.h"

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

/* User Interface */
int poski_MatMult(poski_mat_t A, poski_operation_t op, poski_value_t alpha, poski_vec_t x, poski_value_t beta, poski_vec_t y);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_KERNEL_H
