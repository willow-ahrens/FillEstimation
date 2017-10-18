/**
 *  \file src/malloc.c
 *  \ingroup AUXIL
 *  \brief Customized memory allocation module.
 *
 *  This module implements the memory allocation support routines,
 *  including routines to change the memory allocation and
 *  free memory routines.
 *
 *  NOTE: These routines are not fully thread-safe; the bodies
 *  of #oski_SetMalloc, #oski_SetFree, and #oski_SetRealloc
 *  need to implement a mutual exclusion protocol.
 *
 *  \todo When alloc routines are changed, also change LTDL's
 *  alloc routines.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <oski/common.h>
#include <oski/config.h>

/**
 *  \brief
 *
 *  \param[in,out] buf Buffer to zero out.
 *  \param[in] num_bytes Number of bytes to clear.
 */
void
oski_ZeroMem (void *buf, size_t num_bytes)
{
#if HAVE_BZERO
  bzero (buf, num_bytes);
#elif HAVE_MEMSET
  memset (buf, 0, num_bytes);
#endif
}

/**
 *  \name Global pointers to memory allocation functions.
 *
 *  These pointers should not be manipulated directly, but modified
 *  via calls to #oski_SetMalloc, #oski_SetRealloc, and
 *  #oski_SetFree.
 */
/*@{*/
static oski_mallocfunc_t gfuncp_oski_Malloc = malloc;
static oski_reallocfunc_t gfuncp_oski_Realloc = realloc;
static oski_freefunc_t gfuncp_oski_Free = free;
/*@}*/

/**
 *  \brief
 *
 *  \param[in] elem_type Name of the type of element being allocated
 *  (descriptive string).
 *  \param[in] elem_size Size of each element, in bytes.
 *  \param[in] num_elems Number of such elements to allocate.
 *  \param[in] source_file Source file of the caller (for debugging).
 *  \param[in] line_number Line number at which the call is being made
 *  (for debugging).
 *  \returns A pointer to the newly allocated and zeroed block,
 *  or NULL on error.
 *
 *  This routine is for the library's internal use only and should
 *  not normally be called directly by users of the library.
 *
 *  If source_file == NULL, then this routine does not call the
 *  error handler on failure.
 */
void *
oski_MallocInternal (const char *elem_type, size_t elem_size,
		     size_t num_elems, const char *source_file,
		     unsigned long line_number)
{
  void *new_block;

  if ((elem_size * num_elems) == 0)
    return NULL;

  new_block = gfuncp_oski_Malloc (elem_size * num_elems);
  if (new_block == NULL)
    {
      if (source_file != NULL)
	oski_HandleError (ERR_OUT_OF_MEMORY,
			  "Out of memory",
			  source_file, line_number,
			  "Could not allocate space for %u elements of type '%s', "
			  "each of size %u bytes.\n",
			  num_elems, elem_type, elem_size);
      /* else, silently return */
    }
  else
    oski_ZeroMem (new_block, elem_size * num_elems);

  return new_block;
}

/**
 *  \brief
 *
 *  \param[in,out] p_old_block  Pointer to an existing allocated block.
 *  \param[in] elem_type  Type of each logical element stored in the block.
 *  \param[in] elem_size  Size of each logical element, in bytes.
 *  \param[in] num_elems  New no. of elems to store in this block.
 *  \param[in] source_file  Logical source file of the caller (for debugging).
 *  \param[in] line_number  Logical line number of the realloc call
 *  (for debugging).
 *  \returns 0 on success, or a negative error code otherwise.
 *
 *  This routine is for the library's internal use only and should
 *  not normally be called directly by users of the library.
 *
 *  \todo Implement oski_ReallocInternal().
 */
int
oski_ReallocInternal (void **p_old_block, const char *elem_type,
		      size_t elem_size, size_t num_elems,
		      const char *source_file, unsigned long line_number)
{
  size_t num_bytes = elem_size * num_elems;
  void *new_block = gfuncp_oski_Realloc (*p_old_block,
					 elem_size * num_elems);

  if (new_block == NULL && num_bytes)
    {
      if (source_file != NULL)
	oski_HandleError (ERR_OUT_OF_MEMORY,
			  "Out of memory",
			  source_file, line_number,
			  "Could not reallocate space for %u elements of type '%s', "
			  "each of size %u bytes.\n",
			  num_elems, elem_type, elem_size);
      return ERR_OUT_OF_MEMORY;
    }

  *p_old_block = new_block;
  return 0;
}

static int
MultiMalloc (const char *file, unsigned long line, int k, va_list ap)
{
  size_t num_bytes;
  void **p_dest;
  int err;

  if (k == 0)
    return 0;

  num_bytes = va_arg (ap, size_t);
  p_dest = va_arg (ap, void **);

  if (num_bytes > 0)
    {
      if (p_dest == NULL)
	return ERR_BAD_ARG;
      *p_dest = oski_MallocInternal ("void *", 1, num_bytes, file, line);
      if (*p_dest == NULL)
	return ERR_OUT_OF_MEMORY;
    }
  else if (p_dest != NULL)
    *p_dest = NULL;

  err = MultiMalloc (file, line, k - 1, ap);
  if (err && p_dest != NULL && *p_dest != NULL)
    oski_Free (*p_dest);
  return err;
}

