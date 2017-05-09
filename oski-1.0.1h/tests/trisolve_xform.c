/**
 *  \file tests/trisolve.c
 *  \brief Comprehensive test of sparse triangular solve for an
 *  arbitrary data structure transformation (specified by an
 *  OSKI-Lua program).
 *
 *  This program generates random but well-conditioned triangular
 *  matrices, and uses a crude norm-check to test the results
 *  of calling oski_MatTrisolve().
 *
 *  This initial matrix starts, by default, in CSR format. To
 *  start initially in CSC format, define the compile-time
 *  macro, TEST_CSC. If no transform is specified on the
 *  command-line, this will by default test CSC format if
 *  the initial format is CSR, or CSR format if the initial
 *  format is CSC.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/tune.h>

#include "abort_prog.h"
#include "testvec.h"

#if !defined(TEST_CSC)
/** By default, use CSR (not CSC) as the initial format */
#define TEST_CSC 0
#endif

/** \name Matrix parameter test lists. */
/*@{*/
#define NUM_UNIT_DIAG 2
static const int TL_unit_diag[NUM_UNIT_DIAG] = { 0, 1 };

#define NUM_SHAPE 2
static const oski_inmatprop_t TL_shape[NUM_SHAPE] =
  { MAT_TRI_LOWER, MAT_TRI_UPPER };

#define NUM_SORT_INDS 2
static const int TL_sort_inds[NUM_SORT_INDS] = { 0, 1 };

#define NUM_INDBASE 2
static const int TL_indbase[NUM_INDBASE] = { 0, 1 };

/*@}*/

/** \name Vector parameter test lists. */
/*@{*/
#define NUM_NUM_VECS 2
static const oski_index_t TL_num_vecs[NUM_NUM_VECS] = { 1, 3 };

#define NUM_USE_MINSTRIDE 2
static const int TL_use_minstride[NUM_USE_MINSTRIDE] = { 0, 1 };

#define NUM_ORIENT 2
static const oski_storage_t TL_orient[NUM_ORIENT] =
  { LAYOUT_COLMAJ, LAYOUT_ROWMAJ };
/*@}*/

/** \name Triangular solve kernel parameter test lists. */
/*@{*/
#define NUM_OP 3
static const oski_matop_t TL_op[NUM_OP] =
  { OP_NORMAL, OP_TRANS, OP_CONJ_TRANS };

#define NUM_ALPHA 3
static const oski_value_t TL_alpha[NUM_ALPHA] = {
  MAKE_VAL_COMPLEX (1.0, 0.0),
  MAKE_VAL_COMPLEX (-1.0, 0.0),
  MAKE_VAL_COMPLEX (.5, -.25)
};

/*@}*/

/** \brief Returns a handle to a newly allocated matrix. */
static oski_matrix_t
GenTriMat (oski_index_t m, oski_inmatprop_t shape,
	   int unitdiag, int sorted, int indbase)
{
  oski_index_t max_nnz_row = 100;

  oski_index_t *ptr;
  oski_index_t *ind;
  oski_value_t *val;

  oski_matrix_t T;

#if TEST_CSC

  testmat_GenTriCSC (m, max_nnz_row, shape, unitdiag, sorted, indbase,
		     &ptr, &ind, &val);
  T = oski_CreateMatCSC (ptr, ind, val, m, m, COPY_INPUTMAT, 4,
			 shape,
			 unitdiag ? MAT_UNIT_DIAG_IMPLICIT :
			 MAT_DIAG_EXPLICIT,
			 sorted ? INDEX_SORTED : INDEX_UNSORTED,
			 indbase ? INDEX_ONE_BASED : INDEX_ZERO_BASED);

#else /* !TEST_CSC -- test CSR by default */

  testmat_GenTriCSR (m, max_nnz_row, shape, unitdiag, sorted, indbase,
		     &ptr, &ind, &val);
  T = oski_CreateMatCSR (ptr, ind, val, m, m, COPY_INPUTMAT, 4,
			 shape,
			 unitdiag ? MAT_UNIT_DIAG_IMPLICIT :
			 MAT_DIAG_EXPLICIT,
			 sorted ? INDEX_SORTED : INDEX_UNSORTED,
			 indbase ? INDEX_ONE_BASED : INDEX_ZERO_BASED);
#endif

  ABORT (T == INVALID_MAT, GenTriMat, ERR_OUT_OF_MEMORY);
  oski_Free (ptr);
  oski_Free (ind);
  oski_Free (val);
  return T;
}

