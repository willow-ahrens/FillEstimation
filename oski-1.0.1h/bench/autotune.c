/**
 *  \file bench/autotune.c
 *  \brief Generic SpMV benchmarking utility.
 *  \ingroup OFFBENCH
 *
 *  This utility benchmarks an arbitrary workload before and
 *  after automatic tuning.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <oski/oski.h>
#include <oski/simplelist.h>

#include "abort_prog.h"
#include "testvec.h"
#include "timing.h"
#include "parse_opts.h"
#include "readhbpat.h"
#include "keropts.h"

/** \brief Print command-line options. */
static void
usage (const char *progname)
{
  fprintf (stderr,
	   "\n"
	   "usage: %s [options] <matrix> <kernel_options_1>\n"
	   "                             [kernel_options_2 ...]\n"
	   "where\n"
	   "   <matrix>\n"
	   "      is the name of a Harwell-Boeing formatted matrix file;\n"
	   "   {<kernel_options_k>}\n"
	   "      is a set describing a kernel workload.\n"
	   "\n"
	   "Valid [options] are:\n"
	   "   --workload {explicit, implicit}\n"
	   "       Default: explicit\n"
	   "   --tune {auto, always, aggressive}\n"
	   "       Default: auto\n"
           "   --expand-symm | --no-expand-symm\n"
           "       Default: --no-expand-symm\n"
           "\n", progname);
  keropts_Usage (stderr);
}

static void
abort_option_error (const char *flag, const char *bad_value,
		    const char *good_value_1, ...)
{
  va_list ap;
  const char *value;

  fprintf (stderr, "*** '%s' is not a valid value for option '%s'.\n",
	   bad_value, flag);
  fprintf (stderr, "    Valid values are: %s", good_value_1);
  va_start (ap, good_value_1);
  value = va_arg (ap, const char *);
  while (value != NULL)
    {
      fprintf (stderr, ", %s", value);
      value = va_arg (ap, const char *);
    }
  va_end (ap);
  fprintf (stderr, "\n");
  exit (1);
}

static simplelist_t *
process_options (int argc, char *argv[],
		 char **p_matfile,
		 int *p_is_workload_explicit, int *p_always_tune,
                 int *p_expand_symm)
{
  simplelist_t *workload = simplelist_Create ();
  int i = 1;

  ABORT (workload == NULL, process_options, ERR_OUT_OF_MEMORY);
  assert (p_matfile != NULL);

  /* Global options */
  while (i < argc && argv[i][0] == '-')
    {
      if (StringMatches (argv[i], "--workload", NULL)
	  && (i + 1) < argc && p_is_workload_explicit != NULL)
	{
	  int wtype = StringMatches (argv[++i], "explicit", "implicit", NULL);
	  switch (wtype)
	    {
	    case 1:
	      *p_is_workload_explicit = 1;
	      break;
	    case 2:
	      *p_is_workload_explicit = 0;
	      break;
	    default:
	      abort_option_error ("--workload", argv[i],
				  "explicit", "implicit", NULL);
	    }
	}
      else if (StringMatches (argv[i], "--tune", NULL)
	       && (i + 1) < argc && p_always_tune != NULL)
	{
	  int tune_type = StringMatches (argv[++i],
					 "auto", "always", "aggressive");
	  switch (tune_type)
	    {
	    case 1:
	      *p_always_tune = 0;
	      break;
	    case 2:
	      *p_always_tune = ALWAYS_TUNE;
	      break;
	    case 3:
	      *p_always_tune = ALWAYS_TUNE_AGGRESSIVELY;
	      break;
	    default:
	      abort_option_error ("--tune", argv[i],
				  "auto", "always", "aggressive");
	    }
	}
      else if (StringMatches (argv[i], "--expand-symm", NULL)
               && p_expand_symm)
          *p_expand_symm = 1;
      else if (StringMatches (argv[i], "--no-expand-symm", NULL)
               && p_expand_symm)
        *p_expand_symm = 0;
      else if (StringMatches (argv[i], "--help", "-h", "-H", NULL))
	{
	  usage (argv[0]);
	  exit (0);
	}
      else
	{
	  fprintf (stderr, "*** Unrecognized option, '%s'. ***\n", argv[i]);
	  exit (1);
	}
      i++;
    }

  /* Required options */
  if ((i + 2) >= argc)
    {
      usage (argv[0]);
      exit (1);
    }

  *p_matfile = argv[i];
  i++;

  /* Workload specification */
  do
    {
      kernel_t *new_opt = NULL;
      int i_next = keropts_Process (argc, argv, i, &new_opt);
      size_t i_opt;

      /* error occurred while processing */
      if (new_opt == NULL || i_next <= i)
	{
	  fprintf (stderr,
		   "*** Option '%s' is unrecognized or specified incorrectly\n",
		   argv[i]);
	  exit (1);
	}
      i_opt = simplelist_Append (workload, new_opt);
      ABORT (i_opt == 0, process_options, ERR_OUT_OF_MEMORY);

      assert (i_next > i);
      i = i_next;
    }
  while (i < argc);

  if (workload == NULL || simplelist_GetLength (workload) == 0)
    {
      usage (argv[0]);
      exit (1);
    }
  return workload;
}

