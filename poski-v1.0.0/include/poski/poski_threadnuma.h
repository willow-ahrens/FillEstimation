#ifndef _POSKI_THREADNUMA_H
#define _POSKI_THREADNUMA_H

#include "poski_config.h"

/* Internal */
#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

int poski_SetAffinity_np(int tid);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_THREADCOMMON_H
