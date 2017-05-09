/**
 *  \file src/hint.c
 *  \brief Implementation of hint tracking for the tuning module.
 *  \ingroup TUNING
 */

#include <assert.h>
#include <stdarg.h>

#include <oski/common.h>
#include <oski/hint.h>

#include <oski/matmult.h>
#include <oski/ata.h>

/**
 *  \brief
 *
 *  If hints from the same group are specified, the last hint
 *  specified is the one set.
 *
 *  If an error occurs (e.g., bad arguments), then the hint record
 *  is unchanged.
 *
 *  \pre A_tunable's structural hint field must have been
 *  initialized by a call to oski_InitStructHint().
 *
 *  \todo Restructure so that va_end() is always called.
 */
int
oski_SetHint (oski_matrix_t A_tunable, oski_tunehint_t hint, ...)
{
  oski_structhint_t *hint_rec;
  va_list argp;

  if (A_tunable == INVALID_MAT)
    {
      OSKI_ERR_BAD_MAT (oski_SetHint, 1);
      return ERR_BAD_MAT;
    }

  if (!OSKI_CHECK_TUNEHINT (hint))
    {
      OSKI_ERR_BAD_HINT (oski_SetHint, 2, hint);
      return ERR_BAD_HINT_ARG;
    }

  va_start (argp, hint);

  hint_rec = &(A_tunable->structhints);
  switch (hint)
    {
    case HINT_NO_BLOCKS:
      oski_AllocStructHintBlocksizes (hint_rec, 0);
      break;

    case HINT_SINGLE_BLOCKSIZE:
      {
	int r = va_arg (argp, int);
	if (r != ARGS_NONE)	/* Process optional arguments, if any. */
	  {
	    int c = va_arg (argp, int);

	    if (r < 1)
	      {
		OSKI_ERR_BAD_HINT_ARG (oski_SetHint,
				       1, HINT_SINGLE_BLOCKSIZE, r);
		return ERR_BAD_HINT_ARG;
	      }
	    if (c < 1)
	      {
		OSKI_ERR_BAD_HINT_ARG (oski_SetHint,
				       2, HINT_SINGLE_BLOCKSIZE, c);
		return ERR_BAD_HINT_ARG;
	      }

	    if (oski_AllocStructHintBlocksizes (hint_rec, 1))
	      {
		hint_rec->block_size.r_sizes[0] = r;
		hint_rec->block_size.c_sizes[0] = c;
	      }
	    else
	      {
		oski_ResetStructHintBlocksizes (hint_rec);
		hint_rec->block_size.num_sizes = 1;
	      }
	  }
      }
      break;

    case HINT_MULTIPLE_BLOCKSIZES:
      {
	int k = va_arg (argp, int);

	if (k == ARGS_NONE)
	  {
	    oski_ResetStructHintBlocksizes (hint_rec);
	    hint_rec->block_size.num_sizes = 2;
	  }
	else if (k >= 1)	/* caller specified a list */
	  {
				/**
				 *  Attempt to allocate space for this list. If it
				 *  fails, we still record that there is a mix of
				 *  block sizes present, but do not record the
				 *  list. This behavior is OK because the implementation
				 *  is free to regard/disregard hints as desired.
				 */
	    if (oski_AllocStructHintBlocksizes (hint_rec, k))
	      {
		int i;
		int any_errors = 0;

		for (i = 0; i < k; i++)
		  {
		    int r = va_arg (argp, int);
		    int c = va_arg (argp, int);

		    if (r < 1)
		      {
			OSKI_ERR_BAD_HINT_ARG (oski_SetHint,
					       2 + 2 * i,
					       HINT_MULTIPLE_BLOCKSIZES, r);
			hint_rec->block_size.c_sizes[i] = 0;
			any_errors++;
		      }
		    else
		      hint_rec->block_size.r_sizes[i] = r;

		    if (c < 1)
		      {
			OSKI_ERR_BAD_HINT_ARG (oski_SetHint,
					       3 + 2 * i,
					       HINT_MULTIPLE_BLOCKSIZES, c);
			hint_rec->block_size.c_sizes[i] = 0;
			any_errors++;
		      }
		    else
		      hint_rec->block_size.c_sizes[i] = c;
		  }		/* for( i = ... ) */

		if (any_errors)
		  return ERR_BAD_HINT_ARG;
	      }
	  }
	else			/* k < 0! */
	  {
	    OSKI_ERR_BAD_HINT_ARG (oski_SetHint,
				   1, HINT_MULTIPLE_BLOCKSIZES, k);
	    return ERR_BAD_HINT_ARG;
	  }
      }
      break;

    case HINT_ALIGNED_BLOCKS:
      hint_rec->is_unaligned = 0;
      break;

    case HINT_UNALIGNED_BLOCKS:
      hint_rec->is_unaligned = 1;
      break;

    case HINT_SYMM_PATTERN:
      hint_rec->is_symm = 1;
      break;

    case HINT_NONSYMM_PATTERN:
      hint_rec->is_symm = 0;
      break;

    case HINT_RANDOM_PATTERN:
      hint_rec->is_random = 1;
      break;

    case HINT_CORREL_PATTERN:
      hint_rec->is_random = 0;
      break;

    case HINT_NO_DIAGS:
      oski_AllocStructHintDiaglens (hint_rec, 0);
      break;

    case HINT_DIAGS:
      {
	int k = va_arg (argp, int);

	if (k == ARGS_NONE)
	  {
	    oski_ResetStructHintDiaglens (hint_rec);
	    hint_rec->diag_lens.num_lens = k;
	  }
	else if (k >= 1)
	  {
				/**
				 *  Attempt to allocate space for this list. If it
				 *  fails, we still record that there is a mix of
				 *  block sizes present, but do not record the
				 *  list. This behavior is OK because the implementation
				 *  is free to regard/disregard hints as desired.
				 */
	    if (oski_AllocStructHintBlocksizes (hint_rec, k))
	      {
		int i;

		for (i = 0; i < k; i++)
		  {
		    int d = va_arg (argp, int);

		    if (d <= 0)
		      {
			OSKI_ERR_BAD_HINT_ARG (oski_SetHint,
					       1 + i, HINT_DIAGS, d);
			hint_rec->block_size.c_sizes[i] = 0;
		      }
		    else
		      hint_rec->diag_lens.d_lens[i] = d;
		  }
	      }
	  }
	else			/* Invalid # of args, k, specified */
	  {
	    OSKI_ERR_BAD_HINT_ARG (oski_SetHint, 1, HINT_DIAGS, k);
	    return ERR_BAD_HINT_ARG;
	  }
      }
      break;

    default:
      assert (0);		/* should never occur */
    }

  va_end (argp);
  return 0;
}

