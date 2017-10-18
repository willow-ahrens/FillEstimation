/**
 *  \file oski/error.h
 *  \ingroup AUXIL
 *  \brief Error-handling module.
 *
 *  This module implements the default BeBOP error handler and related
 *  support routines for overriding the global error handling routine
 *  with a user-defined one.
 *
 *  If pthreads multithreading support is enabled, then the
 *  #oski_GetErrorHandler and #oski_SetErrorHandler routines
 *  execute atomically using pthreads locking.
 */

#if !defined(INC_OSKI_ERROR_H)
/** oski/error.h has been included */
#define INC_OSKI_ERROR_H

/**
*  \defgroup ERROR Error codes and error handling.
*  \ingroup PUBLIC
*/
/*@{*/

/**
 *  \brief Pointer to a user-defined error handler.
 *
 *  An error handler has the following signature:
 *
 *      void handler( int error_code, const char* message,
 *               const char* source_filename,
 *               unsigned long line_number,
 *               const char* format_string, ... );
 *
 *   whose parameters are defined as follows:
 *
 *       - error_code: An integer error code less than 0.
 *       - message: A short but descriptive error message for
 *       display to the user.
 *       - source_filename: Name of the source file in which
 *       the error was detected.
 *       - line_number: Approximate line number at which the
 *       error occurred.
 *       - format_string: A printf-style string which, when
 *       used with the remaining arguments, is suitable for
 *       passing to a printf/fprintf/sprintf-type function
 *       to display additional diagonstic information.
 */
typedef void (*oski_errhandler_t) (int, const char *,
				   const char *, unsigned long, const char *,
				   ...);

/**
 *  \brief Returns a pointer to the current error handling routine
 *  for a given matrix object, or the current global handler if a
 *  valid matrix object is not specified.
 *
 */
oski_errhandler_t oski_GetErrorHandler (void);

/**
 *  \brief Changes the current error handler for a given matrix,
 *  or changes the default error handler if no valid matrix object
 *  is specified.
 */
oski_errhandler_t oski_SetErrorHandler (oski_errhandler_t new_handler);

/**
 *  \brief The default error handler, called when one of the BeBOP
 *  routines detects an error condition and the handler has not
 *  been overriden.
 */
void oski_HandleErrorDefault (int error_code, const char *message,
			      const char *source_filename,
			      unsigned long line_number,
			      const char *format_string, ...);

/**
 *  \brief Get generic information about an error code.
 */
const char *oski_GetErrorName (int error_code);

/** \brief Call error handler */
#define oski_HandleError  (oski_GetErrorHandler())

/**
 *  \brief Checks if a value lies in a specified closed interval.
 *
 *  \param[in] x Value to check.
 *  \param[in] a Interval end-point.
 *  \param[in] b Interval end-point.
 *  \returns 1 if and only if x lies between a and b.
 *  \note This macro works whether a < b or b < a, and is fully
 *  parenthesized.
 */
#define IS_VAL_IN_RANGE(x, a, b)  \
	((((a) <= (b)) && ((x) >= (a)) && ((x) <= (b))) \
	|| (((a) > (b)) && ((x) >= (b)) && ((x) <= (a))))

/**
 *  \name Error codes.
 *
 *  If adding a new error code here, also modify the list of
 *  generic messages in #oski_GetErrorName() (\ref error.c).
 */
/*@{*/
#define ERR_OUT_OF_MEMORY -1 /**< Out of memory */
#define ERR_INIT_LIB -2	/**< Error initializing the library */
#define ERR_INIT_SHAREDLIB -3
		/**< Could not initialized shared library module */
#define ERR_BAD_ID -4 /**< Invalid id number specified */
#define ERR_LOAD_MOD -5	/**< Could not load shared library module */
#define ERR_UNLOAD_MOD -6 /**< Could not unload shared library module */
#define ERR_LOAD_MATTYPE -7 /**< Could not load matrix type module */
#define ERR_UNLOAD_MATTYPE -8 /**< Could not unload matrix type module */
#define ERR_NOT_IMPLEMENTED -9 /**< Feature not implemented */
#define ERR_BAD_ARG -10	/**< Bad input argument */
#define ERR_INMATPROP_CONFLICT -11
	/**< Conflicting input matrix properties specified */
#define ERR_DIM_MISMATCH -12
	/**< Matrix multiply operand dimensions don't match */
#define ERR_FALSE_INMATPROP -13
	/**< Asserted input matrix property is false */
#define ERR_BAD_LEADDIM -14
	/**< Invalid leading dimension specified */
#define ERR_BAD_MAT -15
	/**< Invalid matrix object */
#define ERR_BAD_VECVIEW -16
	/**< Invalid vector view object */
