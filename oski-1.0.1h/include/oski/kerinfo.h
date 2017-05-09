/**
 *  \file oski/kerinfo.h
 *  \ingroup KERNELS
 *  \brief Define the kernels available to the library.
 *
 *  This module (see also: kerinfo.c) defines all available
 *  sparse matrix kernels that users may call. Among other
 *  reasons, the library this information to create
 *  fully-qualified dynamic library names for the matrix
 *  type-specific kernel implementations.
 *
 *  Information about new kernels should be defined here.
 *  Optionally, the corresponding test program should be
 *  modified to check for the new kernels (see "tests"
 *  subdirectory for the 'info' test).
 *
 *  $Revision: 1.1.1.1 $
 *
 *  Revision history:
 *    - $Log: kerinfo.h,v $
 *    - Revision 1.1.1.1  2005/06/29 22:07:48  rvuduc
 *    - Initial import
 *    -
 */

#if !defined(INC_OSKI_KERINFO_H)
/** oski/kerinfo.h has been included. */
#define INC_OSKI_KERINFO_H

/**
 *  \brief Kernel descriptor record.
 */
typedef struct tagBebop_kerinfo_t
{
  oski_id_t id;		 /**< Unique positive integer identifier. */
  const char *name;	  /**< Unique short string identifier. */
} oski_kerinfo_t;

/**
 *  \brief Macro used to define a new kernel.
 *
 *  \param macro_tag  Suffix for constant macro defining the kernel's
 *  unique id number.
 *  \param name  String name used to identify this kernel.
 *  \returns Defines an initialization record for #oski_kerinfo_t.
 */
#define OSKI_KERNEL(macro_tag, name) {OSKI_KERNEL_##macro_tag, name}

/** \brief NULL (end) record. */
#define OSKI_KERNEL_END     INVALID_ID
#define OSKI_KERNEL_END_REC OSKI_KERNEL(END, NULL)
	/**< END descriptor */

/**
 *  \name Available kernels.
 *
 *  To make a new kernel available to the system, define a record
 *  here. By default, we predefine the kernels listed below.
 */
/*@{*/

/** \brief MATMULT -- Sparse matrix-vector multiply. */
#define OSKI_KERNEL_MatMult     1
#define OSKI_KERNEL_MatMult_REC OSKI_KERNEL(MatMult, "MatMult")
	/**< MATMULT descriptor */

/** \brief MatTrisolve -- Sparse triangular solve. */
#define OSKI_KERNEL_MatTrisolve     2
#define OSKI_KERNEL_MatTrisolve_REC OSKI_KERNEL(MatTrisolve, "MatTrisolve")
	/**< MatTrisolve descriptor */

/**
 *  \brief MatMultAndMatTransMult --
 *  Simultaneous multiplication by a sparse matrix and its transpose.
 */
#define OSKI_KERNEL_MatMultAndMatTransMult     3
#define OSKI_KERNEL_MatMultAndMatTransMult_REC \
	OSKI_KERNEL(MatMultAndMatTransMult, "MatMultAndMatTransMult")
	/**< MatMultAndMatTransMult descriptor */

/**
 *  \brief MatTransMatMult -- Multiplication by sparse \f$A^TA\f$.
 */
#define OSKI_KERNEL_MatTransMatMult      4
#define OSKI_KERNEL_MatTransMatMult_REC  \
	OSKI_KERNEL(MatTransMatMult, "MatTransMatMult")
	/**< MATTRANSMATMULT descriptor */

/**
 *  \brief MatPowMult -- Multiplication by a matrix power, \f$A^k\f$.
 */
#define OSKI_KERNEL_MatPowMult     5
#define OSKI_KERNEL_MatPowMult_REC OSKI_KERNEL(MatPowMult, "MatPowMult")
	/**< MatPowMult descriptor */

/*@}*/

/**
 *  \brief Lookup information about a kernel.
 */
const oski_kerinfo_t *oski_LookupKernelInfo (oski_id_t id);

#endif

/* eof */
