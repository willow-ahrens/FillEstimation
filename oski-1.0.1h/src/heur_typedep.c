/**
 *  \file heur_typedep.c
 *  \brief Matrix type-dependent part of the heuristic module.
 */

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <oski/common.h>
#include <oski/heur.h>
#include <oski/heur_internal.h>
#include <oski/heur_typedep.h>
#include <oski/simplelist.h>

int
oski_IsHeurApplicable (oski_id_t id, const oski_matrix_t A_tunable)
{
  oski_HeurIsApplicable_funcpt func_HeurIsApplicable;

  if (A_tunable == INVALID_MAT)
    return 0;

  func_HeurIsApplicable = OSKI_HEURID_METHOD (id, HeurIsApplicable);
  if (func_HeurIsApplicable == NULL)
    return 0;

  return (*func_HeurIsApplicable) (A_tunable);
}

double
oski_EstimateHeurCost (oski_id_t id, const oski_matrix_t A_tunable)
{
  oski_HeurEstimateCost_funcpt func_HeurEstimateCost;

  if (A_tunable == INVALID_MAT)
    return CRAZY_HEUR_COST;

  func_HeurEstimateCost = OSKI_HEURID_METHOD (id, HeurEstimateCost);
  if (func_HeurEstimateCost == NULL)
    return CRAZY_HEUR_COST;

  return (*func_HeurEstimateCost) (A_tunable);
}

void *
oski_EvaluateHeur (oski_id_t id, const oski_matrix_t A_tunable)
{
  oski_HeurEvaluate_funcpt func_HeurEvaluate;

  if (A_tunable == INVALID_MAT)
    return NULL;

  func_HeurEvaluate = OSKI_HEURID_METHOD (id, HeurEvaluate);
  if (func_HeurEvaluate == NULL)
    return NULL;

  return (*func_HeurEvaluate) (A_tunable);
}

int
oski_ApplyHeurResults (oski_id_t id, void *results,
		       const oski_matrix_t A_tunable)
{
  oski_HeurApplyResults_funcpt func_HeurApplyResults;

  if (results == NULL)
    return 0;
  if (A_tunable == INVALID_MAT)
    return 0;

  func_HeurApplyResults = OSKI_HEURID_METHOD (id, HeurApplyResults);
  if (func_HeurApplyResults == NULL)
    {
      const char *heur_desc = oski_LookupHeurDescById (id);
      OSKI_ERR_NO_APPLYHEUR (oski_HeurApplyResults, heur_desc);
      return ERR_CANT_CONVERT;
    }

  return (*func_HeurApplyResults) (results, A_tunable);
}

/* eof */
