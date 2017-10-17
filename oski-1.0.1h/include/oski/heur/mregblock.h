/**
 *  \file oski/heur/mregblock.h
 *  \brief Implementation of the register blocking heuristic based on
 *  modified block compressed sparse row (MBCSR) format.
 *  \ingroup TUNING
 *
 *  This heuristic applies to matrices with block structure, and any
 *  OSKI kernel workload.
 */

#if !defined(INC_OSKI_HEUR_MREGBLOCK_H)
/** oski/heur/mregblock.h included. */
#define INC_OSKI_HEUR_MREGBLOCK_H

#include <oski/common.h>
#include <oski/modcommon.h>
#include <oski/heurexport.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_mregblock_t MANGLE_(oski_mregblock_t)
#define oski_LoadProfileMBCSRMatMult \
	MANGLE_(oski_LoadProfileMBCSRMatMult)
#define oski_UnloadProfileMBCSRMatMult \
	MANGLE_(oski_UnloadProfileMBCSRMatMult)
/*@}*/
#endif

/**
 *  Stores the results of evaluating the register blocking heuristic.
 *  This data structure contains the heuristic's best-guess as to
 *  the performance-optimal row and column block size.
 */
typedef struct
{
  oski_index_t r;	/**< Row block size. */
  oski_index_t c;	/**< Column block size. */
} oski_mregblock_t;

/**
 *  Stores a register blocking profile.
 *
 *  This implementation assumes row-major order, but callers
 *  should use the macros \ref PROF_MBCSR_GET and \ref PROF_MBCSR_SET.
 */
typedef struct
{
  size_t max_r;	       /**< Maximum row block size */
  size_t max_c;	       /**< Maximum column block size */
  double *perf;	       /**< Row-major 2-D array of performance data (Mflop/s). */
}
oski_profile_MBCSRMatMult_t;

/**
 *  Returns the performance (Mflop/s) at a block size \f$r\times c\f$,
 *  where \f$r\f$ and \f$c\f$ are 1-based.
 */
#define PROF_MBCSR_GET(P, r, c) ((P)->perf[((r)-1)*(P)->max_c + (c)-1])

/**
 *  Set the performance (Mflop/s) at a block size \f$r\times c\f$,
 *  where \f$r\f$ and \f$c\f$ are 1-based.
 */
#define PROF_MBCSR_SET(P, r, c, v) \
	((P)->perf[((r)-1)*(P)->max_c + (c)-1] = (v))

/**
 *  \brief Load the uniformly-aligned register blocking profile for
 *  sparse matrix-vector multiply.
 */
const oski_profile_MBCSRMatMult_t *oski_LoadProfileMBCSRMatMult (const char
								 *filename);

/**
 *  \brief Unload the uniformly-aligned register blocking profile
 *  for sparse matrix-vector multiply.
 */
void oski_UnloadProfileMBCSRMatMult (void);

#endif

/* eof */
