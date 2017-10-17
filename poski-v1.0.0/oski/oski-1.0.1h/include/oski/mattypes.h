/**
 *  \file oski/mattypes.h
 *  \ingroup MATTYPES
 *  \brief Matrix type management routines.
 *
 *  The library maintains internal tables of modules containing
 *  actual kernel implementations instantiated for particular
 *  matrix types. This module defines an interface for registering
 *  new matrix types and retrieving function pointers to registered
 *  kernel implementations.
 *
 *  \see \ref MATTYPES
 *  \see \ref oski/scalinfo.h
 */

#if !defined(INC_OSKI_MATTYPES_H)
/** oski/mattypes.h has been included. */
#define INC_OSKI_MATTYPES_H

/**
 *  \brief Initialize the matrix type manager.
 */
void oski_InitMatTypeManager (void);

/**
 *  \brief Shutdown the matrix type manager.
 */
void oski_CloseMatTypeManager (void);

/**
 *  \brief Register a new matrix type, making it available to the
 *  entire library system.
 */
oski_id_t oski_RegisterMatType (const char *type_name,
				oski_id_t id_ind_type, oski_id_t id_val_type);

/**
 *  \brief Lookup the id of a matrix type.
 */
oski_id_t oski_LookupMatTypeId (const char *type_name,
				oski_id_t id_ind_type, oski_id_t id_val_type);

/**
 *  \brief Lookup the id of a matrix type, assuming the current
 *  compile-time default scalar types (#OSKI_IND_ID and
 *  #OSKI_VAL_ID).
 *
 *  The implementation is just a wrapper around a call to
 *  oski_LookupMatTypeId with the default scalar types
 *  OSKI_IND_ID and OSKI_VAL_ID.
 *
 *  \param[in] type String type name (e.g., "CSR").
 */
#define OSKI_MATTYPE_ID(type) \
	oski_LookupMatTypeId((const char *)(type), OSKI_IND_ID, OSKI_VAL_ID)

/**
 *  \name Method lookup.
 *
 *  Routines to assist in finding methods within a module that
 *  following the BeBOP shared library naming scheme, and
 *  obtaining their function pointers.
 */
/*@{*/

/**
 *  \brief Retrieve the method for a particular matrix type.
 *
 *  Relies on the compile-time default scalar type macros,
 *  #OSKI_IND_ID and #OSKI_VAL_ID.
 *
 *  \param[in] type Matrix type name, as a string.
 *  \param[in] name Method base name (not as a string constant).
 *  \returns A function pointer of the appropriate type, or NULL
 *  on error.
 */
#define OSKI_MATTYPE_METHOD(type, name) \
	(OSKI_MAKENAME_FUNCPT(name))oski_LookupMatTypeMethod( \
		type, OSKI_IND_ID, OSKI_VAL_ID, \
		"oski_" MACRO_TO_STRING(name) \
	)

/**
 *  \brief Lookup a method by raw matrix type information and method name.
 */
void *oski_LookupMatTypeMethod (const char *type_name,
				oski_id_t id_ind_type, oski_id_t id_val_type,
				const char *method_name);

/**
 *  \brief Retrieve the method for a particular matrix type given
 *  the matrix type id.
 *
 *  \param[in] type_id Matrix type, by ID.
 *  \param[in] name Method base name (not as a string constant).
 *  \returns A function pointer of the appropriate type, or NULL
 *  on error.
 */
#define OSKI_MATTYPEID_METHOD(type_id, name) \
	(OSKI_MAKENAME_FUNCPT(name))oski_LookupMatTypeIdMethod( \
		type_id, "oski_" MACRO_TO_STRING(name) \
	)

/**
 *  \brief Lookup a method by matrix type ID and method name.
 */
void *oski_LookupMatTypeIdMethod (oski_id_t id, const char *method_name);

/*@}*/

#endif

/* eof */
