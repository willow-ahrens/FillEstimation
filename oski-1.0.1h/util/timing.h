/**
 *  \file util/timing.h
 *  \brief Timing wrapper macros.
 *  \ingroup AUXIL
 */

#if !defined(INC_UTIL_TIMING_H)
/** util/timing.h included. */
#define INC_UTIL_TIMING_H

#include <oski/timer.h>
#include "stat.h"

/**
 *  \brief Generic macro to time an arbitrary piece of C code
 *  using the BeBOP Tempo timer (see oski/\ref timer.h).
 *
 *  The timing loop is actually a doubly-nested loop: an 'outer'
 *  loop, and an 'inner' loop structured roughly as follows:
 *  \code
 *  FOR i = 1 to outer_iters DO
 *
 *      EXECUTE caller's PRE_CODE.
 *
 *      Start_timer();
 *      FOR j = 1 to inner_iters DO
 *          EXECUTE CODE.
 *      END
 *      Stop_timer();
 *
 *      EXECUTE caller's POST_CODE.
 *
 *      t = Read_elapsed_time();
 *      times[i] = t / inner_iters;
 *  END
 *  \endcode
 *
 *  \param[in] CODE      Code to benchmark.
 *  \param[in] PRE_CODE  Code executed just before the call to start
 *  the timer (executed once per outer iteration).
 *  \param[in] POST_CODE Code executed just before the call to stop
 *  the timer (executed once per outer iteration).
 *  \param[in] inner_iters The number of inner iterations.
 *  \param[in] outer_iters The number of outer iterations.
 *  \param[in,out] times A pre-allocated array for storing the
 *  mean inner-iteration execution times.
 */
#define TIMING_LOOP_CORE( CODE, PRE_CODE, POST_CODE, outer_iters, inner_iters, times ) \
	{ \
		oski_timer_t timer_; \
		\
		size_t i_outer_; \
		\
		/* Allocate timer and space for outer iteration times. */ \
		timer_ = oski_CreateTimer(); \
		ABORT( timer_ == NULL, TIMING_LOOP_CORE, ERR_OUT_OF_MEMORY ); \
		ABORT( (times) == NULL, TIMING_LOOP_CORE, ERR_BAD_ARG ); \
		ABORT( (inner_iters) <= 0, TIMING_LOOP_CORE, ERR_BAD_ARG ); \
		\
		for( i_outer_ = 0; i_outer_ < (outer_iters); i_outer_++ ) \
		{ \
			size_t i_inner; \
			{ \
				PRE_CODE; \
			} \
			oski_RestartTimer( timer_ ); \
			for( i_inner = 0; i_inner < (inner_iters); i_inner++ ) \
			{ \
				CODE; \
			} \
			oski_StopTimer( timer_ ); \
			{ \
				POST_CODE; \
			} \
			(times)[i_outer_] = oski_ReadElapsedTime(timer_) / (inner_iters); \
		} \
		\
		oski_DestroyTimer( timer_ ); \
	}

/**
 *  \brief Compute the minimum number of inner iterations needed
 *  to consume some minimum amount of execution time.
 *
 *  \param[in] CODE Code to benchmark.
 *  \param[in] PRE_CODE Code to execute before each inner loop.
 *  \param[in] POST_CODE Code to execute after each inner loop.
 *  \param[in] min_time Minimum desired inner loop execution time.
 *  \param[out] num_iters Minimum number of inner iterations needed
 *  to obtain the minimum desired inner loop execution time.
 *
 */
#define CALC_MIN_ITERS( CODE, PRE_CODE, POST_CODE, min_time, num_iters ) \
	{ \
		size_t inner_iters_ = 0; \
		oski_timer_t timer_ = oski_CreateTimer(); \
		double t_cumulative_ = 0.0; /* time so far ... */ \
		\
		oski_PrintDebugMessage( 1, \
			"Estimating minimum # of inner iterations needed" \
			" to get %g seconds of execution time...", \
			(double)min_time ); \
		\
		ABORT( timer_ == NULL, CALC_MIN_ITERS, ERR_OUT_OF_MEMORY ); \
		\
		do \
		{ \
			size_t i_inner; \
			inner_iters_ = (inner_iters_ == 0) ? 1 : 2*inner_iters_; \
			{ \
				PRE_CODE; \
			} \
			oski_RestartTimer( timer_ ); \
			for( i_inner = 0; i_inner < inner_iters_; i_inner++ ) \
			{ \
				CODE; \
			} \
			oski_StopTimer( timer_ ); \
			{ \
				POST_CODE; \
			} \
			t_cumulative_ += oski_ReadElapsedTime( timer_ ); \
		} while( t_cumulative_ < (min_time) ); \
		\
		oski_DestroyTimer( timer_ ); \
		\
		/* return */ \
		oski_PrintDebugMessage( 1, "Need at least %d inner iterations.", \
			inner_iters_ ); \
		(num_iters) = inner_iters_; \
	}

/** Do nothing code */
#define DUMMY_CODE

/**
 *  \brief Basic timing loop.
 *
 *  \param[in] CODE Code to execute.
 *  \param[in] num_trials Number of trials.
 *  \param[in] num_ops Number of operations per execution of CODE.
 *  \param[out] speed Best observed speed.
 *
 *  \returns Sets the argument 'speed' to the best observed speed
 *  (in operations per second) over all outer loop executions.
 */
#define TIMING_LOOP_BASIC( CODE, num_trials, num_ops, speed ) \
	{ \
		double* outer_times_; \
		size_t min_inner_; \
		\
		outer_times_ = oski_Malloc( double, (num_trials) ); \
		ABORT( outer_times_ == NULL, TIMING_LOOP_BASIC, ERR_OUT_OF_MEMORY ); \
		\
		CALC_MIN_ITERS( CODE, DUMMY_CODE, DUMMY_CODE, .2, min_inner_ ); \
		TIMING_LOOP_CORE( CODE, DUMMY_CODE, DUMMY_CODE, \
			(num_trials), min_inner_, outer_times_ ); \
		(speed) = \
			(double)(num_ops) / stat_CalcMin(outer_times_, num_trials); \
		\
		oski_Free( outer_times_ ); \
	}

#endif

/* eof */
