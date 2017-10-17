#ifndef _POSKI_TEST_H
#define _POSKI_TEST_H

#include "poski_commontype.h"
#include "poski_matrixtype.h"

typedef enum
{
	ZERO,
	RANDOM
}poski_value_type_t;

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

/******************************************************************************/
/* User routine space */
/******************************************************************************/
poski_sparse_matrix_t *SetTestSampleSparseMatrix();
void poski_Destroy_CSR(poski_sparse_matrix_t *A);
poski_value_t *CreateTestSingleVec(int length, poski_value_type_t vtype);

int poski_NavieMatMultCSR(poski_sparse_matrix_t *A, poski_value_t alpha, poski_value_t *x, poski_value_t beta, poski_value_t *y, poski_operation_t op);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_TEST_H
