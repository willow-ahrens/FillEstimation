/**
 * \file include/poski/poski_print.h
 * \brief
 *
 */

#ifndef _POSKI_PRINT_H
#define _POSKI_PRINT_H

#include "poski_printcommon.h"

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

/* User interface */
void poski_error(const char *func_name, const char *msg, ...);
void poski_warning(const char *func_name, const char *msg, ...);
void poski_printMessage(unsigned level, const char *fmt, ...);
void poski_printContinue();
/* Test */
int test_main_poski_LogFileHandler();

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_PRINT_H
