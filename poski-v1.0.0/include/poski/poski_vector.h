/**
 * \file include/poski/poski_vector.h
 * \brief
 *
 */

#ifndef _POSKI_VECTOR_H
#define _POSKI_VECTOR_H

#include "poski_vectortype.h"
#include "poski_matrixtype.h"

#define poski_CreateVec poski_CreateVecView

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

/* User interface */
poski_vec_t poski_CreateVecView( poski_value_t *vec, poski_index_t length, poski_int_t inc, poski_partitionvec_t *partitionvec );
int poski_DestroyVec ( poski_vec_t vec );

int poski_check_TwoVectors(poski_sparse_matrix_t *A, poski_value_t *vec1, poski_value_t *vec2, poski_index_t length);

int poski_report_vec(poski_vec_t vec);
int poski_report_vector(poski_value_t *vec, poski_index_t length, char *name);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_VECTOR_H