/**
 *  \brief Checks a solution \f$x\f$ computed by a triangular solve
 *  routine against the true solution \f$x_{\mathrm{true}}\f$,
 *  assuming the original triangular matrix had condition number
 *  bounded by 1.
 */
static void
Check (const oski_vecview_t x_true, oski_vecview_t x)
{
  oski_index_t vec_len = x_true->num_rows;
  oski_index_t num_vecs = x_true->num_cols;

  oski_index_t k;

  /* Crude bound */
  const double threshold = VAL_EPS * 50.0 * vec_len;

  for (k = 0; k < num_vecs; k++)
    {
      const oski_value_t *xp_true = x_true->val + k * x_true->colinc;
      const oski_value_t *xp = x->val + k * x->colinc;

      oski_index_t i;
      double max_mag_diff = 0;

      oski_PrintDebugMessage (3,
			      "\t\t\tChecking vector #%d (threshold = %g)...",
			      (int) k, threshold);

      for (i = 0; i < vec_len; i++)
	{
	  oski_value_t di;	/* difference */
	  double mag_di;	/* magnitude of difference */

	  VAL_ASSIGN (di, xp_true[0]);
	  VAL_DEC (di, xp[0]);

	  mag_di = VAL_ABS (di);
#if 0
	  fprintf (stderr, "[%d] |%g - %g| == %g\n", i, xp_true[0], xp[0], di);
#endif
	  if (mag_di > max_mag_diff)
	    max_mag_diff = mag_di;

	  xp_true += x_true->rowinc;
	  xp += x->rowinc;
	}

      oski_PrintDebugMessage (3,
			      "\t\t\t||x_true - x||_\\infty = %g",
			      max_mag_diff);
      ABORT (max_mag_diff > threshold, Check, ERR_WRONG_ANS);
    }
}

static void
TestKernel (oski_index_t m, const oski_matrix_t T, const oski_matrix_t T_copy,
	    const oski_vecview_t x_true)
{
  int i;
  for (i = 0; i < NUM_ALPHA; i++)
    {
      oski_value_t alpha = TL_alpha[i];
      const oski_value_t ZERO = MAKE_VAL_COMPLEX (0.0, 0.0);

      oski_value_t inv_alpha;
      int j;

      VAL_SET_ONE (inv_alpha);
      VAL_DIVEQ (inv_alpha, alpha);
      for (j = 0; j < NUM_OP; j++)
	{
	  oski_matop_t opT = TL_op[j];
	  oski_vecview_t b_true, x, x_copy;
	  int err;

	  oski_PrintDebugMessage (1, "\t\t-- Kernel options:");
#if IS_VAL_COMPLEX
	  oski_PrintDebugMessage (1, " alpha=%g+%gi (inv=%g+%gi)",
				  _RE (alpha), _IM (alpha), _RE (inv_alpha),
				  _IM (inv_alpha));
#else
	  oski_PrintDebugMessage (1, " alpha=%g (inv=%g)", alpha, inv_alpha);
#endif
	  oski_PrintDebugMessage (1, " op(T)=%s",
				  opT == OP_NORMAL ? "T" : opT ==
				  OP_TRANS ? "T^T" : opT ==
				  OP_CONJ ? "conj(T)" : "conj(T)^T");
	  oski_PrintDebugMessage (1, "");

	  b_true = testvec_Clone (x_true);
	  ABORT (b_true == INVALID_VEC, TestKernel, ERR_OUT_OF_MEMORY);

	  /* First, compute right-hand side for true solution. */
	  err = oski_MatMult (T, opT, inv_alpha, x_true, ZERO, b_true);
	  ABORT (err != 0, TestKernel, err);

	  /* Now perform triangular solve */
	  x = testvec_Clone (b_true);
	  ABORT (x == INVALID_VEC, TestKernel, ERR_OUT_OF_MEMORY);
	  err = oski_MatTrisolve (T, opT, alpha, x);
	  ABORT (err != 0, TestKernel, err);
	  Check (x_true, x);

	  x_copy = testvec_Clone (b_true);
	  ABORT (x_copy == INVALID_VEC, TestKernel, ERR_OUT_OF_MEMORY);
	  err = oski_MatTrisolve (T_copy, opT, alpha, x_copy);
	  ABORT (err != 0, TestKernel, err);
	  Check (x_true, x_copy);

	  /* Clean-up vectors */
	  testvec_Destroy (x_copy);
	  testvec_Destroy (x);
	  testvec_Destroy (b_true);
	}
    }
}

