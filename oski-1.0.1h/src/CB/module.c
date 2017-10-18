/**
 *  \file src/CB/module.c
 *  \ingroup MATTYPE_CB
 *  \brief Generalized compressed sparse row (CB) implementation.
 */

#include <oski/config.h>

#include <stdarg.h>

#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/matcreate.h>
#include <oski/CB/format.h>
#include <oski/CB/module.h>
#include <oski/CSR/module.h>

const char *
oski_GetShortDesc (void)
{
  return "Cache blocked (CB) format implementation";
}

const char *
oski_GetLongDesc (void)
{
  return oski_GetShortDesc ();
}

void
oski_InitModule (void)
{
  oski_PrintDebugMessage (2, "Loading CB(ind_t=%s, val_t=%s)",
			  MACRO_TO_STRING (IND_T), MACRO_TO_STRING (VAL_T));
  oski_PrintDebugMessage (3, oski_GetShortDesc ());
}

void
oski_CloseModule (void)
{
  oski_PrintDebugMessage (2, "Shutting down CB(%s, %s) module",
			  MACRO_TO_STRING (IND_T), MACRO_TO_STRING (VAL_T));
}

static void
DestroyCacheBlock (oski_submatCB_t * b)
{
  if (b != NULL)
    {
      if (b->mat != INVALID_MAT)
	oski_DestroyMat (b->mat);
      oski_Free (b);
    }
}

static void
DestroyCacheBlockList (simplelist_t * cb_list)
{
  size_t i;
  size_t nb = simplelist_GetLength (cb_list);
  for (i = 1; i <= nb; i++)
    DestroyCacheBlock ((oski_submatCB_t *) simplelist_GetElem (cb_list, i));
  simplelist_Destroy (cb_list);
}

void
oski_DestroyMatRepr (void *mat)
{
  oski_matCB_t *A_cb = (oski_matCB_t *) mat;
  simplelist_t *cbl;		/* list of cache blocks */
  if (A_cb == NULL)
    return;
  DestroyCacheBlockList (A_cb->cache_blocks);
  oski_Free (A_cb);
}

static oski_submatCB_t *
DuplicateCacheBlock (const oski_submatCB_t * A)
{
  oski_submatCB_t *A_copy = NULL;
  if (A == NULL)
    return NULL;
  A_copy = oski_Malloc (oski_submatCB_t, 1);
  if (A_copy == NULL)
    return NULL;
  if (A->mat == NULL)
    A_copy->mat = NULL;
  else
    {
      A_copy->mat = oski_CopyMat (A->mat);
      if (A_copy->mat == NULL)
	{
	  DestroyCacheBlock (A_copy);
	  return NULL;
	}
    }
  assert (A_copy != NULL);
  return A_copy;
}

static simplelist_t *
DuplicateCacheBlockList (const simplelist_t * cb_list)
{
  simplelist_t *cb_list_copy;
  size_t i, nb;
  if (cb_list == NULL)
    return NULL;
  cb_list_copy = simplelist_Create ();
  if (cb_list_copy == NULL)
    return NULL;

  nb = simplelist_GetLength (cb_list);
  for (i = 1; i <= nb; i++)
    {
      const oski_submatCB_t *b0 =
	(const oski_submatCB_t *) simplelist_GetElem (cb_list, i);
      oski_submatCB_t *b = DuplicateCacheBlock (b0);
      if (b == NULL)
	break;
      simplelist_Append (cb_list_copy, b);
    }
  if (i <= nb)
    {
      DestroyCacheBlockList (cb_list_copy);
      return NULL;
    }
  assert (cb_list_copy != NULL);
  return cb_list_copy;
}

void *
oski_CopyMatRepr (const void *mat, const oski_matcommon_t * props)
{
  const oski_matCB_t *A = (const oski_matCB_t *) mat;
  oski_matCB_t *A_copy = NULL;

  if (A == NULL)
    return NULL;

  A_copy = oski_Malloc (oski_matCB_t, 1);
  if (A_copy == NULL)
    return NULL;

  A_copy->cache_blocks = DuplicateCacheBlockList (A->cache_blocks);
  if (A_copy->cache_blocks == NULL)
    {
      oski_DestroyMatRepr (A_copy);
      return NULL;
    }
  assert (A_copy != NULL && A_copy->cache_blocks != NULL);
  return A_copy;
}

/* eof */
