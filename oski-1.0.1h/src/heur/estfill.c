/**
 *  \file src/heur/estfill.c
 *  \brief BCSR fill ratio estimator.
 *  \ingroup TUNING
 */

/* Select a random number generator */
#if HAVE_CONFIG_H
#include <config/config.h>

#if HAVE_DRAND48
	/** Returns a random value in the interval [0,1] */
extern double drand48 (void);
#define RAND_DOUBLE   (drand48())

#elif HAVE_RANDOM
	/** Returns a random value in the interval [0,1] */
#define RAND_DOUBLE   ((double)random()/RAND_MAX)

#elif HAVE_RAND
	/** Returns a random value in the interval [a,b] */
#define RAND_DOUBLE   ((double)rand()/RAND_MAX)
#else

#error "Didn't find a suitable random number generator, having tried drand48(), random(), and rand()."

#endif

#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <oski/common.h>
#include <oski/heur/estfill.h>

/**
 *  \brief Given an \f$m\times n\f$ CSR matrix \f$A\f$,
 *  estimates the fill ratio if the matrix were converted
 *  into \f$r\times c\f$ BCSR format.
 *
 *  The caller supplies this routine with a maximum column
 *  block size \f$C\f$, and this routine returns the
 *  estimated fill ratios for all \f$1 \leq c \leq C\f$.
 *
 *  If the converted matrix has \f$n_b\f$ blocks, this
 *  implementation executes in
 *  \f$O(\mbox{stored non-zeros}) = O(n_b\cdot r\cdot c)\f$
 *  time, but requires \f$O(C\cdot n)\f$ auxiliary storage
 *  space to store a dense copies of the block rows.
 *
 *  This routine assumes the CSR matrix uses full storage,
 *  but otherwise is flexible with regard to the following
 *  variations:
 *    - 0 or 1-based indexing
 *    - 'ptr[0]' does not have to start at 'base' (i.e.,
 *    caller can pass a subset of rows of a larger matrix)
 *    - Column indices do not have to be sorted.
 *
 *  \param[in] ptr CSR row pointers.
 *  \param[in] ind CSR column indices.
 *  \param[in] base Index base (0-based or 1-based)
 *  \param[in] m Logical number of matrix rows
 *  \param[in] n Logical number of matrix columns
 *  \param[in] r Desired row block size
 *  \param[in] max_c Maximum desired column block size (\f$C\f$).
 *  \param[in] prob_examine The desired probability of
 *  examining each block row.
 *  \param[in,out] p_nnz_est Used to return the number of
 *  non-zeros actually examined. Must be non-NULL.
 *  \param[in, out] p_nb_est Used to return the number of
 *  \f$r\times c\f$ blocks that would be created for the
 *  non-zeros examined. Must be non-NULL array of length
 *  \f$C = \f$ max_c.
 *
 *  \returns On success, returns 0, sets *p_nnz_est
 *  to the number of non-zeros examined, and sets p_nb_est[c-1]
 *  to the number of non-zero blocks that are needed to
 *  store the examined non-zeros in \f$r \times c\f$ format.
 *  On error, returns an error code and leaves p_bptr, p_bind,
 *  and p_bval unchanged.
 */
