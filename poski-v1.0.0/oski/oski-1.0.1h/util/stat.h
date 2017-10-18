/**
 *  \file util/stat.h
 *  \brief Provides some simple statistical utilities.
 *
 *  These routines are defined using a template-like macro
 *  mechanism to ease generation of other types.
 */

#if !defined(INC_UTIL_STAT_H)
/** util/stat.h included. */
#define INC_UTIL_STAT_H

#if !defined(VALTYPE)
/** Scalar type */
#define VALTYPE  double

/** Define to be 1 if the scalar type is a floating point type */
#define VALTYPE_IS_FLOAT 1

/** Define to generate an alternative set of names */
#define VTMANGLE(x) stat_##x

/** Define to 1 to enable automatic comparison function generation. */
#define VALTYPE_STD_COMPARE 1

#endif

/**
 *  \brief Returns the min, mean, median, and max values of the given array.
 */
void VTMANGLE (CalcBasicStats) (const VALTYPE * x, size_t n,
				VALTYPE * p_min, VALTYPE * p_max,
				VALTYPE * p_mean, VALTYPE * p_median);

/**
 *  \brief Returns the min, mean, and max values of the given array.
 */
void VTMANGLE (CalcMinMaxMean) (const VALTYPE * x, size_t n,
				VALTYPE * p_min, VALTYPE * p_max,
				VALTYPE * p_mean);

/**
 *  \brief Returns the min, mean, and median values of the given array.
 */
void VTMANGLE (CalcMinMaxMedian) (const VALTYPE * x, size_t n,
				  VALTYPE * p_min, VALTYPE * p_max,
				  VALTYPE * p_median);

/**
 *  \brief Returns just the median value of the given array.
 */
VALTYPE VTMANGLE (CalcMedian) (const VALTYPE * x, size_t n);

/**
 *  \brief Returns the minimum value of the given array.
 */
VALTYPE VTMANGLE (CalcMin) (const VALTYPE * x, size_t n);

/**
 *  \brief Returns the minimum value of the given array.
 */
VALTYPE VTMANGLE (CalcMax) (const VALTYPE * x, size_t n);

/**
 *  \brief Returns the maximum value of the given array.
 */
VALTYPE VTMANGLE (CalcMean) (const VALTYPE * x, size_t n);

#endif

/* eof */