/** Allocate temporary arrays for simulating an SpMV call */
static void
create_workload_data_MatMult (oski_index_t m, oski_index_t n,
			      kerargs_MatMult_t * opt)
{
  oski_index_t x_len = 0;
  oski_index_t y_len = 0;
  assert (m >= 1 && n >= 1 && opt != NULL);
  switch (opt->op)
    {
    case OP_NORMAL:
    case OP_CONJ:
      x_len = n;
      y_len = m;
      break;
    case OP_TRANS:
    case OP_CONJ_TRANS:
      x_len = m;
      y_len = n;
      break;
    default:
      assert (0);
      break;
    }
  opt->x = testvec_Create (x_len, opt->num_vecs, opt->x_layout, 1);
  opt->y = testvec_Create (y_len, opt->num_vecs, opt->y_layout, 1);

  fprintf (stderr, "Simulating %d calls to oski_MatMult():\n",
	   opt->num_calls);
  PrintMatTransOp (stderr, "A", opt->op);
  PrintValue (stderr, "alpha", opt->alpha);
  PrintVecViewInfo (stderr, "x", opt->x);
  PrintValue (stderr, "beta", opt->beta);
  PrintVecViewInfo (stderr, "y", opt->y);
}

/** Allocate temporary arrays for simulating a triangular solve call */
static void
create_workload_data_MatTrisolve (oski_index_t m, oski_index_t n,
				  kerargs_MatTrisolve_t * opt)
{
  oski_index_t x_len = (m > n) ? n : m;
  assert (m >= 1 && n >= 1 && opt != NULL);
  opt->x = testvec_Create (x_len, opt->num_vecs, opt->x_layout, 1);

  fprintf (stderr, "Simulating %d calls to oski_MatTrisolve():\n",
	   opt->num_calls);
  PrintMatTransOp (stderr, "A", opt->op);
  PrintValue (stderr, "alpha", opt->alpha);
  PrintVecViewInfo (stderr, "x", opt->x);
}

/** Allocate temporary arrays for simulating an A^T*A*x call */
static void
create_workload_data_MatTransMatMult (oski_index_t m, oski_index_t n,
				      kerargs_MatTransMatMult_t * opt)
{
  oski_index_t xy_len = 0;
  oski_index_t t_len = 0;
  assert (m >= 1 && n >= 1 && opt != NULL);

  switch (opt->op)
    {
    case OP_AT_A:
    case OP_AH_A:
      xy_len = n;
      t_len = m;
      break;
    case OP_A_AT:
    case OP_A_AH:
      xy_len = m;
      t_len = n;
      break;
    default:
      assert (0);
      break;
    }
  opt->x = testvec_Create (xy_len, opt->num_vecs, opt->x_layout, 1);
  opt->y = testvec_Create (xy_len, opt->num_vecs, opt->y_layout, 1);
  opt->t = testvec_Create (t_len, opt->num_vecs, opt->t_layout, 1);

  fprintf (stderr, "Simulating %d calls to oski_MatTransMatMult():\n",
	   opt->num_calls);
  PrintMatATAOp (stderr, "A", opt->op);
  PrintValue (stderr, "alpha", opt->alpha);
  PrintVecViewInfo (stderr, "x", opt->x);
  PrintValue (stderr, "beta", opt->beta);
  PrintVecViewInfo (stderr, "y", opt->y);
  PrintVecViewInfo (stderr, "t", opt->t);
}