#define ERR_WRONG_ANS -17
	/**< Wrong answer computed! */
#define ERR_BAD_HINT_ARG -18
	/**< Invalid argument psased to a hint-setting routine. */
#define ERR_CANT_CONVERT -19
	/**< Cannot convert data structure. */
#define ERR_FILE_READ -20
	/**< Can't open an input file. */
#define ERR_BAD_SYNTAX -21
	/**< BeBOP-Lua transformation program has invalid syntax. */
#define ERR_OLUA_NORET -22
	/**< BeBOP-Lua transformation program did not return a new matrix. */
#define ERR_OLUA_STARTUP -23
	/**< Can't start BeBOP-Lua interpreter. */
#define ERR_OLUA_TYPE_MISMATCH -24
	/**< BeBOP-Lua types differ */
#define ERR_BAD_PERM -25
	/**< An error detected in permutation. */
#define ERR_BAD_ENTRY -26
	/**< An invalid (out-of-range) entry was specified. */
#define ERR_ZERO_ENTRY -27
	/**< Attempt to change a logically zero entry. */
/*@}*/
/*@}*/

/**
 *  \name Predefined error message macros.
 *
 *  These macros provide handy shortcuts for generating certain
 *  standard errors.
 */
/*@{*/

/** \brief Generic error message */
#define OSKI_ERR(method, err_code) \
	{if( (err_code) < 0 ) { \
		oski_HandleError( (err_code), "An error occurred", \
			__FILE__, __LINE__, "In call to '%s()': %s", \
			MACRO_TO_STRING(method), oski_GetErrorName(err_code) ); \
	}}

/** \brief Feature/method not yet implemented. */
#define OSKI_ERR_NOT_IMPLEMENTED(method, desc) \
	oski_HandleError( ERR_NOT_IMPLEMENTED, \
		"Feature not yet implemented", __FILE__, __LINE__, \
		"%s, %s()", desc, MACRO_TO_STRING(method) \
	)

/** \brief Member not implemented. */
#define OSKI_ERR_MATTYPEID_MEMBER(method, id, member) \
	oski_HandleError( ERR_NOT_IMPLEMENTED, \
		"Can't find matrix type-specific method implementation", \
		__FILE__, __LINE__, \
		"In call to %s(): Could not load %s() for matrix type #%d\n", \
		MACRO_TO_STRING(method), "oski_" MACRO_TO_STRING(member), \
		(int)(id) \
	)

/** \brief Member not implemented. */
#define OSKI_ERR_MATTYPE_MEMBER(method, name, member) \
	oski_HandleError( ERR_NOT_IMPLEMENTED, \
		"Can't find matrix type-specific method implementation", \
		__FILE__, __LINE__, \
		"In call to %s(): Could not load %s() for matrix type" \
			" %s(ind=%s, val=%s)" \
		MACRO_TO_STRING(method), "oski_" MACRO_TO_STRING(member), \
		name, MACRO_TO_STRING(IND_T), MACRO_TO_STRING(VAL_T) \
	)

/** \brief Invalid input matrix property specified. */
#define OSKI_ERR_BAD_INMATPROP(method, val) \
	oski_HandleError( ERR_BAD_ARG, \
		"Unrecognized input matrix property specified", __FILE__, __LINE__, \
		"Got '%d' in call to %s()", (int)(val), MACRO_TO_STRING(method) \
	)

/**
 *  \brief Inconsistent input matrix property specified.
 *
 *  \param[in] method Name of the caller (will be converted to a string).
 *  \param[in] arg_num A zero-based integer specifying the troublesome
 *  argument.
 *  \returns Calls the error handler. In the message to the user, the
 *  argument number is printed as a 1-based integer.
 */
#define OSKI_ERR_INMATPROP_CONFLICT(method, arg_num) \
	oski_HandleError( ERR_INMATPROP_CONFLICT, \
		"Conflicting input matrix properties were specified", \
		__FILE__, __LINE__, \
		"Please check property #%d specified in the call to %s()", \
		(int)((arg_num)+1), MACRO_TO_STRING(method) \
	)

/**
 *  \brief Bad matrix dimensions.
 *
 *  \see OSKI_CHECK_MAT_DIMS
 */
#define OSKI_ERR_BAD_DIMS(method, m, n) \
	oski_HandleError( ERR_BAD_ARG, \
		"Matrix dimensions must be non-negative", \
		__FILE__, __LINE__, \
		"Dimensions specified in call to %s(): %d x %d\n", \
		MACRO_TO_STRING(method), (int)m, (int)n \
	)

/**
 *  \brief Invalid copy mode specified.
 */
