/**
 *  \file src/inmatprop.c
 *  \brief Input matrix properties collection and inspection.
 *
 *  \see oski/inmatprop.h
 */

#include <assert.h>
#include <oski/common.h>
#include <oski/inmatprop.h>

/**
 *  \brief
 *
 *  This routine initializes all the fields of props to be
 *  "undefined."
 *
 *  \param[out] props Property data structure to initialize.
 *  \pre props != NULL
 *  \see oski_inmatpropset_t
 */
void
oski_ClearInMatPropSet (oski_inmatpropset_t * props)
{
  assert (props != NULL);

  props->pattern = (oski_inmatprop_t) INMATPROP_UNDEFINED;
  props->has_unit_diag_implicit = INMATPROP_UNDEFINED;
  props->index_base = INMATPROP_UNDEFINED;
  props->has_sorted_indices = INMATPROP_UNDEFINED;
  props->has_unique_indices = INMATPROP_UNDEFINED;
}

/**
 *  \brief
 *
 *  \param[in,out] props Property data structure to complete.
 *  \pre props != NULL
 *  \see oski_inmatpropset_t
 *
 *  \post Checks the final property set for consistency, and
 *  aborts the program if it is not.
 */
void
oski_CompleteDefaultInMatPropSet (oski_inmatpropset_t * props)
{
  assert (props != NULL);

  if (props->pattern == (oski_inmatprop_t) INMATPROP_UNDEFINED)
    props->pattern = MAT_GENERAL;

  if (props->has_unit_diag_implicit == INMATPROP_UNDEFINED)
    props->has_unit_diag_implicit = 0;

  if (props->index_base == INMATPROP_UNDEFINED)
    props->index_base = 1;

  if (props->has_sorted_indices == INMATPROP_UNDEFINED)
    props->has_sorted_indices = 0;

  if (props->has_unique_indices == INMATPROP_UNDEFINED)
    props->has_unique_indices = 0;

  assert (oski_IsInMatPropSetConsistent (props));
}

/**
 *  \brief
 *
 *  \param[in] props Input matrix properties specified.
 *  \returns 0 if there is some detected inconsistency in the
 *  input matrix property specification, or 1 otherwise.
 *
 *  \pre props != NULL.
 */
int
oski_IsInMatPropSetConsistent (const oski_inmatpropset_t * props)
{
  assert (props != NULL);

  return IS_VAL_IN_RANGE (props->pattern, MAT_GENERAL, MAT_HERM_FULL)
    && IS_VAL_IN_RANGE (props->has_unit_diag_implicit, 0, 1)
    && IS_VAL_IN_RANGE (props->index_base, 0, 1)
    && IS_VAL_IN_RANGE (props->has_sorted_indices, 0, 1)
    && IS_VAL_IN_RANGE (props->has_unique_indices, 0, 1);
}

/**
 *  \brief
 *
 *  \param[in] props Asserted input matrix property set.
 *  \pre props != NULL
 *
 *  \note This routine executes only if the debugging level is
 *  3 or higher.
 */
void
oski_DisplayInMatPropSet (const oski_inmatpropset_t * props)
{
  const char *pattern;

  assert (props != NULL);

  oski_PrintDebugMessage (2, "Asserted input matrix properties:");

  switch (props->pattern)
    {
    case MAT_GENERAL:
      pattern = "general";
      break;
    case MAT_TRI_UPPER:
      pattern = "upper triangular";
      break;
    case MAT_TRI_LOWER:
      pattern = "lower triangular";
      break;
    case MAT_SYMM_LOWER:
      pattern = "symmetric w/ lower triangular storage";
      break;
    case MAT_SYMM_UPPER:
      pattern = "symmetric w/ upper triangular storage";
      break;
    case MAT_SYMM_FULL:
      pattern = "symmetric using full storage";
      break;
    case MAT_HERM_LOWER:
      pattern = "Hermitian w/ lower triangular storage";
      break;
    case MAT_HERM_UPPER:
      pattern = "Hermitian w/ upper triangular storage";
      break;
    case MAT_HERM_FULL:
      pattern = "Hermitian using full storage";
      break;
    default:
      pattern = "(undefined)";
    }
  oski_PrintDebugMessage (3, "Pattern: %s", pattern);

  switch (props->has_unit_diag_implicit)
    {
    case 1:
      oski_PrintDebugMessage (3, "Implicit unit diagonal");
      break;

    case 0:
      oski_PrintDebugMessage (3, "Diagonal elements stored explicitly");
      break;

    default:
      oski_PrintDebugMessage (3, "(Diagonal storage undefined.)");
      break;
    }

  switch (props->index_base)
    {
    case 1:
      oski_PrintDebugMessage (3, "1-based indices");
      break;
    case 0:
      oski_PrintDebugMessage (3, "0-based indices");
      break;
    default:
      oski_PrintDebugMessage (3, "(Index base undefined.)");
      break;
    }

  switch (props->has_sorted_indices)
    {
    case 1:
      oski_PrintDebugMessage (3, "Has sorted indices");
      break;

    case 0:
      oski_PrintDebugMessage (3, "Indices may be unsorted");
      break;

    default:
      oski_PrintDebugMessage (3, "(Index sorting undefined.)");
      break;
    }

  switch (props->has_unique_indices)
    {
    case 1:
      oski_PrintDebugMessage (3, "Indices are unique");
      break;

    case 0:
      oski_PrintDebugMessage (3, "Indices may repeat");
      break;

    default:
      oski_PrintDebugMessage (3, "(Unknown if indices are unique.)");
      break;
    }
}

