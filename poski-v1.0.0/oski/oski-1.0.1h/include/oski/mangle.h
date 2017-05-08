/**
 *  \file oski/mangle.h
 *  \ingroup AUXIL
 *  \brief Some macros to mangle index/value type-specific names.
 *
 *  To allow users to build a library in multiple precisions,
 *  we use the preprocessor macros defined in this file to mangle
 *  type and routine names. This header essentially helps simulate
 *  a hack-ish C++ template facility.
 *
 *  In order for name mangling to work, the following compile-time
 *  macros need to be defined:
 *    - IND_T: Equivalent C typename for indices (e.g., int, long)
 *    - IND_TAG: 1 character type that identifies the index type
 *    (e.g., i, l)
 *    - IND_TAG_CAPS: Capitalized version of IND_TAG (e.g., I, L).
 *    - IND_TYPE_ID: Type ID name for indices (e.g., INT, LONG)
 *    - IND_TAG_CHAR: Same as IND_TAG, but wrapped in single quotes
 *    (e.g., 'i', 'l').
 *    - VAL_T: Equivalent C typename for values (e.g., float, double,
 *    complex_t, doublecomplex_t)
 *    - VAL_TAG: 1 character type that identifies the value type
 *    (e.g., s, d, c, z).
 *    - VAL_TAG_CAPS: Capitalized version of VAL_TAG (e.g., S, D,
 *    C, Z).
 *    - VAL_TYPE_ID: Type ID name for scalars (e.g., SINGLE, COMPLEX)
 *    - VAL_TAG_CHAR: Same as VAL_TAG, but wrapped in single quotes
 *    (e.g., 's', 'd', 'c', 'z').
 *
 *  At a minimum, the user must define IND_TAG_CHAR and VAL_TAG_CHAR,
 *  from which this header will select default expansions for the
 *  other macros.
 */

#if !defined(INC_OSKI_MANGLE_H)
/** oski/mangle.h has been included. */
#define INC_OSKI_MANGLE_H

/*
 *  Check that type macros have been defined so we can "instantiate"
 *  template types.
 */
#if !defined(IND_TAG_CHAR)
#error "ERROR: Must instantiate scalar integer index type."
#endif

#if !defined(VAL_TAG_CHAR)
#error "ERROR: Must instantiate scalar non-zero value types."
#endif

/* -----------------------------------------------------------------
 *  Select default macro scalar-type expansions.
 */
#if IND_TAG_CHAR == 'i'
	/** \name Index type macros for 'i' ('int') */
	/*@{ */
#  undef IND_T
#  define IND_T int
#  undef IND_TAG
#  define IND_TAG i
#  undef IND_TAG_CAPS
#  define IND_TAG_CAPS I
#  undef IND_TYPE_ID
#  define IND_TYPE_ID INT
	/*@} */
#elif IND_TAG_CHAR == 'l'
	/** \name Index type macros for 'l' ('long') */
	/*@{ */
#  undef IND_T
#  define IND_T long
#  undef IND_TAG
#  define IND_TAG l
#  undef IND_TAG_CAPS
#  define IND_TAG_CAPS L
#  undef IND_TYPE_ID
#  define IND_TYPE_ID LONG
	/*@} */
#elif !defined(IND_T) || !defined(IND_TAG) || !defined(IND_TAG_CAPS) || !defined(IND_TYPE_ID)
#error "Integer index type specification is incomplete: must define IND_T, IND_TAG, IND_TAG_CAPS, and IND_TYPE_ID"
#endif

#if VAL_TAG_CHAR == 's'
	/** \name Non-zero value type macros for 's' ('single') */
	/*@{ */
#  undef VAL_T
#  define VAL_T float
#  undef VAL_TAG
#  define VAL_TAG s
#  undef VAL_TAG_CAPS
#  define VAL_TAG_CAPS S
#  undef VAL_TYPE_ID
#  define VAL_TYPE_ID SINGLE
	/*@} */
#elif VAL_TAG_CHAR == 'd'
	/** \name Non-zero value type macros for 'd' ('double') */
	/*@{ */
#  undef VAL_T
#  define VAL_T double
#  undef VAL_TAG
#  define VAL_TAG d
#  undef VAL_TAG_CAPS
#  define VAL_TAG_CAPS D
#  undef VAL_TYPE_ID
#  define VAL_TYPE_ID DOUBLE
	/*@} */