/** Allocate temporary arrays for simulating two SpMV calls */
static void
create_workload_data_MatMultAndMatTransMult (oski_index_t m, oski_index_t n,
					     kerargs_MatMultAndMatTransMult_t
					     * opt)
{
  oski_index_t w_len = 0;
  oski_index_t z_len = 0;
  assert (m >= 1 && n >= 1 && opt != NULL);
  switch (opt->op)
    {
    case OP_NORMAL:
    case OP_CONJ:
      w_len = n;
      z_len = m;
      break;
    case OP_TRANS:
    case OP_CONJ_TRANS:
      w_len = m;
      z_len = n;
      break;
    default:
      assert (0);
      break;
    }
  opt->x = testvec_Create (n, opt->num_vecs, opt->x_layout, 1);
  opt->y = testvec_Create (m, opt->num_vecs, opt->y_layout, 1);
  opt->w = testvec_Create (w_len, opt->num_vecs, opt->w_layout, 1);
  opt->z = testvec_Create (z_len, opt->num_vecs, opt->z_layout, 1);

  fprintf (stderr, "Simulating %d calls to oski_MatMultAndMatTransMult():\n",
	   opt->num_calls);
  PrintValue (stderr, "alpha", opt->alpha);
  PrintVecViewInfo (stderr, "x", opt->x);
  PrintValue (stderr, "beta", opt->beta);
  PrintVecViewInfo (stderr, "y", opt->y);
  PrintMatTransOp (stderr, "A", opt->op);
  PrintValue (stderr, "omega", opt->omega);
  PrintVecViewInfo (stderr, "w", opt->w);
  PrintValue (stderr, "zeta", opt->zeta);
  PrintVecViewInfo (stderr, "z", opt->z);
}

/** Allocate temporary arrays for simulating an SpMV call */
static void
create_workload_data_MatPowMult (oski_index_t m, oski_index_t n,
				 kerargs_MatPowMult_t * opt)
{
  oski_index_t x_len = 0;
  oski_index_t y_len = 0;
  assert (m >= 1 && n >= 1 && opt != NULL);
  if (opt->power > 1)
    {
      assert (m == n);
    }
  switch (opt->op)
    {
    case OP_NORMAL:
    case OP_CONJ:
      x_len = n;
      y_len = m;
      break;
    case OP_TRANS:
    case OP_CONJ_TRANS:
      x_len = m;
      y_len = n;
      break;
    default:
      assert (0);
      break;
    }
  opt->x = testvec_Create (x_len, opt->num_vecs, opt->x_layout, 1);
  opt->y = testvec_Create (y_len, opt->num_vecs, opt->y_layout, 1);
  if (opt->power >= 2)
    opt->t = testvec_Create (y_len, opt->power - 1, opt->t_layout, 1);
  else
    opt->t = INVALID_VEC;

  fprintf (stderr, "Simulating %d calls to oski_MatPowMult():\n",
	   opt->num_calls);
  PrintMatTransOp (stderr, "A", opt->op);
  fprintf (stderr, "    Power k = %d\n", opt->power);
  PrintValue (stderr, "alpha", opt->alpha);
  PrintVecViewInfo (stderr, "x", opt->x);
  PrintValue (stderr, "beta", opt->beta);
  PrintVecViewInfo (stderr, "y", opt->y);
  PrintVecViewInfo (stderr, "t", opt->t);
}

/** Allocate temporary arrays for workload simulation */
static void
create_workload_data (oski_index_t num_rows, oski_index_t num_cols,
		      simplelist_t * workload)
{
  size_t i;
  assert (workload != NULL);
  for (i = 1; i <= simplelist_GetLength (workload); i++)
    {
      kernel_t *opt = (kernel_t *) simplelist_GetElem (workload, i);
      assert (opt != NULL);
      switch (opt->kernel)
	{
	case KERNEL_MatMult:
	  create_workload_data_MatMult (num_rows, num_cols,
					&(opt->args.MatMult));
	  break;
	case KERNEL_MatTrisolve:
	  create_workload_data_MatTrisolve (num_rows, num_cols,
					    &(opt->args.MatTrisolve));
	  break;
	case KERNEL_MatTransMatMult:
	  create_workload_data_MatTransMatMult (num_rows, num_cols,
						&(opt->args.MatTransMatMult));
	  break;
	case KERNEL_MatMultAndMatTransMult:
	  create_workload_data_MatMultAndMatTransMult (num_rows, num_cols,
						       &(opt->args.
							 MatMultAndMatTransMult));
	  break;
	case KERNEL_MatPowMult:
	  create_workload_data_MatPowMult (num_rows, num_cols,
					   &(opt->args.MatPowMult));
	  break;
	default:
	  assert (0);
	  break;
	}
    }
}

