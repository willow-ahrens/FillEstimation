/**
 *  \file util/testvec.c
 *  \brief Code to generate vectors and vector views for testing
 *  and off-line benchmarking.
 */

#include <config/config.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "abort_prog.h"
#include "array_util.h"
#include "testvec.h"
#include "rand_util.h"
#include "parse_opts.h"

/** \brief Macro to return the minimum value of a pair. */
#define MIN(a, b)  ((a) < (b) ? (a) : (b))

/**
 *  \brief
 *
 *  This routine aborts the program if the test vector cannot be
 *  created.
 *
 *  \param[in] len Length of a column vector.
 *  \param[in] num_vecs Number of column vectors.
 *  \param[in] orient Storage orientation.
 *  \param[in] use_minstride Set to 0 to choose the minimum possible
 *  stride for the specified orientation, or 1 to choose a
 *  slightly larger stride with perturbation chosen at random.
 *
 *  \returns A test vector conforming to the caller's request.
 */
oski_vecview_t
testvec_Create (oski_index_t len, oski_index_t num_vecs,
		oski_storage_t orient, int use_minstride)
{
  oski_vecview_t x;
  oski_value_t *val;

  oski_index_t stride_adj;
  oski_index_t stride;

  /* Compute a possible small, randomly chosen amount by which
   * to increase stride from the minimum possible.
   */
  if (!use_minstride)
    stride_adj = 1 + (rand () % 10);
  else
    stride_adj = 0;

  if (num_vecs == 1)
    {
      stride = 1 + stride_adj;
      val = array_CreateRandom (len * stride);
    }
  else if (orient == LAYOUT_COLMAJ)
    {				/* num_vecs > 1, col major */
      stride = len + stride_adj;
      val = array_CreateRandom (stride * num_vecs);
    }
  else
    {				/* num_vecs > 1, row major */

      stride = num_vecs + stride_adj;
      val = array_CreateRandom (len * stride);
    }
  ABORT (val == NULL, testvec_Create, ERR_OUT_OF_MEMORY);

  if (num_vecs == 1)
    x = oski_CreateVecView (val, len, stride);
  else
    x = oski_CreateMultiVecView (val, len, num_vecs, orient, stride);
  ABORT (x == INVALID_VEC, testvec_Create, ERR_BAD_VECVIEW);

  return x;
}

size_t
testvec_CalcDataSize (const oski_vecview_t x)
{
  oski_index_t data_size;

  if (x == INVALID_VEC)
    return 0;

  data_size = x->stride;
  if (x->orient == LAYOUT_COLMAJ)
    data_size *= x->num_cols;
  else
    data_size *= x->num_rows;
  return data_size;
}

oski_vecview_t
testvec_Clone (const oski_vecview_t x)
{
  oski_value_t *y_val;
  oski_vecview_t y;
  oski_index_t data_size;

  if (x == INVALID_VEC)
    return INVALID_VEC;

  data_size = testvec_CalcDataSize (x);
  y_val = array_Duplicate (x->val, data_size);
  ABORT (y_val == NULL, testvec_Clone, ERR_OUT_OF_MEMORY);

  y = oski_CreateMultiVecView (y_val, x->num_rows, x->num_cols,
			       x->orient, x->stride);
  ABORT (y == INVALID_VEC, testvec_CopyShallow, ERR_BAD_VECVIEW);
  return y;
}

void
testvec_Destroy (oski_vecview_t x)
{
  oski_value_t *val;

  if (x == INVALID_VEC)
    return;

  val = x->val;
  oski_DestroyVecView (x);
  if (val != NULL)
    array_Destroy (val);
}

/**
 *  \brief
 *
 *  \param[in] m Logical number of matrix rows. Must be positive.
 *  \param[in] n Logical number of matrix columns. Must be positive.
 *  \param[in,out] p_ptr Pointer to a pointer in which to return the
 *  newly allocated row pointers.
 *  \param[in,out] p_ind Pointer to a pointer in which to return the
 *  newly allocated column indices.
 *  \param[in,out] p_val Pointer to a pointer in which to return the
 *  newly allocated non-zero values.
 *  \returns Newly allocated 3-array storage in
 *  (*p_ptr, *p_ind, *p_val). On error, aborts the application with
 *  an error message.
 *
 *  The newly created matrix has the properties specified in
 *  testmat_ChangeSizeDenseCSR().
 *
 *  \todo What is the best way to initialize the dense matrix?
 *  For benchmarking, it seems sufficient to initialize all entries
 *  to 'tiny' values (here, \f$\frac{1}{\max{m}{n}}\f$), which is
 *  faster than calling the random number generator.
 */
void
testmat_GenDenseCSR (oski_index_t m, oski_index_t n,
		     oski_index_t ** p_ptr, oski_index_t ** p_ind,
		     oski_value_t ** p_val)
{
  oski_index_t *ptr;
  oski_index_t *ind;
  oski_value_t *val;

  ABORT (m <= 0 || n <= 0, testmat_GenDenseCSR, ERR_BAD_ARG);
  ABORT (p_ptr == NULL || p_ind == NULL || p_val == NULL,
	 testmat_GenDenseCSR, ERR_BAD_ARG);

  ptr = oski_Malloc (oski_index_t, m + 1);
  ind = oski_Malloc (oski_index_t, m * n);
#if defined(OSKI_MATINITDENSE_RANDOM)
  val = array_CreateRandom (m * n);
#else
  {
    oski_index_t max_dim = (m < n) ? n : m;
    oski_value_t init_val = MAKE_VAL_COMPLEX ((double) 1.0 / max_dim, 0.0);
    val = array_Create (m * n, init_val);
  }
#endif

  ABORT (ptr == NULL || ind == NULL || val == NULL,
	 testmat_GenDenseCSR, ERR_OUT_OF_MEMORY);

  testmat_ChangeSizeDenseCSR (m, n, ptr, ind);

  /* Return */
  *p_ptr = ptr;
  *p_ind = ind;
  *p_val = val;
}

/**
 *  \brief
 *
 *  \param[in] nz_volume Minimum desired size of matrix data array,
 *  in Bytes.
 *  \param[in] elem_size Size of each matrix element value.
 *  \returns The dimension of a square dense matrix whose size
 *  (volume, in bytes) is at least nz_volume.
 */
