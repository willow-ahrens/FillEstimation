/**
 *  \file src/scalinfo.c
 *  \brief Define the scalar types available to the library.
 *
 *  This module defines two global lists, implemented as arrays
 *  private to the module, containing information about available
 *  scalar integer index and non-zero value types. Each list is
 *  composed of entries of type #oski_scalinfo_t, and is
 *  terminated by a dummy entry whose type id number is -1.
 *
 *  These lists are read-only and statically defined, so there
 *  should not be any issues with thread-safety violation.
 *
 *  Users who wish to make additional scalar types available
 *  should add initialization code to g_avail_index_types
 *  and/or g_avail_value_types (below), accordingly.
 */

#include <oski/common.h>
#include <oski/scalinfo.h>

/* ----------------------------------------------------------- */

/**
 *  \brief List of available integer index types.
 */
static oski_scalinfo_t g_avail_index_types[] = {
  OSKI_SCALIND_INT_REC,
  OSKI_SCALIND_LONG_REC,
  OSKI_SCALIND_END_REC
};

size_t
oski_GetNumScalarIndexTypes (void)
{
  size_t i = 0;
  while (g_avail_index_types[i].id != INVALID_ID)
    i++;
  return i;
}

/**
 *  \brief
 *
 *  Scans the global list g_avail_index_types for the specified
 *  type id.
 */
const oski_scalinfo_t *
oski_LookupScalarIndexInfo (oski_id_t id)
{
  const oski_scalinfo_t *retval = NULL;
  int i = 0;
  while (retval == NULL)
    {
      if (g_avail_index_types[i].id == OSKI_SCALIND_END)	/* end of list */
	break;

      if (g_avail_index_types[i].id == id)
	retval = &(g_avail_index_types[i]);

      i++;
    }
  return retval;
}

/**
 *  \brief
 *
 *  \param[in] id Type id.
 *  \returns The scalar integer index type name, or "[unknown]".
 */
const char *
oski_GetScalarIndexName (oski_id_t id)
{
  const oski_scalinfo_t *ind = oski_LookupScalarIndexInfo (id);
  return (ind == NULL) ? "[unknown]" : ind->name;
}

/* ----------------------------------------------------------- */

/**
 *  \brief List of available non-zero value types.
 */
static oski_scalinfo_t g_avail_value_types[] = {
  OSKI_SCALVAL_SINGLE_REC,
  OSKI_SCALVAL_DOUBLE_REC,
  OSKI_SCALVAL_COMPLEX_REC,
  OSKI_SCALVAL_DOUBLECOMPLEX_REC,
  OSKI_SCALVAL_END_REC
};

size_t
oski_GetNumScalarValueTypes (void)
{
  size_t i = 0;
  while (g_avail_value_types[i].id != INVALID_ID)
    i++;
  return i;
}

/**
 *  \brief
 *
 *  Scans the global list g_avail_value_types for the specified
 *  type id.
 */
const oski_scalinfo_t *
oski_LookupScalarValueInfo (oski_id_t id)
{
  const oski_scalinfo_t *retval = NULL;
  int i = 0;
  while (retval == NULL)
    {
      if (g_avail_value_types[i].id == OSKI_SCALVAL_END)	/* end of list */
	break;

      if (g_avail_value_types[i].id == id)
	retval = &(g_avail_value_types[i]);

      i++;
    }
  return retval;
}

/**
 *  \brief
 *
 *  \param[in] id Type id.
 *  \returns The scalar integer index type name, or "[unknown]".
 */
const char *
oski_GetScalarValueName (oski_id_t id)
{
  const oski_scalinfo_t *val = oski_LookupScalarValueInfo (id);
  return (val == NULL) ? "[unknown]" : val->name;
}

/**
 *  \brief
 *
 *  \param[in] id Type id.
 *  \returns The scalar integer index abbreviated name (tag),
 *  or NULL on error (e.g., the id could not be found).
 */
char
oski_GetScalarValueTag (oski_id_t id)
{
  const oski_scalinfo_t *rec = oski_LookupScalarValueInfo (id);
  return (rec == NULL) ? '\0' : rec->tag;
}

/**
 *  \brief
 *
 *  \param[in] id Type id.
 *  \returns The scalar integer index abbreviated name (tag),
 *  or NULL on error (e.g., the id could not be found).
 */
char
oski_GetScalarIndexTag (oski_id_t id)
{
  const oski_scalinfo_t *rec = oski_LookupScalarIndexInfo (id);
  return (rec == NULL) ? '\0' : rec->tag;
}

/* eof */
