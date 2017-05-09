/**
 *  \file src/getset.c
 *  \brief Get/set value routines.
 */

#include <oski/common.h>
#include <oski/config.h>
#include <oski/mangle.h>
#include <oski/matrix.h>
#include <oski/getset.h>

/** Returns the smallest of two values. */
#define MIN(i,j)  ((i) < (j) ? (i) : (j))
/** Returns the largest of two values. */
#define MAX(i,j)  ((i) > (j) ? (i) : (j))

/* ----------------------------------------------------------------
 * Points (individual elements)
 */

/**
 *  \brief Calls matrix type-specific implementation of
 *  oski_GetMatEntry().
 *
 *  \param[in] mat Type-specific data structure.
 *  \param[in] props Type-independent properties.
 *  \param[in] row Row of entry to retrieve (1-based).
 *  \param[in] col Column of entry to retrieve (1-based).
 *  \param[in,out] p_aij Pointer to buffer into which the (row, col)
 *  entry is returned.
 *
 *  \returns On success, sets *p_aij to
 *  \f$a_{\mathrm{row}, \mathrm{col}}\f$ and returns 0. Returns an
 *  error code otherwise. This routine calls the error handler unless
 *  the error that there is no matrix type-specific implementation.
 *  (In this case, the caller has an opportunity to try calling the
 *  type-specific implementation of another representation.)
 */
static int
GetMatEntry (const oski_matspecific_t * mat, const oski_matcommon_t * props,
	     oski_index_t row, oski_index_t col, oski_value_t * p_aij)
{
  oski_GetMatReprEntry_funcpt func_Get;

  func_Get = OSKI_MATTYPEID_METHOD (mat->type_id, GetMatReprEntry);
  if (func_Get == NULL)
    return ERR_NOT_IMPLEMENTED;

  return (*func_Get) (mat->repr, props, row, col, p_aij);
}

/**
 *  \brief Calls matrix type-specific implementation of
 *  oski_SetMatEntry().
 *
 *  \param[in] mat Type-specific data structure.
 *  \param[in] props Type-independent properties.
 *  \param[in] row Row of entry to set (1-based).
 *  \param[in] col Column of entry to set (1-based).
 *  \param[in] new_val New value for the specified entry.
 *
 *  \returns On success, sets \f$a_{\mathrm{row}, \mathrm{col}}\f$
 *  to new_val and returns 0. This routine returns an error code
 *  otherwise, and calls the error handler unless 'mat' has an
 *  invalid type id.
 */
static int
SetMatEntry (const oski_matspecific_t * mat, const oski_matcommon_t * props,
	     oski_index_t row, oski_index_t col, oski_value_t new_val)
{
  oski_SetMatReprEntry_funcpt func_Set;

  if (mat->type_id == INVALID_ID)
    return ERR_BAD_ID;

  func_Set = OSKI_MATTYPEID_METHOD (mat->type_id, SetMatReprEntry);
  if (func_Set == NULL)
    {
      OSKI_ERR_MATTYPEID_MEMBER (oski_SetMatEntry, mat->type_id,
				 SetMatReprEntry);
      return ERR_NOT_IMPLEMENTED;
    }

  return (*func_Set) (mat->repr, props, row, col, new_val);
}

/**
 *  \brief
 *
 *  \param[in] A_tunable The object representing an \f$m\times n\f$
 *  matrix \f$A\f$.
 *  \param[in] row Row index \f$i : 1 \leq i \leq m\f$ (1-based).
 *  \param[in] col Column index \f$j : 1 \leq j \leq n\f$ (1-based).
 *  \returns The element \f$A(i,j)\f$, or a NaN on error.
 */