#elif VAL_TAG_CHAR == 'c'
	/** \name Non-zero value type macros for 'c' ('complex float') */
	/*@{ */
#  undef VAL_T
#  define VAL_T complex_t
#  undef VAL_TAG
#  define VAL_TAG c
#  undef VAL_TAG_CAPS
#  define VAL_TAG_CAPS C
#  undef VAL_TYPE_ID
#  define VAL_TYPE_ID COMPLEX
	/*@} */
#elif VAL_TAG_CHAR == 'z'
	/** \name Non-zero value type macros for 'z' ('complex double') */
	/*@{ */
#  undef VAL_T
#  define VAL_T doublecomplex_t
#  undef VAL_TAG
#  define VAL_TAG z
#  undef VAL_TAG_CAPS
#  define VAL_TAG_CAPS Z
#  undef VAL_TYPE_ID
#  define VAL_TYPE_ID DOUBLECOMPLEX
	/*@} */
#elif !defined(VAL_T) || !defined(VAL_TAG) || !defined(VAL_TAG_CAPS) || !defined(VAL_TYPE_ID)
#  error "Non-zero value type specification is incomplete: must define VAL_T, VAL_TAG, VAL_TAG_CAPS, and VAL_TYPE_ID"
#endif

/* ----------------------------------------------------------------- */

/**
 *  \brief Returns 1 if the current default value type is complex.
 *
 *  This implementation simply checks the currently defined VAL_TAG
 *  macro to see if it is 'c' or 'z'.
 */
#define IS_VAL_COMPLEX  ((VAL_TAG_CHAR=='c') || (VAL_TAG_CHAR=='z'))

/**
 *  \brief Returns 1 if the current default value type is single precision.
 */
#define IS_VALPREC_SINGLE  ((VAL_TAG_CHAR=='s') || (VAL_TAG_CHAR=='c'))

/**
 *  \brief Returns 1 if the current default value type is single precision.
 */
#define IS_VALPREC_DOUBLE  ((VAL_TAG_CHAR=='d') || (VAL_TAG_CHAR=='z'))

#if IS_VALPREC_DOUBLE
#  if HAVE_EPS_DOUBLE
#    include <float.h>
#    define VAL_EPS DBL_EPSILON /**< \brief Double-precision machine epsilon */
#  else /** \brief Approximate double-precision machine epsilon */
#    define VAL_EPS 1e-15
#  endif
#elif IS_VALPREC_SINGLE
#  if HAVE_EPS_FLOAT
#    include <float.h>
#      define VAL_EPS FLT_EPSILON /**< \brief Double-precision machine epsilon */
#  else
#    define VAL_EPS 1e-7 /**< \brief Approx. single-precision machine epsilon */
#  endif
#else
#  warning "WARNING: Not sure how to define machine epsilon..."
#  define VAL_EPS 1e-7 /**< \brief Default machine epsilon */
#endif

#define VAL_ZERO    (0.0)  /**< Numerical zero */
#define VAL_ONE     (1.0)  /**< Numerical one */
#define VAL_NEG_ONE  (-1.0) /**< Numerical negative one */

#include <math.h>

#  if HAVE_C99_FPCLASSIFY
     /** Returns 1 <==> x == 0.0 */
#    define IS_REAL_ZERO(x)  (fpclassify(x) == FP_ZERO)
#  else
     /** Returns 1 <==> x == 0.0 */
#    define IS_REAL_ZERO(x)  ((x) == VAL_ZERO)
#  endif

#if !(IS_VAL_COMPLEX)

#  if HAVE_C99_FPCLASSIFY
		/** Returns 1 <==> x == 0.0 */
#    define IS_VAL_ZERO(x)    (fpclassify(x) == FP_ZERO)
		/** Returns 1 <==> x == 1.0 */
#    define IS_VAL_ONE(x)     (fpclassify((x)-1.0) == FP_ZERO)
		/** Returns 1 <==> x == -1.0 */
#    define IS_VAL_NEG_ONE(x)     (fpclassify((x)+1.0) == FP_ZERO)
#  else /* !HAVE_C99_FPCLASSIFY */
		/** Returns 1 <==> x == 0.0 */
#    define IS_VAL_ZERO(x)    ((x) == VAL_ZERO)
		/** Returns 1 <==> x == 1.0 */
#    define IS_VAL_ONE(x)     ((x) == VAL_ONE)
		/** Returns 1 <==> x == -1.0 */
