#ifndef _POSKI_PERMUTE_H
#define _POSKI_PERMUTE_H

#include "poski_commontype.h"
#include "poski_vectortype.h"

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

int poski_Init_GlobalPermute(poski_index_t *permute);
int poski_report_permute(poski_index_t *permute, char *name);
int poski_permuteMatMult(poski_vec_t vec);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_PERMUTE_H