static void
set_workload_hints_MatMult (oski_matrix_t A, const kerargs_MatMult_t * opt)
{
  assert (A != INVALID_MAT && opt != NULL);
  oski_SetHintMatMult (A, opt->op, opt->alpha, opt->x, opt->beta, opt->y,
		       opt->num_calls);
}

static void
set_workload_hints_MatTrisolve (oski_matrix_t A,
				const kerargs_MatTrisolve_t * opt)
{
  assert (A != INVALID_MAT && opt != NULL);
  oski_SetHintMatTrisolve (A, opt->op, opt->alpha, opt->x, opt->num_calls);
}

static void
set_workload_hints_MatTransMatMult (oski_matrix_t A,
				    const kerargs_MatTransMatMult_t * opt)
{
  assert (A != INVALID_MAT && opt != NULL);
  oski_SetHintMatTransMatMult (A, opt->op, opt->alpha, opt->x,
			       opt->beta, opt->y, opt->t, opt->num_calls);
}

static void
set_workload_hints_MatMultAndMatTransMult (oski_matrix_t A,
					   const
					   kerargs_MatMultAndMatTransMult_t *
					   opt)
{
  assert (A != INVALID_MAT && opt != NULL);
  oski_SetHintMatMultAndMatTransMult (A,
				      opt->alpha, opt->x, opt->beta, opt->y,
				      opt->op, opt->omega, opt->w, opt->zeta,
				      opt->z, opt->num_calls);
}

static void
set_workload_hints_MatPowMult (oski_matrix_t A,
			       const kerargs_MatPowMult_t * opt)
{
  assert (A != INVALID_MAT && opt != NULL);
  oski_SetHintMatPowMult (A, opt->op, opt->power,
			  opt->alpha, opt->x, opt->beta, opt->y, opt->t,
			  opt->num_calls);
}

static int
change_numcalls (kernel_t * opt, int num_calls)
{
  int prev_calls = 0;
  switch (opt->kernel)
    {
    case KERNEL_MatMult:
      prev_calls = opt->args.MatMult.num_calls;
      opt->args.MatMult.num_calls = num_calls;
      break;
    case KERNEL_MatTrisolve:
      prev_calls = opt->args.MatTrisolve.num_calls;
      opt->args.MatTrisolve.num_calls = num_calls;
      break;
    case KERNEL_MatTransMatMult:
      prev_calls = opt->args.MatTransMatMult.num_calls;
      opt->args.MatTransMatMult.num_calls = num_calls;
      break;
    case KERNEL_MatMultAndMatTransMult:
      prev_calls = opt->args.MatMultAndMatTransMult.num_calls;
      opt->args.MatMultAndMatTransMult.num_calls = num_calls;
      break;
    case KERNEL_MatPowMult:
      prev_calls = opt->args.MatPowMult.num_calls;
      opt->args.MatPowMult.num_calls = num_calls;
      break;
    default: /* Should never occur? */
      break;
    }
  return prev_calls;
}

static void
set_workload_hints (oski_matrix_t A, simplelist_t * workload, int always_tune)
{
  size_t i;
  assert (A != INVALID_MAT);

  for (i = 1; i <= simplelist_GetLength (workload); i++)
    {
      kernel_t *opt = (kernel_t *) simplelist_GetElem (workload, i);
      int old_calls = 0;
      assert (opt != NULL);

      if (always_tune)
	old_calls = change_numcalls (opt, always_tune);

      switch (opt->kernel)
	{
	case KERNEL_MatMult:
	  set_workload_hints_MatMult (A, &(opt->args.MatMult));
	  break;
	case KERNEL_MatTrisolve:
	  set_workload_hints_MatTrisolve (A, &(opt->args.MatTrisolve));
	  break;
	case KERNEL_MatTransMatMult:
	  set_workload_hints_MatTransMatMult (A,
					      &(opt->args.MatTransMatMult));
	  break;
	case KERNEL_MatMultAndMatTransMult:
	  set_workload_hints_MatMultAndMatTransMult (A,
						     &(opt->args.
						       MatMultAndMatTransMult));
	  break;
	case KERNEL_MatPowMult:
	  set_workload_hints_MatPowMult (A, &(opt->args.MatPowMult));
	  break;
	default:
	  assert (0);
	  break;
	}

      if (always_tune)
	change_numcalls (opt, old_calls);
    }
}

