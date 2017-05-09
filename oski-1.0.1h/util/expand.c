/**
 *  \file util/hbexpand.c
 *
 *  \brief Routine to expand a symmetric/Hermitian CSR/CSC matrix to
 *  full storage.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <oski/config.h>
#include <oski/common.h>
#include "readhbpat.h"

/**
 *  \brief Temporary array element structure containing an integer index
 *  and its corresponding scalar non-zero value.
 */
typedef struct tagindvalpair_t
{
  oski_index_t index;  /**< Integer index */
  oski_value_t value;  /**< Corresponding non-zero value */
} indvalpair_t;


/**
 *  \brief Compare two index-value pairs (see #indvalpair_t), a and b,
 *  and returns 0 if index(a) == index(b), 1 if index(a) > index(b),
 *  and -1 if index(a) < index(b).
 *
 *  Used as a callback element comparison function for qsort().
 */
static int
CompareIndValPairs (const void *pa, const void *pb)
{
  const indvalpair_t *a = (const indvalpair_t *) pa;
  const indvalpair_t *b = (const indvalpair_t *) pb;

  if (a->index > b->index)
    return 1;
  else if (a->index < b->index)
    return -1;
  else
    return 0;
}

/**
 *  \brief Copies all of the indices ('base'-based) and values from row i
 *  (0-based) of A to the array row of maximum length max_nnz.
 */
static void
CopyToPairs (const oski_index_t* ptr, const oski_index_t* ind,
             const oski_value_t* val,
             oski_index_t base,
             oski_index_t i,
             indvalpair_t * pairs, oski_index_t max_nnz)
{
  oski_index_t a = ptr[i] - base;
  oski_index_t b = ptr[i+1] - base;
  oski_index_t nnz = b - a;

  int k;

  assert (nnz <= max_nnz);

  for (k = 0; k < nnz; k++)
    {
      assert ((a + k) < b);
      pairs[k].index = ind[a + k];
      VAL_ASSIGN (pairs[k].value, val[a + k]);
    }
}

/**
 *  \brief Copies all of the indices and values in
 *  the array row (of maximum length max_nnz) to row i
 *  (0-based) of A to the array row of maximum length max_nnz.
 */
static void
CopyFromPairs (const indvalpair_t* pairs, oski_index_t max_nnz,
               const oski_index_t* ptr, oski_index_t* ind, oski_value_t* val,
               oski_index_t base, oski_index_t i)
{
  oski_index_t a = ptr[i] - base;
  oski_index_t b = ptr[i + 1] - base;
  oski_index_t nnz = b - a;

  int k;

  assert (nnz <= max_nnz);

  for (k = 0; k < nnz; k++)
    {
      assert ((a + k) < b);

      ind[a + k] = pairs[k].index;
      VAL_ASSIGN (val[a + k], pairs[k].value);
    }
}

/**
 *  \brief Sorts the matrix indices in ascending order.
 *
 *  \returns 0 on success, and an error code on error.
 */
static
int
SortIndices (const oski_index_t* ptr, oski_index_t* ind, oski_value_t* val,
             oski_index_t base,
             oski_index_t m)
{
  oski_index_t i;
  oski_index_t max_nnz;

  indvalpair_t* pairs;

  if (m <= 0)
    return 0;

  if (!ptr || ((!ind || !val) && ptr[m]))
    return ERR_BAD_ARG;

  max_nnz = ptr[1] - ptr[0];

  for (i = 1; i < m; i++)
    {
      int nnz = ptr[i + 1] - ptr[i];
      if (nnz > max_nnz)
	max_nnz = nnz;
    }

  pairs = oski_Malloc (indvalpair_t, max_nnz);
  if (pairs == NULL && max_nnz > 0)
    return ERR_OUT_OF_MEMORY;

  for (i = 0; i < m; i++)
    {
      oski_index_t nnz = ptr[i + 1] - ptr[i];

      CopyToPairs (ptr, ind, val, base, i, pairs, max_nnz);
      qsort (pairs, nnz, sizeof (indvalpair_t), CompareIndValPairs);
      CopyFromPairs (pairs, max_nnz, ptr, ind, val, base, i);
    }

  oski_Free (pairs);
  return 0;
}

/**
 *  \brief Allocates the basic CSR/CSC data structure.
 *  \returns 0 on success, or an error code.
 */
static
int
AllocAndZero (oski_index_t m, oski_index_t nnz,
              oski_index_t** p_ptr, oski_index_t** p_ind,
              oski_value_t** p_val)
{
  oski_index_t* ptr = NULL;
  oski_index_t* ind = NULL;
  oski_value_t* val = NULL;

  ptr = oski_Malloc (oski_index_t, m+1);
  if (!ptr) return ERR_OUT_OF_MEMORY;
  ind = oski_Malloc (oski_index_t, nnz);
  if (!ind && nnz) { oski_Free (ptr); return ERR_OUT_OF_MEMORY; }
  val = oski_Malloc (oski_value_t, nnz);
  if (!val && nnz)
    {
      oski_Free (ptr);
      oski_Free (ind);
      return ERR_OUT_OF_MEMORY;
    }

  oski_ZeroMem (ptr, sizeof (oski_index_t) * (m + 1));
  oski_ZeroMem (ind, sizeof (oski_index_t) * nnz);
  oski_ZeroMem (val, sizeof (oski_value_t) * nnz);

  if (p_ptr) *p_ptr = ptr; else oski_Free (ptr);
  if (p_ind) *p_ind = ind; else oski_Free (ind);
  if (p_val) *p_val = val; else oski_Free (val);
  return 0;
}