size_t
testmat_ChooseDim (size_t nz_volume, size_t elem_size)
{
  double n = sqrt ((double) nz_volume / elem_size);

  return ceil (n);
}

size_t
ChooseDivisible (size_t n, size_t d)
{
  return ((n + d - 1) / d) * d;
}

/**
 *  \brief
 *
 *  \param[in] m_new New number of rows.
 *  \param[in] n_new New number of columns.
 *  \param[in,out] ptr Prior row pointers.
 *  \param[in,out] ind Prior index values.
 *  \returns A new pattern for a dense m_new x n_new matrix in
 *  sparse CSR format, conforming to the following specifications:
 *    - It is stored in CSR format.
 *    - It uses 0-based indexing.
 *    - Indices are sorted and unique.
 *
 *  This routine assumes that ptr, ind were previously allocated to
 *  be sufficiently large to store a dense matrix of the requested
 *  size. The previous contents of (ptr, ind) are ignored.
 */
void
testmat_ChangeSizeDenseCSR (oski_index_t m_new, oski_index_t n_new,
			    oski_index_t * ptr, oski_index_t * ind)
{
  oski_index_t i;

  ABORT (m_new <= 0 || n_new <= 0, testmat_ChangeSizeDenseCSR, ERR_BAD_ARG);
  ABORT (ptr == NULL || ind == NULL, testmat_ChangeSizeDenseCSR, ERR_BAD_ARG);

  for (i = 0; i < m_new; i++)
    {
      oski_index_t k = i * n_new;
      oski_index_t j;
      oski_index_t *indp;

      ptr[i] = k;
      for (j = 0, indp = ind + k; j < n_new; j++, indp++)
	indp[0] = j;
    }
  ptr[m_new] = ptr[m_new - 1] + n_new;

  /* Consistency check */
  ABORT (ptr[m_new] != (m_new * n_new), testmat_ChangeSizeDenseCSR,
	 ERR_WRONG_ANS);
}

/**
 *  \brief
 *
 *  \param[in] n_new New dimension.
 *  \param[in] make_lower Set to any non-zero value to generate a
 *  lower triangular matrix; otherwise, this routine generates an
 *  upper-triangular pattern.
 *  \param[in] cond_for_trisolve Set to any non-zero value to
 *  generate random values that lead to a well-conditioned
 *  triangular system solve; otherwise, put all values in the
 *  range [0, 1].
 *  \param[in,out] ptr Prior row pointers.
 *  \param[in,out] ind Prior index values.
 *  \param[in,out] val Prior non-zero values.
 *  \returns A new pattern for a dense triangular n_new x n_new matrix
 *  in sparse CSR format, conforming to the following specifications:
 *    - It is stored in CSR format.
 *    - It uses 0-based indexing.
 *    - Indices are sorted and unique.
 *    - The value of all diagonal elements is O(1), pure real,
 *    and all off-diagonal elements are O(1/n_new).
 *
 *  This routine assumes that ptr, ind were previously allocated to
 *  be sufficiently large to store a dense matrix of the requested
 *  size. The previous contents of (ptr, ind, val) are ignored.
 */
void
testmat_ChangeSizeDenseTriCSR (oski_index_t n_new,
			       int make_lower, int cond_for_trisolve,
			       oski_index_t * ptr, oski_index_t * ind,
			       oski_value_t * val)
{
  oski_index_t i;
  oski_index_t nnz_so_far;
  const oski_value_t inv_n = MAKE_VAL_COMPLEX (1.0 / n_new, 0);

  ABORT (n_new <= 0, testmat_ChangeSizeDenseTriCSR, ERR_BAD_ARG);
  ABORT (ptr == NULL || ind == NULL, testmat_ChangeSizeDenseTriCSR,
	 ERR_BAD_ARG);

  ptr[0] = 0;
  nnz_so_far = 0;
  for (i = 0; i < n_new; i++)
    {
      oski_index_t j_start = make_lower ? 0 : (i + 1);
      oski_index_t j_stop = make_lower ? i : n_new;
      oski_index_t j;

      if (!make_lower)
	{
	  oski_value_t newval = rand_GenValueReal (1, 2);

	  VAL_ASSIGN (val[nnz_so_far], newval);
	  ind[nnz_so_far] = i;
	  nnz_so_far++;
	}

      for (j = j_start; j != j_stop; j++, nnz_so_far++)
	{
	  oski_value_t newval = rand_GenValue (0, 1.0);

	  if (cond_for_trisolve)
	    {
	      VAL_SCALE (newval, inv_n);
	    }
	  VAL_ASSIGN (val[nnz_so_far], newval);
	  ind[nnz_so_far] = j;
	}

      if (make_lower)
	{
	  oski_value_t newval = rand_GenValueReal (1, 2);

	  VAL_ASSIGN (val[nnz_so_far], newval);
	  ind[nnz_so_far] = i;
	  nnz_so_far++;
	}

      ptr[i + 1] = nnz_so_far;
    }
}

/* greatest common divisor */
static oski_index_t
gcd (oski_index_t a, oski_index_t b)
{
  oski_index_t r;
  if (a < b)
    return gcd (b, a);
  /* a >= b */
  r = a % b;
  if (r == 0)
    return b;
  else
    return gcd (b, r);
}

/* least common multiple */
static oski_index_t
lcm (oski_index_t a, oski_index_t b)
{
  return a * b / gcd (a, b);
}

/**
 *  \brief
 *
 *  \param[in] k_min Minimum number of non-zeros.
 *  \param[in] row_min Minimum number of non-zeros per row.
 *  \param[in] r Row block size.
 *  \param[in] c Column block size.
 *  \param[in,out] p_m Pointer to scalar in which to store the
 *  number of rows.
 *  \param[in,out] p_n Pointer to scalar in which to store the
 *  number of columns.
 *  \param[in,out] p_ptr Pointer to row pointers.
 *  \param[in,out] p_ind Pointer to column indices.
 *  \param[in,out] p_val Pointer to values.
 */
