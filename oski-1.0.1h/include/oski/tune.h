/**
 *  \file oski/tune.h
 *  \brief Tuning interface.
 *  \ingroup TUNING
 */

#if !defined(INC_OSKI_TUNE_H) &&! defined(OSKI_UNBIND)
/** oski/tune.h has been included. */
#define INC_OSKI_TUNE_H

#include <oski/common.h>
#include <oski/matrix.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_TuneMat    MANGLE_(oski_TuneMat)

#define oski_GetMatTransforms    \
		MANGLE_(oski_GetMatTransforms)
#define oski_ApplyMatTransforms  \
		MANGLE_(oski_ApplyMatTransforms)
/*@}*/
#endif

/** \name Tuning status codes. */
/*@{*/
#define TUNESTAT_AS_IS  0   /**< Did not change data structure. */
#define TUNESTAT_NEW    1   /**< Changed data structure. */
/*@}*/

/**
 *  \brief Tune the matrix object using all hints and implicit
 *  profiling data.
 *  \ingroup PUBLIC
 */
int oski_TuneMat (oski_matrix_t A_tunable);

/**
 *  \brief Returns a string representation of the data structure
 *  transformations that were applied to the given matrix during
 *  tuning.
 *  \ingroup PUBLIC
 */
char *oski_GetMatTransforms (const oski_matrix_t A_tunable);

/**
 *  \brief Replace the current data structure for a given matrix
 *  object with a new data structure specified by a given string.
 *  \ingroup PUBLIC
 */
int
oski_ApplyMatTransforms (const oski_matrix_t A_tunable, const char *xforms);

/* ----------------------------------------------------------------- */

#endif /* !defined(INC_OSKI_TUNE_H) */

#if defined(OSKI_UNBIND)
#  include <oski/matrix.h>
#  undef INC_OSKI_TUNE_H
#  undef oski_TuneMat
#  undef oski_GetMatTransforms
#  undef oski_ApplyMatTransforms
#  undef TUNESTAT_AS_IS
#  undef TUNESTAT_NEW
#endif

/* eof */
