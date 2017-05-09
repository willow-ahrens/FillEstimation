/**
 *  \file oski/heur.h
 *  \brief Heuristic management module.
 *  \ingroup TUNING
 */

#if !defined(INC_OSKI_HEUR_H)
/** oski/heur.h included. */
#define INC_OSKI_HEUR_H

#include <oski/common.h>

/** Open/initialize the heuristic manager. */
void oski_InitHeurManager (void);

/** Shutdown heuristic manager. */
void oski_CloseHeurManager (void);

/** Register a new heuristic with the heuristic manager. */
oski_id_t oski_RegisterHeur (const char *name,
			     oski_id_t id_ind, oski_id_t id_val);

/** Returns the number of available heuristics. */
size_t oski_GetNumHeur (void);

/**
 *  \brief Retrieve the method for a particular heuristic.
 *
 *  Relies on the compile-time default scalar type macros,
 *  #OSKI_IND_ID and #OSKI_VAL_ID.
 *
 *  \param[in] type Heuristic type name, as a string.
 *  \param[in] name Method base name (not as a string constant).
 *  \returns A function pointer of the appropriate type, or NULL
 *  on error.
 */
#define OSKI_HEUR_METHOD(type, name) \
	(OSKI_MAKENAME_FUNCPT(name))oski_LookupHeurMethod( \
		type, OSKI_IND_ID, OSKI_VAL_ID, \
		"oski_" MACRO_TO_STRING(name) \
	)

/**
 *  \brief Retrieve the method for a particular heuristic type given
 *  the heuristic type id.
 *
 *  \param[in] type_id Heuristic type, by ID.
 *  \param[in] name Method base name (not as a string constant).
 *  \returns A function pointer of the appropriate type, or NULL
 *  on error.
 */
#define OSKI_HEURID_METHOD(type_id, name) \
	(OSKI_MAKENAME_FUNCPT(name))oski_LookupHeurIdMethod( \
		type_id, "oski_" MACRO_TO_STRING(name) \
	)

/** Returns a short descriptive string for the specified heuristic. */
const char *oski_LookupHeurDescById (oski_id_t id);

/** Returns the id number of a specified heuristic. */
oski_id_t oski_LookupHeurId (const char *name,
			     oski_id_t id_ind, oski_id_t id_val);

/** Returns the id number of the i-th heuristic. */
oski_id_t oski_LookupHeurIdByNum (size_t i);

/** Return a method stored in a heuristic module, found by heuristic id. */
void *oski_LookupHeurIdMethod (oski_id_t id, const char *method_name);

/** Return a method stored in a heuristic module, found by heuristic type. */
void *oski_LookupHeurMethod (const char *heur_name,
			     oski_id_t id_ind, oski_id_t id_val,
			     const char *method_name);

/* -----------------------------------------------------------------
 *  Benchmark data file helper routines.
 */
#include <stdio.h>

/**
 *  \brief Opens a file containing off-line benchmarking data for reading.
 */
FILE *oski_OpenBenchmarkData (const char *filename);

/**
 *  \brief Close an open file containing off-line benchmark data.
 */
void oski_CloseBenchmarkData (FILE * fp);

/**
 *  \brief Reads the first non-comment line in the given benchmark
 *  data file.
 */
int oski_ReadBenchmarkLine (FILE * fp, char *buffer, size_t max_len);

#endif /* !defined(INC_OSKI_HEUR_H) */

/* eof */