void
testmat_GenBlockedBandedCSR (oski_index_t k_min, oski_index_t row_min,
			     oski_index_t r, oski_index_t c,
			     oski_index_t * p_n,
			     oski_index_t ** p_ptr, oski_index_t ** p_ind,
			     oski_value_t ** p_val)
{
  oski_index_t b = lcm (r, c);
  oski_index_t nb_row = (row_min + b - 1) / b;	/* # of blocks per row */
  oski_index_t k_row = nb_row * b;	/* # of non-zeros per row */
  oski_index_t nb = ((k_min / k_row) + b) / b;	/* matrix dimension */
  oski_index_t n = nb * b;
  oski_index_t k = n * k_row;	/* total # of non-zeros */

  oski_index_t i;

  oski_index_t *ptr;
  oski_index_t *ind;
  oski_value_t *val;
  int err;

  assert (n >= (nb_row * b));

  err = oski_MultiMalloc (__FILE__, __LINE__, 3,
			  (size_t) (sizeof (oski_index_t) * (n + 1)), &ptr,
			  (size_t) (sizeof (oski_index_t) * k), &ind,
			  (size_t) (sizeof (oski_value_t) * k), &val);
  ABORT (err != 0, testmat_GenBlockedBandedCSR, ERR_OUT_OF_MEMORY);

  for (i = 0; i <= n; i++)
    ptr[i] = i * k_row;

  for (i = 0; i < n; i += b)
    {
      oski_index_t di;
      oski_index_t j_min = i - ((nb_row / 2) * b);
      oski_index_t j_max = j_min + k_row - 1;

      if (j_min < 0)
	{
	  j_max -= j_min;
	  j_min = 0;
	}
      else if (j_max >= n)
	{
	  j_min -= j_max - n + 1;
	  j_max = n - 1;
	}

      assert ((j_max - j_min + 1) == k_row);

      for (di = 0; di < b; di++)
	{
	  oski_index_t j;
	  for (j = j_min; j <= j_max; j++)
	    {
	      assert (j >= 0);
	      assert (j < n);
	      ind[0] = j;
	      ind++;
	    }
	}
    }

  array_FillRandom (val, k);

  if (p_n != NULL)
    *p_n = n;
  if (p_ptr != NULL)
    *p_ptr = ptr;
  else
    oski_Free (ptr);
  if (p_ind != NULL)
    *p_ind = ind;
  else
    oski_Free (ind);
  if (p_val != NULL)
    *p_val = val;
  else
    oski_Free (val);
}

/**
 *  \brief Returns 0 if two distinct computations of a matrix-vector
 *  product are numerical 'equal' within some reasonable error bound,
 *  or #ERR_WRONG_ANS otherwise.
 *
 *  \param[in] A A sparse matrix, \f$A\f$.
 *  \param[in] x A vector, \f$x\f$.
 *  \param[in] y0 The matrix-vector product \f$\alpha A x\f$ computed
 *  using one algorithm, where \f$\alpha\f$ is some scalar.
 *  \param[in] y1 The matrix-vector product \f$\alpha A x\f$ computed
 *  using another algorithm.
 *
 *  \returns 0 if \f$y_0\f$ and \f$y_1\f$ are numerically equivalent
 *  to within round-off error, given that two different algorithms
 *  were used to compute \f$y_0\f$ and \f$y_1\f$.
 */
static int
CheckMatMultVectors (const oski_matrix_t A, const oski_vecview_t x,
		     const oski_vecview_t y0, const oski_vecview_t y1)
{
  /* Compute error */
  double A_norm = oski_CalcMat1Norm (A);
  oski_index_t max_dim = (x->num_rows > y0->num_rows)
    ? x->num_rows : y0->num_rows;
  int err = 0;
  int j;

  for (j = 1; j <= x->num_cols; j++)
    {
      double x_norm = oski_CalcVecViewInfNorm (x, j);
      double y0_norm = oski_CalcVecViewInfNorm (y0, j);
      double dy_norm = oski_CalcVecViewInfNormDiff (y0, j, y1, j);

      double relerr = dy_norm / y0_norm;
      const double CONST = 10.0;
      double threshold = CONST * max_dim * VAL_EPS * A_norm * x_norm;

      if (relerr > threshold)
	{
	  oski_PrintDebugMessage (2,
				  "*** Detected a larger-than-expected error\n"
				  "    in the %d-th vector.\n"
				  "\n"
				  "The relative residual, %g / %g == %g, exceeds\n"
				  "     C_0 * max(m,n) * EPS * ||A||_1 * ||x||_\\infty\n"
				  "  =  %g * %d * %g * %g * %g = %g.\n"
				  "***",
				  j,
				  dy_norm, y0_norm, relerr,
				  CONST, max_dim, VAL_EPS, A_norm, x_norm,
				  threshold);
	  err = ERR_WRONG_ANS;
	}
      else
	{
	  oski_PrintDebugMessage (2, "Column %d OK: %g < %g",
				  j, relerr, threshold);
#if 0
	  oski_PrintDebugMessage (2,
				  "The relative residual = %g, is below the threshold,\n"
				  "C_0 * max(m,n) * EPS * ||A||_1 * ||x||_\\infty\n"
				  " = %g * %d * %g * %g * %g = %g.",
				  relerr,
				  CONST, max_dim, VAL_EPS, A_norm, x_norm,
				  threshold);
#endif
	}
    }
  return err;
}

/** -----------------------------------------------------
 *  \name Routines to display parameter values on stderr.
 */
/*@{*/

static void
DisplayValue (const char *name, oski_value_t x)
{
  PrintDebugValue (1, name, x);
}

static void
DisplayMatop (const char *matname, oski_matop_t op)
{
  PrintDebugMatTransOp (1, matname, op);
}

static void
DisplayVecView (const char *name, const oski_vecview_t x)
{
  PrintDebugVecViewInfo (1, name, x);
}

static void
DisplayATAop (const char *matname, oski_ataop_t op)
{
  PrintDebugMatATAOp (1, matname, op);
}

/*@}*/
/* ------------------------------------------------- */

/**
 *  \brief
 *
 *  This routine only aborts the program if an out-of-memory
 *  error occurs. If the calculation is merely incorrect, then
 *  this routine prints a message and returns ERR_WRONG_ANS.
 */