oski_value_t
oski_GetMatEntry (const oski_matrix_t A_tunable,
		  oski_index_t row, oski_index_t col)
{
  oski_index_t m, n;		/* matrix dimensions */
  oski_value_t a_ij;		/* value */
  int err;

  m = A_tunable->props.num_rows;
  n = A_tunable->props.num_cols;
  VAL_SET_ZERO (a_ij);

  /* error checking */
  if (A_tunable == INVALID_MAT)
    {
      OSKI_ERR_BAD_MAT (oski_GetMatEntry, 1);
      return a_ij;
    }

  if (row <= 0 || row > m)
    {
      OSKI_ERR_BAD_ENTRY (oski_GetMatEntry, 2, row, col, m, n);
      return a_ij;
    }

  if (col <= 0 || col > n)
    {
      OSKI_ERR_BAD_ENTRY (oski_GetMatEntry, 3, row, col, m, n);
      return a_ij;
    }

  err = GetMatEntry (&(A_tunable->input_mat), &(A_tunable->props),
		     row, col, &a_ij);
  if (!err)
    return a_ij;

  err = GetMatEntry (&(A_tunable->tuned_mat), &(A_tunable->props),
		     row, col, &a_ij);
  if (!err)
    return a_ij;

  if (err == ERR_NOT_IMPLEMENTED)
    {
      oski_id_t id;

      if (A_tunable->input_mat.type_id != INVALID_ID)
	id = A_tunable->input_mat.type_id;
      else
	id = A_tunable->tuned_mat.type_id;

      OSKI_ERR_MATTYPEID_MEMBER (oski_GetMatEntry, id, GetMatReprEntry);
    }

  return a_ij;
}

/**
 *  \brief
 *
 *  \param[in] A_tunable The object representing an \f$m\times n\f$
 *  matrix \f$A\f$.
 *  \param[in] row Row index \f$i : 1 \leq i \leq m\f$ (1-based).
 *  \param[in] col Column index \f$j : 1 \leq j \leq n\f$ (1-based).
 *  \param[in] new_val New value for element \f$A(i,j)\f$.
 *  \returns Assigns logical element \f$A(i,j)\f$ to new_val and
 *  returns 0. If \f$A(i,j)\f$ specifies an invalid or structural
 *  zero element, then this routine returns an error code.
 *
 *  If the matrix is symmetric (or Hermitian), then this routine
 *  also assigns \f$A(j,i)\f$ to the new value (or conjugated
 *  value) accordingly.
 */
int
oski_SetMatEntry (const oski_matrix_t A_tunable,
		  oski_index_t row, oski_index_t col, oski_value_t new_val)
{
  oski_index_t m, n;		/* matrix dimensions */
  int err_input, err_tuned;

  m = A_tunable->props.num_rows;
  n = A_tunable->props.num_cols;

  /* error checking */
  if (A_tunable == INVALID_MAT)
    {
      OSKI_ERR_BAD_MAT (oski_SetMatEntry, 1);
      return ERR_BAD_ARG;
    }

  if (row <= 0 || row > m)
    {
      OSKI_ERR_BAD_ENTRY (oski_SetMatEntry, 2, row, col, m, n);
      return ERR_BAD_ARG;
    }

  if (col <= 0 || col > n)
    {
      OSKI_ERR_BAD_ENTRY (oski_SetMatEntry, 3, row, col, m, n);
      return ERR_BAD_ARG;
    }

  err_input = SetMatEntry (&(A_tunable->input_mat), &(A_tunable->props),
			   row, col, new_val);
  if (err_input != ERR_BAD_ID)
    return err_input;

  err_tuned = SetMatEntry (&(A_tunable->tuned_mat), &(A_tunable->props),
			   row, col, new_val);
  if (err_tuned != ERR_BAD_ID)
    return err_tuned;

  if (err_input == ERR_BAD_ID && err_tuned == ERR_BAD_ID)
    {
      OSKI_ERR_NO_REPR (oski_SetMatEntry, 1);
      return ERR_BAD_ARG;
    }

  return err_input;
}

/**
 *  \brief Returns the index of any element not in the specified
 *  closed interval, or -1 if all elements are in the interval.
 *  The index returned is 0-based.
 */
static int
FindIndexOutOfRange (const oski_index_t * x, oski_index_t n,
		     oski_index_t min, oski_index_t max)
{
  oski_index_t i;
  for (i = 0; i < n; i++)
    if (x[i] < min || x[i] > max)
      return i;
  return -1;
}

/**
 *  \brief Returns the largest value in the specified array.
 *
 *  \pre n >= 1 && x != NULL
 */
static int
GetMaxIndex (const oski_index_t * x, oski_index_t n)
{
  oski_index_t i;
  oski_index_t max = x[0];
  for (i = 1; i < n; i++)
    if (x[i] > max)
      max = x[i];
  return max;
}

