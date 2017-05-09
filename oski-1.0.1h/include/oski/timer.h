/**
 *  \file oski/timer.h
 *  \brief Timing module.
 */

#if !defined(INC_OSKI_TIMER_H)
/** oski/timer.h included. */
#define INC_OSKI_TIMER_H

#include <oski/common.h>

/** Timer object */
typedef struct tagBebop_timerstruct_t *oski_timer_t;

/** Invalid timer object. */
#define INVALID_TIMER ((oski_timer_t)NULL)

/** Allocate a new timer */
oski_timer_t oski_CreateTimer (void);

/** Free previously allocated timer */
void oski_DestroyTimer (oski_timer_t timer);

/** Start virtual stopwatch */
void oski_RestartTimer (oski_timer_t timer);

/** Stop virtual stopwatch */
void oski_StopTimer (oski_timer_t timer);

/** Get a short string description of the timer being used. */
const char *oski_GetTimerDesc (oski_timer_t timer);

/**
 *  \brief Check virtual stopwatch and return elapsed time, in seconds,
 *  since the last call to either oski_RestartTimer() or
 *  oski_StopTimer(), whichever happened most recently.
 *
 *  \note If the caller has never restarted this timer, then the
 *  results are garbage.
 */
double oski_ReadElapsedTime (oski_timer_t timer);

/**
 *  \brief Initialize timer.
 *
 *  An application should call this at least once during its
 *  start-up to cache calibration results.
 */
void oski_CalibrateTimer (oski_timer_t timer);

/**
 *  \brief Get the current calibration factor.
 */
double oski_GetTimerSecsPerTick (void);

#endif

/* eof */