#    define IS_VAL_NEG_ONE(x)  ((x) == VAL_NEG_ONE)
#  endif

	/** Returns \f$|x|\f$ as a double-precision value. */
#  define VAL_ABS(x)  fabs(x)

	/** Sets x to 0 */
#  define VAL_SET_ZERO(x)  (x) = 0.0

	/** Sets x to 1 */
#  define VAL_SET_ONE(x)  (x) = 1.0

	/** Conjugates x, i.e., \f$x \leftarrow \bar{x}\f$. */
#  define VAL_CONJ(x)

	/** Set \f$y \leftarrow x\f$, where \f$x, y\f$ are real variables. */
#  define VAL_ASSIGN(y, x)  (y) = (x)

	/** Set \f$y \leftarrow \bar{x}\f$, where \f$x, y\f$ are real variables. */
#  define VAL_ASSIGN_CONJ(y, x)  VAL_ASSIGN(y, x)

	/** Set \f$y \leftarrow -x\f$, where \f$x, y\f$ are real variables. */
#  define VAL_ASSIGN_NEG(y, x)  (y) = -(x)

	/** Set \f$y \leftarrow -\bar{x}\f$, where \f$x, y\f$ are real variables. */
#  define VAL_ASSIGN_NEG_CONJ(y, x)  VAL_ASSIGN_NEG(y, x)

	/** Make an initializer record (ignores imaginary part) */
#  define MAKE_VAL_COMPLEX(x, y)  (x)

	/**
	 *  \brief Compute \f$x *= \alpha\f$, where
	 *  \f$x, \alpha\f$ are real numbers.
	 */
#  define VAL_SCALE(x, alpha)  (x) *= (alpha)

	/**
	 *  \brief Compute \f$x *= \bar{\alpha}\f$, where
	 *  \f$x, \alpha\f$ are real numbers.
	 */
#  define VAL_SCALE_CONJ(x, alpha)  VAL_SCALE(x, alpha)

	/**
	 *  \brief Compute \f$y += \alpha\cdot x\f$ (multiply-accumulate),
	 *  where \f$x, \alpha, y\f$ are all real numbers.
	 */
#  define VAL_MAC(y, alpha, x)  (y) += (alpha) * (x)

	/**
	 *  \brief Compute \f$y += \bar{\alpha}\cdot x\f$ (multiply-accumulate),
	 *  where \f$x, \alpha, y\f$ are all real numbers.
	 */
#  define VAL_MAC_CONJ(y, alpha, x)  VAL_MAC(y, alpha, x)

	/**
	 *  \brief Compute \f$y -= \alpha\cdot x\f$ (multiply-subtract),
	 *  where \f$x, \alpha, y\f$ are all real numbers.
	 */
#  define VAL_MSUB(y, alpha, x)  (y) -= (alpha) * (x)

	/**
	 *  \brief Compute \f$y -= \bar{\alpha}\cdot x\f$ (multiply-subtract),
	 *  where \f$x, \alpha, y\f$ are all real numbers.
	 */
#  define VAL_MSUB_CONJ(y, alpha, x)  VAL_MSUB(y, alpha, x)

	/**
	 *  \brief Compute \f$x -= a\f$ (decrease/decrement),
	 *  where \f$x, a\f$ are real numbers.
	 */
#  define VAL_DEC(x, a)   (x) -= (a)

	/**
	 *  \brief Compute \f$x -= \bar{a}\f$ (decrease/decrement),
	 *  where \f$x, a\f$ are real numbers.
	 */
#  define VAL_DEC_CONJ(x, a)   VAL_DEC(x, a)

	/**
	 *  \brief Compute \f$x += a\f$ (increase/increment),
	 *  where \f$x, a\f$ are real numbers.
	 */
#  define VAL_INC(x, a)   (x) += (a)

	/**
	 *  \brief Compute \f$x += \bar{a}\f$ (increase/increment),
	 *  where \f$x, a\f$ are real numbers.
	 */
#  define VAL_INC_CONJ(x, a)   VAL_INC(x, a)

	/**
	 *  \brief Computes \f$z \leftarrow x\cdot y\f$,
	 *  where \f$x, y, z\f$ are real, unaliased variables.
	 */