int
check_MatMult_instance (const oski_matrix_t A0, const oski_matrix_t A1,
			oski_matop_t opA, oski_value_t alpha,
			const oski_vecview_t x, oski_value_t beta,
			oski_vecview_t y0)
{
  oski_vecview_t y1;

  int err;

  oski_PrintDebugMessage (2, "*** TESTING...");
  DisplayMatop ("A", opA);
  DisplayValue ("alpha", alpha);
  DisplayVecView ("x", x);
  DisplayValue ("beta", beta);
  DisplayVecView ("y", y0);

  y1 = testvec_Clone (y0);
  ABORT (y1 == INVALID_VEC, check_MatMult_instance, ERR_OUT_OF_MEMORY);

  err = oski_MatMult (A0, opA, alpha, x, beta, y0);
  ABORT (err != 0, check_MatMult, err);

  err = oski_MatMult (A1, opA, alpha, x, beta, y1);
  ABORT (err != 0, check_MatMult, err);

  err = CheckMatMultVectors (A0, x, y0, y1);

  testvec_Destroy (y1);
  return err;
}

/**
 *  \brief
 *
 *  This routine only aborts the program if an out-of-memory
 *  error occurs. If the calculation is merely incorrect, then
 *  this routine prints a message and returns ERR_WRONG_ANS.
 */
int
check_MatTrisolve_instance (const oski_matrix_t A0, const oski_matrix_t A1,
			    oski_matop_t opA, oski_value_t alpha,
			    const oski_vecview_t b)
{
  oski_vecview_t x0, b0;
  oski_vecview_t x1, b1;
  oski_value_t inv_alpha;

  int err;

  VAL_INV (inv_alpha, alpha);

  oski_PrintDebugMessage (2, "*** TESTING...");
  DisplayMatop ("A", opA);
  DisplayValue ("alpha", alpha);
  DisplayValue ("alpha^(-1)", inv_alpha);
  DisplayVecView ("b", b);

  x0 = testvec_Clone (b);
  ABORT (x0 == INVALID_VEC, check_MatTrisolve_instance, ERR_OUT_OF_MEMORY);
  b0 = testvec_Clone (b);
  ABORT (b0 == INVALID_VEC, check_MatTrisolve_instance, ERR_OUT_OF_MEMORY);
  x1 = testvec_Clone (b);
  ABORT (x1 == INVALID_VEC, check_MatTrisolve_instance, ERR_OUT_OF_MEMORY);
  b1 = testvec_Clone (b);
  ABORT (b1 == INVALID_VEC, check_MatTrisolve_instance, ERR_OUT_OF_MEMORY);

  err = oski_MatTrisolve (A0, opA, alpha, x0);
  ABORT (err != 0, check_MatTrisolve_instance, err);
  err = oski_MatMult (A0, opA, inv_alpha, x0, TVAL_ZERO, b0);
  ABORT (err != 0, check_MatTrisolve_instance, err);

  err = oski_MatTrisolve (A1, opA, alpha, x1);
  ABORT (err != 0, check_MatTrisolve_instance, err);
  err = oski_MatMult (A1, opA, inv_alpha, x1, TVAL_ZERO, b1);
  ABORT (err != 0, check_MatTrisolve_instance, err);

  err = CheckMatMultVectors (A0, x0, b0, b1);

  testvec_Destroy (x0);
  testvec_Destroy (x1);
  testvec_Destroy (b0);
  testvec_Destroy (b1);
  return err;
}

/**
 *  \brief
 *
 *  This routine only aborts the program if an out-of-memory
 *  error occurs. If the calculation is merely incorrect, then
 *  this routine prints a message and returns ERR_WRONG_ANS.
 */
int
check_MatMultAndMatTransMult_instance (const oski_matrix_t A0,
				       const oski_matrix_t A1,
				       oski_value_t alpha,
				       const oski_vecview_t x,
				       oski_value_t beta,
				       oski_vecview_t y0, oski_matop_t opA,
				       oski_value_t omega,
				       const oski_vecview_t w,
				       oski_value_t zeta, oski_vecview_t z0)
{
  oski_vecview_t y1;
  oski_vecview_t z1;

  int err = 0;

  oski_PrintDebugMessage (2, "*** TESTING...");
  DisplayMatop ("A", opA);
  DisplayValue ("alpha", alpha);
  DisplayVecView ("x", x);
  DisplayValue ("beta", beta);
  DisplayVecView ("y", y0);
  DisplayValue ("omega", omega);
  DisplayVecView ("w", w);
  DisplayValue ("zeta", zeta);
  DisplayVecView ("z", z0);

  y1 = testvec_Clone (y0);
  ABORT (y1 == INVALID_VEC, check_MatMultAndMatTransMult_instance,
	 ERR_OUT_OF_MEMORY);
  z1 = testvec_Clone (z0);
  ABORT (z1 == INVALID_VEC, check_MatMultAndMatTransMult_instance,
	 ERR_OUT_OF_MEMORY);

#if 0
  err = oski_MatMultAndMatTransMult (A0, alpha, x, beta, y0,
				     opA, omega, w, zeta, z0);
  ABORT (err != 0, check_MatMultAndMatTransMult, err);
#else
  err = oski_MatMult (A0, OP_NORMAL, alpha, x, beta, y0);
  ABORT (err != 0, check_MatMultAndMatTransMult, err);
  err = oski_MatMult (A0, opA, omega, w, zeta, z0);
  ABORT (err != 0, check_MatMultAndMatTransMult, err);
#endif

  err = oski_MatMultAndMatTransMult (A1, alpha, x, beta, y1,
				     opA, omega, w, zeta, z1);
  ABORT (err != 0, check_MatMultAndMatTransMult, err);

  err = CheckMatMultVectors (A0, x, y0, y1);
  if (!err)
    {
      err = CheckMatMultVectors (A0, w, z0, z1);
      if (err)
	oski_PrintDebugMessage (2, "**** --> Error in computation of z");
    }
  else
    {
      oski_PrintDebugMessage (2, "**** --> Error in computation of y");
    }

  testvec_Destroy (z1);
  testvec_Destroy (y1);
  return err;
}

/**
 *  \brief
 *
 *  This routine only aborts the program if an out-of-memory
 *  error occurs. If the calculation is merely incorrect, then
 *  this routine prints a message and returns ERR_WRONG_ANS.
 */