/**
 *  \brief Checks the input arguments to oski_GetMatClique()
 *  and oski_SetMatClique(), returning 0 if the arguments are
 *  valid or an error code otherwise. This routine calls the
 *  error handler on error, using the method name, 'caller'.
 */
static int
CheckCliqueArgs (const oski_matrix_t A_tunable,
		 const oski_index_t * rows, oski_index_t num_rows,
		 const oski_index_t * cols, oski_index_t num_cols,
		 oski_vecview_t vals, const char *caller)
{
  oski_index_t m, n;		/* matrix dimensions */
  oski_index_t bad_index;
  oski_index_t r, c;		/* element with largest indices in clique */

  if (caller == NULL)
    caller = "(unknown)";

  if (A_tunable == INVALID_MAT)
    {
      OSKI_ERR_BAD_MAT_MS (caller, 1);
      return ERR_BAD_ARG;
    }

  m = A_tunable->props.num_rows;
  n = A_tunable->props.num_cols;

  if (num_rows <= 0)
    {
      OSKI_ERR_SMALL_ARG_MS (caller, 3, "num_rows", 1);
      return ERR_BAD_ARG;
    }
  if (rows == NULL)
    {
      OSKI_ERR_NULL_ARG_MS (caller, 2, "rows");
      return ERR_BAD_ARG;
    }
  if (num_cols <= 0)
    {
      OSKI_ERR_SMALL_ARG_MS (caller, 3, "num_cols", 1);
      return ERR_BAD_ARG;
    }
  if (cols == NULL)
    {
      OSKI_ERR_NULL_ARG_MS (caller, 4, "cols");
      return ERR_BAD_ARG;
    }

  bad_index = FindIndexOutOfRange (rows, num_rows, 1, m);
  if (bad_index >= 0)
    {
      OSKI_ERR_BAD_ROW_MS (caller, 2, rows[bad_index], 1, m);
      return ERR_BAD_ARG;
    }
  bad_index = FindIndexOutOfRange (cols, num_cols, 1, n);
  if (bad_index >= 0)
    {
      OSKI_ERR_BAD_ROW_MS (caller, 2, cols[bad_index], 1, n);
      return ERR_BAD_ARG;
    }

  if (vals == INVALID_VEC)
    {
      OSKI_ERR_BAD_VEC_MS (caller, 6);
      return ERR_BAD_ARG;
    }

  r = GetMaxIndex (rows, num_rows);
  c = GetMaxIndex (cols, num_cols);
  if (r > vals->num_rows || c > vals->num_cols)
    {
      OSKI_ERR_CLIQUE_VALMAT_SMALL_MS (caller, 6, vals, r, c);
      return ERR_BAD_ARG;
    }

  return 0;
}

/* ----------------------------------------------------------------
 * Cliques
 */

/**
 *  \brief Given a type-specific representation, try to find a
 *  native oski_GetMatReprClique() implementation and call it.
 *  If none exists, returns ERR_NOT_IMPLEMENTED. Otherwise,
 *  calls the native function and returns its error code.
 */
static int
GetClique (const oski_matspecific_t * mat, const oski_matcommon_t * props,
	   const oski_index_t * rows, oski_index_t R,
	   const oski_index_t * cols, oski_index_t C, const oski_vecview_t V)
{
  oski_GetMatReprClique_funcpt func_Get;

  /* Get matrix-specific representation */
  func_Get = OSKI_MATTYPEID_METHOD (mat->type_id, GetMatReprClique);
  if (func_Get == NULL)
    return ERR_NOT_IMPLEMENTED;

  return (*func_Get) (mat->repr, props, rows, R, cols, C, V);
}

/**
 *  Returns a clique using a default naive algorithm that just
 *  calls oski_GetMatEntry() repeatedly.
 *
 *  \pre Assumes all arguments are valid. See CheckCliqueArgs().
 */