/**
 *  \brief
 *
 *  Attempts to perform \f$k\f$ memory allocations. Returns a 0 and
 *  pointers to all allocated buffers on success. Otherwise, returns
 *  an error code and allocates no buffers.
 *
 *  For example, the call:
 *  \code
 *     size_t m_bytes = sizeof(double) * m;
 *     double* pm;
 *     size_t n_bytes = sizeof(char) * n;
 *     char* pn;
 *     size_t k_bytes = sizeof(struct my_object) * k;
 *     struct my_object* pk;
 *
 *     int err = oski_MultiMalloc( file, line, 3,
 *                  m_bytes, &pm, n_bytes, &pn, k_bytes, &pk );
 *  \endcode
 *  allocates an array of 'm' doubles, 'n' characters, and
 *  'k' objects. The value 'err' is 0 on success, and 1 on
 *  error. Note that the array sizes must be of type 'size_t'.
 */
int
oski_MultiMalloc (const char *file, unsigned long line, int k, ...)
{
  va_list ap;
  int retval;

  if (k < 0)
    {
      OSKI_ERR (oski_MultiMalloc, ERR_BAD_ARG);
      return ERR_BAD_ARG;
    }

  va_start (ap, k);
  retval = MultiMalloc (file, line, k, ap);
  va_end (ap);
  return retval;
}

/**
 *  \brief
 *
 *  \param[in,out] ptr  Pointer to block of memory to free, previously
 *  created by a call to #oski_MallocInternal.
 *
 *  This routine frees the given block of memory only if it is
 *  not NULL. Otherwise, no action takes place (and no error
 *  is generated).
 *
 *  This routine is for the library's internal use only and should
 *  not normally be called directly by users of the library.
 *
 *  \post ptr is no longer usable.
 */
void
oski_FreeInternal (void *ptr)
{
  if (ptr != NULL)
    gfuncp_oski_Free (ptr);
}

void
oski_FreeAll (int k, ...)
{
  va_list ap;
  int i;

  if (k <= 0)
    return;

  va_start (ap, k);
  for (i = 0; i < k; i++)
    {
      void *ptr = va_arg (ap, void *);
      oski_Free (ptr);
    }
  va_end (ap);
}

oski_mallocfunc_t
oski_GetMalloc (void)
{
  return gfuncp_oski_Malloc;
}

oski_reallocfunc_t
oski_GetRealloc (void)
{
  return gfuncp_oski_Realloc;
}

oski_freefunc_t
oski_GetFree (void)
{
  return gfuncp_oski_Free;
}

/**
 *  \brief
 *
 *  \param new_func  New 'malloc'-compatible function.
 *  \returns Previous function used to allocate memory.
 *
 *  If new_func == NULL, then the memory allocation
 *  function is restored to the default, the C 'malloc'
 *  routine.
 *
 *  \post If new_func == NULL, then gfuncp_oski_Malloc == new_func
 *  else gfuncp_oski_Malloc == new_func.
 */
oski_mallocfunc_t
oski_SetMalloc (const oski_mallocfunc_t new_func)
{
  oski_mallocfunc_t old_func = oski_GetMalloc ();
  if (new_func == NULL)
    gfuncp_oski_Malloc = malloc;
  else
    gfuncp_oski_Malloc = new_func;
  return old_func;
}

/**
 *  \brief
 *
 *  \param new_func  New 'realloc'-compatible function.
 *  \returns Previous function used to allocate memory.
 *
 *  If new_func == NULL, then the memory allocation
 *  function is restored to the default, the C 'realloc'
 *  routine.
 *
 *  \post If new_func == NULL, then gfuncp_oski_Realloc == new_func
 *  else gfuncp_oski_Realloc == new_func.
 */
oski_reallocfunc_t
oski_SetRealloc (const oski_reallocfunc_t new_func)
{
  oski_reallocfunc_t old_func = oski_GetRealloc ();
  if (new_func == NULL)
    gfuncp_oski_Realloc = realloc;
  else
    gfuncp_oski_Realloc = new_func;
  return old_func;
}

/**
 *  \brief
 *
 *  \param new_func  New 'free'-compatible function.
 *  \returns Previous function used to allocate memory.
 *
 *  If new_func == NULL, then the memory allocation
 *  function is restored to the default, the C 'free'
 *  routine.
 *
 *  \post If new_func == NULL, then gfuncp_oski_Free == new_func
 *  else gfuncp_oski_Free == new_func.
 */
oski_freefunc_t
oski_SetFree (const oski_freefunc_t new_func)
{
  oski_freefunc_t old_func = oski_GetFree ();
  if (new_func == NULL)
    gfuncp_oski_Free = free;
  else
    gfuncp_oski_Free = new_func;
  return old_func;
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
char *
oski_DuplicateString (const char *s)
{
  char *s_new;
  size_t s_len;

  if (s == NULL)
    return NULL;

  s_len = strlen (s);
  s_new = oski_Malloc (char, s_len + 1);
  if (s_new != NULL)
    {
      oski_CopyMem (s_new, s, char, strlen (s));
      s_new[s_len] = 0;
    }

  return s_new;
}

/* eof */