#  define VAL_MUL(z, x, y)  (z) = (x) * (y)

	/**
	 *  Computes \f$z \leftarrow \bar{x}\cdot y\f$,
	 *  where \f$x, y, z\f$ are real, unaliased variables.
	 */
#  define VAL_MUL_CONJ(z, x, y)  VAL_MUL(z, x, y)

	/**
	 *  \brief Compute \f$z = y + alpha*x\f$ (triad),
	 *  where \f$z, y, x, \alpha\f$ are all real numbers.
	 */
#  define VAL_TRIAD(z, y, alpha, x) (z) = (y) + (alpha) * (x)

	/**
	 *  \brief Compute \f$z = y + \bar{alpha}*x\f$ (triad),
	 *  where \f$z, y, x, \alpha\f$ are all real numbers.
	 */
#  define VAL_TRIAD_CONJ(z, y, alpha, x)   VAL_TRIAD(z, y, alpha, x)

	/**
	 *  \brief Compute \f$x /= \alpha\f$,
	 *  where \f$x\f$ and \f$\alpha\f$ are real.
	 */
#  define VAL_DIVEQ(x, a)  (x) /= (a)

	/**
	 *  \brief Compute \f$x /= \bar{\alpha}\f$,
	 *  where \f$x\f$ and \f$\alpha\f$ are real.
	 */
#  define VAL_DIVEQ_CONJ(x, a)  VAL_DIVEQ(x, a)

	/**
	 *  \brief Computes \f$y \leftarrow \frac{1}{x}\f$, where
	 *  \f$x, y\f$ are real.
	 */
#  define VAL_INV(y, x)  (y) = 1.0/(x)

#else /* IS_VAL_COMPLEX */

#  if HAVE_C99_FPCLASSIFY
		/** Returns 1 <==> x == 0.0 + 0.0i */
#    define IS_VAL_ZERO(x)    \
			(fpclassify(_RE(x)) == FP_ZERO && fpclassify(_IM(x)) == FP_ZERO)
		/** Returns 1 <==> x == 1.0 + 0.0i */
#    define IS_VAL_ONE(x)     \
			(fpclassify(_RE(x)-1.0) == FP_ZERO && fpclassify(_IM(x)) == FP_ZERO)
		/** Returns 1 <==> x == -1.0 + 0.0i */
#    define IS_VAL_NEG_ONE(x)  \
			(fpclassify(_RE(x)+1.0) == FP_ZERO && fpclassify(_IM(x)) == FP_ZERO)
#  else /* !HAVE_C99_FPCLASSIFY */
		/** Returns 1 <==> x == 0.0 + 0.0i */
#    define IS_VAL_ZERO(x)    \
			((_RE(x) == VAL_ZERO) && (_IM(x) == VAL_ZERO))
		/** Returns 1 <==> x == 1.0 + 0.0i */
#    define IS_VAL_ONE(x)     \
			((_RE(x) == VAL_ONE) && (_IM(x) == VAL_ZERO))
		/** Returns 1 <==> x == -1.0 + 0.0i */
#    define IS_VAL_NEG_ONE(x)  \
			((_RE(x) == VAL_NEG_ONE) && (_IM(x) == VAL_ZERO))
#  endif

	/** Returns \f$|x|\f$ as a double-precision value. */
#  define VAL_ABS(x)  sqrt(_RE(x)*_RE(x)+_IM(x)*_IM(x))

	/** Set \f$y \leftarrow x\f$, where \f$x, y\f$ are complex variables. */
#  define VAL_ASSIGN(y, x)  _RE(y) = _RE(x), _IM(y) = _IM(x)

	/** Set \f$y \leftarrow \bar{x}\f$, where \f$x, y\f$ are complex variables. */
#  define VAL_ASSIGN_CONJ(y, x)  _RE(y) = _RE(x), _IM(y) = -_IM(x)

	/** Set \f$y \leftarrow -x\f$, where \f$x, y\f$ are complex variables. */
#  define VAL_ASSIGN_NEG(y, x)  _RE(y) = -_RE(x), _IM(y) = -_IM(x)

	/** Set \f$y \leftarrow -\bar{x}\f$, where \f$x, y\f$ are complex variables. */
#  define VAL_ASSIGN_NEG_CONJ(y, x)  _RE(y) = -_RE(x), _IM(y) = _IM(x)

	/** Sets x to 0.0 + 0.0i */
#  define VAL_SET_ZERO(x)  _RE(x) = 0.0, _IM(x) = 0.0

	/** Sets x to 1.0 + 0.0i */