static int
GetCliqueDefault (const oski_matrix_t A_tunable,
		  const oski_index_t * rows, oski_index_t num_rows,
		  const oski_index_t * cols, oski_index_t num_cols,
		  oski_vecview_t V)
{
  oski_value_t *vpr = V->val;
  oski_index_t r;
  for (r = 0; r < num_rows; r++)
    {
      oski_value_t *vprc = vpr + r * V->rowinc;
      oski_index_t i = rows[r];	/* 1-based */
      oski_index_t c;
      for (c = 0; c < num_cols; c++)
	{
	  oski_index_t j = cols[c];	/* 1-based */
	  oski_value_t aij = oski_GetMatEntry (A_tunable, i, j);

	  VAL_ASSIGN (vprc[0], aij);
	  vprc += V->colinc;
	}
    }

  return 0;
}

/**
 *  \brief
 *
 *  This implementation tries to find a matrix type-specific
 *  implementation (oski_GetMatReprClique()) and, failing
 *  that, calls a default implementation based on repeatedly
 *  calling oski_GetMatEntry().
 *
 *  \todo Test thoroughly!
 */
int
oski_GetMatClique (const oski_matrix_t A_tunable,
		   const oski_index_t * rows, oski_index_t num_rows,
		   const oski_index_t * cols, oski_index_t num_cols,
		   oski_vecview_t vals)
{
  int err;

  err = CheckCliqueArgs (A_tunable, rows, num_rows, cols, num_cols, vals,
			 MACRO_TO_STRING (oski_GetMatClique));
  if (err)
    return err;

  /* Try the tuned matrix data structure first */
  err = GetClique (&(A_tunable->tuned_mat), &(A_tunable->props),
		   rows, num_rows, cols, num_cols, vals);

  /* No luck; try input data structure */
  if (err == ERR_NOT_IMPLEMENTED)
    err = GetClique (&(A_tunable->input_mat), &(A_tunable->props),
		     rows, num_rows, cols, num_cols, vals);

  /* No specialized routines available; use a slower default
   * algorithm. */
  if (err == ERR_NOT_IMPLEMENTED)
    err = GetCliqueDefault (A_tunable, rows, num_rows, cols, num_cols, vals);

  return err;
}

/**
 *  \brief Given a type-specific representation, try to find a
 *  native oski_SetMatReprClique() implementation and call it.
 *  If none exists, returns ERR_NOT_IMPLEMENTED. Otherwise,
 *  calls the native function and returns its error code.
 */
static int
SetClique (const oski_matspecific_t * mat, const oski_matcommon_t * props,
	   const oski_index_t * rows, oski_index_t R,
	   const oski_index_t * cols, oski_index_t C, const oski_vecview_t V)
{
  oski_SetMatReprClique_funcpt func_Set;

  /* Set matrix-specific representation */
  func_Set = OSKI_MATTYPEID_METHOD (mat->type_id, SetMatReprClique);
  if (func_Set == NULL)
    return ERR_NOT_IMPLEMENTED;

  return (*func_Set) (mat->repr, props, rows, R, cols, C, V);
}

/**
 *  Returns a clique using a default naive algorithm that just
 *  calls oski_SetMatEntry() repeatedly.
 *
 *  \pre Assumes all arguments are valid. See CheckCliqueArgs().
 */
static int
SetCliqueDefault (const oski_matrix_t A_tunable,
		  const oski_index_t * rows, oski_index_t num_rows,
		  const oski_index_t * cols, oski_index_t num_cols,
		  oski_vecview_t V)
{
  oski_value_t *vpr = V->val;
  oski_index_t r;
  for (r = 0; r < num_rows; r++)
    {
      oski_value_t *vprc = vpr + r * V->rowinc;
      oski_index_t i = rows[r];	/* 1-based */
      oski_index_t c;
      for (c = 0; c < num_cols; c++)
	{
	  oski_index_t j = cols[c];	/* 1-based */
	  oski_value_t aij;
	  int err;

	  VAL_ASSIGN (aij, vprc[0]);
	  vprc += V->colinc;

	  err = oski_SetMatEntry (A_tunable, i, j, aij);
	  if (err)
	    return err;
	}
    }

  return 0;
}

/**
 *  \brief
 *
 *  This implementation tries to find a matrix type-specific
 *  implementation (oski_SetMatReprClique()) and, failing
 *  that, calls a default implementation based on repeatedly
 *  calling oski_SetMatEntry().
 *
 *  \todo Test thoroughly!
 */