#define OSKI_ERR_BAD_COPYMODE(method, mode) \
	oski_HandleError( ERR_BAD_ARG, \
		"Unrecognized copy mode specified", \
		__FILE__, __LINE__, "Mode specified to %s(): %d", \
		MACRO_TO_STRING(method), (int)(mode) \
	)

/**
 *  \brief Invalid storage orientation specified.
 */
#define OSKI_ERR_BAD_STORAGE( method, mode ) \
	oski_HandleError( ERR_BAD_ARG, \
		"Unrecognized dense vector/multivector data layout specified", \
		__FILE__, __LINE__, "Layout specified to %s(): %d", \
		MACRO_TO_STRING(method), (int)(mode) \
	)

/**
 *  \brief Bad matrix-transpose operation specified.
 */
#define OSKI_ERR_BAD_MATOP_MS( method, argnum, opA ) \
	oski_HandleError( ERR_BAD_ARG, \
		"Unrecognized matrix transpose operation", __FILE__, __LINE__, \
		"Please check parameter #%d passed to %s() which equals %d", \
		(int)(argnum), (method), (int)(opA) \
	)

/**
 *  \brief Bad matrix-transpose operation specified.
 */
#define OSKI_ERR_BAD_MATOP( method, argnum, opA ) \
	OSKI_ERR_BAD_MATOP_MS( MACRO_TO_STRING(method), (argnum), (opA) )

/**
 *  \brief Bad A^T*A operation specified.
 */
#define OSKI_ERR_BAD_ATAOP_MS( method, argnum, op ) \
	oski_HandleError( ERR_BAD_ARG, \
		"Unrecognized A^T*A transpose operation", __FILE__, __LINE__, \
		"Please check parameter #%d passed to %s() which equals %d", \
		(int)(argnum), (method), (int)(op) \
	)

/**
 *  \brief Bad A^T*A operation specified.
 */
#define OSKI_ERR_BAD_ATAOP( method, argnum, op ) \
	OSKI_ERR_BAD_ATAOP_MS( MACRO_TO_STRING(method), argnum, op )

/**
 *  \brief Matrix multiply operand dimensions don't match
 */
#define OSKI_ERR_DIM_MISMATCH_MS(method, m_A, n_A, m_B, n_B, m_C, n_C) \
	oski_HandleError( ERR_DIM_MISMATCH, \
		"Cannot multiply matrices with incompatible dimensions", \
		__FILE__, __LINE__, \
		"In %s(), for C = A*B, C is %d x %d, A is %d x %d, and B is %d x %d", \
			method, m_C, n_C, m_A, n_A, m_B, n_B \
	)

/**
 *  \brief Matrix multiply operand dimensions don't match
 */
#define OSKI_ERR_DIM_MISMATCH(method, m_A, n_A, m_B, n_B, m_C, n_C) \
	OSKI_ERR_DIM_MISMATCH_MS( MACRO_TO_STRING(method), \
		m_A, n_A, m_B, n_B, m_C, n_C )

#define OSKI_ERR_DIM_UNEQUAL(method, m_A, n_A, m_B, n_B) \
	oski_HandleError( ERR_DIM_MISMATCH, \
		"The vector/matrix operands have incompatible dimensions", \
		__FILE__, __LINE__, \
		"In call to %s(), the operand dimensions are %dx%d and %dx%d", \
		(int)(m_A), (int)(n_A), (int)(m_B), (int)(n_B) \
	)

/**
 *  \brief Matrix-transpose multiply operand dimensions don't match
 */
#define OSKI_ERR_DIM_MISMATCH_TRANS_MS(method, m_A, n_A, m_B, n_B, m_C, n_C) \
	oski_HandleError( ERR_DIM_MISMATCH, \
		"Cannot multiply matrices with incompatible dimensions", \
		__FILE__, __LINE__, \
		"In %s(), for C = op(A)*B, C is %d x %d," \
	   		"op(A) is %d x %d, and B is %d x %d", \
			method, m_C, n_C, n_A, m_A, m_B, n_B \
	)

/**
 *  \brief Matrix-transpose multiply operand dimensions don't match
 */
#define OSKI_ERR_DIM_MISMATCH_TRANS(method, m_A, n_A, m_B, n_B, m_C, n_C) \
	OSKI_ERR_DIM_MISMATCH_TRANS_MS( MACRO_TO_STRING(method), \
		m_A, n_A, m_B, n_B, m_C, n_C )

/**
 *  \brief Matrix multiply operand dimensions don't match.
 *
 *  This macro is a high-level object wrapper around an equivalent
 *  call to #OSKI_ERR_DIM_MISMATCH(). The operands to this macro
 *  are assumed to be pointers to structs containing 'num_rows'
 *  and 'num_cols' fields.
 *
 *  A must be a valid object. However, either B or C may be NULL,
 *  in which case the missing argument is assumed to have the
 *  correct dimensions. B and C must not both be NULL, however.
 */
