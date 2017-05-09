/**
 *  \file oski/blas_names.h
 *  \ingroup AUXIL
 *  \brief Header containing mangled F77 BLAS routine names.
 *
 *  This file depends on config.h, and so is intended
 *  for use only at library build time.
 */

#if !defined(INC_OSKI_BLAS_NAMES_H)
/** blas_names.h included. */
#define INC_OSKI_BLAS_NAMES_H

#include <oski/config.h>

#if !defined(F77_FUNC)
/** Default definition for F77_FUNC if none exists */
#define F77_FUNC(x, y)   x
#endif

/**
 *  \name Macros for creating BLAS routine names.
 */
/*@{*/
/**
 *  \brief Creates an Fortran-77 name by concatenating a type tag
 *  and a base name.
 *
 *  This macro directly concatenates. To allow for macro expansion,
 *  see #MAKE_BLAS_NAME1().
 *
 *  \param[in] tag Lowercase, single-letter type tag.
 *  \param[in] name Lowercase base name.
 *  \param[in] TAG Uppercase, single-letter type tag.
 *  \param[in] NAME Uppercase base name.
 *  \returns A Fortran 77 name following the naming conventions
 *  determined at configure-time (e.g., adding a trailing
 *  underscore).
 *
 *  \see #MAKE_BLAS_NAME1()
 *  \see #F77_FUNC()
 */
