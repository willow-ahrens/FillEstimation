/**
 *  \file oski/heur/symmrb.h
 *  \brief Implementation of the register blocking heuristic for
 *  symmetric matrices, as described in the paper by Lee, et al., in
 *  ICPP'04, but extended to the Symmetric or Hermitian cases and
 *  based on MBCSR format.
 *  \ingroup TUNING
 */

#if !defined(INC_OSKI_HEUR_SYMMRB_H)
/** oski/heur/symmrb.h included. */
#define INC_OSKI_HEUR_SYMMRB_H

#include <oski/common.h>
#include <oski/modcommon.h>
#include <oski/heurexport.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_symmrb_t MANGLE_(oski_symmrb_t)
#define oski_LoadProfileMBCSRSymmMatMult \
	MANGLE_(oski_LoadProfileMBCSRSymmMatMult)
#define oski_UnloadProfileMBCSRSymmMatMult \
	MANGLE_(oski_UnloadProfileMBCSRSymmMatMult)
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
} oski_symmrb_t;

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
oski_profile_MBCSRSymmMatMult_t;

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
const oski_profile_MBCSRSymmMatMult_t *oski_LoadProfileMBCSRSymmMatMult (const
									 char
									 *filename);

/**
 *  \brief Unload the uniformly-aligned register blocking profile
 *  for sparse matrix-vector multiply.
 */
void oski_UnloadProfileMBCSRSymmMatMult (void);

#endif

/* eof */