#define OSKI_ERR_DIM_MISMATCH_OBJ_MS(method, A, B, C) \
	OSKI_ERR_DIM_MISMATCH_MS( method, (A)->num_rows, (A)->num_cols, \
		((B) == INVALID_VEC) ? (A)->num_cols : (B)->num_rows, \
		((B) == INVALID_VEC) ? (C)->num_cols : (B)->num_cols, \
		((C) == INVALID_VEC) ? (A)->num_rows : (C)->num_rows, \
		((C) == INVALID_VEC) ? (B)->num_cols : (C)->num_cols \
	)

/**
 *  \brief Matrix multiply operand dimensions don't match.
 *
 *  This macro is a high-level object wrapper around an equivalent
 *  call to #OSKI_ERR_DIM_MISMATCH(). The operands to this macro
 *  are assumed to be pointers to structs containing 'num_rows'
 *  and 'num_cols' fields.
 *
 *  A must be a valid object. However, either B or C may be NULL,
 *  in which case the missing argument is assumed to have the
 *  correct dimensions. B and C must not both be NULL, however.
 */
#define OSKI_ERR_DIM_MISMATCH_OBJ(method, A, B, C) \
	OSKI_ERR_DIM_MISMATCH_OBJ_MS( MACRO_TO_STRING(method), A, B, C )

/**
 *  \brief Matrix multiply operand dimensions don't match.
 *
 *  This macro is a high-level object wrapper around an equivalent
 *  call to #OSKI_ERR_DIM_MISMATCH(). The operands to this macro
 *  are assumed to be pointers to structs containing 'num_rows'
 *  and 'num_cols' fields.
 *
 *  A must be a valid object. However, either B or C may be NULL,
 *  in which case the missing argument is assumed to have the
 *  correct dimensions. B and C must not both be NULL, however.
 */
#define OSKI_ERR_DIM_MISMATCH_TRANS_OBJ_MS(method, A, B, C) \
	OSKI_ERR_DIM_MISMATCH_TRANS_MS(method, (A)->num_rows, (A)->num_cols, \
		((B) == INVALID_VEC) ? (A)->num_rows : (B)->num_rows, \
		((B) == INVALID_VEC) ? (C)->num_cols : (B)->num_cols, \
		((C) == INVALID_VEC) ? (A)->num_cols : (C)->num_rows, \
		((C) == INVALID_VEC) ? (B)->num_cols : (C)->num_cols \
	)

/**
 *  \brief Matrix multiply operand dimensions don't match.
 *
 *  This macro is a high-level object wrapper around an equivalent
 *  call to #OSKI_ERR_DIM_MISMATCH(). The operands to this macro
 *  are assumed to be pointers to structs containing 'num_rows'
 *  and 'num_cols' fields.
 *
 *  A must be a valid object. However, either B or C may be NULL,
 *  in which case the missing argument is assumed to have the
 *  correct dimensions. B and C must not both be NULL, however.
 */
#define OSKI_ERR_DIM_MISMATCH_TRANS_OBJ(method, A, B, C) \
	OSKI_ERR_DIM_MISMATCH_TRANS_OBJ_MS( MACRO_TO_STRING(method), A, B, C )

/**
 *  \brief NULL pointer argument.
 *
 *  \param[in] method Name of the calling routine.
 *  \param[in] argnum Integer argument number (1-based).
 *  \param[in] argname Short string description of the argument.
 */
#define OSKI_ERR_NULL_ARG_MS(method, argnum, argname) \
	oski_HandleError( ERR_BAD_ARG, \
		"Parameter should not equal NULL", __FILE__, __LINE__, \
		"Please check parameter #%d (%s) passed to %s()", \
		(int)(argnum), (const char *)((argname) ? (argname) : "?"), method \
	)

/** \brief NULL pointer argument. */
#define OSKI_ERR_NULL_ARG( method, argnum, argname ) \
	OSKI_ERR_NULL_ARG_MS( MACRO_TO_STRING(method), argnum, argname )

/**
 *  \brief Argument was too small.
 *
 *  \param[in] method Name of the calling routine.
 *  \param[in] min_val Minimum value possible.
 *  \param[in] argnum Integer argument number (1-based).
 *  \param[in] argname Short string description of the argument.
 */
#define OSKI_ERR_SMALL_ARG_MS(method, argnum, argname, min_val) \
	oski_HandleError( ERR_BAD_ARG, \
		((min_val) == 0 ? "Parameter must be non-negative" \
			: (min_val) == 1 ? "Parameter must be positive" \
			: "Parameter value too small"), \
		__FILE__, __LINE__, \
		"Please check parameter #%d (%s) passed to %s()", \
		(int)(argnum), (const char *)((argname) ? (argname) : "?"), method \
	)