static void
run_workload_MatMult (const oski_matrix_t A, kerargs_MatMult_t * opt)
{
  int i;
  assert (A != INVALID_MAT && opt != NULL);
  for (i = 0; i < opt->num_calls; i++)
    {
      int err = oski_MatMult (A, opt->op,
			      opt->alpha, opt->x, opt->beta, opt->y);
      ABORT (err != 0, run_workload_MatMult, err);
    }
}

static void
run_workload_MatTrisolve (const oski_matrix_t A, kerargs_MatTrisolve_t * opt)
{
  int i;
  assert (A != INVALID_MAT && opt != NULL);
  for (i = 0; i < opt->num_calls; i++)
    {
      int err = oski_MatTrisolve (A, opt->op, opt->alpha, opt->x);
      ABORT (err != 0, run_workload_MatTrisolve, err);
    }
}

static void
run_workload_MatTransMatMult (const oski_matrix_t A,
			      kerargs_MatTransMatMult_t * opt)
{
  int i;
  assert (A != INVALID_MAT && opt != NULL);
  for (i = 0; i < opt->num_calls; i++)
    {
      int err = oski_MatTransMatMult (A, opt->op, opt->alpha, opt->x,
				      opt->beta, opt->y, opt->t);
      ABORT (err != 0, run_workload_MatTransMatMult, err);
    }
}

static void
run_workload_MatMultAndMatTransMult (const oski_matrix_t A,
				     kerargs_MatMultAndMatTransMult_t * opt)
{
  int i;
  assert (A != INVALID_MAT && opt != NULL);
  for (i = 0; i < opt->num_calls; i++)
    {
      int err = oski_MatMultAndMatTransMult (A,
					     opt->alpha, opt->x, opt->beta,
					     opt->y,
					     opt->op, opt->omega, opt->w,
					     opt->zeta, opt->z);
      ABORT (err != 0, run_workload_MatMultAndMatTransMult, err);
    }
}

static void
run_workload_MatPowMult (const oski_matrix_t A, kerargs_MatPowMult_t * opt)
{
  int i;
  assert (A != INVALID_MAT && opt != NULL);
  for (i = 0; i < opt->num_calls; i++)
    {
      int err = oski_MatPowMult (A, opt->op, opt->power,
				 opt->alpha, opt->x, opt->beta, opt->y,
				 opt->t);
      ABORT (err != 0, run_workload_MatPowMult, err);
    }
}

static void
run_workload (const oski_matrix_t A, const simplelist_t * workload)
{
  size_t i;
  assert (A != INVALID_MAT);

  for (i = 1; i <= simplelist_GetLength (workload); i++)
    {
      kernel_t *opt = (kernel_t *) simplelist_GetElem (workload, i);
      assert (opt != NULL);
      switch (opt->kernel)
	{
	case KERNEL_MatMult:
	  run_workload_MatMult (A, &(opt->args.MatMult));
	  break;
	case KERNEL_MatTrisolve:
	  run_workload_MatTrisolve (A, &(opt->args.MatTrisolve));
	  break;
	case KERNEL_MatTransMatMult:
	  run_workload_MatTransMatMult (A, &(opt->args.MatTransMatMult));
	  break;
	case KERNEL_MatMultAndMatTransMult:
	  run_workload_MatMultAndMatTransMult (A,
					       &(opt->args.
						 MatMultAndMatTransMult));
	  break;
	case KERNEL_MatPowMult:
	  run_workload_MatPowMult (A, &(opt->args.MatPowMult));
	  break;
	default:
	  assert (0);
	  break;
	}
    }
}

/**
 *  \brief Given a workload, returns the number of flops executed per
 *  non-zero element.
 */
