/**
 *  \defgroup PERM Permutations
 *  \ingroup TUNING
 *
 *  These routines allow the user to access underlying matrix row
 *  and column permutations, as well as a permuted form of the
 *  matrix, in the special case that tuning produces a permuted
 *  representation.
 */

/**
 *  \file oski/perm.h
 *  \brief Permutations.
 *  \ingroup PERM
 */

#if !defined(INC_OSKI_PERM_H) && !defined(OSKI_UNBIND)
/** oski/perm.h included. */
#define INC_OSKI_PERM_H

#include <oski/common.h>
#include <oski/mangle.h>
#include <oski/matrix.h>
#include <oski/vecview.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_permstruct_t MANGLE_(oski_permstruct_t)
#define oski_perm_t MANGLE_(oski_perm_t)
#define const_oski_perm_t MANGLE_(const_oski_perm_t)

#define PERM_IDENTITY MANGLE_(PERM_IDENTITY)

#define oski_IsMatPermuted MANGLE_(oski_IsMatPermuted)
#define oski_ViewPermutedMat MANGLE_(oski_ViewPermutedMat)
#define oski_ViewPermutedMatRowPerm MANGLE_(oski_ViewPermutedMatRowPerm)
#define oski_ViewPermutedMatColPerm MANGLE_(oski_ViewPermutedMatColPerm)
#define oski_PermuteVecView MANGLE_(oski_PermuteVecView)
/*@}*/
#endif

/**
 *  \brief Generic row/column permutation structure.
 *  \ingroup PUBLIC
 *
 *  This data structure stores an \f$n\times n\f$ permutation
 *  matrix \f$P\f$ in an array 'perm' such that the operation
 *  \f$y \leftarrow Px\f$ is implemented by assigning
 *  y[i] = x[perm[i]] for 0 <= i < \f$n\f$.
 *
 *  The transpose operation \f$y \leftarrow P^Tx\f$ is
 *  implemented by assigning y[perm[i]] = x[i].
 */
typedef struct
{
  oski_index_t length;	       /**< \f$n\f$ */
  oski_index_t *perm;	       /**< \f$y\leftarrow Px\f$ ==> y[i]=x[perm[i]] */
}
oski_permstruct_t;

/** \brief Initialize a statically declared oski_permstruct_t object. */
#define MAKE_PERMSTRUCT_INIT(n,p)  {(n), (p)}

/**
 *  \brief Permutation handle.
 *  \ingroup PUBLIC
 */
typedef oski_permstruct_t *oski_perm_t;

/**
 *  \brief Read-only (const) permutation, for function return values.
 */
typedef const oski_permstruct_t *const_oski_perm_t;

/**
 *  \brief Checks whether a matrix has been tuned by reordering.
 *  \ingroup PUBLIC
 */
int oski_IsMatPermuted (const oski_matrix_t A_tunable);

/**
 *  \brief Given a matrix \f$A\f$, possibly reordered during tuning
 *  to the form \f$\hat{A} = P_r\cdot A\cdot P_c^T\f$, returns a
 *  read-only object corresponding to \f$\hat{A}\f$.
 *  \ingroup PUBLIC
 */
const_oski_matrix_t oski_ViewPermutedMat (const oski_matrix_t A_tunable);

/**
 *  \brief Given a matrix \f$A\f$, possible reordered during tuning
 *  to the form \f$\hat{A} = P_r\cdot A\cdot P_c^T\f$, returns a
 *  read-only object corresponding to \f$P_r\f$.
 *  \ingroup PUBLIC
 */
const_oski_perm_t oski_ViewPermutedMatRowPerm (const oski_matrix_t A_tunable);

/**
 *  \brief Given a matrix \f$A\f$, possible reordered during tuning
 *  to the form \f$\hat{A} = P_r\cdot A\cdot P_c^T\f$, returns a
 *  read-only object corresponding to \f$P_c\f$ (and NOT \f$P_c^T\f$).
 *  \ingroup PUBLIC
 */
const_oski_perm_t oski_ViewPermutedMatColPerm (const oski_matrix_t A_tunable);

/**
 *  \brief Permute a vector view object, i.e., computes
 *  \f$x \leftarrow \mathrm{op}(P)x\f$, where
 *  \f$\matrm{op}(A) \in \{A, A^T\}\f$.
 *  \ingroup PUBLIC
 */
int oski_PermuteVecView (const oski_perm_t P, oski_matop_t opP,
			 oski_vecview_t x_view);

extern const oski_perm_t PERM_IDENTITY;

#endif /* !defined(INC_OSKI_PERM_H) */

#if defined(OSKI_UNBIND)
#  include <oski/mangle.h>
#  include <oski/matrix.h>
#  include <oski/vecview.h>
#  undef INC_OSKI_PERM_H
#  undef oski_permstruct_t
#  undef oski_perm_t
#  undef const_oski_perm_t
#  undef PERM_IDENTITY
#  undef oski_IsMatPermuted
#  undef oski_ViewPermutedMat
#  undef oski_ViewPermutedMatRowPerm
#  undef oski_ViewPermutedMatColPerm
#  undef oski_PermuteVecView
#  undef MAKE_PERMSTRUCT_INIT
#endif

/* eof */
