/**
 * \file src/Common/poski_Malloc.c
 * \brief Customized memory allocation routines in the pOSKI library.
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include <poski/poski_config.h>
#include <poski/poski_malloc.h>

//-----------------------------------------------------------------------------------------
// Internal functions

/**
 * \brief Initialize buffer with zeros.
 *
 * \param[in,out] buf Buffer to zero out.
 * \param[in] num_bytes Number of bytes to clear.
 */
void poski_ZeroMem (void *buf, size_t num_bytes)
{
#if HAVE_BZERO
	bzero (buf, num_bytes);
#elif HAVE_MEMSET
	memset (buf, 0, num_bytes);
#endif
}

/**
 *  \brief
 *
 *  \param[in] s String to duplicate.
 *
 *  \returns A pointer to a newly allocated buffer containing a
 *  copy of the input string, s. If s == NULL, returns NULL.
 *  If an error occurs, returns NULL but does not call the
 *  error handler.
 */
char *poski_DuplicateString (const char *s)
{
	char *s_new;
	size_t s_len;

	if (s == NULL)
		return NULL;

	s_len = strlen (s);
	s_new = poski_malloc (char, s_len + 1, 0);
	if (s_new != NULL)
	{
		poski_CopyMem (s_new, s, char, strlen (s));
		s_new[s_len] = 0;
	}

	return s_new;
}


// Internal functions
//-----------------------------------------------------------------------------------------


