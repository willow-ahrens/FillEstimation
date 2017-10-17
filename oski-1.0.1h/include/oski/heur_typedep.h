/**
 *  \file oski/heur_typedep.h
 *  \brief Matrix type-dependent part of the heuristic module.
 *  \ingroup TUNING
 */

#if !defined(INC_OSKI_HEUR_TYPEDEP_H)
/** oski/heur.h included. */
#define INC_OSKI_HEUR_TYPEDEP_H

#include <oski/common.h>
#include <oski/matrix.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_IsHeurApplicable MANGLE_(oski_IsHeurApplicable)
#define oski_EstimateHeurCost MANGLE_(oski_EstimateHeurCost)
#define oski_EvaluateHeur  MANGLE_(oski_EvaluateHeur)
#define oski_ApplyHeurResults  MANGLE_(oski_ApplyHeurResults)

#define oski_HeurIsApplicable_funcpt \
    MANGLE_(oski_HeurIsApplicable_funcpt)
#define oski_HeurEstimateCost_funcpt \
    MANGLE_(oski_HeurEstimateCost_funcpt)
#define oski_HeurEvaluate_funcpt \
    MANGLE_(oski_HeurEvaluate_funcpt)
#define oski_HeurApplyResults_funcpt \
    MANGLE_(oski_HeurApplyResults_funcpt)
/*@}*/
#endif

/**
 *  \brief Returns 1 <==> the specified heuristic can be applied to
 *  the given matrix and trace.
 */
int oski_IsHeurApplicable (oski_id_t id, const oski_matrix_t A_tunable);

/**
 *  \brief Returns the estimated cost (in seconds) of running
 *  the specified heuristic.
 */
double oski_EstimateHeurCost (oski_id_t id, const oski_matrix_t A_tunable);

/**
 *  \brief Evaluates the specified heuristic on the given matrix and
 *  its trace, and returns a pointer to a structure containing
 *  information about the results (or NULL if the heuristic does
 *  not apply to A_tunable).
 */
void *oski_EvaluateHeur (oski_id_t id, const oski_matrix_t A_tunable);

/**
 *  \brief Applies the results of heuristic evaluation to the given
 *  matrix.
 */
int oski_ApplyHeurResults (oski_id_t id, void *results,
			   const oski_matrix_t A_tunable);

/** Some ridiculous tuning cost (surrogate "infinity" value). */
#define CRAZY_HEUR_COST (double)1e9

/** \brief Function pointer type for oski_HeurIsApplicable(). */
typedef int (*oski_HeurIsApplicable_funcpt) (const oski_matrix_t A_tunable);

/** \brief Function pointer type for a heuristic module-specific
 *  implementation of oski_EstimateHeurCost(). */
typedef double (*oski_HeurEstimateCost_funcpt) (const oski_matrix_t A);

/** \brief Function pointer type for a heuristic module-specific
 *  implementation of oski_EvaluateHeur(). */
typedef void *(*oski_HeurEvaluate_funcpt) (const oski_matrix_t A);

/** \brief Function pointer type for a heuristic module-specific
 *  implementation of oski_ApplyHeurResults(). */
typedef int (*oski_HeurApplyResults_funcpt) (void *results,
					     const oski_matrix_t A);

#endif /* !defined(INC_OSKI_HEUR_TYPEDEP_H) */

/* eof */
