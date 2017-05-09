/**
 *  \file bench/keropts.h
 *  \brief Process kernel-specific command-line options.
 *  \ingroup OFFBENCH
 */

#if !defined(INC_BENCH_KEROPTS_H)
#define INC_BENCH_KEROPTS_H

#include <stdio.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
#define kerargs_MatMult_t MANGLE_(kerags_MatMult_t)
#define kerargs_MatTrisolve_t MANGLE_(kerags_MatTrisolve_t)
#define kerargs_MatTransMatMult_t MANGLE_(kerags_MatTransMatMult_t)
#define kerargs_MatMultAndMatTransMult_t \
	MANGLE_(kerags_MatMultAndMatTransMult_t)
#define kerargs_MatPowMult_t MANGLE_(kerags_MatPowMult_t)
#define keropts_process MANGLE_(keropts_process)
#endif

/** List of kernel id numbers */
typedef enum
{
  KERNEL_NULL = 0,
  KERNEL_MatMult = 1,
  KERNEL_MatTrisolve = 2,
  KERNEL_MatTransMatMult = 3,
  KERNEL_MatMultAndMatTransMult = 4,
  KERNEL_MatPowMult = 5
} kernel_types_t;

/**
 *  \brief Arguments for the kernel,
 *  \f$y \leftarrow \beta\cdot y + \alpha\cdot \mathrm{op}(A)\cdot x\f$
 */
typedef struct
{
  int num_calls;	/**< Number of times to execute this kernel */
  oski_index_t num_vecs;       /**< No. of vectors, \f$k\f$ */
  oski_matop_t op;	 /**< \f$\mathrm{op}(A)\f$ */
  oski_value_t alpha;	    /**< \f$\alpha\f$ */
  oski_vecview_t x;	  /**< Operand \f$x\f$ */
  oski_storage_t x_layout;	 /**< Storage layout for \f$x\f$ */
  oski_value_t beta;	   /**< \f$\beta\f$ */
  oski_vecview_t y;	  /**< Operand \f$y\f$ */
  oski_storage_t y_layout;	 /**< Storage layout for \f$y\f$ */
} kerargs_MatMult_t;

/**
 *  Arguments for the kernel,
 *  \f$x \leftarrow \alpha\mathrm{op}(T^{-1})\cdot x\f$
 */
typedef struct
{
  int num_calls;	/**< Number of times to execute this kernel */
  oski_index_t num_vecs;       /**< No. of vectors, \f$k\f$ */
  oski_matop_t op;	 /**< \f$\mathrm{op}(A)\f$ */
  oski_value_t alpha;	    /**< \f$\alpha\f$ */
  oski_vecview_t x;	  /**< Operand \f$x\f$ */
  oski_storage_t x_layout;	 /**< Storage layout for \f$x\f$ */
} kerargs_MatTrisolve_t;

/**
 *  Arguments for the kernel,
 *  \f$y \leftarrow \beta\cdot y + \alpha\cdot \mathrm{op}(A)\cdot x\f$
 */
typedef struct
{
  int num_calls;	/**< Number of times to execute this kernel */
  oski_index_t num_vecs;       /**< No. of vectors, \f$k\f$ */
  oski_ataop_t op;	 /**< \f$\mathrm{op}(A)\f$ */
  oski_value_t alpha;	    /**< \f$\alpha\f$ */
  oski_vecview_t x;	  /**< Operand \f$x\f$ */
  oski_storage_t x_layout;	 /**< Storage layout for \f$x\f$ */
  oski_value_t beta;	   /**< \f$\beta\f$ */
  oski_vecview_t y;	  /**< Operand \f$y\f$ */
  oski_storage_t y_layout;	 /**< Storage layout for \f$y\f$ */
  oski_vecview_t t;	  /**< Operand \f$t\f$ */
  oski_storage_t t_layout;	 /**< Storage layout for \f$t\f$ */
} kerargs_MatTransMatMult_t;

/**
 *  \brief Arguments for the kernel,
 *  \f$y \leftarrow \beta\cdot y + \alpha\cdot A\cdot x\f$,
 *  \f$z \leftarrow \zeta\cdot z + \omega\cdot \mathrm{op}(A)\cdot w\f$
 */
typedef struct
{
  int num_calls;	/**< Number of times to execute this kernel */
  oski_index_t num_vecs;       /**< Number of vectors */
  oski_matop_t op;	 /**< \f$\mathrm{op}(A)\f$ */
  oski_value_t alpha;	    /**< \f$\alpha\f$ */
  oski_vecview_t x;	  /**< Operand \f$x\f$ */
  oski_storage_t x_layout;	 /**< Storage layout for \f$x\f$ */
  oski_value_t beta;	   /**< \f$\beta\f$ */
  oski_vecview_t y;	  /**< Operand \f$y\f$ */
  oski_storage_t y_layout;	 /**< Storage layout for \f$y\f$ */
  oski_value_t omega;	    /**< \f$\omega\f$ */
  oski_vecview_t w;	  /**< Operand \f$w\f$ */
  oski_storage_t w_layout;	 /**< Storage layout for \f$w\f$ */
  oski_value_t zeta;	   /**< \f$\zeta\f$ */
  oski_vecview_t z;	  /**< Operand \f$z\f$ */
  oski_storage_t z_layout;	 /**< Storage layout for \f$z\f$ */
} kerargs_MatMultAndMatTransMult_t;

/**
 *  \brief \f$y \leftarrow \beta y + \alpha \mathrm{op}(A)^p\cdot x\f$
 */
typedef struct
{
  int num_calls;	/**< Number of times to execute this kernel */
  oski_index_t num_vecs;       /**< Number of vectors */
  oski_matop_t op;	 /**< \f$\mathrm{op}(A)\f$ */
  int power;	   /**< \f$p\f$ */
  oski_value_t alpha;	    /**< \f$\alpha\f$ */
  oski_vecview_t x;	  /**< Operand \f$x\f$ */
  oski_storage_t x_layout;	 /**< Storage layout for \f$x\f$ */
  oski_value_t beta;	   /**< \f$\beta\f$ */
  oski_vecview_t y;	  /**< Operand \f$y\f$ */
  oski_storage_t y_layout;	 /**< Storage layout for \f$y\f$ */
  oski_vecview_t t;	  /**< Operand \f$t\f$ */
  oski_storage_t t_layout;	 /**< Storage layout for \f$t\f$ */
} kerargs_MatPowMult_t;

/** Stores workload component arguments */
typedef struct
{
  kernel_types_t kernel;	 /**< Kernel to execute */

	/** Kernel-specific arguments */
  union
  {
    kerargs_MatMult_t MatMult;		   /**< Matrix-vector multiply */
    kerargs_MatTrisolve_t MatTrisolve;		   /**< Triangular solve */
    kerargs_MatTransMatMult_t MatTransMatMult;		   /**< \f$A^TA\cdot x\f$ */
    kerargs_MatMultAndMatTransMult_t MatMultAndMatTransMult;
			/**< \f$(A\cdot x, \mathrm{op}(A)\cdot y)\f$ */
    kerargs_MatPowMult_t MatPowMult;		 /**< \f$A^k\cdot x\f$ */
  } args;
} kernel_t;

/** Print usage statement for arguments relative to kernel options */
void keropts_Usage (FILE * fp);

/** Process a kernel option. */
int keropts_Process (int argc, char *argv[], int i_start, kernel_t ** p_opt);

#endif

/* eof */