#  define VAL_SET_ONE(x)  _RE(x) = 1.0, _IM(x) = 0.0

	/** Conjugates x, i.e., \f$x \leftarrow \bar{x}\f$. */
#  define VAL_CONJ(x)  _IM(x) = -_IM(x)

	/** Make a complex initializer record */
#  define MAKE_VAL_COMPLEX(x, y)   {(x), (y)}

	/** Return real part of product x*a */
#  define VAL_MUL_RE(x, a)  (_RE(x)*_RE(a) - _IM(x)*_IM(a))

	/** Return real part of product \bar{x}*a */
#  define VAL_MUL_CONJ_RE(x, a)  (_RE(x)*_RE(a) + _IM(x)*_IM(a))

	/** Return imaginary part of product x*a */
#  define VAL_MUL_IM(x, a)  (_RE(x)*_IM(a) + _IM(x)*_RE(a))

	/** Return imaginary part of product \bar{x}*a */
#  define VAL_MUL_CONJ_IM(x, a)  (_RE(x)*_IM(a) - _IM(x)*_RE(a))

	/**
	 *  \brife Computes \f$z \leftarrow x\cdot y\f$,
	 *  where \f$x, y, z\f$ are complex, unaliased variables.
	 */
#  define VAL_MUL(z, x, y)  \
		_RE(z) = VAL_MUL_RE(x, y), _IM(z) = VAL_MUL_IM(x, y)

	/**
	 *  \brife Computes \f$z \leftarrow \bar{x}\cdot y\f$,
	 *  where \f$x, y, z\f$ are complex, unaliased variables.
	 */
#  define VAL_MUL_CONJ(z, x, y)  \
		_RE(z) = VAL_MUL_CONJ_RE(x, y), _IM(z) = VAL_MUL_CONJ_IM(x, y)

	/**
	 *  \brief Compute \f$x *= \alpha\f$, where
	 *  \f$x, \alpha\f$ are complex numbers.
	 */
#  define VAL_SCALE(x, a)  { \
		oski_value_t y = x; \
		_RE(x) = VAL_MUL_RE(y, a); \
		_IM(x) = VAL_MUL_IM(y, a); \
		}

	/**
	 *  \brief Compute \f$x *= \bar{\alpha}\f$, where
	 *  \f$x, \alpha\f$ are complex numbers.
	 */
#  define VAL_SCALE_CONJ(x, a)  { \
		oski_value_t y = x; \
		_RE(x) = VAL_MUL_CONJ_RE(a, y); \
		_IM(x) = VAL_MUL_CONJ_IM(a, y); \
		}

	/**
	 *  \brief Compute \f$x += a\f$ (increase/increment),
	 *  where \f$x, a\f$ are complex numbers.
	 */
#  define VAL_INC(x, a)   _RE(x) += _RE(a), _IM(x) += _IM(a)

	/**
	 *  \brief Compute \f$x += \bar{a}\f$ (increase/increment),
	 *  where \f$x, a\f$ are complex numbers.
	 */
#  define VAL_INC_CONJ(x, a)   _RE(x) += _RE(a), _IM(x) -= _IM(a)

	/**
	 *  \brief Compute \f$x -= a\f$ (decrease/decrement),
	 *  where \f$x, a\f$ are complex numbers.
	 */
#  define VAL_DEC(x, a)   _RE(x) -= _RE(a), _IM(x) -= _IM(a)

	/**
	 *  \brief Compute \f$x -= \bar{a}\f$ (decrease/decrement),
	 *  where \f$x, a\f$ are complex numbers.
	 */
#  define VAL_DEC_CONJ(x, a)   _RE(x) -= _RE(a), _IM(x) += _IM(a)

	/**
	 *  \brief Compute \f$y += \alpha\cdot x\f$ (multiply-accumulate),
	 *  where \f$x, \alpha, y\f$ are all complex numbers.
	 */
#  define VAL_MAC(y, alpha, x)  \
		_RE(y) += VAL_MUL_RE(alpha, x), _IM(y) += VAL_MUL_IM(alpha, x)

	/**
	 *  \brief Compute \f$y += \bar{\alpha}\cdot x\f$ (multiply-accumulate),
	 *  where \f$x, \alpha, y\f$ are all complex numbers.
	 */