int
check_MatTransMatMult_instance (const oski_matrix_t A0,
				const oski_matrix_t A1, oski_ataop_t opA,
				oski_value_t alpha, const oski_vecview_t x,
				oski_value_t beta, oski_vecview_t y0,
				oski_vecview_t t0)
{
  oski_vecview_t t0_force;
  oski_vecview_t t1;
  oski_vecview_t y1;
  int err;

  oski_PrintDebugMessage (2, "*** TESTING...");
  DisplayATAop ("A", opA);
  DisplayValue ("alpha", alpha);
  DisplayVecView ("x", x);
  DisplayValue ("beta", beta);
  DisplayVecView ("y", y0);
  DisplayVecView ("t", t0);

  y1 = testvec_Clone (y0);
  ABORT (y1 == INVALID_VEC, check_MatTransMatMult_instance,
	 ERR_OUT_OF_MEMORY);

  if (t0 == INVALID_VEC)
    t1 = INVALID_VEC;
  else
    {
      t1 = testvec_Clone (t0);
      ABORT (t1 == INVALID_VEC, check_MatTransMatMult_instance,
	     ERR_OUT_OF_MEMORY);
    }

  err = oski_MatTransMatMult (A0, opA, alpha, x, beta, y0, t0);
  ABORT (err != 0, check_MatTransMatMult_instance, err);

  err = oski_MatTransMatMult (A1, opA, alpha, x, beta, y1, t1);
  ABORT (err != 0, check_MatTransMatMult_instance, err);

  oski_PrintDebugMessage (2, ">>> Checking t and y vectors...");

  t0_force = INVALID_VEC;
  if (t0 == INVALID_VEC && t1 == INVALID_VEC)
    {
      /* Even if the caller did not explicitly compute the
       * intermediate vector, t0, we must do it in order to
       * check the error on y0, y1.
       */
      oski_matop_t op_matmult;

      if (opA == OP_AT_A || opA == OP_AH_A)
	{
	  op_matmult = OP_NORMAL;
	  t0_force = testvec_Create (A0->props.num_rows, x->num_cols,
				     LAYOUT_COLMAJ, 1);
	}
      else
	{
	  t0_force = testvec_Create (A0->props.num_cols, x->num_cols,
				     LAYOUT_COLMAJ, 1);
	  if (opA == OP_A_AH)
	    op_matmult = OP_CONJ_TRANS;
	  else
	    op_matmult = OP_TRANS;
	}
      ABORT (t0_force == INVALID_VEC, check_MatTransMatMult_instance,
	     ERR_OUT_OF_MEMORY);

      err = oski_MatMult (A0, op_matmult, TVAL_ONE, x, TVAL_ZERO, t0_force);
      ABORT (err != 0, check_MatTransMatMult_instance, err);

      t1 = testvec_Clone (t0_force);
      ABORT (t1 == INVALID_VEC, check_MatTransMatMult_instance,
	     ERR_OUT_OF_MEMORY);
      err = CheckMatMultVectors (A0, x, t0_force, t1);
    }
  else
    err = CheckMatMultVectors (A0, x, t0, t1);

  if (!err)
    {
      oski_PrintDebugMessage (2, "    (t is OK)");
      if (t0_force != INVALID_VEC)
	err = CheckMatMultVectors (A0, t0_force, y0, y1);
      else
	err = CheckMatMultVectors (A0, t0, y0, y1);
      if (!err)
	oski_PrintDebugMessage (2, "    (y is also OK)");
    }

  if (t0_force != INVALID_VEC)
    testvec_Destroy (t0_force);
  testvec_Destroy (t1);
  testvec_Destroy (y1);
  return err;
}

/* -------------------------------------------------------------------
 * Code that generates random CSR matrices.
 */

/**
 *  \brief Defines a 'non-zero filter' function type.
 *
 *  A non-zero filter function f(m, n, i, j) returns 1 if (i, j)
 *  is an 'acceptable' non-zero for a matrix of size m x n,
 *  or 0 otherwise. The indices (i, j) are assume to be 1-based.
 *
 *  For instance, the following is a composition of two filter
 *  functions that can be used to generate a lower triangular
 *  matrix:
 *
\code
int
filter_any( oski_index_t m, oski_index_t n,
            oski_index_t i, oski_index_t j )
{
	// Ensures valid indices
	return (1 <= i) && (i <= m) && (1 <= j) && (j <= n);
}

int
filter_lowertri( oski_index_t m, oski_index_t n,
                 oski_index_t i, oski_index_t j )
{
  return filter_any(m, n, i, j) && j <= i;
}
\endcode
 */
typedef int (*filter_funcpt) (oski_index_t m, oski_index_t n,
			      oski_index_t i, oski_index_t j);

/**
 *  \brief A filter that allows any valid entry to be a non-zero.
 */
static int
filter_valid (oski_index_t m, oski_index_t n, oski_index_t i, oski_index_t j)
{
  return (1 <= i) && (i <= m) && (1 <= j) && (j <= n);
}

/**
 *  \brief Filter for a strictly lower triangular matrix.
 */
static int
filter_strictly_lower_tri (oski_index_t m, oski_index_t n,
			   oski_index_t i, oski_index_t j)
{
  return j < i;
}

/**
 *  \brief Filter for a strictly upper triangular matrix.
 */
static int
filter_strictly_upper_tri (oski_index_t m, oski_index_t n,
			   oski_index_t i, oski_index_t j)
{
  return i < j;
}

/**
 *  \brief Filter for diagonal entries.
 */
static int
filter_diagonal (oski_index_t m, oski_index_t n,
		 oski_index_t i, oski_index_t j)
{
  return i == j;
}

/**
 *  \brief Filter for off-diagonal entries.
 */
static int
filter_offdiag (oski_index_t m, oski_index_t n,
		oski_index_t i, oski_index_t j)
{
  return !filter_diagonal (m, n, i, j);
}

/**
 *  \brief Filters a lower triangular matrix.
 */
static int
filter_lower_tri (oski_index_t m, oski_index_t n,
		  oski_index_t i, oski_index_t j)
{
  return filter_strictly_lower_tri (m, n, i, j)
    || filter_diagonal (m, n, i, j);
}

/**
 *  \brief Filters a upper triangular matrix.
 */
static int
filter_upper_tri (oski_index_t m, oski_index_t n,
		  oski_index_t i, oski_index_t j)
{
  return filter_strictly_upper_tri (m, n, i, j)
    || filter_diagonal (m, n, i, j);
}

/**
 *  \brief Create a raw CSR matrix, using 0-based indices, with
 *  a bounded number of non-zeros per row.
 *
 *  This routine aborts the program if a memory error occurs.
 */