static void
TestVec (oski_index_t m, const oski_matrix_t T, const oski_matrix_t T_copy)
{
  int i;
  for (i = 0; i < NUM_NUM_VECS; i++)
    {
      oski_index_t num_vecs = TL_num_vecs[i];
      int j;
      for (j = 0; j < NUM_USE_MINSTRIDE; j++)
	{
	  oski_index_t use_minstride = TL_use_minstride[j];
	  int k;
	  for (k = 0; k < NUM_ORIENT; k++)
	    {
	      oski_storage_t x_orient = TL_orient[k];
	      oski_vecview_t x_true = testvec_Create (m, num_vecs,
						      x_orient,
						      use_minstride);
	      ABORT (x_true == INVALID_VEC, TestVec, ERR_OUT_OF_MEMORY);

	      oski_PrintDebugMessage (1, "\t-- Vector options:");
	      oski_PrintDebugMessage (1, " nv=%d", num_vecs);
	      oski_PrintDebugMessage (1, " minstride=%d", use_minstride);
	      oski_PrintDebugMessage (1, " layout=%s",
				      x_orient ==
				      LAYOUT_ROWMAJ ? "row" : "col");
	      oski_PrintDebugMessage (1, "");

	      TestKernel (m, T, T_copy, x_true);

	      testvec_Destroy (x_true);
	    }
	}
    }
}

static void
TestAll (oski_index_t m, const char *xform)
{
  int i;
  for (i = 0; i < NUM_SHAPE; i++)
    {
      oski_inmatprop_t shape = TL_shape[i];
      int j;
      for (j = 0; j < NUM_UNIT_DIAG; j++)
	{
	  int unitdiag = TL_unit_diag[j];
	  int k;
	  for (k = 0; k < NUM_SORT_INDS; k++)
	    {
	      int sorted = TL_sort_inds[k];
	      int l;
	      for (l = 0; l < NUM_INDBASE; l++)
		{
		  int indbase = TL_indbase[l];

		  oski_matrix_t T =
		    GenTriMat (m, shape, unitdiag, sorted, indbase);
		  oski_matrix_t T_copy = oski_CopyMat (T);
		  int err;

		  ABORT (T_copy == NULL, TestAll, ERR_OUT_OF_MEMORY);
		  err = oski_ApplyMatTransforms (T_copy, xform);
		  ABORT (err != 0, TestAll, err);

		  oski_PrintDebugMessage (1, "-- Matrix options:");
		  oski_PrintDebugMessage (1, " shape=%s",
					  shape ==
					  MAT_TRI_LOWER ? "lower" : "upper");
		  oski_PrintDebugMessage (1, " unitdiag=%d", unitdiag);
		  oski_PrintDebugMessage (1, " sorted=%d", sorted);
		  oski_PrintDebugMessage (1, " indbase=%d", indbase);
		  oski_PrintDebugMessage (1, "");

		  TestVec (m, T, T_copy);
		  oski_DestroyMat (T_copy);
		  oski_DestroyMat (T);
		}
	    }
	}
    }
}

int
main (int argc, char *argv[])
{
  char *xform = NULL;
  if (argc <= 1)
    {
      fprintf (stderr, "\nusage: %s <xform>\n\n", argv[0]);
      return -1;
    }
  xform = argv[1];

  ABORT (oski_Init () == 0, main, ERR_INIT_LIB);

  oski_PrintDebugMessage (1,
			  "///////////////////////////////////////////////");
  oski_PrintDebugMessage (1, "// Testing: '%s'", xform);
  oski_PrintDebugMessage (1,
			  "///////////////////////////////////////////////");
  TestAll (47, xform);

  oski_Close ();
  return 0;
}

/* eof */