/** \brief Argument was too small. */
#define OSKI_ERR_SMALL_ARG( method, argnum, argname, min_val ) \
	OSKI_ERR_SMALL_ARG_MS(MACRO_TO_STRING(method), argnum, argname, min_val)

/**
 *  \brief Stride was too small.
 *  
 *  \param[in] method Name of the calling routine.
 *  \param[in] stride Value of stride specified.
 *  \param[in] min_val Minimum possible stride value.
 *  \param[in] argnum Integer argument number (1-based).
 */
#define OSKI_ERR_BAD_STRIDE( method, argnum, stride, min_val ) \
	oski_HandleError( ERR_BAD_LEADDIM, \
		"Illegal leading dimension (stride) specified", __FILE__, __LINE__, \
		"Stride passed to %s() equals %d, but should be at least %d", \
		MACRO_TO_STRING(method), (int)stride, (int)min_val \
	)

/**
 *  \brief Invalid matrix object.
 *
 *  \param[in] method Name of the calling routine, as a string.
 *  \param[in] argnum Integer argument number (1-based).
 */
#define OSKI_ERR_BAD_MAT_MS( method, argnum ) \
	oski_HandleError( ERR_BAD_MAT, \
		"Invalid matrix handle", __FILE__, __LINE__, \
		"Please check matrix object, parameter #%d in call to %s()", \
		(int)(argnum), (method) \
	)

/**
 *  \brief Invalid matrix object.
 *
 *  \param[in] method Name of the calling routine.
 *  \param[in] argnum Integer argument number (1-based).
 */
#define OSKI_ERR_BAD_MAT( method, argnum ) \
	OSKI_ERR_BAD_MAT_MS( MACRO_TO_STRING(method), argnum )

/**
 *  \brief Matrix object has no representations (i.e., no input matrix
 *  and no tuned matrix).
 */
#define OSKI_ERR_NO_REPR( method, argnum ) \
	oski_HandleError( ERR_BAD_ARG, \
		"Matrix handle does not have any representations", __FILE__, __LINE__, \
		"Please check parameter #%d in call to %s()", \
		(int)(argnum), MACRO_TO_STRING(method) \
	)

/**
 *  \brief Invalid permutation matrix object.
 *
 *  \param[in] method Name of the calling routine.
 *  \param[in] argnum Integer argument number (1-based).
 */
#define OSKI_ERR_BAD_PERM( method, argnum ) \
	oski_HandleError( ERR_BAD_MAT, \
		"Invalid permutation matrix object", __FILE__, __LINE__, \
		"Please check permutation object, parameter #%d in call to %s()", \
		(int)(argnum), MACRO_TO_STRING(method) \
	)

/**
 *  \brief Non-square matrix received where a square one is expected.
 *
 *  \param[in] method Name of the calling routine.
 *  \param[in] argnum Integer argnument number (1-based).
 */
#define OSKI_ERR_MAT_NOTSQUARE_MS( method, argnum ) \
	oski_HandleError( ERR_BAD_MAT, \
		"Expected a square input matrix object", __FILE__, __LINE__, \
		"Please check parameter #%d in call to %s()", \
		(int)(argnum), (method) \
	)

/**
 *  \brief Non-square matrix received where a square one is expected.
 *
 *  \param[in] method Name of the calling routine.
 *  \param[in] argnum Integer argnument number (1-based).
 */
#define OSKI_ERR_MAT_NOTSQUARE( method, argnum ) \
	OSKI_ERR_MAT_NOTSQUARE_MS( MACRO_TO_STRING(method), argnum )

/**
 *  \brief Invalid vector view object.
 *
 *  \param[in] method Name of the calling routine, as a string.
 *  \param[in] argnum Integer argument number (1-based).
 */
#define OSKI_ERR_BAD_VEC_MS( method, argnum ) \
	oski_HandleError( ERR_BAD_VECVIEW, \
		"Invalid vector view object", __FILE__, __LINE__, \
		"Please check vector view object, parameter #%d in call to %s()", \
		(int)(argnum), (method) \
	)

/**
 *  \brief Invalid vector view object.
 *
 *  \param[in] method Name of the calling routine.
 *  \param[in] argnum Integer argument number (1-based).
 */
#define OSKI_ERR_BAD_VEC( method, argnum ) \
	OSKI_ERR_BAD_VEC_MS( MACRO_TO_STRING(method), argnum )

/**
 *  \brief Computed the wrong answer.
 */