/**
 *  \brief
 *
 *  \note Returns with no action when num_calls <= 0.
 */
int
oski_SetHintMatMult (oski_matrix_t A_tunable, oski_matop_t opA,
		     oski_value_t alpha, const oski_vecview_t x_view,
		     oski_value_t beta, const oski_vecview_t y_view,
		     int num_calls)
{
  oski_traceargs_MatMult_t args;
  int err;
  if (num_calls <= 0)
    return 0;

  /* Error check input parameters */
  err = oski_CheckArgsMatMult (A_tunable, opA,
			       alpha, x_view, beta, y_view,
			       MACRO_TO_STRING (oski_SetHintMatMult));
  if (err)
    return err;

  /*
   *  Record this hint. Although this call may fail, ignore it
   *  since hints are strictly optional anyway.
   */
  oski_MakeArglistMatMult (opA, alpha, x_view, beta, y_view, &args);
  oski_RecordCalls (A_tunable->workhints, OSKI_KERNEL_MatMult,
		    &args, sizeof (args), NULL, num_calls, TIME_NOT_AVAIL);

  return 0;
}

/**
 *  \brief
 *
 *  \note Returns with no action when num_calls <= 0.
 */
int
oski_SetHintMatTransMatMult (oski_matrix_t A_tunable,
			     oski_ataop_t opA,
			     oski_value_t alpha, const oski_vecview_t x_view,
			     oski_value_t beta, const oski_vecview_t y_view,
			     const oski_vecview_t t_view, int num_calls)
{
  oski_traceargs_MatTransMatMult_t args;
  int err;

  if (num_calls <= 0)
    return 0;

  /* Error check input parameters */
  err = oski_CheckArgsMatTransMatMult (A_tunable, opA,
				       alpha, x_view, beta, y_view, t_view,
				       MACRO_TO_STRING
				       (oski_SetHintMatTransMatMult));
  if (err)
    return err;

  /*
   *  Record this hint. Although this call may fail, ignore it
   *  since hints are strictly optional anyway.
   */
  oski_MakeArglistMatTransMatMult (opA, alpha, x_view, beta, y_view, t_view,
				   &args);
  oski_RecordCalls (A_tunable->workhints, OSKI_KERNEL_MatTransMatMult,
		    &args, sizeof (args), NULL, num_calls, TIME_NOT_AVAIL);

  return 0;
}

