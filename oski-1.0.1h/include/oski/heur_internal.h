/**
 *  \file oski/heur_internal.h
 *  \brief Heuristic management module.
 *  \ingroup TUNING
 */

#if !defined(INC_OSKI_HEUR_INTERNAL_H)
/** oski/heur_internal.h included. */
#define INC_OSKI_HEUR_INTERNAL_H

#include <oski/common.h>
#include <oski/modloader.h>

/** Heuristic record. */
typedef struct
{
  oski_id_t id;		/**< ID# for this heuristic. */
  char *name;	   /**< Unique string name used to identify the
                       module containing its implementation. */
  oski_id_t id_index_type;	   /**< Index type. */
  oski_id_t id_value_type;	   /**< Value type. */

	/** \brief A short descriptive string, for debugging. */
  char *desc;

	/** \brief Shared library module name, e.g.,
	 *  "liboski_heur_NAME_Tid".
	 */
  char *modname;

  oski_modhandle_t module;	 /**< Cached module */
}
oski_heurrec_t;

/** Returns a matching heuristic record, found by heuristic type. */
const oski_heurrec_t *oski_LookupHeur (const char *name,
				       oski_id_t id_ind, oski_id_t id_val);

/** Returns a matching heuristic record, found by id number. */
const oski_heurrec_t *oski_FindHeurRecord (oski_id_t id);

#endif /* !defined(INC_OSKI_HEUR_INTERNAL_H) */

/* eof */