#define OSKI_ERR_WRONG_ANS_ELEM( method, x_true, x_calc, index ) \
	oski_HandleError( ERR_WRONG_ANS, \
		"Mistake detected in computation", __FILE__, __LINE__, \
		"Please check array element %d: " \
			"computed %g but expected %g.", index, x_calc, x_true \
	)

/**
 *  \brief Computed the wrong answer (complex version).
 */
#define OSKI_ERR_WRONG_ANS_ELEM_COMPLEX( method, x_true, x_calc, index ) \
	oski_HandleError( ERR_WRONG_ANS, \
		"Mistake detected in computation", __FILE__, __LINE__, \
		"Please check array element %d: " \
			"computed %g+%gi but expected %g+%gi.", \
		index, _RE(x_calc), _IM(x_calc), _RE(x_true), _IM(x_true) \
	)

/**
 *  \brief Caller specified an unrecognized hint.
 */
#define OSKI_ERR_BAD_HINT( method, argnum, hint ) \
	oski_HandleError( ERR_BAD_HINT_ARG, \
		"An unrecognized hint was specified", __FILE__, __LINE__, \
		"Please check the value (%d) of the hint " \
	   		"passed as argument %d in call to %s().", \
		(int)(hint), (int)(argnum), MACRO_TO_STRING(method) \
	)

/**
 *  \brief Caller specified a bad argument to a hint.
 */
#define OSKI_ERR_BAD_HINT_ARG( method, argnum, hint, int_val ) \
	oski_HandleError( ERR_BAD_HINT_ARG, \
		"An invalid hint parameter was specified", __FILE__, __LINE__, \
		"Please check the parameter value %d (=%d) passed to the" \
			" structural hint '%s' in call to %s().", \
		(int)(argnum), (int)(int_val), MACRO_TO_STRING(hint), \
		MACRO_TO_STRING(method) \
	)

/**
 *  \brief Caller provided vector views with incompatible numbers
 *  of dimensions.
 */
#define OSKI_ERR_INCOMPAT_NUMVECS_MS( method, argnum_x, nv_x, argnum_y, nv_y ) \
	oski_HandleError( ERR_DIM_MISMATCH, \
		"Vector views have incompatible dimensions", __FILE__, __LINE__, \
		"In call to %s(), vector operands, parameters #%d and #%d," \
			" should have same number of vectors but don't" \
			" (%d and %d, respectively)", \
		(method), (int)(argnum_x), (int)(argnum_y), \
		(int)(nv_x), (int)(nv_y) \
	)

/**
 *  \brief Caller provided vector views with incompatible numbers
 *  of dimensions.
 */
#define OSKI_ERR_INCOMPAT_NUMVECS( method, argnum_x, nv_x, argnum_y, nv_y ) \
	OSKI_ERR_INCOMPAT_NUMVECS_MS( MACRO_TO_STRING(method), \
		argnum_x, nv_x, argnum_y, nv_y )

/** \brief Caller provides method name and string description of the
 *  heuristic whose results were being applied.
 */
#define OSKI_ERR_NO_APPLYHEUR( method, heur_desc ) \
	oski_HandleError( ERR_CANT_CONVERT, \
		"Can't convert to new data structure", __FILE__, __LINE__, \
		"In call to %s(), can't apply results of heuristic '%s'", \
		MACRO_TO_STRING(method), (heur_desc) \
	)

/** \brief Can't open the requested input file. */
#define OSKI_ERR_FILE_READ( method, filename ) \
	oski_HandleError( ERR_FILE_READ, \
		"Can't read a file", __FILE__, __LINE__, \
		"In call to %s(): Please check that the requested file" \
			", '%s', exists and is properly formatted.", \
		MACRO_TO_STRING(method), filename )

/** \brief Invalid row or column specified. */
#define OSKI_ERR_BAD_ENTRY( method, argnum, i, j, m, n ) \
	oski_HandleError( ERR_BAD_ARG, \
		"Caller specified an invalid matrix entry", __FILE__, __LINE__, \
		"Parameter #%d in call to %s(): The entry (%d, %d) is invalid." \
			" (The row/column indices must be at least 1 and less than the" \
			" matrix dimensions, %d x %d.)", \
		(int)(argnum), MACRO_TO_STRING(method), (int)(i), (int)(j), \
		(int)(m), (int)(n) \
	)

/** \brief Attempt to change the unit diagonal */
#define OSKI_ERR_MOD_UNIT_DIAG_REAL( method, row, col, val ) \
	oski_HandleError( ERR_INMATPROP_CONFLICT, \
		"Attempt to change the unit diagonal", __FILE__, __LINE__, \
		"In %s(): Attempt to change the (%d, %d) entry to %g", \
		MACRO_TO_STRING(method), (int)(row), (int)(col), \
		(double)(val) \
	)