/**
 *  \brief Updates 'props' with the property 'p'.
 *
 *  \returns 0 on success, or 1 if 'p' is invalid.
 *
 *  \note As written, assumes this routine only invoked by
 *  oski_CollectInMatProps (or related).
 */
static
int
UpdateProp (oski_inmatpropset_t* props,
	    oski_inmatprop_t cur_prop,
	    int cur_argnum)
{
  int err = 0;

  /* Error checking */
  if( !IS_VAL_IN_RANGE(cur_prop, MIN_INMATPROP, MAX_INMATPROP) )
    {
      OSKI_ERR_BAD_INMATPROP (oski_CollectInMatProps, cur_prop);
      err = 1;
    }
  else
    /* Translate property. */
    switch (cur_prop)
      {
      case MAT_GENERAL:
      case MAT_TRI_UPPER:
      case MAT_TRI_LOWER:
      case MAT_SYMM_LOWER:
      case MAT_SYMM_UPPER:
      case MAT_SYMM_FULL:
      case MAT_HERM_LOWER:
      case MAT_HERM_UPPER:
      case MAT_HERM_FULL:
	if (props->pattern != (oski_inmatprop_t)INMATPROP_UNDEFINED)
	  {
	    OSKI_ERR_INMATPROP_CONFLICT (oski_CollectInMatProps,
					 cur_argnum);
	    err = 1;
	  }
	else
	  props->pattern = cur_prop;
	break;
      case MAT_DIAG_EXPLICIT:
      case MAT_UNIT_DIAG_IMPLICIT:
	if (props->has_unit_diag_implicit != INMATPROP_UNDEFINED)
	  {
	    OSKI_ERR_INMATPROP_CONFLICT (oski_CollectInMatProps, cur_argnum);
	    err = 1;
	  }
	else
	  props->has_unit_diag_implicit =
	    cur_prop == MAT_UNIT_DIAG_IMPLICIT;
	break;
      case INDEX_ONE_BASED:
      case INDEX_ZERO_BASED:
	if (props->index_base != INMATPROP_UNDEFINED)
	  {
	    OSKI_ERR_INMATPROP_CONFLICT (oski_CollectInMatProps, cur_argnum);
	    err = 1;
	  }
	else
	  props->index_base = cur_prop == INDEX_ONE_BASED;
	break;
      case INDEX_UNSORTED:
      case INDEX_SORTED:
	if (props->has_sorted_indices != INMATPROP_UNDEFINED )
	  {
	    OSKI_ERR_INMATPROP_CONFLICT (oski_CollectInMatProps, cur_argnum);
	    err = 1;
	  }
	else
	  props->has_sorted_indices = cur_prop == INDEX_SORTED;
	break;
      case INDEX_REPEATED:
      case INDEX_UNIQUE:
	if (props->has_unique_indices != INMATPROP_UNDEFINED)
	  {
	    OSKI_ERR_INMATPROP_CONFLICT (oski_CollectInMatProps, cur_argnum);
	    err = 1;
	  }
	else
	  props->has_unique_indices = cur_prop == INDEX_UNIQUE;
	break;
      default:
	err = 1;
	assert(0);
      }
  return err;
}

int
oski_CollectInMatProps (oski_inmatpropset_t* props, int k, ...)
{
  int err;
  va_list ap;
  va_start (ap, k);
  err = oski_CollectInMatProps_va (props, k, ap);
  va_end (ap);
  return err;
}

int
oski_CollectInMatProps_va (oski_inmatpropset_t* props,
                           int num_var_args, va_list ap)
{
  int err = 0;
  int cur_argnum = 0; /* Current argument number, in 0..k-1 */

  /* Initialize props to "unset" */
  oski_ClearInMatPropSet (props);

  /* Loop over variable arguments */
  while (!err && cur_argnum < num_var_args)
    {
      /* Current property */
      oski_inmatprop_t cur_prop = va_arg(ap, oski_inmatprop_t);
      err = UpdateProp (props, cur_prop, cur_argnum);
      cur_argnum++;
    }
  if (!err)
    oski_CompleteDefaultInMatPropSet (props);

  /* For debugging purposes, dump input matrix property set */
  oski_DisplayInMatPropSet (props);
  return err ? ERR_BAD_ARG : 0;
}

int
oski_CollectInMatProps_arr (oski_inmatpropset_t* props,
			    int num_var_args,
			    const oski_inmatprop_t* in_props)
{
  int err = 0;
  int cur_argnum = 0; /* Current argument number, in 0..k-1 */

  /* Initialize props to "unset" */
  oski_ClearInMatPropSet (props);

  /* Loop over variable arguments */
  while (!err && cur_argnum < num_var_args)
    {
      /* Current property */
      oski_inmatprop_t cur_prop = in_props[cur_argnum];
      err = UpdateProp (props, cur_prop, cur_argnum);
      cur_argnum++;
    }
  if (!err)
    oski_CompleteDefaultInMatPropSet (props);

  /* For debugging purposes, dump input matrix property set */
  oski_DisplayInMatPropSet (props);
  return err ? ERR_BAD_ARG : 0;
}

/* eof */
