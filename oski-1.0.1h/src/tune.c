/**
 *  \file src/tune.c
 *  \brief Tuning module implementation.
 *  \ingroup TUNING
 *
 *  Current list of heuristics:
 *    - Cache blocked SpMV
 *    - Register blocked SpMV
 *    - Register blocked sparse \f$A^TA\cdot x\f$.
 *    - Register blocked, switch-to-dense triangular solve
 *
 *  Basic structure of a heuristic:
 *
 *  - Has two associated cost estimates
 *      -# time to run heuristic,
 *      -# time to convert.
 *  - Here's a typical implementation of a heuristic:
 *  \n
 *  \code
 *
FUNCTION HeurType :: EvaluateHeuristic( heur:HeurType, A:matrix )
	IF this heuristic does not apply to A and its trace THEN
		RETURN NULL;
	ENDIF

	LET results = Choose tuning parameters for A
	RETURN results

 *  \endcode
 *  \n
 *  For example, here is the heuristic for register blocked SpMV:
 *  \n
 *  \code
FUNCTION RBSpMV :: EvaluateHeuristic( heur:HeurType, A:matrix )
	IF A->trace not "dominated" by calls to SpMV THEN
		RETURN NULL;
	ENDIF

	LET EstFill[MAX_R, MAX_C] = EstimateFill( A )
	LET Mflops_dense[MAX_R, MAX_C] = LoadRegProfile( A )

	LET r, c = max_{r,c} Mflops_dense[r,c] / EstFill[r, c]

	IF r == 1 AND c == 1 THEN
		RETURN NULL;
	ELSE
		LET results = new RbSpMV_results( r, c )
		RETURN results
	ENDIF
 *  \endcode
 */

#include <math.h>

#include <oski/common.h>
#include <oski/simplelist.h>
#include <oski/heur_internal.h>
#include <oski/heur_typedep.h>
#include <oski/heurexport.h>
#include <oski/trace.h>
#include <oski/tune.h>

#include <oski/timer.h>
#include <oski/matrix.h>
#include <oski/vecview.h>
#include <oski/xforms.h>

/* -------------------------------------------------------------------
 *  Private to this module.
 */

/** Fraction of observed workload available for tuning. */
#define BEST_FRAC_OBSERVED .25

/** Fraction of hint workload available for tuning. */
#define BEST_FRAC_WORKLOAD .25

/**
 *  \brief Returns the estimated number of seconds available for tuning.
 *
 *  The estimate is based on the larger of the following two
 *  quantities:
 *    - Estimated time to execute the trace determined by the
 *      workload hints.
 *    - The actual accumulated kernel execution time so far.
 */
static double
ComputeTuningBudget (oski_matrix_t A_tunable)
{
  double time_workhints;
  double time_measured;

  if (A_tunable == INVALID_MAT)
    return 0.0;

  time_workhints = oski_EstimateTraceTimeTotal (A_tunable->workhints,
						A_tunable->time_stream);
  time_measured = oski_EstimateTraceTimeTotal (A_tunable->trace,
					       A_tunable->time_stream);

  oski_PrintDebugMessage (2, "Tuning budget computation:");
  oski_PrintDebugMessage (2,
			  "   Workload hints (simulated): %g seconds",
			  time_workhints);
  oski_PrintDebugMessage (2, "   Observed trace: %g seconds", time_measured);

  if (time_workhints > time_measured)
    return time_workhints * BEST_FRAC_WORKLOAD;
  else
    return time_measured * BEST_FRAC_OBSERVED;
}

/**
 *  \brief Determines whether the heuristic-selected data structure
 *  leads to faster execution times than the input data structure.
 *
 *  \returns 1 if the tuned implementation is faster, and 0 otherwise.
 *
 *  \todo Implement this routine, given that we will need to
 *  allocate temporary vectors.
 */