/** \brief Attempt to change the unit diagonal */
#define OSKI_ERR_MOD_UNIT_DIAG_COMPLEX( method, row, col, val ) \
	oski_HandleError( ERR_INMATPROP_CONFLICT, \
		"Attempt to change the unit diagonal", __FILE__, __LINE__, \
		"In %s(): Attempt to change the (%d, %d) entry to %g+%gi", \
		MACRO_TO_STRING(method), (int)(row), (int)(col), \
		(double)_RE(val), (double)_IM(val) \
	)

/** \brief Attempt to change wrong triangle */
#define OSKI_ERR_MOD_TRI_REAL( method, is_upper, row, col, val ) \
	oski_HandleError( ERR_ZERO_ENTRY, \
		"Attempt to change zero part of triangular matrix", \
		__FILE__, __LINE__, \
		"In %s(): Tried to change (%d, %d) entry of a %s-triangular" \
			" matrix to %g", \
		MACRO_TO_STRING(method), (int)(row), (int)(col), \
		(is_upper) ? "upper" : "lower", (double)(val) \
	)

/** \brief Attempt to change wrong triangle */
#define OSKI_ERR_MOD_TRI_COMPLEX( method, is_upper, row, col, val ) \
	oski_HandleError( ERR_ZERO_ENTRY, \
		"Attempt to change zero part of triangular matrix", \
		__FILE__, __LINE__, \
		"In %s(): Tried to change (%d, %d) entry of a %s-triangular" \
			" matrix to %g+%gi", \
		MACRO_TO_STRING(method), (int)(row), (int)(col), \
		(is_upper) ? "upper" : "lower", (double)_RE(val), (double)_IM(val) \
	)

/** \brief Attempt to change a zero entry */
#define OSKI_ERR_ZERO_ENTRY_REAL( method, row, col, val ) \
	oski_HandleError( ERR_ZERO_ENTRY, \
		"Attempt to change an implicit zero entry", __FILE__, __LINE__, \
		"In %s(): Tried to change implicit entry (%d, %d) to %g", \
		MACRO_TO_STRING(method), (int)(row), (int)(col), (double)(val) \
	)

/** \brief Attempt to change a zero entry */
#define OSKI_ERR_ZERO_ENTRY_COMPLEX( method, row, col, val ) \
	oski_HandleError( ERR_ZERO_ENTRY, \
		"Attempt to change an implicit zero entry", __FILE__, __LINE__, \
		"In %s(): Tried to change implicit entry (%d, %d) to %g+%gi", \
		MACRO_TO_STRING(method), (int)(row), (int)(col), \
		(double)_RE(val), (double)_IM(val) \
	)

/** \brief Invalid row index specified. */
#define OSKI_ERR_BAD_ROW_MS( method, argnum, row, min, max ) \
	oski_HandleError( ERR_BAD_ENTRY, \
		"Caller specified an invalid row index", __FILE__, __LINE__, \
		"In %s(): The row index %d, given in parameter %d, is out of" \
			" the range [%d, %d]", \
		method, (int)(row), (int)(argnum), (int)(min), (int)(max) \
	)

/** \brief Invalid row index specified. */
#define OSKI_ERR_BAD_ROW( method, argnum, row, min, max ) \
	OSKI_ERR_BAD_ROW_MS( MACRO_TO_STRING(method), argnum, row, min, max )

/** \brief Invalid column index specified. */
#define OSKI_ERR_BAD_COL_MS( method, argnum, col, min, max ) \
	oski_HandleError( ERR_BAD_ENTRY, \
		"Caller specified an invalid column index", __FILE__, __LINE__, \
		"In %s(): The column index %d, given in parameter %d, is out of" \
			" the range [%d, %d]", \
		method, (int)(col), (int)(argnum), (int)(min), (int)(max) \
	)

/** \brief Invalid row index specified. */
#define OSKI_ERR_BAD_COL( method, argnum, col, min, max ) \
	OSKI_ERR_BAD_COL_MS( MACRO_TO_STRING(method), argnum, col, min, max )

/** \brief Matrix holding clique values is too small. */
#define OSKI_ERR_CLIQUE_VALMAT_SMALL_MS( method, argnum, V, r, c ) \
	oski_HandleError( ERR_BAD_VECVIEW, \
		"The array holding clique values is too small", __FILE__, __LINE__, \
		"In %s(): The array holding clique values (parameter #%d)" \
			" is %d x %d, but should be at least %d x %d", \
		method, (int)(argnum), \
		(int)((V)->num_rows), (int)((V)->num_cols), (int)(r), (int)(c) \
	)

