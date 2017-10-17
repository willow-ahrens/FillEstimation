/**
 *  \file src/structhint.c
 *  \brief Implementation of structural tuning hint data structure.
 *  \ingroup TUNING
 */

#include <assert.h>
#include <oski/common.h>
#include <oski/structhint.h>

/**
 *  \brief
 *
 *  \param[in,out] hint Structural hint record.
 *
 *  \post Initializes the fields of a structural hint record
 *  to default values.
 */
void
oski_InitStructHint (oski_structhint_t * hint)
{
  if (hint == NULL)
    return;

  hint->block_size.num_sizes = 0;
  hint->block_size.r_sizes = NULL;
  hint->block_size.c_sizes = NULL;

  hint->is_unaligned = 0;

  hint->is_symm = 0;

  hint->is_random = 0;

  hint->diag_lens.num_lens = 0;
  hint->diag_lens.d_lens = NULL;
}

void
oski_ResetStructHintDiaglens (oski_structhint_t * hint)
{
  if (hint == NULL)
    return;

  if (hint->diag_lens.d_lens != NULL)
    oski_Free (hint->diag_lens.d_lens);
  hint->diag_lens.d_lens = NULL;
  hint->diag_lens.num_lens = 0;
}

/**
 *  \brief
 *
 *  \param[in,out] hint Hint to which this routine applies.
 *  \param[in] num_lens Number of diagonal lengths for which to allocate
 *  space.
 *
 *  Even if the allocation fails, the diagonal length size count is set to
 *  'num_lens' and the previous diagonal length list is lost (freed).
 *
 *  If num_lens is 0, then no space is allocated.
 *
 *  \returns Returns 1 <==> the lists could be allocated.
 */
int
oski_AllocStructHintDiaglens (oski_structhint_t * hint, int num_lens)
{
  oski_ResetStructHintDiaglens (hint);

  hint->diag_lens.num_lens = num_lens;

  if (num_lens > 0)
    {
      hint->diag_lens.d_lens = oski_Malloc (int, num_lens);
      return (hint->diag_lens.d_lens != NULL) || (num_lens == 0);
    }
  else
    return 1;
}

void
oski_ResetStructHintBlocksizes (oski_structhint_t * hint)
{
  if (hint == NULL)
    return;

  if (hint->block_size.r_sizes != NULL)
    oski_Free (hint->block_size.r_sizes);
  hint->block_size.r_sizes = NULL;

  if (hint->block_size.c_sizes != NULL)
    oski_Free (hint->block_size.c_sizes);
  hint->block_size.c_sizes = NULL;

  hint->block_size.num_sizes = 0;
}

/**
 *  \brief
 *
 *  \param[in,out] hint Hint to set.
 *  \param[in] num_sizes Number of block sizes to set.
 *
 *  Even if the allocation fails, the block size count is set to
 *  'num_sizes' and the previous block size list is lost (freed).
 *
 *  If num_sizes is 0, then no space is allocated.
 *
 *  \returns Returns 1 <==> no errors occurred.
 */
int
oski_AllocStructHintBlocksizes (oski_structhint_t * hint, int num_sizes)
{
  oski_ResetStructHintBlocksizes (hint);

  hint->block_size.num_sizes = num_sizes;

  if (num_sizes > 0)
    {
      hint->block_size.r_sizes = oski_Malloc (int, num_sizes);
      hint->block_size.c_sizes = oski_Malloc (int, num_sizes);
      return (hint->block_size.r_sizes != NULL)
	&& (hint->block_size.c_sizes != NULL);
    }
  else
    return 1;
}

/**
 *  \brief
 *
 *  \param[in,out] hint Reinitializes a previously allocated and
 *  used structural hint record.
 *
 *  This routine reinitializes the fields of 'hint' to their
 *  default values, and frees any associated data stored with
 *  the 'hint' on call to this routine.
 */
void
oski_ResetStructHint (oski_structhint_t * hint)
{
  if (hint == NULL)
    return;

  oski_ResetStructHintBlocksizes (hint);
  oski_ResetStructHintDiaglens (hint);
  oski_InitStructHint (hint);
}

/* eof */
