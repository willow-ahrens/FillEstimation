/**
 *  \file tests/info.c
 *  \brief Test scalar/kernel info lookup routines.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <oski/common.h>

static void
assert_info (const void *info_ptr, int id, const char *rec_desc)
{
  if (info_ptr == NULL)		/* error on info call */
    {
      fprintf (stderr,
	       "*** ERROR: Could not find pre-defined type '%d' (%s). ***\n",
	       id, rec_desc);
      exit (1);
    }
  /* else, do nothing */
}

static void
display_scalar_info (const oski_scalinfo_t * info)
{
  assert (info != NULL);

  printf ("\t[S%d] %s (tag='%c', %d bytes)\n",
	  (int) info->id, info->name, info->tag, (int) info->num_bytes);
}

static void
check_scalar_indices (void)
{
  const oski_scalinfo_t *info = NULL;

  oski_PrintDebugMessage (1,
			  "... Checking for built-in integer index types ...");

  info = oski_LookupScalarIndexInfo (OSKI_SCALIND_INT);
  assert_info (info, OSKI_SCALIND_INT, "integer index type");
  display_scalar_info (info);

  info = oski_LookupScalarIndexInfo (OSKI_SCALIND_LONG);
  assert_info (info, OSKI_SCALIND_LONG, "long integer index type");
  display_scalar_info (info);

  /* Check for a non-existent record */
  info = oski_LookupScalarIndexInfo (-100);
  if (info != NULL)
    {
      fprintf (stderr,
	       "*** ERROR: Search for a dummy record did NOT return NULL! ***\n");
      exit (1);
    }
  else
    printf ("\t(Dummy record search correctly returned NULL.)\n");
}

static void
check_scalar_nzvals (void)
{
  const oski_scalinfo_t *info = NULL;

  oski_PrintDebugMessage (1,
			  "... Checking for built-in non-zero value types ...");

  info = oski_LookupScalarValueInfo (OSKI_SCALVAL_SINGLE);
  assert_info (info, OSKI_SCALVAL_SINGLE, "single precision real");
  display_scalar_info (info);

  info = oski_LookupScalarValueInfo (OSKI_SCALVAL_DOUBLE);
  assert_info (info, OSKI_SCALVAL_DOUBLE, "double precision real");
  display_scalar_info (info);

  info = oski_LookupScalarValueInfo (OSKI_SCALVAL_COMPLEX);
  assert_info (info, OSKI_SCALVAL_COMPLEX, "single precision complex");
  display_scalar_info (info);

  info = oski_LookupScalarValueInfo (OSKI_SCALVAL_DOUBLECOMPLEX);
  assert_info (info, OSKI_SCALVAL_DOUBLECOMPLEX, "double precision complex");
  display_scalar_info (info);

  /* Check for a non-existent record */
  info = oski_LookupScalarValueInfo (-100);
  if (info != NULL)
    {
      fprintf (stderr,
	       "*** ERROR: Search for a dummy record did NOT return NULL! ***\n");
      exit (1);
    }
  else
    printf ("\t(Dummy record search correctly returned NULL.)\n");
}

static void
display_kernel_info (const oski_kerinfo_t * info)
{
  assert (info != NULL);
  printf ("\t[K%d] %s\n", (int) info->id, info->name);
}

static void
check_kernels (void)
{
  const oski_kerinfo_t *info = NULL;

  oski_PrintDebugMessage (1, "... Checking for built-in kernels ...");

  info = oski_LookupKernelInfo (OSKI_KERNEL_MatMult);
  assert_info (info, OSKI_KERNEL_MatMult, "sparse matrix-vector multiply");
  display_kernel_info (info);

  info = oski_LookupKernelInfo (OSKI_KERNEL_MatTrisolve);
  assert_info (info, OSKI_KERNEL_MatTrisolve, "sparse triangular solve");
  display_kernel_info (info);

  info = oski_LookupKernelInfo (OSKI_KERNEL_MatMultAndMatTransMult);
  assert_info (info, OSKI_KERNEL_MatMultAndMatTransMult,
	       "sparse matrix and matrix-transpose multiply");
  display_kernel_info (info);

  info = oski_LookupKernelInfo (OSKI_KERNEL_MatTransMatMult);
  assert_info (info, OSKI_KERNEL_MatTransMatMult, "sparse A^T*A*x");
  display_kernel_info (info);

  info = oski_LookupKernelInfo (OSKI_KERNEL_MatPowMult);
  assert_info (info, OSKI_KERNEL_MatPowMult, "sparse A^k*x");
  display_kernel_info (info);

  /* Check for a non-existent record */
  info = oski_LookupKernelInfo (-100);
  if (info != NULL)
    {
      fprintf (stderr,
	       "*** ERROR: Search for a dummy record did not return NULL! ***\n");
      exit (1);
    }
  else
    printf ("\t(Dummy record search correctly returned NULL.)\n");
}

int
main (int argc, char *argv[])
{
  oski_Init ();

  check_scalar_indices ();
  check_scalar_nzvals ();
  check_kernels ();

  oski_Close ();
  return 0;
}

/* eof */
