/**
 *  \file src/perm.c
 *  \brief Permutations.
 *  \ingroup PERM
 */

#include <assert.h>
#include <oski/perm.h>

/** Statically declared symbolic identity permutation */
static oski_permstruct_t OBJ_PERM_IDENTITY = MAKE_PERMSTRUCT_INIT (0, NULL);
/** Alias for OBJ_PERM_IDENTITY */
const oski_perm_t PERM_IDENTITY = &OBJ_PERM_IDENTITY;

/**
 *  \brief
 *
 *  \param[in] A_tunable Matrix to check.
 *  \returns 1 <==> the matrix was permuted using a non-trivial
 *  permutation, or 0 otherwise.
 *
 *  \todo Update this routine when Ali Pinar's TSP reordering
 *  code is implemented.
 */
int
oski_IsMatPermuted (const oski_matrix_t A_tunable)
{
  return 0;
}

/**
 *  \brief
 *
 *  \param[in] A_tunable Matrix to check.
 *  \returns The permuted matrix \f$\hat{A}\f$, or INVALID_MAT
 *  on error.
 *
 *  \todo Update this routine when Ali Pinar's TSP reordering
 *  code is implemented.
 */
const_oski_matrix_t
oski_ViewPermutedMat (const oski_matrix_t A_tunable)
{
  return A_tunable;
}

/**
 *  \brief
 *
 *  \todo Update this routine when Ali Pinar's TSP reordering
 *  code is implemented.
 */
const_oski_perm_t
oski_ViewPermutedMatRowPerm (const oski_matrix_t A_tunable)
{
  return PERM_IDENTITY;
}

/**
 *  \brief
 *
 *  \todo Update this routine when Ali Pinar's TSP reordering
 *  code is implemented.
 */
const_oski_perm_t
oski_ViewPermutedMatColPerm (const oski_matrix_t A_tunable)
{
  return PERM_IDENTITY;
}

/**
 *  Returns an error code if P is an invalid permutation matrix,
 *  i.e., if P is NULL, has length < 0, or has any element out
 *  of range.
 */
static int
CheckValidPerm (const oski_perm_t P)
{
  int err = 0;			/* No obvious error detected... */

  if (P != NULL && ((P->length > 0 && P->perm != NULL) || (P->length == 0)))
    {
      const oski_index_t *p;
      oski_index_t i;

      for (i = 0, p = P->perm; i < P->length; i++, p++)
	if (p[i] < 0 || p[i] >= P->length)
	  {
	    err = ERR_BAD_PERM;
	    oski_HandleError (err,
			      "Permutation has a bad element", __FILE__,
			      __LINE__,
			      "Element %d (value==%d) is out of the range, %d to %d.",
			      (int) i, (int) p[i], 0, (int) (P->length - 1));
	    break;
	  }

      if (!err)
	return 0;
    }

  /* error detected */
  OSKI_ERR_BAD_PERM (oski_PermuteVecView, 1);
  return err;
}

/**
 *  \brief Compute \f$y \leftarrow \mathrm{op}(P)\cdot x\f$, where
 *  \f$x, y\f$ are single dense strided vectors.
 *
 *  \pre All input arguments are valid.
 */
static void
Permute (const oski_index_t * P, oski_matop_t opP, oski_index_t len,
	 const oski_value_t * x, oski_index_t incx,
	 oski_value_t * y, oski_index_t incy)
{
  oski_index_t i;

  if (opP == OP_NORMAL || opP == OP_CONJ)
    {
      oski_value_t *yp;
      for (i = 0, yp = y; i < len; i++, yp += incy)
	yp[0] = x[P[i] * incx];
    }
  else				/* opP == OP_TRANS || opP == OP_CONJ_TRANS */
    {
      const oski_value_t *xp;
      for (i = 0, xp = x; i < len; i++, xp += incx)
	y[P[i] * incy] = xp[0];
    }
}

/**
 *  \brief
 *
 *  \param[in] P Permutation matrix \f$P\f$.
 *  \param[in] opP Transpose option.
 *  \param[in,out] x_view Vector view to permute.
 *  \returns 0 on success, or an error code.
 */
int
oski_PermuteVecView (const oski_perm_t P, oski_matop_t opP,
		     oski_vecview_t x_view)
{
  int err;

  oski_value_t *x_copy;
  oski_index_t j;

  if (!OSKI_CHECK_MATOP (opP))
    {
      OSKI_ERR_BAD_MATOP (oski_PermuteVecView, 2, opP);
      return ERR_BAD_ARG;
    }
  if (x_view == NULL)
    {
      OSKI_ERR_BAD_VEC (oski_PermuteVecView, 3);
      return ERR_BAD_VECVIEW;
    }
  if (x_view->num_rows != P->length)
    {
      oski_HandleError (ERR_DIM_MISMATCH,
			"Permutation and vector dimensions do not match",
			__FILE__, __LINE__,
			"The permutation is %dx%d, while the vector has %d rows.",
			(int) (P->length), (int) (P->length),
			(int) (x_view->num_rows));
      return ERR_DIM_MISMATCH;
    }

  err = CheckValidPerm (P);
  if (err)
    return err;

  x_copy = oski_Malloc (oski_value_t, P->length);
  if (x_copy == NULL)
    return ERR_OUT_OF_MEMORY;

  for (j = 0; j < x_view->num_cols; j++)	/* loop over columns of x */
    {
      oski_index_t i;
      oski_value_t *xp;

      /* Copy column j of x, i.e., x(:,j) */
      for (i = 0, xp = x_view->val + j * x_view->colinc;
	   i < x_view->num_rows; i++, xp += x_view->rowinc)
	{
	  x_copy[i] = xp[0];
	}

      xp = x_view->val + j * x_view->colinc;
      Permute (P->perm, opP, P->length, x_copy, 1, xp, x_view->rowinc);
    }

  oski_Free (x_copy);
  return 0;
}

/* eof */