int
oski_SetMatClique (const oski_matrix_t A_tunable,
		   const oski_index_t * rows, oski_index_t num_rows,
		   const oski_index_t * cols, oski_index_t num_cols,
		   const oski_vecview_t vals)
{
  int err;

  err = CheckCliqueArgs (A_tunable, rows, num_rows, cols, num_cols, vals,
			 MACRO_TO_STRING (oski_GetMatClique));
  if (err)
    return err;

  /* Try the tuned matrix data structure first */
  err = SetClique (&(A_tunable->tuned_mat), &(A_tunable->props),
		   rows, num_rows, cols, num_cols, vals);

  /* No luck; try input data structure */
  if (err == ERR_NOT_IMPLEMENTED)
    err = SetClique (&(A_tunable->input_mat), &(A_tunable->props),
		     rows, num_rows, cols, num_cols, vals);

  /* No specialized routines available; use a slower default
   * algorithm. */
  if (err == ERR_NOT_IMPLEMENTED)
    err = SetCliqueDefault (A_tunable, rows, num_rows, cols, num_cols, vals);

  return err;
}

/* ----------------------------------------------------------------
 * Diagonals
 */

/**
 *  \brief Checks the input arguments to oski_GetMatDiagValues()
 *  and oski_SetMatDiagValues(), returning 0 if the arguments are
 *  valid or an error code otherwise. This routine calls the
 *  error handler on error, using the method name, 'caller'.
 */
static int
CheckDiagValuesArgs (const oski_matrix_t A_tunable,
		     oski_index_t d, oski_vecview_t vals, const char *caller)
{
  oski_index_t m, n;		/* matrix dimensions */
  oski_index_t r;		/* length of diagonal d */

  if (caller == NULL)
    caller = "(unknown)";

  if (A_tunable == INVALID_MAT)
    {
      OSKI_ERR_BAD_MAT_MS (caller, 1);
      return ERR_BAD_ARG;
    }

  m = A_tunable->props.num_rows;
  n = A_tunable->props.num_cols;

  if (!IS_VAL_IN_RANGE (d, 1 - m, n - 1))
    {
      OSKI_ERR_BAD_DIAG_MS (caller, 2, d, m, n);
      return ERR_BAD_ARG;
    }

  /* Length of the d-th diagonal */
  r = MIN (MAX (m, n) - d, MIN (m, n));
  if (vals->num_rows < r || vals->num_cols < 1)
    {
      OSKI_ERR_DIAG_VALMAT_SMALL_MS (caller, 3, vals, r, 1);
      return ERR_BAD_ARG;
    }

  return 0;
}

/**
 *  \brief Given a type-specific representation, try to find a
 *  native oski_GetMatReprDiagValues() implementation and call it.
 *  If none exists, returns ERR_NOT_IMPLEMENTED. Otherwise,
 *  calls the native function and returns its error code.
 */
static int
GetDiagValues (const oski_matspecific_t * mat, const oski_matcommon_t * props,
	       oski_index_t d, oski_vecview_t V)
{
  oski_GetMatReprDiagValues_funcpt func_Get;

  /* Get matrix-specific representation */
  func_Get = OSKI_MATTYPEID_METHOD (mat->type_id, GetMatReprDiagValues);
  if (func_Get == NULL)
    return ERR_NOT_IMPLEMENTED;

  return (*func_Get) (mat->repr, props, d, V);
}

/**
 *  Returns a clique using a default naive algorithm that just
 *  calls oski_GetMatEntry() repeatedly.
 *
 *  \pre Assumes all arguments are valid. See CheckDiagValuesArgs().
 */
static int
GetDiagValuesDefault (const oski_matrix_t A_tunable,
		      oski_index_t d, oski_vecview_t V)
{
  oski_index_t m = A_tunable->props.num_rows;
  oski_index_t n = A_tunable->props.num_cols;
  oski_value_t *vpr;
  oski_index_t i;

  vpr = V->val + MIN (0, d) * V->rowinc;

  for (i = 1; i <= MIN (m, n); i++)
    {
      oski_value_t aij = oski_GetMatEntry (A_tunable, i, i + d);
      VAL_ASSIGN (vpr[0], aij);
      vpr += V->rowinc;
    }

  return 0;
}