int
oski_SetHintMatMultAndMatTransMult (oski_matrix_t A_tunable,
				    oski_value_t alpha,
				    const oski_vecview_t x_view,
				    oski_value_t beta,
				    const oski_vecview_t y_view,
				    oski_matop_t opA, oski_value_t omega,
				    const oski_vecview_t w_view,
				    oski_value_t zeta,
				    const oski_vecview_t z_view,
				    int num_calls)
{
  oski_traceargs_MatMultAndMatTransMult_t args;
  int err;

  if (num_calls <= 0)
    return 0;

  /* Error check input parameters */
  err = oski_CheckArgsMatMultAndMatTransMult (A_tunable,
					      alpha, x_view, beta, y_view,
					      opA, omega, w_view, zeta,
					      z_view,
					      MACRO_TO_STRING
					      (oski_SetHintMatMultAndMatTransMult));
  if (err)
    return err;

  /* Record hint */
  oski_MakeArglistMatMultAndMatTransMult (alpha, x_view, beta, y_view,
					  opA, omega, w_view, zeta, z_view,
					  &args);
  oski_RecordCalls (A_tunable->workhints, OSKI_KERNEL_MatMultAndMatTransMult,
		    &args, sizeof (args), NULL, num_calls, TIME_NOT_AVAIL);

  return 0;
}

int
oski_SetHintMatTrisolve (oski_matrix_t T_tunable, oski_matop_t opT,
			 oski_value_t alpha, const oski_vecview_t x_view,
			 int num_calls)
{
  oski_traceargs_MatTrisolve_t args;
  int err;

  if (num_calls <= 0)
    return 0;

  /* Error check input parameters */
  err = oski_CheckArgsMatTrisolve (T_tunable, opT, alpha, x_view,
				   MACRO_TO_STRING (oski_SetHintMatTrisolve));
  if (err)
    return err;

  /* Record hint */
  oski_MakeArglistMatTrisolve (opT, alpha, x_view, &args);
  oski_RecordCalls (T_tunable->workhints, OSKI_KERNEL_MatTrisolve,
		    &args, sizeof (args), NULL, num_calls, TIME_NOT_AVAIL);

  return 0;
}

int
oski_SetHintMatPowMult (oski_matrix_t A_tunable, oski_matop_t opA, int power,
			oski_value_t alpha, const oski_vecview_t x_view,
			oski_value_t beta, const oski_vecview_t y_view,
			const oski_vecview_t T_view, int num_calls)
{
  oski_traceargs_MatPowMult_t args;
  int err;

  if (num_calls <= 0)
    return 0;

  /* Error check input parameters */
  err = oski_CheckArgsMatPowMult (A_tunable, opA, power,
				  alpha, x_view, beta, y_view, T_view,
				  MACRO_TO_STRING (oski_SetHintMatPowMult));
  if (err)
    return err;

  /* Record hint */
  oski_MakeArglistMatPowMult (opA, power,
			      alpha, x_view, beta, y_view, T_view, &args);
  oski_RecordCalls (A_tunable->workhints, OSKI_KERNEL_MatPowMult,
		    &args, sizeof (args), NULL, num_calls, TIME_NOT_AVAIL);

  return 0;
}

/* eof */
