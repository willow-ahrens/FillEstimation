/**
 *  \file src/matrix.c
 *  \brief Matrix handle implementation.
 *
 *  The interfaces specifies the creation of handles from concrete
 *  array representations (e.g., CSR & CSC format).
 */

#include <assert.h>
#include <oski/config.h>
#include <oski/common.h>
#include <oski/matrix.h>

/**
 *  \brief
 *
 *  Allocates space for a new matrix structure, and initializes
 *  its fields.
 *
 *  \returns A new matrix structure, or NULL on memory allocation
 *  error.
 *
 *  \see oski_DestroyMatStruct()
 */
oski_matstruct_t *
oski_CreateMatStruct (void)
{
  oski_matstruct_t *new_mat;

  new_mat = oski_Malloc (oski_matstruct_t, 1);
  if (new_mat == NULL)
    return NULL;

  new_mat->input_mat.type_id = INVALID_ID;
  new_mat->input_mat.repr = NULL;
  new_mat->is_shared = 0;

  new_mat->tuned_mat.type_id = INVALID_ID;
  new_mat->tuned_mat.repr = NULL;
  new_mat->tuned_xforms = NULL;

  new_mat->props.num_rows = 0;
  new_mat->props.num_cols = 0;
  new_mat->props.num_nonzeros = 0;
  new_mat->props.has_unit_diag = 0;
  new_mat->props.pattern.is_symm = 0;
  new_mat->props.pattern.is_herm = 0;
  new_mat->props.pattern.is_tri_upper = 0;
  new_mat->props.pattern.is_tri_lower = 0;

  new_mat->trace = oski_CreateTrace ();
  new_mat->workhints = oski_CreateTrace ();
  oski_InitStructHint (&(new_mat->structhints));

  new_mat->timer = oski_CreateTimer ();
  new_mat->time_stream = TIME_NOT_AVAIL;

  return new_mat;
}

/**
 *  \brief Dump the contents of a matrix handle's trace (DEBUGGING ONLY).
 */
static void
DumpTrace (oski_trace_t trace, const oski_matcommon_t * props)
{
#if !defined(NDEBUG)
  size_t i;

  if (trace != NULL)
    {
      oski_PrintDebugMessage (3, "Dumping matrix trace [%d records]...",
			      (int) simplelist_GetLength (trace));

      for (i = 1; i <= simplelist_GetLength (trace); i++)
	{
	  const oski_tracerec_t *rec = (const oski_tracerec_t *)
	    simplelist_GetElem (trace, i);
	  double x_flop = 1.0;	/* flop factor */

	  if (rec == NULL)
	    continue;

	  switch (rec->kernel_id)
	    {
	    case OSKI_KERNEL_MatMult:
	      x_flop = GET_XFLOP_MatMult (rec->args);
	      break;

	    case OSKI_KERNEL_MatTrisolve:
	      x_flop = GET_XFLOP_MatTrisolve (rec->args);
	      break;

	    case OSKI_KERNEL_MatMultAndMatTransMult:
	      x_flop = GET_XFLOP_MatMultAndMatTransMult (rec->args);
	      break;

	    case OSKI_KERNEL_MatTransMatMult:
	      x_flop = GET_XFLOP_MatTransMatMult (rec->args);
	      break;

	    case OSKI_KERNEL_MatPowMult:
	      x_flop = GET_XFLOP_MatPowMult (rec->args);
	      break;
	    }

	  if (rec->time_elapsed > 0)
	    {
	      oski_PrintDebugMessage (3,
				      "[%d;%d] %d calls to kernel=%d args=%p took %g seconds",
				      (int) i, (int) rec->id,
				      (int) rec->num_calls,
				      (int) rec->kernel_id,
				      (const void *) rec->args,
				      rec->time_elapsed);

	      if (props != NULL)
		{
		  /* Mflop/s rate */
		  double Mflops = rec->num_calls * x_flop
		    * props->num_nonzeros * 1e-6 / rec->time_elapsed;

		  oski_PrintDebugMessage (4,
					  "(Approximately %.1f Mflop/s)",
					  Mflops);
#if IS_VAL_COMPLEX
		  oski_PrintDebugMessage (4,
					  "(Approximately %.1f scalar Mflop/s)",
					  Mflops * 4);
#endif
		}
	    }
	  else
	    oski_PrintDebugMessage (3,
				    "[%d;%d] %d calls to kernel=%d args=%p"
				    " (execution time unknown)",
				    (int) i, (int) rec->id,
				    (int) rec->num_calls,
				    (int) rec->kernel_id,
				    (const void *) rec->args);
	}
    }
#endif
}

/**
 *  \brief
 *
 *  \param[in,out] A Matrix object/handle.
 *
 *  \see oski_CreateMatStruct()
 */
void
oski_DestroyMatStruct (oski_matstruct_t * A)
{
  oski_DestroyMatRepr_funcpt func_DestroyMatRepr;

  if (A == INVALID_MAT)
    return;

  /* Free input matrix representation */
  func_DestroyMatRepr =
    OSKI_MATTYPEID_METHOD (A->input_mat.type_id, DestroyMatRepr);
  if (func_DestroyMatRepr)
    (*func_DestroyMatRepr) (A->input_mat.repr);

  /* Free tuned matrix representation */
  func_DestroyMatRepr =
    OSKI_MATTYPEID_METHOD (A->tuned_mat.type_id, DestroyMatRepr);
  if (func_DestroyMatRepr)
    (*func_DestroyMatRepr) (A->tuned_mat.repr);

  if (A->tuned_xforms != NULL)
    oski_Free (A->tuned_xforms);

  oski_PrintDebugMessage (2, "Dumping matrix trace, if any.");
  DumpTrace (A->trace, &A->props);

  oski_PrintDebugMessage (2, "Dumping workload hints, if any.");
  DumpTrace (A->workhints, NULL);

  oski_ResetStructHint (&(A->structhints));
  oski_DestroyTrace (A->trace);
  oski_DestroyTrace (A->workhints);

  oski_DestroyTimer (A->timer);
  oski_Free (A);
}

/* eof */
