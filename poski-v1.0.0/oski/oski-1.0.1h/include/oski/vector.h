/**
 *  \file oski/vector.h
 *  \brief Multivector view implementation.
 */

#if !defined(INC_OSKI_VECTOR_H) && !defined(OSKI_UNBIND)
/** oski/vector.h included. */
#define INC_OSKI_VECTOR_H

#include <oski/mangle.h>

#if defined(DO_MANGLE_NAMES)
/** \name Name mangling. */
/*@{*/
#define oski_vecstruct_t  MANGLE_(oski_vecstruct_t)
/*@}*/
#endif

/**
 *  \brief Multivector view structure.
 *
 *  A multivector view is a wrapper around a conventional
 *  linearized dense matrix representation. The view
 *  abstracts these implementation details:
 *    - Row vs. column major layout
 *    - Leading dimension (stride)
 *
 *  In addition, we maintain two redundant parameters: the
 *  row element distance and column element distance. The
 *  row element distance is the number of array elements
 *  between A(i, j) and A(i+1, j). Similarly, the column
 *  element distance is the distance between A(i, j) and
 *  A(i, j+1).
 *
 *  For instance, a matrix stored in row major layout with
 *  stride L has a row distance of L and a column distance
 *  of 1.
 */
typedef struct
{
  oski_index_t num_rows;	/**< Number of logical rows */
  oski_index_t num_cols;	/**< Number of logical columns */

  oski_storage_t orient;	/**< Row- or column-major layout */
  oski_index_t stride;		/**< Leading dimension */

  oski_index_t rowinc;	      /**< Distance in elements between rows */
  oski_index_t colinc;	      /**< Distance in elements between columns */

  oski_value_t *val;		/**< Matrix storage */
}
oski_vecstruct_t;

/** Macro for initializing a declared #oski_vecstruct_t variable. */
#define MAKE_VECSTRUCT_INIT(m, n, orient, stride, rowinc, colinc, val) \
	{m, n, orient, stride, rowinc, colinc, val}

/**
 *  \brief Get the (i, j) element from an array v stored in
 *  row-major layout with stride s.
 *
 *  \param[in] v Raw vector array storage.
 *  \param[in] i Row index (zero-based).
 *  \param[in] j Column index (zero-based).
 *  \param[in] s Stride (leading-dimension).
 *  \returns The logical (i, j) element of v when v is interpreted
 *  as a 2-D array stored in row-major layout.
 */
#define VECRM_GET(v, i, j, s)   (v)[(i)*(s) + (j)]

/**
 *  \brief Get the (i, j) element from an array v stored in
 *  column-major layout with stride s.
 *
 *  \param[in] v Raw vector array storage.
 *  \param[in] i Row index (zero-based).
 *  \param[in] j Column index (zero-based).
 *  \param[in] s Stride (leading-dimension).
 *  \returns The logical (i, j) element of v when v is interpreted
 *  as a 2-D array stored in column-major layout.
 */
#define VECCM_GET(v, i, j, s)   (v)[(i) + (j)*(s)]

/**
 *  \brief Get the (i, j) element of an arbitrary vector view.
 *
 *  \param[in] x Vector view object (handle).
 *  \param[in] i Row index (zero-based).
 *  \param[in] j Column index (zero-based).
 *  \returns The logical (i, j) element of x.
 */
#define VECVIEW_GET(x, i, j)  \
	(((x)->orient == LAYOUT_ROWMAJ) \
	 	? VECRM_GET((x)->val, i, j, (x)->stride) \
	 	: VECCM_GET((x)->val, i, j, (x)->stride) \
	)

/** \brief Returns 1 if x is a column vector. */
#define IS_VECVIEW_COLVEC(x)  ((x)->num_cols == 1)

/** \brief Returns 1 if x is a unit stride column vector. */
#define IS_VECVIEW_COLVEC_STRIDE1(x) \
	( \
	 	IS_VECVIEW_COLVEC(x) \
		&& ( \
			(((x)->orient == LAYOUT_ROWMAJ) && ((x)->stride == 1)) \
			|| (((x)->orient == LAYOUT_COLMAJ)) \
		) \
	)

#endif /* !defined(INC_OSKI_VECTOR_H) */

#if defined(OSKI_UNBIND)
#  include <oski/mangle.h>
#  undef oski_vecstruct_t
#  undef MAKE_VECSTRUCT_INIT
#  undef VECRM_GET
#  undef VECCM_GET
#  undef VECVIEW_GET
#  undef IS_VECVIEW_COLVEC
#  undef IS_VECVIEW_COLVEC_STRIDE1
#endif

/* eof */
