/**
 *  \file src/kerinfo.c
 *  \ingroup KERNELS
 *  \brief Define the kernels available to the library.
 *
 *  This module defines a global list, implemented as an
 *  array private to this module, containing information
 *  about available kernels. Each list is composed of
 *  entries of type #oski_kerinfo_t, and is terminated
 *  by a dummy entry whose type id is OSKI_KERNEL_END.
 *
 *  These lists are read-only and statically defined, so
 *  there should not be any issues with thread-safety.
 *
 *  Users who wish to make additional kernel types available
 *  should add initialization code to g_avail_kernels, below.
 */

#include <oski/common.h>
#include <oski/kerinfo.h>

/**
 *  \brief List of available kernels.
 */
static oski_kerinfo_t g_avail_kernels[] = {
  OSKI_KERNEL_MatMult_REC,
  OSKI_KERNEL_MatTrisolve_REC,
  OSKI_KERNEL_MatMultAndMatTransMult_REC,
  OSKI_KERNEL_MatTransMatMult_REC,
  OSKI_KERNEL_MatPowMult_REC,
  OSKI_KERNEL_END_REC
};

/**
 *  \brief
 *
 *  Scans the global list g_avail_kernels for the specified
 *  kernel id.
 */
const oski_kerinfo_t *
oski_LookupKernelInfo (oski_id_t id)
{
  const oski_kerinfo_t *retval = NULL;
  int i = 0;

  while (retval == NULL)
    {
      if (g_avail_kernels[i].id == OSKI_KERNEL_END)	/* end of list */
	break;

      if (g_avail_kernels[i].id == id)
	retval = &(g_avail_kernels[i]);

      i++;
    }

  return retval;
}

/* eof */