#  define VAL_MAC_CONJ(y, alpha, x)  \
		_RE(y) += VAL_MUL_CONJ_RE(alpha, x), _IM(y) += VAL_MUL_CONJ_IM(alpha, x)

	/**
	 *  \brief Compute \f$y -= \alpha\cdot x\f$ (multiply-subtract),
	 *  where \f$x, \alpha, y\f$ are all complex numbers.
	 */
#  define VAL_MSUB(y, alpha, x)  \
		_RE(y) -= VAL_MUL_RE(alpha, x), \
		_IM(y) -= VAL_MUL_IM(alpha, x)

	/**
	 *  \brief Compute \f$y -= \bar{\alpha}\cdot x\f$ (multiply-subtract),
	 *  where \f$x, \alpha, y\f$ are all complex numbers.
	 */
#  define VAL_MSUB_CONJ(y, alpha, x)  \
		_RE(y) -= VAL_MUL_CONJ_RE(alpha, x), \
		_IM(y) -= VAL_MUL_CONJ_IM(alpha, x)

	/**
	 *  \brief Compute \f$z = y + alpha*x\f$ (triad),
	 *  where \f$z, y, x, \alpha\f$ are all complex numbers.
	 */
#  define VAL_TRIAD(z, y, alpha, x) \
		_RE(z) = _RE(y), _IM(z) = _IM(y), VAL_MAC(z, alpha, x)

	/**
	 *  \brief Compute \f$z = y + \bar{alpha}*x\f$ (triad),
	 *  where \f$z, y, x, \alpha\f$ are all complex numbers.
	 */
#  define VAL_TRIAD_CONJ(z, y, alpha, x) \
		_RE(z) = _RE(y), _IM(z) = _IM(y), VAL_MAC_CONJ(z, alpha, x)

	/**
	 *  \brief Compute \f$x /= \alpha\f$,
	 *  where \f$x\f$ and \f$\alpha\f$ are complex.
	 *
	 *  \note This implementation assumes the compiler will apply
	 *  common subexpression elimination for the final scaling
	 *  by \f$|\alpha|\f$.
	 */
#  define VAL_DIVEQ(x, a)  { \
		oski_value_t t = (x); \
		_RE(x) = _RE(t)*_RE(a) + _IM(t)*_IM(a); \
		_IM(x) = _IM(t)*_RE(a) - _RE(t)*_IM(a); \
		_RE(x) /= _RE(a)*_RE(a) + _IM(a)*_IM(a); \
		_IM(x) /= _RE(a)*_RE(a) + _IM(a)*_IM(a); \
	}

	/**
	 *  \brief Compute \f$x /= \bar{\alpha}\f$,
	 *  where \f$x\f$ and \f$\alpha\f$ are complex.
	 *
	 *  \note This implementation assumes the compiler will apply
	 *  common subexpression elimination for the final scaling
	 *  by \f$|\alpha|\f$.
	 */
#  define VAL_DIVEQ_CONJ(x, a)  { \
		oski_value_t t = (x); \
		_RE(x) = _RE(t)*_RE(a) - _IM(t)*_IM(a); \
		_IM(x) = _IM(t)*_RE(a) + _RE(t)*_IM(a); \
		_RE(x) /= _RE(a)*_RE(a) + _IM(a)*_IM(a); \
		_IM(x) /= _RE(a)*_RE(a) + _IM(a)*_IM(a); \
	}

	/**
	 *  \brief Computes \f$y \leftarrow \frac{1}{x}\f$, where
	 *  \f$x, y\f$ are complex
	 */
#  define VAL_INV(y, x)  \
		_RE(y) = _RE(x) / (_RE(x)*_RE(x) + _IM(x)*_IM(x)), \
		_IM(y) = -_IM(x) / (_RE(x)*_RE(x) + _IM(x)*_IM(x))

#endif

/** Make a complex initializer record */
#define MAKE_VAL_REAL(x)   MAKE_VAL_COMPLEX((x), VAL_ZERO)

/**
 *  A dummy value to represent a "general" value.
 *
 *  This is used in trace representation.
 */
#define MAKE_VAL_GENERAL   MAKE_VAL_COMPLEX(-9, -9)

/**
 *  Returns \f$\alpha\f$ if \f$\alpha \in \{-1, 0, 1\}\f$, or a
 *  constant value (see \ref MAKE_VAL_GENERAL) otherwise.
 */
