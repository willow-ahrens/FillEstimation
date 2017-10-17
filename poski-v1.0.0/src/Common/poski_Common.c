/**
 * \file src/Common/poski_Common.c
 * \brief Commonly used routines in the pOSKI library.
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include <poski/poski_commontype.h>

//-----------------------------------------------------------------------------------------
// Internal functions
/**
 * \brief Check that two lengths are equal or not.
 * \param[in] length1 The first length.
 * \param[in] length2 The second length.
 * \return 0 if two input lengths are equal, -1 otherwise.
 */ 
int poski_CheckLength(poski_index_t length1, poski_index_t length2)
{
	if (length1 < 1 || length2 < 1) return -1;
	if (length1 != length2) return -1;
	return 0;	
}



// Internal functions
//-----------------------------------------------------------------------------------------