static int
EstimateBlockCounts (const oski_index_t * ptr, const oski_index_t * ind,
		     oski_index_t base,
		     oski_index_t m, oski_index_t n,
		     oski_index_t r, oski_index_t max_c,
		     double prob_examine,
		     oski_index_t * p_nnz_est, oski_index_t * p_nb_est)
{
  /* block dimensions */
  oski_index_t M;

  /* stores total number of non-zero blocks */
  oski_index_t num_nonzeros;

  /* auxiliary storage: reused for each block-row */
  oski_index_t *block_count;	/* size N */
  oski_index_t I;		/* block-row iteration variable */

  assert (p_nnz_est != NULL);
  assert (p_nb_est != NULL);

  M = m / r;			/* # of full block-rows */

  if (n == 0)
    {
      *p_nnz_est = 0;
      oski_ZeroMem (p_nb_est, sizeof (oski_index_t) * max_c);
      return 0;			/* Quick return */
    }

  /* ----------------------------------------------------
   *  Allocate temporary space.
   */

  assert (n >= 1);
  block_count = oski_Malloc (oski_index_t, max_c * n);
  if (block_count == NULL)
    {
      return ERR_OUT_OF_MEMORY;
    }
  oski_ZeroMem (block_count, sizeof (oski_index_t) * max_c * n);

/** Get the block count for block column size c, block column J. */
#define GET_BC(A, c, J) (A)[((c)-1)*n + (J)]
/** Increment the block count for block column size c, block column J. */
#define INC_BC(A, c, J) (A)[((c)-1)*n + (J)]++
/** Set the block count for block column size c, block column J, to zero. */
#define ZERO_BC(A, c, J) (A)[((c)-1)*n + (J)] = 0

  /* ----------------------------------------------------
   *  Phase I: Count the number of new blocks to
   *  create.
   */
  oski_ZeroMem (p_nb_est, sizeof (oski_index_t) * max_c);
  num_nonzeros = 0;
  for (I = 0; I < M; I++)	/* loop over block rows */
    {
      oski_index_t i;
      oski_index_t di;

      double rand_val = RAND_DOUBLE;

      if (rand_val > prob_examine)
	continue;		/* skip this block row */

      /* Count the number of blocks within block-row I, and
       * remember in 'block_count' which of the possible blocks
       * have been 'visited' (i.e., contain at least 1 non-zero).
       */
      for (i = I * r, di = 0; di < r; di++, i++)
	{
	  oski_index_t k;

	  /* Invariant: block_count[J] == # of non-zeros
	   * encountered in rows I*r .. I*r+di that should
	   * be stored in column-block J (i.e., that have
	   * column indices J*c <= j < (J+1)*c).
	   */

	  /* Count the number of additional logical blocks
	   * needed to store non-zeros in row i, and mark
	   * the blocks in block row I that have been visited.
	   */
	  for (k = ptr[i] - base; k < ptr[i + 1] - base; k++)
	    {
	      oski_index_t j = ind[k] - base;	/* column index */
	      oski_index_t c;

	      for (c = 1; c <= max_c; c++)
		{
		  oski_index_t J = j / c;	/* block column index */

		  if (GET_BC (block_count, c, J) == 0)
		    {
		      /* "create" (count) new block */
		      INC_BC (block_count, c, J);
		      p_nb_est[c - 1]++;
		    }
		}
	    }
	}
      num_nonzeros += ptr[i] - ptr[I * r];

      /* POST: num_nonzeros == total # of non-zeros examined so far */
      /* POST: num_blocks == total # of new blocks in rows 0..i */
      /* POST: block_count[c,J] == # of non-zeros in block J of block-row I */

      /* Reset block_count for next iteration, I+1. This second loop
       * is needed to keep the complexity of phase I to O(nnz).
       */
      for (i = I * r, di = 0; di < r; di++, i++)
	{
	  oski_index_t k;

	  for (k = ptr[i] - base; k < ptr[i + 1] - base; k++)
	    {
	      oski_index_t j = ind[k] - base;	/* column index */
	      oski_index_t c;

	      for (c = 1; c <= max_c; c++)
		{
		  oski_index_t J = j / c;	/* block column index */
		  ZERO_BC (block_count, c, J);
		}
	    }
	}
    }
  /* POST: num_blocks == total # of blocks in examined rows. */
  /* POST: num_nonzeros == total # of non-zeros in examined rows. */

  oski_Free (block_count);

  *p_nnz_est = num_nonzeros;
  return 0;
}

#if 0
EstimateBlockCounts (const oski_index_t * ptr, const oski_index_t * ind,
		     oski_index_t base,
		     oski_index_t m, oski_index_t n,
		     oski_index_t r, oski_index_t c,
		     double prob_examine,
		     oski_index_t * p_nnz_est, oski_index_t * p_nb_est);
#endif