#define MAKE_BLAS_NAME0(tag, name, TAG, NAME)  \
	F77_FUNC(tag ## name, TAG ## NAME)

/**
 *  \brief Generate a non-standard BLAS name which includes an
 *  additional prefix to indicate the integer precision.
 */
#define MAKE_BLAS_NAME0_I(itag, tag, name, ITAG, TAG, NAME) \
        F77_FUNC(itag ## tag ## name, ITAG ## TAG ## NAME)

/**
 *  \brief Creates a Fortran 77 name by concatenating a type tag
 *  and a base name.
 *
 *  This macro immediately calls #MAKE_BLAS_NAME0(), thereby
 *  allowing the tags and base names to be macros that are
 *  expanded just before concatenation.
 *
 *  \see #MAKE_BLAS_NAME0()
 */
#define MAKE_BLAS_NAME1(tag, name, TAG, NAME)  \
	MAKE_BLAS_NAME0(tag, name, TAG, NAME)

/**
 *  \brief Generates a non-standard BLAS name which includes an
 *  additional prefix to indicate the integer precision.
 */
#define MAKE_BLAS_NAME1_I(itag, tag, name, ITAG, TAG, NAME) \
        MAKE_BLAS_NAME0_I(itag, tag, name, ITAG, TAG, NAME)

/**
 *  \brief Make the standard name of a BLAS routine.
 *
 *  Given the base BLAS routine name, in both lowercase and
 *  uppercase versions (e.g., gemm and GEMM, trsv and TRSV),
 *  create a Fortran 77 compatible name, given the C-to-Fortran
 *  calling conventions determined at configure time.
 */
#define MAKE_BLAS_NAME(name, NAME) \
	  MAKE_BLAS_NAME1(VAL_TAG, name, VAL_TAG_CAPS, NAME)

/**
 *  \brief Make the non-standard name of a BLAS routine which uses a
 *  non-standard integer precision.
 */
#define MAKE_BLAS_NAME_I(name, NAME) \
        MAKE_BLAS_NAME1_I(IND_TAG, VAL_TAG, name, \
                          IND_TAG_CAPS, VAL_TAG_CAPS, NAME)
/*@}*/

/**
 *  \name BLAS routine names, hidden behind macros.
 */
/*@{*/
#if IND_TAG_CHAR == 'i'   /* Use standard names */
#  define BLAS_xSCAL  MAKE_BLAS_NAME(scal, SCAL)
#  define BLAS_xAXPY  MAKE_BLAS_NAME(axpy, AXPY)
#  define BLAS_xGEMV  MAKE_BLAS_NAME(gemv, GEMV)
#  define BLAS_xTRSV  MAKE_BLAS_NAME(trsv, TRSV)
#else /* non-standard integer type */
#  define BLAS_xSCAL  MAKE_BLAS_NAME_I(scal, SCAL)
#  define BLAS_xAXPY  MAKE_BLAS_NAME_I(axpy, AXPY)
#  define BLAS_xGEMV  MAKE_BLAS_NAME_I(gemv, GEMV)
#  define BLAS_xTRSV  MAKE_BLAS_NAME_I(trsv, TRSV)
#endif
/*@}*/

/**
 *  \brief BLAS vector scale routine, ?scal, which computes \f$x
 *  \leftarrow \alpha\cdot x\f$.
 *
 *  \param[in] len Length of the vector.
 *  \param[in] alpha Scalar multiplier, \f$\alpha\f$.
 *  \param[in,out] x Vector, \f$x\f$.
 *  \param[in] stride Distance between consecutive elements.
 */
void BLAS_xSCAL (const oski_index_t * len, oski_value_t * alpha,
		 oski_value_t * x, const oski_index_t * stride);

/**
 *  \brief BLAS vector AXPY operation, ?axpy, which computes \f$y
 *  \leftarrow y + \alpha\cdot x\f$.
 *
 *  \param[in] len Length of x and y.
 *  \param[in] alpha Scalar multiplier, \f$\alpha\f$.
 *  \param[in] x Vector \f$x\f$.
 *  \param[in] incx Distance between consecutive elements of \f$x\f$.
 *  \param[in,out] y Vector \f$y\f$.
 *  \param[in] incy Distance between consecutive elements of \f$x\f$.
 */
void BLAS_xAXPY (const oski_index_t * len, const oski_value_t * alpha,
		 const oski_value_t * x, const oski_index_t * incx,
		 oski_value_t * y, const oski_index_t * incy);

/**
 *  \brief BLAS matrix-vector multiply routine, ?gemv, which computes
 *  the dense matrix-vector multiply operation, \f$y \leftarrow
 *  \beta\cdot y + \alpha\cdot\mathrm{op}(A)\cdot x\f$.
 *
 *  \param[in] op Transpose operation ('n', 't'),
 *  \param[in] m Number of rows in \f$A\f$.
 *  \param[in] n Number of columns in \f$A\f$.
 *  \param[in] A Array storing \f$A\f$, in column major format.
 *  \param[in] lda Leading dimension of A.
 *  \param[in] alpha Scalar \f$\alpha\f$.
 *  \param[in] x Vector \f$x\f$.
 *  \param[in] incx Distance between \f$x_i, x_{i+1}\f$ in x.
 *  \param[in] beta Scalar \f$\beta\f$.
 *  \param[in,out] y Vector \f$y\f$.
 *  \param[in] incy Distance between \f$y_i, y_{i+1}\f$ in y.
 */
void BLAS_xGEMV (const char *op,
		 const oski_index_t * m, const oski_index_t * n,
		 const oski_value_t * alpha,
		 const oski_value_t * A, const oski_index_t * lda,
		 const oski_value_t * x, const oski_index_t * incx,
		 const oski_value_t * beta, oski_value_t * y,
		 const oski_index_t * incy);

/**
 *  \brief BLAS triangular solve routine, ?trsv, which computes dense
 *  triangular solve operation, \f$x \leftarrow
 *  \mathrm{op}(A^{-1})\cdot x\f$.
 *
 *  \param[in] shape Lower or upper triangular ('l' or 'u').
 *  \param[in] op Transpose operation ('n', 't', 'c').
 *  \param[in] diag Has unit diag or not ('u', 'n').
 *  \param[in] n Dimension of \f$A\f$.
 *  \param[in] A Values of the dense matrix \f$A\f$, in column
 *  major layout.
 *  \param[in] lda Leading dimension of A.
 *  \param[in,out] x Initial right-hand side, which is overwritten
 *  by the solution on output.
 *  \param[in] incx Distance between elements of x.
 */
void BLAS_xTRSV (const char* shape, const char* op, const char* diag,
		 const oski_index_t* n,
		 const oski_value_t* A, const oski_index_t* lda,
		 oski_value_t* x, const oski_index_t* incx);

#endif

/* eof */