static int
ChooseFastest (oski_matrix_t A_tunable)
{
  oski_id_t id_save;
  oski_index_t m, n; /* dimensions */

  /* temporary vectors */
  oski_value_t* x;
  oski_value_t* y;
  oski_vecstruct_t xv;
  oski_vecstruct_t yv;

  /* timing */
  oski_timer_t timer = oski_CreateTimer();
  double t_ref, t_tuned;

  int err;

  if (A_tunable == INVALID_MAT || A_tunable->tuned_mat.type_id == INVALID_ID
      || timer == INVALID_TIMER)
    return 0; /* Don't know, so assume tuned is slower */

  m = A_tunable->props.num_rows;
  n = A_tunable->props.num_cols;

  err = oski_MultiMalloc (NULL, __LINE__, 2,
			  (size_t)(sizeof(oski_value_t)*m), &y,
			  (size_t)(sizeof(oski_value_t)*n), &x);
  if (err) { oski_DestroyTimer(timer); return 0; }

  /* "warm" cache */
  oski_ZeroMem (x, sizeof(oski_value_t)*n);
  oski_ZeroMem (y, sizeof(oski_value_t)*m);

  xv.num_rows = n;
  xv.num_cols = 1;
  xv.orient = LAYOUT_ROWMAJ;
  xv.stride = 1;
  xv.val = x;
  xv.rowinc = 1;
  xv.colinc = 1;

  yv.num_rows = m;
  yv.num_cols = 1;
  yv.orient = LAYOUT_ROWMAJ;
  yv.stride = 1;
  yv.val = y;
  yv.rowinc = 1;
  yv.colinc = 1;

  /* Warm up call */
  oski_MatMult (A_tunable, OP_NORMAL, TVAL_ONE, &xv, TVAL_ZERO, &yv);

  /* Time tuned implementation */
  oski_RestartTimer (timer);
  oski_MatMult (A_tunable, OP_NORMAL, TVAL_ONE, &xv, TVAL_ZERO, &yv);
  oski_StopTimer (timer);
  t_tuned = oski_ReadElapsedTime (timer);

  /* Temporarily "hide" the tuned implementation to time the reference */
  id_save = A_tunable->tuned_mat.type_id;
  A_tunable->tuned_mat.type_id = INVALID_ID;

  /* Warm up reference call */
  oski_MatMult (A_tunable, OP_NORMAL, TVAL_ONE, &xv, TVAL_ZERO, &yv);

  /* Time reference */
  oski_RestartTimer (timer);
  oski_MatMult (A_tunable, OP_NORMAL, TVAL_ONE, &xv, TVAL_ZERO, &yv);
  oski_StopTimer (timer);
  t_ref = oski_ReadElapsedTime (timer);
  A_tunable->tuned_mat.type_id = id_save; /* unhide */

  /* clean-up */
  oski_FreeAll (2, x, y);
  oski_DestroyTimer (timer);

  if (1.05*t_ref < t_tuned)
    {
      oski_PrintDebugMessage (4, "Ref is %gx faster than tuned!",
			      t_tuned / t_ref);
      oski_PrintDebugMessage (5, "t_ref = %g s", t_ref);
      oski_PrintDebugMessage (5, "t_tuned = %g s", t_tuned);
      return 0;
    }
  else
    return 1;
}

static void
FreeTunedMat (oski_matrix_t A_tunable)
{
  if (A_tunable != INVALID_MAT && A_tunable->tuned_mat.type_id != INVALID_ID)
    {
      oski_DestroyMatRepr_funcpt func_DestroyTuned =
	OSKI_MATTYPEID_METHOD (A_tunable->tuned_mat.type_id, DestroyMatRepr);
      if (func_DestroyTuned != NULL)
	{
	  (*func_DestroyTuned) (A_tunable->tuned_mat.repr);
	  A_tunable->tuned_mat.repr = NULL;
	  A_tunable->tuned_mat.type_id = INVALID_ID;
	}
      oski_Free (A_tunable->tuned_xforms);
      A_tunable->tuned_xforms = NULL;
    }
}

/* -------------------------------------------------------------------
 *  Public
 */