static double
count_workload_flops_per_nz (const simplelist_t * workload)
{
  double flops = 0.0;
  size_t i;
  for (i = 1; i <= simplelist_GetLength (workload); i++)
    {
      const kernel_t *opt =
	(const kernel_t *) simplelist_GetElem (workload, i);
      switch (opt->kernel)
	{
	case KERNEL_MatMult:
	  flops += 2.0
	    * opt->args.MatMult.num_vecs * opt->args.MatMult.num_calls;
	  break;
	case KERNEL_MatTrisolve:
	  flops += 2.0
	    * opt->args.MatTrisolve.num_vecs
	    * opt->args.MatTrisolve.num_calls;
	  break;
	case KERNEL_MatTransMatMult:
	  flops += 4.0
	    * opt->args.MatTransMatMult.num_vecs
	    * opt->args.MatTransMatMult.num_calls;
	  break;
	case KERNEL_MatMultAndMatTransMult:
	  flops += 4.0
	    * opt->args.MatMultAndMatTransMult.num_vecs
	    * opt->args.MatMultAndMatTransMult.num_calls;
	  break;
	case KERNEL_MatPowMult:
	  flops += 2.0
	    * opt->args.MatPowMult.power
	    * opt->args.MatPowMult.num_vecs * opt->args.MatPowMult.num_calls;
	  break;
	default:
	  assert (0);
	  break;
	}
    }
#if IS_VAL_COMPLEX
  flops *= 4.0;
#endif
  return flops;
}

/** Main benchmark program. */
int
main (int argc, char *argv[])
{
  /* Command-line options */
  char *matfile = NULL;
  simplelist_t *workload = NULL;	/* Kernel workload */
  oski_index_t num_rows, num_cols;	/* Matrix dimensions */
  oski_index_t nnz_stored, nnz;	/* # of non-zeros */

  /* Matrix */
  char mattype[4];
  oski_matrix_t A_ref;
  oski_matrix_t A_tuned;

  /* Timing stuff */
  oski_timer_t timer;
  double Mflop_count;
  double Mflops_obs_ref, Mflops_obs_tuned;

  /* Tuning options */
  int is_workload_explicit = 1;
  int always_tune = 0;
  int expand_symm = 0;
  int tune_result = 0;

  timer = oski_CreateTimer ();
  ABORT (timer == INVALID_TIMER, main, ERR_OUT_OF_MEMORY);
  workload = process_options (argc, argv, &matfile,
			      &is_workload_explicit,
                              &always_tune,
                              &expand_symm);

  /* Generate matrix and workload vectors */
  assert (matfile != NULL);
  oski_Init ();
  A_ref = readhb_pattern_matrix_nnz (matfile, &num_rows, &num_cols,
				     &nnz_stored, &nnz, mattype,
                                     expand_symm);
  ABORT (A_ref == INVALID_MAT, main, ERR_BAD_MAT);
  A_tuned = oski_CopyMat (A_ref);
  ABORT (A_tuned == INVALID_MAT, main, ERR_OUT_OF_MEMORY);

  create_workload_data (num_rows, num_cols, workload);
  Mflop_count = 1e-6 * nnz * count_workload_flops_per_nz (workload);

  /* Benchmark untuned version in CSR format */
  oski_ApplyMatTransforms (A_ref, "return CSR(InputMat)");
  fprintf (stderr, "Benchmarking untuned CSR workload ...\n");
  TIMING_LOOP_BASIC (run_workload (A_ref, workload),
		     1, Mflop_count, Mflops_obs_ref);

  /* Tune */
  fprintf (stderr, "Tuning...\n");
  if (is_workload_explicit)
    set_workload_hints (A_tuned, workload, always_tune);
  else				/* implicit workload */
    run_workload (A_tuned, workload);
  oski_RestartTimer (timer);
  tune_result = oski_TuneMat (A_tuned);
  if (tune_result == TUNESTAT_AS_IS)
    fprintf (stderr, "\tDid not tune.\n");
  else
    {
      char* xform = oski_GetMatTransforms (A_tuned);
      if (xform)
	{
	  fprintf (stderr, "\tDid tune: '%s'\n", xform);
	  oski_Free (xform);
	}
      else
	fprintf (stderr, "\t*** WARNING: Tuned, but can't read result? ***\n");
    }
  oski_StopTimer (timer);

  fprintf (stderr, "Benchmarking tuned workload ...\n");
  TIMING_LOOP_BASIC (run_workload (A_tuned, workload),
		     1, Mflop_count, Mflops_obs_tuned);

  printf ("%g %g %g %% %s\n", Mflops_obs_ref, Mflops_obs_tuned,
	  oski_ReadElapsedTime (timer), matfile);

  oski_DestroyTimer (timer);
  oski_Close ();
  return 0;
}

/* eof */
