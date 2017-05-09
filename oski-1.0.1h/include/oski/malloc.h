/**
 *  \file oski/malloc.h
 *  \ingroup AUXIL
 *  \brief Customized memory allocation macros that provide
 *  diagnostic information when out-of-memory errors occur.
 *
 *  The memory allocation macros assume C's 'malloc', 'realloc'
 *  and 'free' by default. However, the user may override these
 *  by calling \ref oski_SetMalloc(), \ref oski_SetRealloc(),
 *  and \ref oski_SetFree(), respectively.
 */

#if !defined(INC_OSKI_MALLOC_H)
/** oski/malloc.h has been included. */
#define INC_OSKI_MALLOC_H

/**
 *  \brief Type specifying a 'malloc'-compatible function.
 */
typedef void *(*oski_mallocfunc_t) (size_t);

/**
 *  \brief Type specifying a 'realloc'-compatible function.
 */
typedef void *(*oski_reallocfunc_t) (void *, size_t);

/**
 *  \brief Type specifying a 'free'-compatible function.
 */
typedef void (*oski_freefunc_t) (void *);

/**
 *  \brief Allocate a block of memory, and call the error handler on
 *  error.
 */
void *oski_MallocInternal (const char *elem_type, size_t elem_size,
			   size_t num_elems, const char *source_file,
			   unsigned long line_number);

/**
 *  \brief Reallocate a block of memory, and call the error handler on
 *  error.
 */
int oski_ReallocInternal (void **p_ptr,
			  const char *elem_type, size_t elem_size,
			  size_t num_elems, const char *source_file,
			  unsigned long line_number);

/**
 *  \brief Free a previously allocated block of memory.
 */
void oski_FreeInternal (void *ptr);

/**
 *  \brief Performs multiple memory allocations.
 */
int oski_MultiMalloc (const char *file, unsigned long line, int k, ...);

/**
 *  \brief Customized macro for memory allocation.
 *
 *  \param[in] elem_type  Type of the element to allocate.
 *  \param[in] num_elems  Number of such elements to allocate.
 *
 *  This macro allocates a block of memory using the current
 *  memory allocation routine (see #oski_GetMalloc and
 *  #oski_SetMalloc). This block logically contains #num_elems
 *  elements, each of type #elem_type.
 *
 *  On error, this macro executes the default global error
 *  handler with information about the allocation error.
 *
 *  \returns Returns pointer to the new block, or NULL on error.
 */
#define oski_Malloc( elem_type, num_elems ) \
	(elem_type *)oski_MallocInternal( \
		#elem_type, sizeof(elem_type), (num_elems), __FILE__, __LINE__ )

/**
 *  \brief Customized macro for memory allocation with no error reporting.
 *
 *  This macro is identical to #oski_Malloc() except that it does
 *  not call the error handler on error.
 */
#define oski_MallocNoError( elem_type, num_elems ) \
	(elem_type *)oski_MallocInternal( \
		#elem_type, sizeof(elem_type), (num_elems), NULL, -1 )

/**
 *  \brief Customized macro for memory re-allocation.
 *
 *  \param[in,out] ptr_addr Address of a pointer to an allocated block.
 *  \param[in] elem_type Type of each element in the block.
 *  \param[in] num_elems Size of the new block, in elements.
 *  \returns If the block can be resized, this routine returns 0 and
 *  changes ptr to be equal the starting address of the new block.
 *  Otherwise (on error), returns an error code, calls the current
 *  handler, and leaves the original memory block unchanged.
 *
 *  Like the C-routine 'realloc', this routine copies as much data as
 *  possible from the previous block to the new block.
 *
 *  On error, this routine returns an error code (integer less than 0)
 *  and calls the current error handler.
 */
#define oski_Realloc( ptr_addr, elem_type, num_elems ) \
	oski_ReallocInternal( ptr_addr, #elem_type, sizeof(elem_type), \
		num_elems, __FILE__, __LINE__ )

/**
 *  \brief Customized macro for memory release.
 *
 *  \param ptr Address of object, allocated using #oski_Malloc, to free.
 *
 *  This macro frees the given memory block using the current
 *  free-memory routine (see #oski_GetFree and #oski_SetFree).
 */
#define oski_Free( ptr ) oski_FreeInternal( ptr )

/**
 *  \brief Free a list of pointers.
 */
void oski_FreeAll (int k, ...);

/**
 *  \brief Returns the current memory allocation function.
 */
oski_mallocfunc_t oski_GetMalloc (void);

/**
 *  \brief Returns the current memory reallocation routine.
 */
oski_reallocfunc_t oski_GetRealloc (void);

/**
 *  \brief Returns the function used to free allocated memory.
 */
oski_freefunc_t oski_GetFree (void);

/**
 *  \brief Change the current memory allocation function.
 */
oski_mallocfunc_t oski_SetMalloc (oski_mallocfunc_t new_func);

/**
 *  \brief Change the current memory re-allocation function.
 */
oski_reallocfunc_t oski_SetRealloc (oski_reallocfunc_t new_func);

/**
 *  \brief Change the function used to free allocated memory.
 */
oski_freefunc_t oski_SetFree (oski_freefunc_t new_func);

/**
 *  \brief Zero out bytes in a buffer.
 */
void oski_ZeroMem (void *buf, size_t num_bytes);

#endif /* !defined(INC_OSKI_MALLOC_H) */

/* eof */