/**
 *  \brief
 *
 *  This implementation tries to find a matrix type-specific
 *  implementation (oski_GetMatReprDiagValues()) and, failing
 *  that, calls a default implementation based on repeatedly
 *  calling oski_GetMatEntry().
 *
 *  \todo Test thoroughly!
 */
int
oski_GetMatDiagValues (const oski_matrix_t A_tunable,
		       oski_index_t diag_num, oski_vecview_t diag_vals)
{
  int err;

  err = CheckDiagValuesArgs (A_tunable, diag_num, diag_vals,
			     MACRO_TO_STRING (oski_GetMatDiagValues));
  if (err)
    return err;

  /* Try the tuned matrix data structure first */
  err = GetDiagValues (&(A_tunable->tuned_mat), &(A_tunable->props),
		       diag_num, diag_vals);

  /* No luck; try input data structure */
  if (err == ERR_NOT_IMPLEMENTED)
    err = GetDiagValues (&(A_tunable->input_mat), &(A_tunable->props),
			 diag_num, diag_vals);

  /* No specialized routines available; use a slower default
   * algorithm. */
  if (err == ERR_NOT_IMPLEMENTED)
    err = GetDiagValuesDefault (A_tunable, diag_num, diag_vals);

  return err;
}

/**
 *  \brief Given a type-specific representation, try to find a
 *  native oski_SetMatReprDiagValues() implementation and call it.
 *  If none exists, returns ERR_NOT_IMPLEMENTED. Otherwise,
 *  calls the native function and returns its error code.
 */
static int
SetDiagValues (const oski_matspecific_t * mat, const oski_matcommon_t * props,
	       oski_index_t d, const oski_vecview_t V)
{
  oski_SetMatReprDiagValues_funcpt func_Set;

  /* Set matrix-specific representation */
  func_Set = OSKI_MATTYPEID_METHOD (mat->type_id, SetMatReprDiagValues);
  if (func_Set == NULL)
    return ERR_NOT_IMPLEMENTED;

  return (*func_Set) (mat->repr, props, d, V);
}

/**
 *  Sets values along a given diagonal using a default naive algorithm
 *  that just calls oski_SetMatEntry() repeatedly.
 *
 *  \pre Assumes all arguments are valid. See CheckDiagValuesArgs().
 */
static int
SetDiagValuesDefault (const oski_matrix_t A_tunable,
		      oski_index_t d, const oski_vecview_t V)
{
  oski_index_t m = A_tunable->props.num_rows;
  oski_index_t n = A_tunable->props.num_cols;
  const oski_value_t *vpr;
  oski_index_t i;

  vpr = V->val + MIN (0, d) * V->rowinc;

  for (i = 1; i <= MIN (m, n); i++)
    {
      oski_value_t aij;
      int err;

      VAL_ASSIGN (aij, vpr[0]);
      vpr += V->rowinc;

      err = oski_SetMatEntry (A_tunable, i, i + d, aij);
      if (err)
	return err;
    }

  return 0;
}

/**
 *  \brief
 *
 *  This implementation tries to find a matrix type-specific
 *  implementation (oski_SetMatReprDiagValues()) and, failing
 *  that, calls a default implementation based on repeatedly
 *  calling oski_SetMatEntry().
 *
 *  \todo Test thoroughly!
 */
int
oski_SetMatDiagValues (const oski_matrix_t A_tunable,
		       oski_index_t diag_num, const oski_vecview_t diag_vals)
{
  int err;

  err = CheckDiagValuesArgs (A_tunable, diag_num, diag_vals,
			     MACRO_TO_STRING (oski_SetMatDiagValues));
  if (err)
    return err;

  /* Try the tuned matrix data structure first */
  err = SetDiagValues (&(A_tunable->tuned_mat), &(A_tunable->props),
		       diag_num, diag_vals);

  /* No luck; try input data structure */
  if (err == ERR_NOT_IMPLEMENTED)
    err = SetDiagValues (&(A_tunable->input_mat), &(A_tunable->props),
			 diag_num, diag_vals);

  /* No specialized routines available; use a slower default
   * algorithm. */
  if (err == ERR_NOT_IMPLEMENTED)
    err = SetDiagValuesDefault (A_tunable, diag_num, diag_vals);

  return err;
}

/* eof */
