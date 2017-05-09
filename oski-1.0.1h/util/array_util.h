/**
 *  \file util/array_util.h
 *  \brief Some array manipulation utility functions for the test suite.
 */

#if !defined(INC_TESTS_ARRAY_UTIL_H)
/** util/array_util.h included. */
#define INC_TESTS_ARRAY_UTIL_H

#include <oski/common.h>
#include <oski/mangle.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define array_Duplicate MANGLE_(array_Duplicate)
#define array_Destroy MANGLE_(array_Destroy)
#define array_CompareElems MANGLE_(array_CompareElems)
#define array_CompareElems_NoAbort MANGLE_(array_CompareElems_NoAbort)
#define array_FillRandom MANGLE_(array_FillRandom)
#define array_CreateRandom MANGLE_(array_CreateRandom)
#define array_Create MANGLE_(array_Create)
#define array_SortIndValPairs MANGLE_(array_SortIndValPairs)
/*@}*/
#endif

/**
 *  \brief Returns a pointer to a newly allocated copy of the
 *  given array, or NULL on error.
 */
oski_value_t *array_Duplicate (const oski_value_t * x0, oski_index_t len);

/**
 *  \brief Release previously allocated array.
 */
void array_Destroy (oski_value_t * x);

/**
 *  \brief Compare elements of two arrays, and exit the program
 *  with an error code of 1 if the element-wise difference greater
 *  than len*(machine epsilon) is detected.
 */
void array_CompareElems (const char *test_func,
			 const oski_value_t * x, const oski_value_t * y,
			 oski_index_t len);

/**
 *  \brief Compare elements of two arrays, and exit the program
 *  with an error code of 1 if the element-wise difference greater
 *  than len*(machine epsilon) detected.
 *
 *  This routine is identical to array_CompareElems(), except that
 *  it does not abort the program if x and y are numerically
 *  inconsistent. (It does abort on memory errors.)
 */
int array_CompareElems_NoAbort (const char *test_func,
				const oski_value_t * x,
				const oski_value_t * y, oski_index_t len);

/**
 *  \brief Initialize an array with random values between -1 and 1.
 */
void array_FillRandom (oski_value_t * x, oski_index_t n);

/**
 *  \brief Allocate an array of random values between -1 and 1.
 */
oski_value_t *array_CreateRandom (oski_index_t n);

/**
 *  \brief Allocate an array with elements initialized to
 *  a particular value.
 */
oski_value_t *array_Create (oski_index_t n, oski_value_t val);

/**
 *  \brief Sorts an (index, value) pair of arrays.
 */
void array_SortIndValPairs (oski_index_t * ind, oski_value_t * val,
			    oski_index_t n);

#endif

/* eof */