#define CATEGORIZE_VAL(alpha) ( \
		(IS_VAL_ONE(alpha) || IS_VAL_ZERO(alpha) || IS_VAL_NEG_ONE(alpha)) \
			? (alpha) : MAKE_VAL_GENERAL \
	)

#if !defined(MOD_NAME) && defined(OSKI_BUILD_TIME)
#pragma WARNING "Should instantiate a module name explicitly."
	/**
	 *  \brief Compile-time macro indicating the name of the current module.
	 *
	 *  This macro is used by the module name mangler to create
	 *  unique names when the user builds the library with static
	 *  libraries only.
	 */
#define MOD_NAME self
#endif


/**
 *  \brief Makes a type-mangled name, [BASE]_T[IND][VAL].
 *
 *  \param[in] base Base type name.
 *  \param[in] ind Type abbreviation for the integer index type.
 *  \param[in] val Type abbreviation for the non-zero value type.
 *  \returns The mangled name, [base]_T[ind][val].
 *
 *  This routine directly concatenates its arguments into the
 *  form indicated. If the caller wishes to use macro arguments
 *  for any of base, ind, and val, and have those arguments
 *  expanded before concatenation, then she should call
 *  #MAKETYPENAME1 instead.
 *  \see MANGLE_, MAKETYPENAME1
 */
#define MAKETYPENAME0(base, ind, val) base ## _T ## ind ## val

/**
 *  \brief Name mangling helper macro.
 *
 *  This macro immediately calls #MAKETYPENAME0 with the same
 *  arguments. Since base, ind, and/or val will often be macros
 *  themselves, this intermediate macro provides a way to force
 *  the C preprocessor to expand the macro arguments before
 *  concatenation.
 *
 *  \see MAKETYPENAME0, MANGLE_
 */
#define MAKETYPENAME1(base, ind, val) MAKETYPENAME0(base, ind, val)

/**
 *  \brief Makes a type-mangled shared library module routine name,
 *  [MOD]_T[IND][VAL]_LTX_[BASE].
 *
 *  This macro immediately concatenates its arguments to make
 *  a static-compatible module routine name of the form
 *
 *      [mod]_T[ind][val]_LTX_[base]
 *
 *   This naming convention allows the LTDL-based module loader
 *   to find methods dynamically.
 */
#define MAKEMODNAME0(mod, base, ind, val) \
	mod ## _T ## ind ## val ## _LTX_ ## base

/**
 *  \brief Name mangling helper macro.
 *
 *  This macro immediately calls MAKEMODNAME0 with the same
 *  arguments. Since mod, base, ind, and/or val will often be
 *  macros themselves, this intermediate macro provides a way
 *  to force the C preprocessor to expand the macro arguments
 *  before concatenation.
 */
#define MAKEMODNAME1(mod, base, ind, val) MAKEMODNAME0(mod, base, ind, val)

/**
 *  \brief Mangle a base type name.
 *
 *  \param[in] base Base type name.
 *
 *  This macro mangles the type name, base, using the currently
 *  instantiated (defined) scalar types, through the
 *  compile-time macros IND_TAG and VAL_TAG.
 *
 *  \note The described behavior above applies to this macro
 *  only when the compile-time macro DO_NAME_MANGLING is
 *  defined.
 *
 *  \pre The compile-time macros IND_TAG and VAL_TAG
 *  <em>must</em> be defined.
 *
 *  \see MAKETYPENAME0, MAKETYPENAME1
 */
#define MANGLE_(base) MAKETYPENAME1(base, IND_TAG, VAL_TAG)

/**
 *  \brief Mangle a module routine name.
 *
 *  \param[in] base Base routine name.
 *
 *  This macro mangles the routine name, base, using the
 *  currently instantiated module name macro, MOD_NAME.
 *
 *  \pre The compile-time macro MOD_NAME <em>must</em> be defined.
 */
#define MANGLE_MOD_(base) MAKEMODNAME1(MOD_NAME, base, IND_TAG, VAL_TAG)

/** \name Mangler for type ID numbers for the default scalar types. */
/*@{*/
/**
 *  \brief Direct concatenation of input arguments.
 */
#define MANGLE_CONCAT0(a, b)  a ## b

/**
 *  \brief Macro-expanded concatenation of input arguments.
 *
 *  This macro serves as an intermediate step to ensure that
 *  the input arguments, a and b, will be expanded if they are
 *  themselves macros.
 */