static void
create_rawmat_csr (oski_index_t m, oski_index_t n,
		   oski_index_t min_nnz_row, oski_index_t max_nnz_row,
		   filter_funcpt filter,
		   oski_index_t ** p_ptr, oski_index_t ** p_ind,
		   oski_value_t ** p_val)
{
  oski_index_t i;
  oski_index_t next_pos;

  oski_index_t *ptr;
  oski_index_t *ind;
  oski_value_t *val;

  if (filter == NULL)
    filter = filter_valid;	/* default filter */

  if (max_nnz_row < min_nnz_row)
    max_nnz_row = n;

  /* Preallocate data structure. */
  ptr = oski_Malloc (oski_index_t, m + 1);
  ABORT (ptr == NULL, create_rawmat_csr, ERR_OUT_OF_MEMORY);

  ind = oski_Malloc (oski_index_t, m * max_nnz_row);
  ABORT (ind == NULL, create_rawmat_csr, ERR_OUT_OF_MEMORY);

  ptr[0] = next_pos = 0;
  for (i = 0; i < m; i++)
    {				/* create one row at a time */
      /* Generate between min_nnz_row and max_nnz_row entries */
      oski_index_t nnz_row = rand_GenIndex (min_nnz_row, max_nnz_row);
      oski_index_t k;

      for (k = 0; k < nnz_row; k++)
	{
	  oski_index_t j = rand_GenIndex (0, n - 1);

	  assert (next_pos < (m * max_nnz_row));

	  if ((*filter) (m, n, i + 1, j + 1))
	    {
	      ind[next_pos] = j;
	      next_pos++;
	    }
	}
      ptr[i + 1] = next_pos;
    }

  assert (next_pos <= m * max_nnz_row);

  /* Assign random values. */
  val = array_CreateRandom (ptr[m]);
  ABORT (val == NULL, create_rawmat_csr, ERR_OUT_OF_MEMORY);

  /* Return */
  *p_ptr = ptr;
  *p_ind = ind;
  *p_val = val;
}

/**
 *  \brief Given an array of index pointers, adjust the base index.
 */
static void
adjust_base (oski_index_t * array, oski_index_t len, oski_index_t b)
{
  oski_index_t i;

  for (i = 0; i < len; i++)
    array[i] += b;
}

/**
 *  \brief Expands a raw, 0-based CSR matrix into a full-storage
 *  0-based CSR matrix if shape == MAT_SYMM_FULL or MAT_HERM_FULL.
 *
 *  \note This implementation just duplicates all the non-zeros in
 *  their transpose position, so column indices will not be unique.
 */
static void
expand_rawmat_csr (oski_index_t ** p_ptr, oski_index_t ** p_ind,
		   oski_value_t ** p_val, oski_index_t m, oski_index_t n,
		   oski_inmatprop_t shape)
{
  /* Source */
  const oski_index_t *sptr;
  const oski_index_t *sind;
  const oski_value_t *sval;

  /* Destination, i.e., expanded copy */
  oski_index_t *dptr;
  oski_index_t *dind;
  oski_value_t *dval;

  oski_index_t *count_nnz_row;
  oski_index_t i;

  if (shape != MAT_SYMM_FULL && shape != MAT_HERM_FULL)
    return;

  assert (m == n);

  ABORT (p_ptr == NULL, expand_rawmat_csr, ERR_BAD_ARG);
  ABORT (p_ind == NULL, expand_rawmat_csr, ERR_BAD_ARG);
  ABORT (p_val == NULL, expand_rawmat_csr, ERR_BAD_ARG);
  sptr = *p_ptr;
  sind = *p_ind;
  sval = *p_val;

  /* Count the # of new non-zeros to add to each row. */
  count_nnz_row = oski_Malloc (oski_index_t, m);
  ABORT (count_nnz_row == NULL, expand_rawmat_csr, ERR_OUT_OF_MEMORY);
  oski_ZeroMem (count_nnz_row, sizeof (oski_index_t) * m);

  for (i = 0; i < m; i++)
    {
      oski_index_t k;

      for (k = sptr[i]; k < sptr[i + 1]; k++)
	{
	  oski_index_t j = sind[k];

	  if (i != j)		/* only duplicate off-diagonal values */
	    count_nnz_row[j]++;
	}
    }

  /* Initialize destination row pointers */
  dptr = oski_Malloc (oski_index_t, m + 1);
  ABORT (dptr == NULL, expand_rawmat_csr, ERR_OUT_OF_MEMORY);

  dptr[0] = 0;
  for (i = 0; i < m; i++)
    {
      dptr[i + 1] = dptr[i] + sptr[i + 1] - sptr[i]	/* existing non-zeros */
	+ count_nnz_row[i]	/* new non-zeros */
	;
      count_nnz_row[i] = 0;	/* reset */
    }

  /* Reset count_nnz_row. */
  oski_ZeroMem (count_nnz_row, sizeof (oski_index_t) * m);

  /* Now count_nnz_row[i] == # of non-zeros copied to dind, dval */
  dind = oski_Malloc (oski_index_t, dptr[m]);
  ABORT (dind == NULL, expand_rawmat_csr, ERR_OUT_OF_MEMORY);
  dval = oski_Malloc (oski_value_t, dptr[m]);
  ABORT (dval == NULL, expand_rawmat_csr, ERR_OUT_OF_MEMORY);

  /* Copy existing non-zeros */
  for (i = 0; i < m; i++)
    {
      oski_index_t k;

      for (k = sptr[i]; k < sptr[i + 1]; k++)
	{
	  oski_index_t k_d = dptr[i] + count_nnz_row[i];

	  assert (k_d <= dptr[i + 1]);

	  dind[k_d] = sind[k];
	  VAL_ASSIGN (dval[k_d], sval[k]);

	  /* Mark as copied */
	  count_nnz_row[i]++;
	}
    }

  /* Copy new non-zeros */
  if (shape == MAT_SYMM_FULL)
    {
      for (i = 0; i < m; i++)
	{
	  oski_index_t k;

	  for (k = sptr[i]; k < sptr[i + 1]; k++)
	    {
	      /* Copy element into transpose position */
	      oski_index_t j = sind[k];
	      oski_index_t k_d;

	      if (i == j)
		continue;	/* skip diagonals */

	      k_d = dptr[j] + count_nnz_row[j];
	      assert (k_d <= dptr[j + 1]);

	      dind[k_d] = i;
	      VAL_ASSIGN (dval[k_d], sval[k]);

	      /* Mark as copied */
	      count_nnz_row[j]++;
	    }
	}
    }
  else
    {				/* shape == MAT_HERM_FULL */

      for (i = 0; i < m; i++)
	{
	  oski_index_t k;

	  for (k = sptr[i]; k < sptr[i + 1]; k++)
	    {
	      /* Copy conjugated element into transpose position */
	      oski_index_t j = sind[k];
	      oski_index_t k_d;

	      if (i == j)
		continue;	/* skip diagonals */

	      k_d = dptr[j] + count_nnz_row[j];
	      assert (k_d <= dptr[j + 1]);

	      dind[k_d] = i;
	      VAL_ASSIGN_CONJ (dval[k_d], sval[k]);

	      /* Mark as copied */
	      count_nnz_row[j]++;
	    }
	}
    }				/* Copy new non-zeros */

  oski_Free (*p_ptr);
  oski_Free (*p_ind);
  oski_Free (*p_val);

  *p_ptr = dptr;
  *p_ind = dind;
  *p_val = dval;
}

