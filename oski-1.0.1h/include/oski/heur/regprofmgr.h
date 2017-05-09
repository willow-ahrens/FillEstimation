/**
 *  \file include/oski/heur/regprofmgr.c
 *  \brief Register profile manager.
 *  \ingroup TUNING
 */

#if !defined(INC_HEUR_REGPROFMGR_H)
#define INC_HEUR_REGPROFMGR_H

#include <oski/common.h>
#include <oski/mangle.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_regprof_t  MANGLE_(oski_regprof_t)

#define oski_CreateRegProfile MANGLE_(oski_CreateRegProfile)
#define oski_CopyRegProfile MANGLE_(oski_CopyRegProfile)
#define oski_LoadRegProfile MANGLE_(oski_LoadRegProfile)
#define oski_InitRegProfile MANGLE_(oski_InitRegProfile)
#define oski_ResetRegProfile MANGLE_(oski_ResetRegProfile)
#define oski_ZeroRegProfile MANGLE_(oski_ZeroRegProfile)
#define oski_InvRegProfile MANGLE_(oski_InvRegProfile)
#define oski_ScaleRegProfile MANGLE_(oski_InvRegProfile)
#define oski_MinRegProfile MANGLE_(oski_MinRegProfile)
#define oski_MaxRegProfile MANGLE_(oski_MaxRegProfile)
#define oski_MulEqRegProfile MANGLE_(oski_MulEqRegProfile)
#define oski_AddEqRegProfile MANGLE_(oski_AddEqRegProfile)
#define oski_DestroyRegProfile MANGLE_(oski_DestroyRegProfile)
#define oski_PrintDebugRegProfile MANGLE_(oski_PrintDebugRegProfile)

#define oski_CalcHarmonicMeanRegProfile \
	MANGLE_(oski_CalcHarmonicMeanRegProfile)
/*@}*/
#endif

/** Stores a complete register profile */
typedef struct
{
  oski_index_t max_r;	    /**< Maximum row block size, \f$r_\mathrm{max}\f$ */
  oski_index_t max_c;	    /**< Maximum column block size, \f$c_\mathrm{max}\f$ */
  double *perf;	      /**< Performance data for all \f$r, c\f$ (in Mflop/s) */
} oski_regprof_t;

/**
 *  Returns the performance of a profile at a given value of \f$(r, c)\f$
 *  where \f$1 \leq r \leq r_\mathrm{max}\f$ and
 *  \f$1 \leq c \leq c_\mathrm{max}\f$.
 */
#define REGPROF_GET(P, r, c)     ((P)->perf[((r)-1)*(P)->max_c + (c)-1])

/**
 *  Scales a profile entry, \f$P(r,c) \leftarrow P(r,c)\cdot a\f$.
 */
#define REGPROF_MULEQ(P, r, c, a) (REGPROF_GET(P, r, c) *= (a))

/**
 *  Adds to a profile entry, \f$P(r,c) \leftarrow P(r,c) + a\f$.
 */
#define REGPROF_ADDEQ(P, r, c, a) (REGPROF_GET(P, r, c) += (a))

/**
 *  Sets the performance of a profile at a given value of \f$(r, c)\f$,
 *  where \f$1 \leq r \leq r_\mathrm{max}\f$ and
 *  \f$1 \leq c \leq c_\mathrm{max}\f$.
 */
#define REGPROF_SET(P, r, c, v)  (REGPROF_GET(P, r, c) = (v))

/**
 *  \brief Allocate a new register profile.
 */
oski_regprof_t *oski_CreateRegProfile (oski_index_t max_r,
				       oski_index_t max_c);

/**
 *  \brief Read a register profile from the specified file.
 */
int oski_LoadRegProfile (const char *filename, oski_regprof_t * profile);

/**
 *  \brief Given a profile in an initialized state, returns it to an
 *  uninitialized state.
 */
void oski_ResetRegProfile (oski_regprof_t * profile);

/**
 *  \brief Initialize a previously uninitialized register profile
 *  with 0/NULL entries.
 */
int oski_InitRegProfile (oski_regprof_t * profile,
			 oski_index_t max_r, oski_index_t max_c);

/**
 *  \brief Set all entries of a previously allocated profile to 0.
 */
void oski_ZeroRegProfile (oski_regprof_t * profile);

/**
 *  \brief Copy a register profile, where the source is logically
 *  truncated and zero-padded to match the shape of the destination
 *  profile.
 */
void oski_CopyRegProfile (const oski_regprof_t * src, oski_regprof_t * dest);

/**
 *  \brief Invert each entry of a register profile.
 */
void oski_InvRegProfile (oski_regprof_t * profile);

/**
 *  \brief Scale all entries of a register profile.
 */
void oski_ScaleRegProfile (oski_regprof_t * profile, double alpha);

/**
 *  \brief Given two profiles, \f$P_1(r,c)\f$ and \f$P_2(r,c)\f$,
 *  computes an entrywise-multiply operation,
 *  \f$P_1(r,c) \leftarrow P_1(r,c) \cdot P_2(r,c)\f$, where \f$P_2\f$
 *  is logically truncated or padded with zeros to match the shape
 *  of \f$P_1\f$.
 */
void oski_MulEqRegProfile (oski_regprof_t * P1, const oski_regprof_t * P2);

/**
 *  \brief Given two profiles, \f$P_1(r,c)\f$ and \f$P_2(r,c)\f$,
 *  computes an entrywise-sum operation,
 *  \f$P_1(r,c) \leftarrow P_1(r,c) + P_2(r,c)\f$, where \f$P_2\f$
 *  is logically truncated or padded with zeros to match the shape
 *  of \f$P_1\f$.
 */
void oski_AddEqRegProfile (oski_regprof_t * P1, const oski_regprof_t * P2);

/**
 *  \brief Computes the weighted harmonic mean of two profiles,
 *  \f$P_1(r,c) \leftarrow \frac{1}{\frac{w_1}{P_1(r,c)}+\frac{w_2}{P_2(r,c)}}\f$,
 *  where \f$w_1, w_2\f$ are non-negative weights that should sum
 *  to 1.
 */
void oski_CalcHarmonicMeanRegProfile (oski_regprof_t * P1, double w1,
				      const oski_regprof_t * P2, double w2);

/**
 *  \brief Free the memory associated with a register profile.
 */
void oski_DestroyRegProfile (oski_regprof_t * profile);

/**
 *  \brief Returns the smallest entry in a profile.
 */
double oski_MinRegProfile (const oski_regprof_t * profile,
			   oski_index_t * p_r, oski_index_t * p_c);

/**
 *  \brief Returns the largest entry in a profile.
 */
double oski_MaxRegProfile (const oski_regprof_t * profile,
			   oski_index_t * p_r, oski_index_t * p_c);

/**
 *  \brief Display an array of profile data (for debugging).
 */
void oski_PrintDebugRegProfile (int debug_level, const char *title,
				const oski_regprof_t * profile);

#endif

/* eof */
