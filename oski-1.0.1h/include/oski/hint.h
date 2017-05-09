/**
 *  \file oski/hint.h
 *  \brief Set-hint routines.
 *  \ingroup TUNING
 *
 *  Structural hints are stored in records defined by the
 *  #oski_structhint_t type, below, and stored with the
 *  matrix (see #oski_matstruct_t).
 *
 *  Workload hints are stored as (simulated) traces with the matrix
 *  (see #oski_matstruct_t).
 */

#if !defined(INC_OSKI_HINT_H) && !defined(OSKI_UNBIND)
/** oski/hint.h has been included. */
#define INC_OSKI_HINT_H

#include <oski/common.h>
#include <oski/matrix.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_SetHintMatMult  \
		MANGLE_(oski_SetHintMatMult)
#define oski_SetHintMatTrisolve  \
		MANGLE_(oski_SetHintMatTrisolve)
#define oski_SetHintMatMultAndMatTransMult  \
		MANGLE_(oski_SetHintMatMultAndMatTransMult)
#define oski_SetHintMatTransMatMult \
		MANGLE_(oski_SetHintMatTransMatMult)
#define oski_SetHintMatPowMult \
		MANGLE_(oski_SetHintMatPowMult)

#define oski_SetHint   MANGLE_(oski_SetHint)
/*@}*/
#endif

/* ------------------------------------------------------ */

/**
 *  \defgroup HINT Set explicit workload hints.
 *  \ingroup PUBLIC
 */
/*@{*/

/**
 *  \brief Set a matrix structural hint for subsequent tuning.
 */
int oski_SetHint (oski_matrix_t A_tunable, oski_tunehint_t hint, ...);

/**
 *  \brief Workload hint for the kernel operation #oski_MatMult.
 */
int oski_SetHintMatMult (oski_matrix_t A_tunable, oski_matop_t opA,
			 oski_value_t alpha, const oski_vecview_t x_view,
			 oski_value_t beta, const oski_vecview_t y_view,
			 int num_calls);

/**
 *  \brief Workload hint for the kernel operation #oski_MatTrisolve.
 */
int oski_SetHintMatTrisolve (oski_matrix_t T_tunable,
			     oski_matop_t opT,
			     oski_value_t alpha, const oski_vecview_t x_view,
			     int num_calls);

/**
 *  \brief Workload hint for the kernel operation #oski_MatTransMatMult.
 */
int oski_SetHintMatTransMatMult (oski_matrix_t A_tunable,
				 oski_ataop_t opA,
				 oski_value_t alpha,
				 const oski_vecview_t x_view,
				 oski_value_t beta,
				 const oski_vecview_t y_view,
				 const oski_vecview_t t_view, int num_calls);

/**
 *  \brief Workload hint for the kernel operation
 *  #oski_MatMultAndMatTransMult.
 */
int oski_SetHintMatMultAndMatTransMult (oski_matrix_t A_tunable,
					oski_value_t alpha,
					const oski_vecview_t x_view,
					oski_value_t beta,
					const oski_vecview_t y_view,
					oski_matop_t opA, oski_value_t omega,
					const oski_vecview_t w_view,
					oski_value_t zeta,
					const oski_vecview_t z_view,
					int num_calls);

/**
 *  \brief Workload hint for the kernel operation #oski_MatPowMult.
 */
int oski_SetHintMatPowMult (oski_matrix_t A_tunable,
			    oski_matop_t opA, int power,
			    oski_value_t alpha, const oski_vecview_t x_view,
			    oski_value_t beta, const oski_vecview_t y_view,
			    const oski_vecview_t T_view, int num_calls);
/*@}*/

#endif /* !defined(INC_OSKI_HINT_H) */

#if defined(OSKI_UNBIND)
#  include <oski/matrix.h>
#  undef INC_OSKI_HINT_H
#  undef oski_SetHintMatMult
#  undef oski_SetHintMatTrisolve
#  undef oski_SetHintMatMultAndMatTransMult
#  undef oski_SetHintMatTransMatMult
#  undef oski_SetHintMatPowMult
#  undef oski_SetHint
#endif

/* eof */