/**
 *  \brief
 *
 *  \param[in] m Number of rows.
 *  \param[in] n Number of columns.
 *  \param[in] min_nnz_row Minimum # of non-zeros per row.
 *  \param[in] max_nnz_row Maximum # of non-zeros per row. If max_nnz_row
 *  is less than min_nnz_row, then it is taken to be n.
 *  \param[in] shape Matrix shape (MAT_GENERAL ... MAT_HERM_FULL).
 *  \param[in] implicit_diag Set to 1 if the diagonal should be
 *  unit and implicit, or 0 otherwise.
 *  \param[in] index_base Set to 1 or 0 to set the base index.
 *  \param[in,out] p_ptr Pointer to buffer to hold the row pointers,
 *  or NULL if none is desired.
 *  \param[in,out] p_ind Pointer to buffer to hold the column indices,
 *  or NULL if none is desired.
 *  \param[in,out] p_val Pointer to buffer to hold the stored non-zero
 *  values, or NULL if none is desired.
 *
 *  \returns On success, returns a valid matrix handle and sets
 *  (*p_ptr, *p_ind, *p_val) to point to the raw CSR representation.
 *  Otherwise, aborts the program.
 *
 *  \note If p_ptr, p_ind, or p_val are NULL but the copy mode is
 *  set to SHARE_INPUTMAT, then there is the potential for a memory
 *  leak.
 */
oski_matrix_t
testmat_GenRandomCSR (oski_index_t m, oski_index_t n,
		      oski_index_t min_nnz_row, oski_index_t max_nnz_row,
		      oski_inmatprop_t shape, int implicit_diag,
		      int index_base,
		      oski_copymode_t copymode,
		      oski_index_t ** p_ptr, oski_index_t ** p_ind,
		      oski_value_t ** p_val)
{
  oski_index_t *ptr;
  oski_index_t *ind;
  oski_value_t *val;
  oski_matrix_t A;

  filter_funcpt filter;

  /* Determine filter based on shape */
  switch (shape)
    {
    case MAT_GENERAL:
      filter = implicit_diag ? filter_offdiag : filter_valid;
      break;

    case MAT_SYMM_FULL:
    case MAT_HERM_FULL:
    case MAT_SYMM_UPPER:
    case MAT_HERM_UPPER:
    case MAT_TRI_UPPER:
      filter = implicit_diag ? filter_strictly_upper_tri : filter_upper_tri;
      break;

    case MAT_SYMM_LOWER:
    case MAT_HERM_LOWER:
    case MAT_TRI_LOWER:
      filter = implicit_diag ? filter_strictly_lower_tri : filter_lower_tri;
      break;

    default:
      ABORT (1, check_case, ERR_BAD_ARG);
    }

  /* Ensure square matrix, using the largest dimension */
  if (shape != MAT_GENERAL)
    {
      m = (m > n) ? m : n;
      n = m;
    }

  if (max_nnz_row < min_nnz_row)
    max_nnz_row = n;

  /* Create raw pattern of values */
  create_rawmat_csr (m, n, min_nnz_row, max_nnz_row, filter, &ptr, &ind,
		     &val);
  expand_rawmat_csr (&ptr, &ind, &val, m, n, shape);

  if (index_base)
    {
      adjust_base (ind, ptr[m], index_base);
      adjust_base (ptr, m + 1, index_base);
    }

  /* Create logical matrix object */
  A = oski_CreateMatCSR (ptr, ind, val, m, n, copymode,
			 3, shape,
			 implicit_diag ? MAT_UNIT_DIAG_IMPLICIT :
			 MAT_DIAG_EXPLICIT,
			 index_base ? INDEX_ONE_BASED : INDEX_ZERO_BASED);
  ABORT (A == INVALID_MAT, create_matrix, ERR_OUT_OF_MEMORY);

  /* Return */
  if (p_ptr != NULL)
    *p_ptr = ptr;
  else if (copymode == COPY_INPUTMAT)
    oski_Free (ptr);
  if (p_ind != NULL)
    *p_ind = ind;
  else if (copymode == COPY_INPUTMAT)
    oski_Free (ind);
  if (p_val != NULL)
    *p_val = val;
  else if (copymode == COPY_INPUTMAT)
    oski_Free (val);
  return A;
}

/**
 *  \brief Returns the minimum and maximum non-zero column index
 *  for a given row of an m x m triangular matrix with the
 *  prescribed shape (lower or upper triangular).
 *
 *  \param[in] shape MAT_TRI_LOWER or MAT_TRI_UPPER.
 *  \param[in] m Matrix dimension.
 *  \param[in] i Row index (0-based).
 *  \param[out] p_j_min Pointer to variable in which to hold the
 *  minimum non-zero column index (0-based).
 *  \param[out] p_j_max Pointer to variable in which to hold the
 *  maximum non-zero column index (0-based).
 *
 *  \returns Returns the minimum and maximum bounds on the
 *  non-zero column index. Aborts the program if 'shape' is
 *  invalid. Does not store any result in *p_j_min (or
 *  *p_j_max) if it is NULL.
 */
