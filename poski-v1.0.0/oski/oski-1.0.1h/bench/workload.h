/**
 *  \file bench/workload.h
 *  \brief Routines for simulating an arbitrary workload.
 *  \ingroup OFFBENCH
 */

#if !defined(INC_BENCH_WORKLOAD_H)
/** bench/workload.h included */
#define INC_BENCH_WORKLOAD_H

#include <oski/oski.h>
#include <oski/simplelist.h>
#include <oski/mangle.h>
#include "keropts.h"

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define workload_CreateData MANGLE_(workload_CreateData)
#define workload_SetHints MANGLE_(workload_SetHints)
#define workload_Run MANGLE_(workload_Run)
#define workload_CountFlopsPerNz MANGLE_(workload_CountFlopsPerNz)
/*@}*/
#endif

/** Allocate temporary arrays for workload simulation */
void workload_CreateData (oski_index_t num_rows, oski_index_t num_cols,
			  simplelist_t * workload);

/** Call OSKI to set workload hints */
void workload_SetHints (oski_matrix_t A, simplelist_t * workload,
			int always_tune);

/** Simulate one execution of a given workload. */
void workload_Run (const oski_matrix_t A, const simplelist_t * workload);

/**
 *  Return the number of flops executed per non-zero element for a
 *  given workload.
 */
double workload_CountFlopsPerNz (const simplelist_t * workload);

#endif
/* eof */
