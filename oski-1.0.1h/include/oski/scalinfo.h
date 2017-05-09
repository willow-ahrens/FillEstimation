/**
 *  \file oski/scalinfo.h
 *  \ingroup MATTYPES
 *  \brief Define the scalar types available to the library.
 *
 *  This module (see also: scalinfo.c) defines all possible
 *  scalar types that may be used to create a sparse matrix.
 *  Among other reasons, the library needs this information
 *  to create fully-qualified dynamic library names for the
 *  matrix type-specific kernel implementations.
 *
 *  Users who wish to make alternative scalar types available
 *  for use as indices or non-zero values should add their
 *  definitions here. In addition, we advise these users to
 *  modify the corresponding test program (see "tests"
 *  subdirectory, 'info' test) to check for any such new
 *  scalar types.
 */

#if !defined(INC_OSKI_SCALINFO_H)
/** oski/scalinfo.h has been included. */
#define INC_OSKI_SCALINFO_H

/**
 *  \name Complex-valued data types.
 */
/*@{*/

/**
 *  \brief Single precision complex data type.
 */
typedef struct tagComplex_t
{
  float real;	     /**< real part */
  float imag;	     /**< imaginary part */
} complex_t;

/**
 *  \brief Double precision complex data type.
 */
typedef struct tagDoublecomplex_t
{
  double real;	     /**< real part */
  double imag;	     /**< imaginary part */
} doublecomplex_t;

/** \brief Synonym for single precision complex type. */
#define _COMPLEX complex_t
/** \brief Synonym for double precision complex type. */
#define _DOUBLE_COMPLEX double_complex_t

/** \brief Macro to extract the real part of a complex number. */
#define _RE(x)  (x).real
/** \brief Macro to extract the imaginary part of a complex number. */
#define _IM(x)  (x).imag

/*@}*/

/**
 *  \brief Information about an available scalar type.
 */
typedef struct tagBebop_scalinfo_t
{
  oski_id_t id;		   /**< Unique positive integer identifier */
  const char *name;	    /**< Unique short string identifier. */
  char tag;		    /**< Unique one-letter abbreviation tag. */
  size_t num_bytes;	    /**< Size of this type, in bytes. */
}
oski_scalinfo_t;

/**
 *  \brief Macro used to define a new scalar index type.
 *
 *  \param macro_tag Suffix for constant macro defining the type's
 *  unique id number.
 *  \param conc_typ  Corresponding concrete C type.
 *  \param name      String name used to identify this type.
 *  \param tag       Unique one character identifier.
 *  \returns Defines an initialization record for #oski_scalinfo_t type.
 */
#define OSKI_SCALIND(macro_tag, conc_typ, name, tag) \
	{ OSKI_SCALIND_##macro_tag, name, tag, sizeof(conc_typ) }

/** \brief NULL (end) record. */
#define OSKI_SCALIND_END      INVALID_ID
#define OSKI_SCALIND_END_REC  OSKI_SCALIND(END, char, "", (char)NULL)
	/**< END descriptor */

/**
 *  \brief Macro used to define a new scalar non-zero value type.
 *
 *  \param macro_tag Suffix for constant macro defining the type's
 *  unique id number.
 *  \param conc_typ  Corresponding concrete C type.
 *  \param name      String name used to identify this type.
 *  \param tag       Unique one character identifier.
 *  \returns Defines an initialization record for #oski_scalinfo_t type.
 */
#define OSKI_SCALVAL(macro_tag, conc_typ, name, tag) \
	{ OSKI_SCALVAL_##macro_tag, name, tag, sizeof(conc_typ) }

/** \brief NULL (end) record. */
#define OSKI_SCALVAL_END      INVALID_ID
#define OSKI_SCALVAL_END_REC  OSKI_SCALVAL(END, char, "", (char)NULL)
	/**< END descriptor */

/**
 *  \name Available integer index types.
 *
 *  To make a new integer index type available to the system,
 *  define a record here. By default, we predefine types for
 *  integers and long-integers.
 */
/*@{*/

/** \brief INT -- Equivalent C type: int. */
#define OSKI_SCALIND_INT       1
#define OSKI_SCALIND_INT_REC   OSKI_SCALIND(INT, int, "integer", 'i')
	/**< INT descriptor. */

/** \brief LONG -- Equivalent C type: long. */
#define OSKI_SCALIND_LONG      2
#define OSKI_SCALIND_LONG_REC  OSKI_SCALIND(LONG, long, "long", 'l')
	/**< LONG descriptor. */

/*@}*/

/**
 *  \name Available non-zero value types.
 *
 *  To make a new integer index type available to the system,
 *  define a record here. By default, we predefine types for
 *  {single, double}-precision x {real, complex} types.
 *
 *  \note Developers who add additional complex types should
 *  modify the corresponding macro, #IS_VAL_COMPLEX
 *  (mangle.h).
 */
/*@{*/

/** \brief SINGLE -- Equivalent C type: float. */
#define OSKI_SCALVAL_SINGLE      1
#define OSKI_SCALVAL_SINGLE_REC  OSKI_SCALVAL(SINGLE, float, "single", 's')
		/**< SINGLE descriptor. */

/** \brief DOUBLE -- Equivalent C type: double. */
#define OSKI_SCALVAL_DOUBLE      2
#define OSKI_SCALVAL_DOUBLE_REC  OSKI_SCALVAL(DOUBLE, double, "double", 'd')
		/**< DOUBLE descriptor. */

/** \brief COMPLEX -- Equivalent C++ type: complex<float> */
#define OSKI_SCALVAL_COMPLEX      3
#define OSKI_SCALVAL_COMPLEX_REC  \
	OSKI_SCALVAL(COMPLEX, complex_t, "complex", 'c')
	/**< COMPLEX descriptor. */

/** \brief DOUBLECOMPLEX -- Equivalent C++ type: complex<double> */
#define OSKI_SCALVAL_DOUBLECOMPLEX      4
#define OSKI_SCALVAL_DOUBLECOMPLEX_REC  \
	OSKI_SCALVAL(DOUBLECOMPLEX, doublecomplex_t, "double_complex", 'z')
	/**< DOUBLECOMPLEX descriptor. */

/*@}*/

/**
 *  \brief Returns 1 iff the given scalar type is a complex type.
 */

/**
 *  \brief Returns the number of registered scalar index types.
 */
size_t oski_GetNumScalarIndexTypes (void);

/**
 *  \brief Lookup information about a scalar integer index type.
 */
const oski_scalinfo_t *oski_LookupScalarIndexInfo (oski_id_t id);

/**
 *  \brief Lookup the string name of a given scalar integer index type.
 */
const char *oski_GetScalarIndexName (oski_id_t id);

/**
 *  \brief Lookup the abbreviated name of a given scalar integer index type.
 */
char oski_GetScalarIndexTag (oski_id_t id);

/**
 *  \brief Returns the number of registered non-zero value types.
 */
size_t oski_GetNumScalarValueTypes (void);

/**
 *  \brief Lookup information about a scalar non-zero value type.
 */
const oski_scalinfo_t *oski_LookupScalarValueInfo (oski_id_t id);

/**
 *  \brief Lookup the string name of a given scalar non-zero value type.
 */
const char *oski_GetScalarValueName (oski_id_t id);

/**
 *  \brief Lookup the abbreviated name of a given scalar non-zero value type.
 */
char oski_GetScalarValueTag (oski_id_t id);

#endif /* !defined(INC_OSKI_SCALINFO_H) */

/* eof */