/**
 *  \brief Check input arguments to a function with a signature
 *  like \ref oski_EstimateFillBCSR().
 */
static int
CheckArgs (const oski_matspecific_t * A, const oski_matcommon_t * props,
	   size_t max_r, size_t max_c, double prob_examine,
	   const char *caller)
{
  int id_CSR, id_CSC;

  if (A == NULL)
    {
      OSKI_ERR_BAD_MAT_MS (caller, 1);
      return ERR_BAD_ARG;
    }

  id_CSR = oski_LookupMatTypeId ("CSR", OSKI_IND_ID, OSKI_VAL_ID);
  id_CSC = oski_LookupMatTypeId ("CSC", OSKI_IND_ID, OSKI_VAL_ID);

  if (A->type_id != id_CSR && A->type_id != id_CSC)
    {
      oski_HandleError (ERR_BAD_MAT,
			"Fill can only be estimated for CSR or CSC matrices",
			__FILE__, __LINE__,
			"In %s(), parameter #%d specifies a matrix of type %d.\n",
			caller, 1, (int) A->type_id);
      return ERR_BAD_ARG;
    }

  if (props == NULL)
    {
      OSKI_ERR_BAD_MAT_MS (caller, 2);
      return ERR_BAD_ARG;
    }

  if (max_r <= 0)
    {
      OSKI_ERR_SMALL_ARG_MS (caller, 3, "maximum row block size", 1);
      return ERR_BAD_ARG;
    }

  if (max_c <= 0)
    {
      OSKI_ERR_SMALL_ARG_MS (caller, 4, "maximum column block size", 1);
      return ERR_BAD_ARG;
    }

  if (!IS_VAL_IN_RANGE (prob_examine, 0.0, 1.0))
    {
      oski_HandleError (ERR_BAD_ARG,
			"Invalid probability specified", __FILE__, __LINE__,
			"In %s(), check the probability value specified"
			" (%f, parameter #%d), is not in the closed interval"
			" [0, 1].", caller, prob_examine, 5);
      return ERR_BAD_ARG;
    }

  return 0;
}

/**
 *  \brief This routine is a wrapper around \ref EstimateBlockCounts().
 *
 *  \param[in] A Matrix in CSR format.
 *  \param[in] props Properties of A.
 *  \param[in] max_r Maximum row block size to consider.
 *  \param[in] max_c Maximum column block size to consider.
 *  \param[in] prob_examine Probability of examining a given
 *  block row.
 *  \param[in,out] fill Structure in which to store the fill
 *  ratio profile for A.
 *
 *  \note This routine does not have the property that
 *  setting prob_examine == 1.0 yields the exact fill
 *  ratio, because it ignores leftover rows.
 *
 *  \returns Returns a non-zero error code on error.
 */
static int
EstimateFillFromCSR (const oski_matCSR_t * A,
		     const oski_matcommon_t * props, size_t max_r,
		     size_t max_c, double prob_examine,
		     oski_fillprofile_BCSR_t * fill)
{
  int r;
  oski_index_t *nb_est;
  int err;

  assert (A != NULL);

  nb_est = oski_Malloc (oski_index_t, max_c);
  if (nb_est == NULL)
    return ERR_OUT_OF_MEMORY;

  for (r = 1; r <= max_r; r++)
    {
      oski_index_t nnz_est;
      int c;

      nnz_est = 0;
      oski_ZeroMem (nb_est, sizeof (oski_index_t) * max_c);
      err = EstimateBlockCounts (A->ptr, A->ind, A->base_index,
				 props->num_rows, props->num_cols, r, max_c,
				 prob_examine, &nnz_est, nb_est);
      if (err)
	{
	  oski_Free (nb_est);
	  return err;
	}

      for (c = 1; c <= max_c; c++)
	{
          oski_index_t nb_nnz = nb_est[c-1] * r * c;
	  double ratio;
          if (!nnz_est)
            ratio = nb_nnz ? (1.0 / 0.0) : 1.0;
          else
            ratio = (double)nb_nnz / nnz_est;

	  PROF_FILLBCSR_SET (fill, r, c, ratio);
	}
    }

  oski_Free (nb_est);
  return 0;
}