/** \brief Matrix holding clique values is too small. */
#define OSKI_ERR_CLIQUE_VALMAT_SMALL( method, argnum, V, r, c ) \
	OSKI_ERR_CLIQUE_VALMAT_SMALL( MACRO_TO_STRING(method), argnum, V, r, c )

/** \brief Invalid diagonal specified. */
#define OSKI_ERR_BAD_DIAG_MS( method, argnum, d, m, n ) \
	oski_HandleError( ERR_BAD_ARG, \
		"The specified diagonal does not exist", __FILE__, __LINE__, \
		"In %s(): The specified diagonal (parameter #%d), %d, is not" \
	   		" in the proper range of [%d, %d] for an %d x %d matrix", \
		method, (int)(argnum), (int)(d), \
			(int)(1-(m)), (int)((n)-1), (m), (n) \
	)

/** \brief Invalid diagonal specified. */
#define OSKI_ERR_BAD_DIAG( method, argnum, d, m, n ) \
	OSKI_ERR_BAD_DIAG_MS( MACRO_TO_STRING(method), (argnum), (d), (m), (n) )

/** \brief Matrix holding clique values is too small. */
#define OSKI_ERR_DIAG_VALMAT_SMALL_MS( method, argnum, V, r, c ) \
	oski_HandleError( ERR_BAD_VECVIEW, \
		"The array to hold the diagonal values is too small", \
			__FILE__, __LINE__, \
		"In %s(): The array to hold the diagonal values (parameter #%d)" \
			" is %d x %d, but should be at least %d x %d", \
		method, (int)(argnum), \
		(int)((V)->num_rows), (int)((V)->num_cols), (int)(r), (int)(c) \
	)

/** \brief Matrix holding clique values is too small. */
#define OSKI_ERR_DIAG_VALMAT_SMALL( method, argnum, V, r, c ) \
	OSKI_ERR_DIAG_VALMAT_SMALL( MACRO_TO_STRING(method), argnum, V, r, c )

/** \brief Input matrix is not triangular. */
#define OSKI_ERR_MAT_NOTTRI_MS( method, argnum ) \
	oski_HandleError( ERR_BAD_MAT, \
		"The matrix may not be triangular", __FILE__, __LINE__, \
		"Please check parameter #%d in the call to %s().", \
		(int)(argnum), (method) \
	)

/** \brief Input matrix is not triangular. */
#define OSKI_ERR_MAT_NOTTRI( method, argnum ) \
	OSKI_ERR_MAT_NOTTRI_MS( MACRO_TO_STRING(method), argnum )

/** \brief Matrix power requested is invalid. */
#define OSKI_ERR_BAD_POWER_MS( method, argnum, power ) \
	oski_HandleError( ERR_BAD_ARG, \
		"Matrix power must be non-negative", __FILE__, __LINE__, \
		"Please check paramter #%d (power == %d) in the call to %s().", \
		(int)(argnum), (int)(power), (method) \
	)

/** \brief Matrix power requested is invalid. */
#define OSKI_ERR_BAD_POWER( method, argnum, power ) \
	OSKI_ERR_BAD_POWER_MS( MACRO_TO_STRING(method), argnum, power )

/** \brief Multiple vectors provided where a single vector is expected. */
#define OSKI_ERR_VEC1_ONLY( method, argnum, num_vecs ) \
	oski_HandleError( ERR_BAD_VECVIEW, \
		"Only single-vectors supported", \
		"Please check parameter #%d in call to %s(): " \
			"Caller passed in a multivector object (%d vectors) " \
			"where a single vector was expected.", \
			(int)(argnum), (method), (num_vecs) \
	)

/** \brief Multiple vectors provided where a single vector is expected. */
#define OSKI_ERR_VEC1_ONLY_MS( method, argnum, num_vecs ) \
	OSKI_ERR_VEC1_ONLY( MACRO_TO_STRING(method), argnum, num_vecs )

/** \brief Too few vectors provided */
#define OSKI_ERR_VEC_FEW_MS( method, argnum, num_vecs, num_needed ) \
	oski_HandleError( ERR_BAD_VECVIEW, \
		"Too few vectors provided", \
		"Please check parameter #%d in call to %s(): " \
			"Caller passed a %d-vector object, but at least %d required.", \
		(int)(argnum), (method), (int)(num_vecs), (int)(num_needed) \
	)

/** \brief Too few vectors provided */
#define OSKI_ERR_VEC_FEW( method, argnum, num_vecs, num_needed ) \
	OSKI_ERR_VEC_FEW_MS( MACRO_TO_STRING(method), \
		argnum, num_vecs, num_needed )

/*@}*/

#endif /* !defined(INC_BEBOP_OSKI_ERROR_H) */

/* eof */
