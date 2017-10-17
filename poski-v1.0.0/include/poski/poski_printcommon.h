#ifndef _POSKI_PRINTCOMMON_H
#define _POSKI_PRINTCOMMON_H

#ifdef __cplusplus
extern "C" {
#endif	// __cpluspluc

/* Internal */
void LINECUT(int i); 
void TEST_LINECUT(int i);

void poski_printMessage1(unsigned level, const char *fmt, ...);
void poski_InitSetMessageLevel (void);

void poski_InitSetLogFile();
void poski_ResetLogFile();
void poski_CloseLogFile();

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // _POSKI_PRINT_H