/**
 *  \brief Estimate the fill ratio for a matrix initially in
 *  CSC format.
 *
 *  This implementation exploits the fact that the CSC
 *  representation is based on the transpose of the CSR
 *  implementation.
 *
 *  \see #EstimateFillFromCSR()
 */
static int
EstimateFillFromCSC (const oski_matCSC_t * A,
		     const oski_matcommon_t * props, size_t max_r,
		     size_t max_c, double prob_examine,
		     oski_fillprofile_BCSR_t * fill)
{
  const oski_matCSR_t *AT;
  int c;
  oski_index_t *nb_est;
  int err;

  assert (A != NULL && A->mat_trans != NULL);

  nb_est = oski_Malloc (oski_index_t, max_r);
  if (nb_est == NULL)
    return ERR_OUT_OF_MEMORY;

  AT = A->mat_trans;
  for (c = 1; c <= max_c; c++)
    {
      oski_index_t nnz_est;
      int r;

      nnz_est = 0;
      oski_ZeroMem (nb_est, sizeof (oski_index_t) * max_r);
      err = EstimateBlockCounts (AT->ptr, AT->ind, AT->base_index,
				 props->num_cols, props->num_rows, c, max_r,
				 prob_examine, &nnz_est, nb_est);
      if (err)
	{
	  oski_Free (nb_est);
	  return err;
	}

      for (r = 1; r <= max_r; r++)
	{
	  double ratio = (double) nb_est[r - 1] * r * c / nnz_est;
	  PROF_FILLBCSR_SET (fill, r, c, ratio);
	}
    }

  oski_Free (nb_est);
  return 0;
}

/**
 *  \brief
 *
 *  \returns If a fill estimate cannot be computed for the given
 *  matrix or matrix type, returns NULL and calls the error
 *  handler. Otherwise, returns a new fill profile.
 */
oski_fillprofile_BCSR_t *
oski_EstimateFillBCSR (const oski_matspecific_t * mat,
		       const oski_matcommon_t * props,
		       size_t max_r, size_t max_c, double prob_examine)
{
  oski_fillprofile_BCSR_t *fill;
  int id_CSR, id_CSC;
  int err;

  err = CheckArgs (mat, props, max_r, max_c, prob_examine,
		   MACRO_TO_STRING (oski_EstimateFillBCSR));
  if (err)
    return NULL;

  fill = oski_Malloc (oski_fillprofile_BCSR_t, 1);
  if (fill == NULL)
    return NULL;
  fill->ratio = oski_Malloc (double, max_r * max_c);
  if (fill->ratio == NULL)
    {
      oski_Free (fill);
      return NULL;
    }
  fill->max_r = max_r;
  fill->max_c = max_c;

  /* Dispatch based on matrix type */
  id_CSR = oski_LookupMatTypeId ("CSR", OSKI_IND_ID, OSKI_VAL_ID);
  id_CSC = oski_LookupMatTypeId ("CSC", OSKI_IND_ID, OSKI_VAL_ID);
  if (mat->type_id == id_CSR)
    err = EstimateFillFromCSR ((const oski_matCSR_t *) (mat->repr), props,
			       max_r, max_c, prob_examine, fill);
  else if (mat->type_id == id_CSC)
    err = EstimateFillFromCSC ((const oski_matCSC_t *) (mat->repr), props,
			       max_r, max_c, prob_examine, fill);
  else				/* should never execute */
    err = ERR_BAD_MAT;

  if (err)
    {
      oski_Free (fill->ratio);
      oski_Free (fill);
      fill = NULL;
    }
  return fill;
}

/**
 *  \brief
 *
 *  \todo Does not need to be type-dependent.
 */
void
oski_DestroyBCSRFillProfile (oski_fillprofile_BCSR_t * fill)
{
  if (fill != NULL)
    {
      if (fill->ratio != NULL)
	oski_Free (fill->ratio);
      oski_Free (fill);
    }
}

/* eof */
