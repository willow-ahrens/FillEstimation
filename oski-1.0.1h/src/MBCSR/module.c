/**
 *  \file src/MBCSR/module.c
 *  \ingroup MATTYPE_MBCSR
 *  \brief Modified block compressed sparse row (MBCSR) module.
 */

#include <oski/config.h>
#include <oski/common.h>
#include <oski/mattypes.h>
#include <oski/modloader.h>
#include <oski/MBCSR/format.h>
#include <oski/MBCSR/module.h>

const char *
oski_GetShortDesc (void)
{
  return "Modified block compressed sparse row (MBCSR) module";
}

const char *
oski_GetLongDesc (void)
{
  return
    "This module implements aligned modified block compressed sparse row (MBCSR) format.";
}

void
oski_InitModule (void)
{
  oski_PrintDebugMessage (2, "Loading MBCSR(ind_t=%s, val_t=%s)",
			  MACRO_TO_STRING (IND_T), MACRO_TO_STRING (VAL_T));
  oski_PrintDebugMessage (3, oski_GetShortDesc ());
}

void
oski_CloseModule (void)
{
  oski_PrintDebugMessage (2, "Shutting down MBCSR(%s, %s) module",
			  MACRO_TO_STRING (IND_T), MACRO_TO_STRING (VAL_T));
}

/**
 *  \brief
 *
 *  This routine loads the requested method if it exists, and
 *  caches the \f$r\times c\f$ module if it has not yet been
 *  loaded.
 *
 *  \param[in] A An \f$r\times c\f$ MBCSR matrix representation.
 *  \param[in] name Name of the method to load from the
 *  \f$r\times c\f$ submodule.
 *
 *  \returns A pointer to the requested method, or NULL if none
 *  could be found.
 */
void *
MBCSR_GetSubmatKernel (const oski_submatMBCSR_t * A, const char *name)
{
  oski_modhandle_t mod;

  if (A == NULL)
    return NULL;

  /* Cache module */
  if (A->mod_cached)
    mod = (oski_modhandle_t) A->mod_cached;
  else
    mod = ((oski_submatMBCSR_t *) A)->mod_cached
      = oski_LoadModule (A->mod_name);

  if (mod != NULL)
    {
      /* Load and return method */
      char *method_name = oski_StringPrintf ("%s_%dx%d", name, A->r, A->c);
      if (method_name != NULL)
	{
	  void *func = oski_LookupModuleMethod (mod, method_name);
	  oski_Free (method_name);
	  return func;
	}
    }
  /* not found */
  return NULL;
}

/**
 *  \brief
 *
 *  Initializes the fields of an allocated vector view \f$x'\f$
 *  to point to a subset of the rows of an existing view \f$x\f$.
 *
 *  \param[in] x A vector view \f$x\f$.
 *  \param[in] row_start Row offset in x (0-based).
 *  \param[in] len Number of rows to select.
 *  \param[in,out] x_sub Output view.
 *
 *  \pre x_sub != INVALID_VEC. If x == INVALID_VEC, then x_sub
 *  is simply initialized to a zero vector.
 *
 *  \note This routine makes a shallow copy of the source vector
 *  view x, and so the caller should not destroy x_sub by a call
 *  to #oski_DestroyVecView(). We recommend the caller allocate
 *  x_sub as a #oski_vecstruct_t object on the stack.
 */
void
MBCSR_InitSubVecView (const oski_vecview_t x,
		      oski_index_t row_start, oski_index_t len,
		      oski_vecview_t x_sub)
{
  assert (x_sub != INVALID_VEC);
  if (x != INVALID_VEC)
    {
      assert (row_start >= 0);
      assert (row_start + len <= x->num_rows);

      oski_CopyMem (x_sub, x, oski_vecstruct_t, 1);
      x_sub->val += row_start * x->rowinc;
      x_sub->num_rows = len;
    }
  else
    oski_ZeroMem (x_sub, sizeof (oski_vecstruct_t));
}

/* eof */
