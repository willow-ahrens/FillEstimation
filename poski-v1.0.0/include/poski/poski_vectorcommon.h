#ifndef _POSKI_VECTORCOMMON_H
#define _POSKI_VECTORCOMMON_H

#include "poski_vectortype.h"

#define poski_CreateVec_s oski_CreateVecView

/* Internal */
#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

poski_vec_t poski_Init_Vector(poski_value_t *vec, poski_index_t length, poski_int_t inc, poski_partitionvec_t *partitionvec);

char *poski_findvecprop(poski_vecprop_t vecprop);
char *poski_findvectorstorage(poski_storage_t storage);

int poski_Check_vecprop(poski_vecprop_t vecprop);
int poski_Check_vector(poski_value_t *x, poski_index_t length, poski_int_t inc);
int poski_Check_CreateVecViewInput(poski_value_t *vec, poski_index_t length, poski_int_t inc, poski_partitionvec_t *partitionvec);
int poski_Check_Vector(poski_vec_t vec, poski_index_t length);

void poski_report_subvec(poski_vecview_t subvec);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_VECTORCOMMON_H
