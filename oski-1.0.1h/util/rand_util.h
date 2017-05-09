/**
 *  \file util/rand_util.h
 *  \brief Wrappers around the available random number generators.
 */

#if !defined(INC_UTIL_RAND_UTIL_H)
/** util/rand_util.h included. */
#define INC_UTIL_RAND_UTIL_H

#include <oski/common.h>
#include <oski/mangle.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define rand_GenIndex MANGLE_(rand_GenIndex)
#define rand_GenValue MANGLE_(rand_GenValue)
#define rand_GenValueReal MANGLE_(rand_GenValueReal)
#define rand_SortIndices MANGLE_(rand_SortIndices)
#define rand_PermuteIndices MANGLE_(rand_PermuteIndices)
#define rand_PermuteIndValPairs MANGLE_(rand_PermuteIndValPairs)
/*@}*/
#endif

/** \brief Returns a random integer index in the interval \f$[a, b]\f$. */
oski_index_t rand_GenIndex (oski_index_t a, oski_index_t b);

/** \brief Returns a random value in the interval \f$[a, b]\f$. If
 *  \ref oski_value_t is complex, then the real and imaginary parts
 *  each lie in the interval.
 */
oski_value_t rand_GenValue (double a, double b);

/**
 *  \brief Returns a random pure real scalar in the interval
 *  \f$[a, b]\f$. Thus, if \ref oski_value_t is complex, then
 *  the imaginary part is set to 0.
 */
oski_value_t rand_GenValueReal (double a, double b);

/** \brief Randomly permute the elements of an array of indices. */
void rand_PermuteIndices (oski_index_t * ind, oski_index_t len);

/** \brief Randomly permute corresponding (index, value) pairs of
 *  elements. */
void rand_PermuteIndValPairs (oski_index_t * ind, oski_value_t * val,
			      oski_index_t len);

#endif

/* eof */