/**
 *  \brief
 *
 *  Basic outline of this routine's implementation:
 *  \code
WHILE
    !IsTuned(A)
    AND tuning_time_left > 0
    AND i_heur <= NUM_HEURISTICS
DO
    LET heur = GetHeuristic( i_heur );
    LET results = NULL

    IF GetTotalCostEstimate(heur, A) <= tuning_time_left THEN
        LET t0 = GetTimer();
        results = EvaluateHeuristic( heur, A );
            // results == NULL if heuristic does not apply to A
        LET elapsed_time = GetTimer() - t0;
        tuning_time_left -= elapsed_time;
    ENDIF

    IF results THEN
        LET t0 = GetTimer();
        A_tuned = ApplyHeuristic( heur, results, A );
            // convert A to new data structure
        LET elapsed_time = GetTimer() - t0;
        tuning_time_left -= elapsed_time;

        A = ChooseFastest( A, A_tuned, A->trace );
    ENDIF

    i_heur = i_heur + 1;
DONE
 *  \endcode
 *
 *  \todo The current implementation does not try to re-tune
 *  if already tuned.
 *  \todo Check that the new data structure really is faster
 *  than the old.
 */
int
oski_TuneMat (oski_matrix_t A_tunable)
{
  double tuning_time_left;
  size_t i_heur, num_heur;

  if (A_tunable->tuned_mat.type_id != INVALID_ID)	/* Already tuned */
    return TUNESTAT_AS_IS;

  oski_PrintDebugMessage (1, "Tuning");

  /* Try each heuristic. */
  tuning_time_left = ComputeTuningBudget (A_tunable);
  i_heur = 1;
  num_heur = oski_GetNumHeur ();

  oski_PrintDebugMessage (2, "Checking %d heuristics...", num_heur);
  while ((A_tunable->tuned_mat.type_id == INVALID_ID)
	 && (tuning_time_left > 0) && (i_heur <= num_heur))
    {
      oski_id_t id_heur;
      double heur_cost;
      void *results;

      id_heur = oski_LookupHeurIdByNum (i_heur);
      i_heur++;
      if (id_heur == INVALID_ID)
	continue;

      oski_PrintDebugMessage (2, "Trying heuristic %d (ID=%d): '%s'",
			      (int) i_heur - 1, (int) id_heur,
			      oski_LookupHeurDescById (id_heur));
      oski_PrintDebugMessage (2, "%g seconds remain", tuning_time_left);

      if (!oski_IsHeurApplicable (id_heur, A_tunable))
	{
	  oski_PrintDebugMessage (3, "Heuristic does not apply.");
	  continue;
	}

      heur_cost = oski_EstimateHeurCost (id_heur, A_tunable);
      if (heur_cost > tuning_time_left)
	{
	  oski_PrintDebugMessage (3,
				  "Heuristic is too expensive (~%d SpMVs)",
				  (int) (ceil
					 (heur_cost /
					  A_tunable->time_stream)));
	  continue;
	}

      /* Heuristic is cheap enough to run, so do it. */
      oski_RestartTimer (A_tunable->timer);

      oski_PrintDebugMessage (3, "Evaluating heuristic...");
      results = oski_EvaluateHeur (id_heur, A_tunable);
      if (results != NULL)
	{
	  int is_shared = A_tunable->is_shared; /* save shared state */

	  oski_PrintDebugMessage (3, "Applying heuristic...");

	  A_tunable->is_shared = 1; /* pretend input mat is shared */
	  oski_ApplyHeurResults (id_heur, results, A_tunable);
	  if (ChooseFastest (A_tunable))
	    {
	      oski_PrintDebugMessage (4, "Keeping tuned matrix. (%d)",
				      A_tunable->tuned_mat.type_id);
	      if (!is_shared)
		oski_FreeInputMatRepr (A_tunable);
	    }
	  else
	    {
	      oski_PrintDebugMessage (4, "Reverting to reference format.");
	      FreeTunedMat (A_tunable);
	    }
	  A_tunable->is_shared = is_shared; /* restore original state */
	}
      else
	oski_PrintDebugMessage (3, "Skipping this optimization...");

      oski_StopTimer (A_tunable->timer);
      tuning_time_left -= oski_ReadElapsedTime (A_tunable->timer);

    }				/* while not tuned */

  if (A_tunable->tuned_mat.type_id != INVALID_ID)
    return TUNESTAT_NEW;
  else
    return TUNESTAT_AS_IS;
}

/* eof */
