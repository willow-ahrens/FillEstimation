/**
 *  \file oski/heur/estfill.h
 *  \brief BCSR fill ratio estimator.
 *  \ingroup TUNING
 */

#if !defined(INC_OSKI_HEUR_ESTFILL_H)
/** oski/heur/estfill.h included. */
#define INC_OSKI_HEUR_ESTFILL_H

#include <oski/common.h>
#include <oski/mangle.h>
#include <oski/matrix.h>
#include <oski/CSR/format.h>
#include <oski/CSC/format.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_EstimateFillBCSR   MANGLE_(oski_EstimateFillBCSR)
#define oski_DestroyBCSRFillProfile MANGLE_(oski_DestroyBCSRFillProfile)
/*@}*/
#endif

/** Stores a 'fill ratio estimate' profile. */
typedef struct
{
  size_t max_r;	       /**< Maximum row block size */
  size_t max_c;	       /**< Maximum column block size */
  double *ratio;       /**< Fill ratio, or 0.0 if uninitialized */
}
oski_fillprofile_BCSR_t;

/** Returns the fill ratio at an r x c block size */
#define PROF_FILLBCSR_GET( P, r, c ) (P)->ratio[(r-1)*(P)->max_c + (c-1)]

/** Sets the fill ratio at an r x c block size */
#define PROF_FILLBCSR_SET( P, r, c, rho ) \
	(P)->ratio[(r-1)*(P)->max_c + (c-1)] = (rho)

/**
 *  \brief Estimate the fill ratio at a variety of block sizes
 *  for BCSR storage.
 */
oski_fillprofile_BCSR_t *oski_EstimateFillBCSR (const oski_matspecific_t *
						mat,
						const oski_matcommon_t *
						props, size_t max_r,
						size_t max_c,
						double prob_examine);

/**
 *  \brief Free the memory associated with a fill profile.
 */
void oski_DestroyBCSRFillProfile (oski_fillprofile_BCSR_t * fill);

#endif

/* eof */