static void
GetShapeBounds (oski_inmatprop_t shape, oski_index_t m, oski_index_t i,
		oski_index_t * p_j_min, oski_index_t * p_j_max)
{
  oski_index_t j_min = 0;
  oski_index_t j_max = m - 1;

  switch (shape)
    {
    case MAT_TRI_LOWER:
      j_max = i;
      break;
    case MAT_TRI_UPPER:
      j_min = i;
      break;
    default:
      ABORT (1, GetShapeBounds, ERR_BAD_ARG);
    }

  if (p_j_min != NULL)
    *p_j_min = j_min;
  if (p_j_max != NULL)
    *p_j_max = j_max;
}

void
testmat_GenTriCSC (oski_index_t m, oski_index_t max_nnz_row,
		   oski_inmatprop_t shape, int is_unitdiag, int is_sorted,
		   int indbase, oski_index_t ** p_ptr,
		   oski_index_t ** p_ind, oski_value_t ** p_val)
{
  ABORT (shape != MAT_TRI_LOWER && shape != MAT_TRI_UPPER,
	 testmat_GenTriCSC, ERR_BAD_ARG);
  if (shape == MAT_TRI_LOWER)
    testmat_GenTriCSR (m, max_nnz_row, MAT_TRI_UPPER, is_unitdiag,
		       is_sorted, indbase, p_ptr, p_ind, p_val);
  else				/* shape == MAT_TRI_UPPER */
    testmat_GenTriCSR (m, max_nnz_row, MAT_TRI_LOWER, is_unitdiag,
		       is_sorted, indbase, p_ptr, p_ind, p_val);
}

/**
 *  \brief
 *
 *  \param[in] m Dimension of the desired matrix.
 *  \param[in] max_nnz_row Maximum desired number of non-zeros
 *  per row.
 *  \param[in] shape MAT_TRI_LOWER or MAT_TRI_UPPER.
 *  \param[in] is_unitdiag Set to 0 if the matrix should have
 *  \param[in] indbase Index base, 0 or 1.
 *  an implicit unit diagonal, or non-zero otherwise.
 *  \param[in] is_sorted Set to any non-zero value if the column
 *  indices should be sorted in increasing order.
 *  \param[in,out] p_ptr Pointer in which to store a newly allocated
 *  array of row pointers for the matrix, or NULL if not desired.
 *  \param[in,out] p_ind Pointer in which to store a newly allocated
 *  array of column indices for the matrix, or NULL if not desired.
 *  \param[in,out] p_val Pointer in which to store a newly allocated
 *  array of non-zero values, or NULL if not desired.
 */
void
testmat_GenTriCSR (oski_index_t m, oski_index_t max_nnz_row,
		   oski_inmatprop_t shape, int is_unitdiag, int is_sorted,
		   int indbase, oski_index_t ** p_ptr,
		   oski_index_t ** p_ind, oski_value_t ** p_val)
{
  oski_index_t *ptr;
  oski_index_t *ind;
  oski_value_t *val;
  oski_index_t i;

  ptr = oski_Malloc (oski_index_t, m + 1);
  ABORT (ptr == NULL, testmat_GenTriCSR, ERR_OUT_OF_MEMORY);

  /*  Select a random number of non-zeros per row greater than 1 but
   *  not in excess of max_nnz_row. This value includes the diagonal
   *  element if !is_unitdiag.
   */
  ptr[0] = 0;
  for (i = 0; i < m; i++)
    {				/* i == row index, 0-based */
      /* min/max column index for row i, 0-based, excluding diagonal */
      oski_index_t j_min, j_max;

      /* # non-zeros to be allocated in this row, excluding diagonal */
      oski_index_t nnz_this_row;

      GetShapeBounds (shape, m, i, &j_min, &j_max);
      nnz_this_row = rand_GenIndex (0, j_max - j_min) + (is_unitdiag == 0);

      ptr[i + 1] = ptr[i] + nnz_this_row;
    }

  /* Allocate space for the prescribed number of non-zeros */
  ind = oski_Malloc (oski_index_t, ptr[m]);
  ABORT (ind == NULL, testmat_GenTriCSR, ERR_OUT_OF_MEMORY);
  val = oski_Malloc (oski_value_t, ptr[m]);
  ABORT (val == NULL, testmat_GenTriCSR, ERR_OUT_OF_MEMORY);

  /* Next, create random indices and values */
  for (i = 0; i < m; i++)
    {
      oski_index_t j_min, j_max;
      oski_index_t k;		/* iterate over non-zeros */
      oski_index_t offdiag_nnz;	/* # of off-diagonal non-zeros in row i */

      GetShapeBounds (shape, m, i, &j_min, &j_max);

      offdiag_nnz = ptr[i + 1] - ptr[i] - (is_unitdiag == 0);
      if (offdiag_nnz > 0)
	for (k = ptr[i]; k < ptr[i] + offdiag_nnz; k++)
	  {
	    oski_value_t aij = rand_GenValue (0, 1.0 / m);
	    oski_index_t j;

	    do
	      {
		j = rand_GenIndex (j_min, j_max);
	      }
	    while (j == i);

	    ind[k] = j;
	    VAL_ASSIGN (val[k], aij);
	  }

      if (!is_unitdiag)
	{
	  /* Select diagonal so system is well-conditioned */
	  oski_value_t aii = rand_GenValue (1, 2);

	  ind[ptr[i + 1] - 1] = i;
	  VAL_ASSIGN (val[ptr[i + 1] - 1], aii);
	}
    }

  if (is_sorted)
    for (i = 0; i < m; i++)
      array_SortIndValPairs (ind + ptr[i], val + ptr[i], ptr[i + 1] - ptr[i]);
  else				/* !is_sorted */
    for (i = 0; i < m; i++)
      rand_PermuteIndValPairs (ind + ptr[i], val + ptr[i],
			       ptr[i + 1] - ptr[i]);

  if (indbase > 0)
    {				/* want 1-based indices */
      for (i = 0; i < ptr[m]; i++)
	ind[i]++;
      for (i = 0; i <= m; i++)
	ptr[i]++;
    }

  if (p_ptr != NULL)
    *p_ptr = ptr;
  else
    oski_Free (ptr);
  if (p_ind != NULL)
    *p_ind = ind;
  else
    oski_Free (ind);
  if (p_val != NULL)
    *p_val = val;
  else
    oski_Free (val);
}

/* eof */
