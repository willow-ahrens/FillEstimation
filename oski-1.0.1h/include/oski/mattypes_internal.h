/**
 *  \file oski/mattypes_internal.h
 *  \ingroup MATTYPES
 *  \brief Defines a matrix type record.
 *
 *  This file is used for debugging only, and should not be used by
 *  end-users.
 *
 *  This file depends on \ref oski/modloader.h
 */

#if !defined(INC_OSKI_MATTYPE_INTERNAL_H)
/** mattype_internal.h has been included. */
#define INC_OSKI_MATTYPE_INTERNAL_H

#include <oski/modloader.h>

/**
 *  \brief Matrix type record.
 *
 *  A "matrix type" is a particular storage format, instantiated with
 *  a particular concrete integer index type and non-zero value type.
 *  In addition, this record stores a pointer to the module defining
 *  the type.
 */
typedef struct tagBebop_mattype_t
{
  oski_id_t id;	      /**< Unique positive integer id */
  char *name;	   /**< Unique short string identifier, [0-9a-zA-Z_]+ */
  oski_id_t id_index_type;
		/**< Integer index type (see \ref oski/scalinfo.h) */
  oski_id_t id_value_type;
		/**< Non-zero value type (see \ref oski/scalinfo.h) */

	/**
	 *  \brief A short, descriptive string for this matrix type
	 *  (primarily used for debugging).
	 */
  char *desc;

	/**
	 *  \brief Name of this type's shared library module, e.g.,
	 *  "liboski_mat_CSR_Tid".
	 */
  char *modname;

	/**
	 *  \brief Pointer to the shared library module implementing this type.
	 */
  oski_modhandle_t module;
}
oski_mattype_t;

/**
 *  \brief Find a matrix type record (FOR INTERNAL USE ONLY).
 */
const oski_mattype_t *oski_FindMatTypeRecord (oski_id_t id);

/**
 *  \brief Display a matrix type record (FOR DEBUGGING ONLY).
 */
void oski_DisplayMatTypeInfo (const oski_mattype_t * record, FILE * fp);

/**
 *  \brief Returns the total number of registered matrix types.
 */
size_t oski_GetNumMatTypes (void);

#endif

/* eof */
