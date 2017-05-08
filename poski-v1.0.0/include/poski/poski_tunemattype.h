#ifndef _POSKI_TUNEMATTYPE_H
#define _POSKI_TUNEMATTYPE_H

#include "poski_commontype.h"
#include "oski/oski.h"

#define poski_tunehint_t oski_tunehint_t
#define poski_traceargs_MatMult_t oski_traceargs_MatMult_t

#define POSKI_ERR_BAD_HINT OSKI_ERR_BAD_HINT

#define HINT_SINGLE_BLOCK HINT_SINGLE_BLOCKSIZE
#define HINT_MULTIPLE_BLOCKS HINT_MULTIPLE_BLOCKSIZES
#define SYMBOLIC_VECTOR (poski_vec_t)SYMBOLIC_VEC

#define POSKI_KERNEL_MatMult OSKI_KERNEL_MatMult

#define poski_check_tunehint OSKI_CHECK_TUNEHINT

#define poski_TuneSubMat oski_TuneMat
#define poski_CheckArgsMatMult oski_CheckArgsMatMult
#define poski_MakeArglistMatMult oski_MakeArglistMatMult
#define poski_RecordCalls oski_RecordCalls

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_TUNEMATTYPE_H
