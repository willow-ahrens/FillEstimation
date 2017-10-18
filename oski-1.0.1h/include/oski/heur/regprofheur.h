/**
 *  \file include/oski/heur/regprofheur.h
 *  \brief Register blocking heuristic for general workloads.
 */

#if !defined(INC_HEUR_REGPROFHEUR_H)
/** oski/heur/regprofheur.h included. */
#define INC_HEUR_REGPROFHEUR_H

#include <oski/common.h>
#include <oski/modcommon.h>
#include <oski/heurexport.h>

#include <oski/heur/regprofmgr.h>
#include <oski/heur/regprofquery.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_regprofheur_t   MANGLE_(oski_regprofheur_t)
/*@}*/
#endif

/** Stores results of register blocking heuristic evaluation. */
typedef struct
{
  oski_index_t r;	  /**< Row block size */
  oski_index_t c;	  /**< Column block size */
  double perf_est;	  /**< Heuristic's performance estimate */

  struct
  {
    int MatTransMatMult;
    int MatMultAndMatMult;
    int MatMultAndMatTransMult;
    int MatPowMult;
    int MatTransPowMult;
  } enabled;
} oski_regprofheur_t;

#endif

/* eof */
