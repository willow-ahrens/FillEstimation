/**
 *  \file src/corelib/matcommon.c
 *  \brief Support routines for the data structure containing
 *  common matrix properties.
 */

#include <oski/common.h>
#include <oski/mangle.h>
#include <oski/matcommon.h>

/**
 *  \brief
 *
 *  This routine returns the same pattern if it is unknown or
 *  does not have a corresponding transpose.
 */
oski_inmatprop_t
oski_TransposePatternProp (oski_inmatprop_t pattern)
{
  oski_inmatprop_t trans_pattern;

  switch (pattern)
    {
    case MAT_TRI_UPPER:
      trans_pattern = MAT_TRI_LOWER;
      break;
    case MAT_TRI_LOWER:
      trans_pattern = MAT_TRI_UPPER;
      break;
    case MAT_SYMM_UPPER:
      trans_pattern = MAT_SYMM_LOWER;
      break;
    case MAT_SYMM_LOWER:
      trans_pattern = MAT_SYMM_UPPER;
      break;
    case MAT_HERM_UPPER:
      trans_pattern = MAT_HERM_LOWER;
      break;
    case MAT_HERM_LOWER:
      trans_pattern = MAT_HERM_UPPER;
      break;
    default:			/* full */
      trans_pattern = pattern;
      break;
    }

  return trans_pattern;
}

/**
 *  \brief
 *
 *  Changes the common properties of some matrix \f$A\f$ to
 *  the equivalent properties of \f$A^T\f$.
 *
 *  \param[in,out] props Properties to transpose. Overwritten
 *  on return.
 */
void
oski_TransposeProps (oski_matcommon_t * props)
{
  oski_index_t m;
  oski_index_t n;
  int is_tri_upper;
  int is_tri_lower;

  if (props == NULL)
    return;

  m = props->num_rows;
  n = props->num_cols;
  is_tri_upper = props->pattern.is_tri_upper;
  is_tri_lower = props->pattern.is_tri_lower;

  props->num_rows = n;
  props->num_cols = m;

  if (is_tri_upper)
    {
      props->pattern.is_tri_upper = 0;
      props->pattern.is_tri_lower = 1;
    }
  else if (is_tri_lower)
    {
      props->pattern.is_tri_upper = 1;
      props->pattern.is_tri_lower = 0;
    }
}

/* eof */
