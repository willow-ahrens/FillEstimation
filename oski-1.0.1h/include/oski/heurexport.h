/**
 *  \file oski/heurexport.h
 *  \brief Matrix type-dependent part of the heuristic management module.
 *  \ingroup TUNING
 */

#if !defined(INC_OSKI_HEUREXPORT_H)
/** heurexport.h included. */
#define INC_OSKI_HEUREXPORT_H

#include <oski/matrix.h>

#if defined(DO_NAME_MANGLING)
/** \name Heuristic type module implementations. */
/*@{*/
#define oski_HeurIsApplicable MANGLE_MOD_(oski_HeurIsApplicable)
#define oski_HeurEstimateCost MANGLE_MOD_(oski_HeurEstimateCost)
#define oski_HeurEvaluate MANGLE_MOD_(oski_HeurEvaluate)
#define oski_HeurApplyResults MANGLE_MOD_(oski_HeurApplyResults)
/*@}*/
#endif

/**
 *  \brief Heuristic module-specific implementation of
 *  oski_IsHeurApplicable().
 */
int oski_HeurIsApplicable (const oski_matrix_t A);

/**
 *  \brief Heuristic module-specific implementation of
 *  oski_EstimateHeurCost().
 */
double oski_HeurEstimateCost (const oski_matrix_t A);

/**
 *  \brief Heuristic module-specific implementation of
 *  oski_EvaluateHeur().
 */
void *oski_HeurEvaluate (const oski_matrix_t A);

/**
 *  \brief Heuristic module-specific implementation of
 *  oski_ApplyHeurResults().
 */
int oski_HeurApplyResults (const void *results, oski_matrix_t A);

#endif /* !defined(INC_OSKI_HEUREXPORT_H) */

/* eof */