int
readhb_expand_symm (oski_index_t n, oski_index_t base, int is_herm,
                    const oski_index_t* Sptr, const oski_index_t* Sind,
                    const oski_value_t* Sval,
                    oski_index_t** p_Aptr, oski_index_t** p_Aind,
                    oski_value_t** p_Aval)
{
  oski_index_t* Aptr;
  oski_index_t* Aind;
  oski_value_t* Aval;

  oski_index_t *cur_row_nnz;	/* # of non-zeros in each row of S */
  oski_index_t *new_row_nnz;	/* # of non-zeros in each row of A */
  oski_index_t new_nnz;		/* total # of non-zeros in A */

  oski_index_t i;		/* current row (0-based index) */
  int err;

  if (!Sptr) return ERR_BAD_ARG;
  if (!Sind && Sptr[n]) return ERR_BAD_ARG;
  if (!Sval && Sptr[n]) return ERR_BAD_ARG;

  /* Set-up */
  cur_row_nnz = oski_Malloc (oski_index_t, n);
  if (cur_row_nnz == NULL && n > 0)
    return ERR_OUT_OF_MEMORY;
  oski_ZeroMem (cur_row_nnz, sizeof (oski_index_t) * n);

  new_row_nnz = oski_Malloc (oski_index_t, n);
  if (new_row_nnz == NULL && n > 0)
    {
      oski_Free (cur_row_nnz);
      return ERR_OUT_OF_MEMORY;
    }
  oski_ZeroMem (new_row_nnz, sizeof (oski_index_t) * n);

  /*
   * Scan S and count how many new non-zeros we'll need to create.
   *
   * Post:
   *   cur_row_nnz[i] == # of non-zeros in row i of S.
   *   new_row_nnz[i] == # of non-zeros to be stored in row i of A.
   *   new_nnz == total # of non-zeros to be stored in A.
   */
  new_nnz = 0;
  for (i = 0; i < n; i++)
    {
      cur_row_nnz[i] = Sptr[i + 1] - Sptr[i];
      new_row_nnz[i] = cur_row_nnz[i];
      new_nnz += new_row_nnz[i];
    }
  for (i = 0; i < n; i++)
    {
      oski_index_t k;
      for (k = Sptr[i] - base; k < Sptr[i + 1] - base; k++)
	{
	  oski_index_t j = Sind[k] - base;	/* column index, 0-based */
	  if (j != i)
	    {
	      new_row_nnz[j]++;
	      new_nnz++;
	    }
	}
    }

  /* initialize new matrix */
  err = AllocAndZero (n, new_nnz, &Aptr, &Aind, &Aval);
  if (err)
    {
      oski_Free (cur_row_nnz);
      oski_Free (new_row_nnz);
      return err;
    }

  /*
   *  Initialize row pointers in A.
   *
   *  Post:
   *    Aptr initialized to the correct, final values.
   *    new_row_nnz[i] reset to be equal to cur_row_nnz[i].
   */
  Aptr[0] = base;
  for (i = 1; i <= n; i++)
    {
      Aptr[i] = Aptr[i - 1] + new_row_nnz[i - 1];
      new_row_nnz[i - 1] = cur_row_nnz[i - 1];
    }

  /*
   *  Complete expansion of S to full storage in A.
   *
   *  Post:
   *    A is the full-storage equivalent of S.
   *    new_row_nnz[i] == # of non-zeros in row i of A.
   */
  for (i = 0; i < n; i++)
    {
      oski_index_t cur_nnz = cur_row_nnz[i];
      oski_index_t k_cur = Sptr[i] - base;
      oski_index_t k_new = Aptr[i] - base;
      oski_index_t k;

      /* copy current non-zeros from S to A */
      for (k = 0; k < cur_nnz; k++)
	Aind[k_new + k] = Sind[k_cur + k];
      oski_CopyMem (Aval + k_new, Sval + k_cur, oski_value_t, cur_nnz);

      /* fill in the symmetric/Hermitian "missing" values */
      while (k_cur < (Sptr[i + 1] - base))
	{
	  /* column index of non-zero of S, 0-based */
	  oski_index_t j = Sind[k_cur] - base;

	  if (j != i)		/* if not a non-diagonal element ... */
	    {
	      oski_value_t a_ij;
	      oski_value_t a_ji;

	      VAL_ASSIGN (a_ij, Sval[k_cur]);

              if (is_herm)
		VAL_ASSIGN_CONJ (a_ji, a_ij);
	      else		/* just symmetric */
		VAL_ASSIGN (a_ji, a_ij);

	      /* position of this transposed element in A */
	      k_new = Aptr[j] + new_row_nnz[j] - base;

	      /* store */
	      Aind[k_new] = i + base;
	      VAL_ASSIGN (Aval[k_new], a_ji);

	      /*  update so next element stored at row j will appear
	       *  at the right place.
	       */
	      new_row_nnz[j]++;
	    }

	  k_cur++;
	}
    }

  /* clean-up */
  oski_Free (cur_row_nnz);
  oski_Free (new_row_nnz);

  /* Sort column indices */
  err = SortIndices (Aptr, Aind, Aval, 1, n);
  if (err)
    {
      oski_Free (Aptr);
      oski_Free (Aind);
      oski_Free (Aval);
      return err;
    }

  if (p_Aptr) *p_Aptr = Aptr; else oski_Free (Aptr);
  if (p_Aind) *p_Aind = Aind; else oski_Free (Aind);
  if (p_Aval) *p_Aval = Aval; else oski_Free (Aval);
  return 0;
}

/* eof */
