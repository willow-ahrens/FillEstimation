/**
 *  \file oski/structhint.h
 *  \brief Structural hint implementation.
 *  \ingroup HINT
 *
 *  Structural hints are stored in records defined by the
 *  #oski_structhint_t type, below, and stored with the
 *  matrix (see #oski_matstruct_t).
 *
 *  Workload hints are stored as (simulated) traces with the matrix
 *  (see #oski_matstruct_t).
 */

#if !defined(INC_OSKI_STRUCTHINT_H)
/** oski/structhint.h has been included. */
#define INC_OSKI_STRUCTHINT_H

#include <oski/common.h>

/* ---------------------------------------- */

/**
 *  \ingroup HINT
 */
/*@{*/

/**
 *  \brief Structural hint record.
 */
typedef struct
{
	/** Dense block substructure (Group 1 hints). */
  struct
  {
    int num_sizes;	       /**< Number of specified block sizes, \f$k\f$. */
    int *r_sizes;	       /**< \f$r_1, \ldots, r_k\f$. */
    int *c_sizes;	       /**< \f$c_1, \ldots, c_k\f$. */
  } block_size;

	/** Dense block alignment (Group 2 hints). */
  int is_unaligned;

	/** Symmetric structure (Group 3 hints). */
  int is_symm;

	/** Correlated pattern hints (Group 4 hints). */
  int is_random;

	/** Diagonal lengths structure (Group 5 hints). */
  struct
  {
    int num_lens;	      /**< Number of specified diagonal lengths, \f$k\f$. */
    int *d_lens;	      /**< Diagonal lengths, \f$d_1, \ldots, d_k\f$. */
  } diag_lens;
}
oski_structhint_t;

/* ---------------------------------------- */


/** \brief Initialize a structural hint record. */
void oski_InitStructHint (oski_structhint_t * hint);

/** \brief Reset a structural hint record. */
void oski_ResetStructHint (oski_structhint_t * hint);

/**
 *  \brief Clears the diagonal length block list,
 *  and resets the count of number of diagonal lengths to 0.
 */
void oski_ResetStructHintDiaglens (oski_structhint_t * hint);

/**
 *  \brief Attempt to allocate space for some number of diagonal lengths.
 */
int oski_AllocStructHintDiaglens (oski_structhint_t * hint, int num_lens);

/**
 *  \brief Clears the dense block size list,
 *  and resets the count of number of block sizes to 0.
 */
void oski_ResetStructHintBlocksizes (oski_structhint_t * hint);

/**
 *  \brief Attempt to allocate space of 'num_sizes' block sizes.
 */
int oski_AllocStructHintBlocksizes (oski_structhint_t * hint, int num_sizes);

/*@}*/

#endif /* !defined(INC_OSKI_STRUCTHINT_H) */

/* eof */