#define MANGLE_CONCAT(a, b)   MANGLE_CONCAT0(a, b)

/** \brief Type ID for the default scalar integer index type */
#define OSKI_IND_ID  MANGLE_CONCAT(OSKI_SCALIND_, IND_TYPE_ID)

/** \brief Type ID for the default scalar non-zero value type */
#define OSKI_VAL_ID  MANGLE_CONCAT(OSKI_SCALVAL_, VAL_TYPE_ID)
/*@}*/

/**
 *  \brief Make a function pointer type name of the form,
 *  oski_[name]_funcpt.
 */
#define OSKI_MAKENAME_FUNCPT(name) oski_ ## name ## _funcpt

#if 0
#if defined(DO_NAME_MANGLING)
#define oski_index_t  MANGLE_(oski_index_t)
#define oski_value_t  MANGLE_(oski_value_t)
#endif
#endif
#define oski_index_t IND_T
#define oski_value_t VAL_T

/**
 * \name Basic scalar types.
 * \ingroup PUBLIC
 */
/*@{*/
#if 0
typedef IND_T oski_index_t; /**< Integer indices. */
typedef VAL_T oski_value_t; /**< Non-zero values. */
#endif
/*@}*/

/** \name Typed value constants. */
/*@{*/
#define TVAL_ZERO  MANGLE_(TVAL_ZERO)
#define TVAL_ONE   MANGLE_(TVAL_ONE)
#define TVAL_NEG_ONE  MANGLE_(TVAL_NEG_ONE)
#define TVAL_IMAG  MANGLE_(TVAL_IMAG)
#define TVAL_NEG_IMAG  MANGLE_(TVAL_NEG_IMAG)

extern const oski_value_t TVAL_ZERO;
extern const oski_value_t TVAL_ONE;
extern const oski_value_t TVAL_NEG_ONE;
extern const oski_value_t TVAL_IMAG;
extern const oski_value_t TVAL_NEG_IMAG;
/*@}*/

#endif /* !defined(INC_OSKI_MANGLE_H) */

#if defined(OSKI_UNBIND)
#  undef INC_OSKI_MANGLE_H
#  undef IS_VAL_COMPLEX
#  undef IS_VALPREC_SINGLE
#  undef IS_VALPREC_DOUBLE
#  undef VAL_EPS
#  undef VAL_ZERO
#  undef VAL_ONE
#  undef VAL_NEG_ONE
#  undef IS_VAL_ZERO
#  undef IS_VAL_ONE
#  undef IS_VAL_NEG_ONE
#  undef IS_REAL_ZERO
#  undef VAL_ABS
#  undef VAL_SET_ZERO
#  undef VAL_SET_ONE
#  undef VAL_CONJ
#  undef VAL_ASSIGN
#  undef VAL_ASSIGN_CONJ
#  undef VAL_ASSIGN_NEG
#  undef VAL_ASSIGN_NEG_CONJ
#  undef MAKE_VAL_COMPLEX
#  undef VAL_SCALE
#  undef VAL_SCALE_CONJ
#  undef VAL_MAC
#  undef VAL_MAC_CONJ
#  undef VAL_MSUB
#  undef VAL_MSUB_CONJ
#  undef VAL_DEC
#  undef VAL_DEC_CONJ
#  undef VAL_INC
#  undef VAL_INC_CONJ
#  undef VAL_MUL
#  undef VAL_MUL_CONJ
#  undef VAL_TRIAD
#  undef VAL_TRIAD_CONJ
#  undef VAL_DIVEQ
#  undef VAL_DIVEQ_CONJ
#  undef VAL_INV
#  undef MAKE_VAL_REAL
#  undef MAKE_VAL_GENERAL
#  undef CATEGORIZE_VAL
#  undef MAKETYPENAME0
#  undef MAKETYPENAME1
#  undef MAKEMODNAME0
#  undef MAKEMODNAME1
#  undef MANGLE_
#  undef MANGLE_MOD_
#  undef MANGLE_CONCAT0
#  undef MANGLE_CONCAT
#  undef OSKI_IND_ID
#  undef OSKI_VAL_ID
#  undef OSKI_MAKENAME_FUNCPT
#  undef TVAL_ZERO
#  undef TVAL_ONE
#  undef TVAL_NEG_ONE
#  undef TVAL_IMAG
#  undef TVAL_NEG_IMAG
#endif

/* eof */
