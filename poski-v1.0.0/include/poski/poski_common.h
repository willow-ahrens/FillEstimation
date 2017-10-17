/** 
 * \file include/poski/poski_common.h
 * \brief 
 *
 */

#ifndef _POSKI_COMMON_H
#define _POSKI_COMMON_H

#include "poski_commontype.h"

#define pcheck(i) printf("check point %d\n",i);
#define ptcheck(i) printf("check pointer %d\n",i);

#define poski_copyarg(dst, src, type) memcpy(dst, src, sizeof(type))

#define POSKI_UPDATE_VALUE(des, src) des = src

#define POSKI_MAKENAME(ftype, ttype, ptype, kernel, op, prop) poski_ ## ftype ## _ ## ttype ## ptype ## kernel ## op ## prop

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

int poski_CheckLength(poski_index_t length1, poski_index_t length2);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_COMMON_H


