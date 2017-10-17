/**
 *  \file src/timer/timer.c
 *  \brief Timing module implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <oski/common.h>
#include <oski/timer.h>

/* -------------------------------------------------------------- */

#include <oski/config.h>

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#include "cycle.h"

/** Multiplicative factor converting ticks to seconds */
static double g_seconds_per_tick = -1;

/* --------------------------------------------------------------
 *  Use high-resolution hardware-based cycle counter.
 */
#if defined(HAVE_TICK_COUNTER) && !defined(WITH_SLOW_TIMER)

#define rawtime_t  ticks
#define getrawtime getticks

#define NEEDS_CALIBRATION

#else
/* --------------------------------------------------------------
 *  Use "slow" (low resolution) timer.
 *
 *  This procedure adapted/copied from FFTW-3.0 ("timer.c").
 */

/* ----- gettimeofday() ----- */
#if defined(HAVE_GETTIMEOFDAY)
typedef struct timeval rawtime_t;

static rawtime_t
getrawtime (void)
{
  rawtime_t tv;
  gettimeofday (&tv, 0);
  return tv;
}

static double
elapsed (rawtime_t t1, rawtime_t t0)
{
  return (double) (t1.tv_sec - t0.tv_sec) +
    (double) (t1.tv_usec - t0.tv_usec) * 1e-6;
}

#define TIMER_DESC "gettimeofday()"
#elif defined(HAVE_MPI_TIME)

#pragma WARNING "MPI_Wtime() support not yet enabled."
/** \todo Include MPI-based timer */

/* ----- use clock() ----- */
#else

#include <time.h>

typedef clock_t rawtime_t;

static rawtime_t
getrawtime (void)
{
  return clock ();
}

static double
elapsed (rawtime_t t1, rawtime_t t0)
{
  return ((double) (t1 - t0)) / CLOCKS_PER_SEC;
}

#define TIMER_DESC "ANSI C clock() routine"
#endif

#endif /* defined(WITH_SLOW_TIMER) */

/* -------------------------------------------------------
 *  Generic timer implementation.
 */

/**
 *  \brief Stores 'stop-watch' timer data.
 */
typedef struct tagBebop_timerstruct_t
{
  double seconds_per_tick;	 /**< Ticks-to-seconds conversion factor. */
  rawtime_t t_start;	   /**< Time at which timer started */
  rawtime_t t_end;	  /**< Time at which timer stopped */
  int is_running;	/**< 1 <==> stop-watch is running, 0 otherwise. */
}
oski_timerstruct_t;

double
oski_GetTimerSecsPerTick ()
{
  if (g_seconds_per_tick < 0)
    {				/* not calibrated yet */
      oski_timer_t timer = oski_CreateTimer ();
      oski_CalibrateTimer (timer);
      oski_DestroyTimer (timer);
    }
  return g_seconds_per_tick;
}

static size_t
strlen_ignore_spaces (const char *s)
{
  const char *p = s;
  size_t count = 0;
  if (p != NULL)
    {
      while (*p != 0)
	{
	  if (!isspace (*p))
	    count++;
	  p++;
	}
    }
  return count;
}

void
oski_CalibrateTimer (oski_timer_t timer)
{
  const char *env_conv;
  ticks t_start;
  ticks t_end;

  if (timer == INVALID_TIMER)
    return;

  /* Try to extract tick conversion factor from the environment */
  env_conv = getenv ("OSKI_TICKS_PER_SEC");
  if (env_conv != NULL)
    {
      if (strlen_ignore_spaces (env_conv) > 0)
	{
	  g_seconds_per_tick = 1.0 / atof (env_conv);
	}
    }
  if (g_seconds_per_tick < 0)
    {
#if defined(NEEDS_CALIBRATION)
      unsigned int sleep_time = 1;	/* in seconds */

      while (g_seconds_per_tick < 0)
	{
	  oski_PrintDebugMessage (1,
				  "Calibrating timer based on '%s'"
				  " using a %d second interval",
				  oski_GetTimerDesc (timer), sleep_time);

	  t_start = getrawtime ();
	  sleep (sleep_time);
	  t_end = getrawtime ();

	  g_seconds_per_tick = (double) sleep_time / elapsed (t_end, t_start);
	  sleep_time++;
	}
#else
      oski_PrintDebugMessage (1, "Calibrating timer based on '%s'",
			      oski_GetTimerDesc ());
      g_seconds_per_tick = 1.0;
#endif
    }
  else
    oski_PrintDebugMessage (1, "Using cached timer calibration result");

  timer->seconds_per_tick = g_seconds_per_tick;

  oski_PrintDebugMessage (2, "Found %g seconds per tick (%g Gt/s)",
			  g_seconds_per_tick, 1e-9 / g_seconds_per_tick);
}

oski_timer_t
oski_CreateTimer (void)
{
  oski_timer_t new_timer;
  new_timer = oski_Malloc (struct tagBebop_timerstruct_t, 1);
  if (new_timer != INVALID_TIMER)
    {
      new_timer->is_running = 0;
      oski_CalibrateTimer (new_timer);
    }
  return new_timer;
}

void
oski_DestroyTimer (oski_timer_t timer)
{
  oski_Free (timer);
}

const char *
oski_GetTimerDesc (oski_timer_t timer)
{
  return TIMER_DESC;
}

void
oski_RestartTimer (oski_timer_t timer)
{
  timer->t_start = getrawtime ();
  timer->is_running = 1;
}

void
oski_StopTimer (oski_timer_t timer)
{
  if (timer->is_running)
    {
      timer->t_end = getrawtime ();
      timer->is_running = 0;
    }
}

double
oski_ReadElapsedTime (oski_timer_t timer)
{
  ticks t_end;

  if (timer->is_running)
    t_end = getrawtime ();
  else
    t_end = timer->t_end;

#if defined(NEEDS_CALIBRATION)
  return elapsed (t_end, timer->t_start) * timer->seconds_per_tick;
#else
  return elapsed (t_end, timer->t_start);
#endif
}

/* eof */
