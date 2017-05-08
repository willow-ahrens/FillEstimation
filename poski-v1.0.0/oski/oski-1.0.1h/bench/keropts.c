/**
 *  \file bench/keropts.c
 *  \brief Process kernel-specific command-line options
 *  \ingroup OFFBENCH
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <oski/oski.h>

#include "keropts.h"
#include "parse_opts.h"

void
keropts_Usage (FILE * fp)
{
  FILE *fp_out = fp == NULL ? stderr : fp;
  fprintf (fp_out,
	   "Kernel specification has the following format:\n"
	   "  <kernel> [options]\n"
	   "\n"
	   "where <kernel> is the name of a kernel, [options] are\n"
	   "specify the options with which to execute <kernel>:\n"
	   "\n"
	   "  MatMult\n"
	   "    [--op {normal, trans, conj_trans} | -o ...]\n"
	   "      Default: normal\n"
	   "    [--alpha <value> | -a <value>]\n"
	   "      Default: 1.0\n"
	   "    [--x-layout {row, col} | -x {row, col}]\n"
	   "      Default: col\n"
	   "    [--beta <value> | -b <value>]\n"
	   "      Default: 0.0\n"
	   "    [--y-layout {row, col} | -y {row, col}]\n"
	   "      Default: col\n"
	   "  MatTrisolve\n"
	   "    [--op {normal, trans, conj_trans} | -o ...]\n"
	   "      Default: normal\n"
	   "    [--alpha <value> | -a <value>]\n"
	   "      Default: 1.0\n"
	   "    [--x-layout {row, col} | -x {row, col}]\n"
	   "      Default: col\n"
	   "  MatTransMatMult\n"
	   "    [--op {ata, aat, aha, aah} | -o ...]\n"
	   "      Default: ata\n"
	   "    [--alpha <value> | -a <value>]\n"
	   "      Default: 1.0\n"
	   "    [--x-layout {row, col} | -x {row, col}]\n"
	   "      Default: col\n"
	   "    [--beta <value> | -b <value>]\n"
	   "      Default: 0.0\n"
	   "    [--y-layout {row, col} | -y {row, col}]\n"
	   "      Default: col\n"
	   "    [--t-layout {row, col} | -t {row, col}]\n"
	   "      Default: col\n"
	   "  MatMultAndMatTransMult\n"
	   "    [--alpha <value> | -a <value>]\n"
	   "      Default: 1.0\n"
	   "    [--x-layout {row, col} | -x {row, col}]\n"
	   "      Default: col\n"
	   "    [--beta <value> | -b <value>]\n"
	   "      Default: 0.0\n"
	   "    [--y-layout {row, col} | -y {row, col}]\n"
	   "      Default: col\n"
	   "    [--op {normal, trans, conj_trans} | -o ...]\n"
	   "      Default: normal\n"
	   "    [--omega <value> | -g <value>]\n"
	   "      Default: 1.0\n"
	   "    [--w-layout {row, col} | -w {row, col}]\n"
	   "      Default: col\n"
	   "    [--zeta <value> | -e <value>]\n"
	   "      Default: 0.0\n"
	   "    [--z-layout {row, col} | -z {row, col}]\n"
	   "      Default: col\n"
	   "  MatPowMult\n"
	   "    [--op {normal, trans, conj_trans} | -o ...]\n"
	   "      Default: normal\n"
	   "    [--power <integer>]\n"
	   "      Default: 2\n"
	   "    [--alpha <value> | -a <value>]\n"
	   "      Default: 1.0\n"
	   "    [--x-layout {row, col} | -x {row, col}]\n"
	   "      Default: col\n"
	   "    [--beta <value> | -b <value>]\n"
	   "      Default: 0.0\n"
	   "    [--y-layout {row, col} | -y {row, col}]\n"
	   "      Default: col\n"
	   "    [--t-layout {row, col} | -t {row, col}]\n"
	   "      Default: col\n"
	   "\n"
	   "The following additional options are valid for all\n"
	   "kernels:\n"
	   "    [--num-vecs <v> | -v ...]\n"
	   "       Default: 1\n"
	   "    [--num-calls <c> | -c ...]\n" "       Default: 1\n" "\n");
}

static int
process_MatMult (int argc, char *argv[], int i_start,
		 kerargs_MatMult_t * args)
{
  int i;
  assert (args != NULL);
  if (i_start >= argc)
    return i_start - 1;

  /* Set default options */
  args->num_calls = 1;
  args->num_vecs = 1;
  args->op = OP_NORMAL;
  VAL_SET_ONE (args->alpha);
  VAL_SET_ZERO (args->beta);
  args->x_layout = LAYOUT_COLMAJ;
  args->y_layout = LAYOUT_COLMAJ;

  /* Override */
  i = i_start;
  while (i < argc && argv[i][0] == '-')
    {
      if (StringMatches (argv[i], "--op", "-o", NULL) && (i + 1) < argc)
	args->op = ParseMatTransOp (argv[++i]);
      else if (StringMatches (argv[i], "--alpha", "-a", NULL)
	       && (i + 1) < argc)
	args->alpha = ParseValue (argv[++i]);
      else if (StringMatches (argv[i], "--beta", "-b", NULL)
	       && (i + 1) < argc)
	args->beta = ParseValue (argv[++i]);
      else if (StringMatches (argv[i], "--num-vecs", "-v", NULL)
	       && (i + 1) < argc)
	args->num_vecs = atoi (argv[++i]);
      else if (StringMatches (argv[i], "--num-calls", "-c", NULL)
	       && (i + 1) < argc)
	args->num_calls = atoi (argv[++i]);
      else if (StringMatches (argv[i], "--x-layout", "-x", NULL)
	       && (i + 1) < argc)
	args->x_layout = ParseDenseMatLayout (argv[++i]);
      else if (StringMatches (argv[i], "--y-layout", "-y", NULL)
	       && (i + 1) < argc)
	args->y_layout = ParseDenseMatLayout (argv[++i]);
      else
	break;
      i++;
    }
  return i;
}

static int
process_MatTrisolve (int argc, char *argv[], int i_start,
		     kerargs_MatTrisolve_t * args)
{
  int i;
  assert (args != NULL);
  if (i_start >= argc)
    return i_start - 1;

  /* Set default options */
  args->num_calls = 1;
  args->num_vecs = 1;
  args->op = OP_NORMAL;
  VAL_SET_ONE (args->alpha);
  args->x_layout = LAYOUT_COLMAJ;

  /* Override */
  i = i_start;
  while (i < argc && argv[i][0] == '-')
    {
      if (StringMatches (argv[i], "--op", "-o", NULL) && (i + 1) < argc)
	args->op = ParseMatTransOp (argv[++i]);
      else if (StringMatches (argv[i], "--alpha", "-a", NULL)
	       && (i + 1) < argc)
	args->alpha = ParseValue (argv[++i]);
      else if (StringMatches (argv[i], "--num-vecs", "-v", NULL)
	       && (i + 1) < argc)
	args->num_vecs = atoi (argv[++i]);
      else if (StringMatches (argv[i], "--num-calls", "-c", NULL)
	       && (i + 1) < argc)
	args->num_calls = atoi (argv[++i]);
      else if (StringMatches (argv[i], "--x-layout", "-x", NULL)
	       && (i + 1) < argc)
	args->x_layout = ParseDenseMatLayout (argv[++i]);
      else
	break;
      i++;
    }
  return i;
}

static int
process_MatTransMatMult (int argc, char *argv[], int i_start,
			 kerargs_MatTransMatMult_t * args)
{
  int i;
  assert (args != NULL);
  if (i_start >= argc)
    return i_start - 1;

  /* Set default options */
  args->num_calls = 1;
  args->num_vecs = 1;
  args->op = OP_AT_A;
  VAL_SET_ONE (args->alpha);
  VAL_SET_ZERO (args->beta);
  args->x_layout = LAYOUT_COLMAJ;
  args->y_layout = LAYOUT_COLMAJ;
  args->t_layout = LAYOUT_COLMAJ;

  /* Override */
  i = i_start;
  while (i < argc && argv[i][0] == '-')
    {
      if (StringMatches (argv[i], "--op", "-o", NULL) && (i + 1) < argc)
	args->op = ParseMatATAOp (argv[++i]);
      else if (StringMatches (argv[i], "--alpha", "-a", NULL)
	       && (i + 1) < argc)
	args->alpha = ParseValue (argv[++i]);
      else if (StringMatches (argv[i], "--beta", "-b", NULL)
	       && (i + 1) < argc)
	args->beta = ParseValue (argv[++i]);
      else if (StringMatches (argv[i], "--num-vecs", "-v", NULL)
	       && (i + 1) < argc)
	args->num_vecs = atoi (argv[++i]);
      else if (StringMatches (argv[i], "--num-calls", "-c", NULL)
	       && (i + 1) < argc)
	args->num_calls = atoi (argv[++i]);
      else if (StringMatches (argv[i], "--x-layout", "-x", NULL)
	       && (i + 1) < argc)
	args->x_layout = ParseDenseMatLayout (argv[++i]);
      else if (StringMatches (argv[i], "--y-layout", "-y", NULL)
	       && (i + 1) < argc)
	args->y_layout = ParseDenseMatLayout (argv[++i]);
      else if (StringMatches (argv[i], "--t-layout", "-t", NULL)
	       && (i + 1) < argc)
	args->t_layout = ParseDenseMatLayout (argv[++i]);
      else
	break;
      i++;
    }
  return i;
}

static int
process_MatMultAndMatTransMult (int argc, char *argv[], int i_start,
				kerargs_MatMultAndMatTransMult_t * args)
{
  int i;
  assert (args != NULL);
  if (i_start >= argc)
    return i_start - 1;

  /* Set default options */
  args->num_calls = 1;
  args->num_vecs = 1;
  args->op = OP_TRANS;
  VAL_SET_ONE (args->alpha);
  VAL_SET_ONE (args->omega);
  VAL_SET_ZERO (args->beta);
  VAL_SET_ZERO (args->zeta);
  args->x_layout = LAYOUT_COLMAJ;
  args->y_layout = LAYOUT_COLMAJ;
  args->w_layout = LAYOUT_COLMAJ;
  args->z_layout = LAYOUT_COLMAJ;

  /* Override */
  i = i_start;
  while (i < argc && argv[i][0] == '-')
    {
      if (StringMatches (argv[i], "--op", "-o", NULL) && (i + 1) < argc)
	args->op = ParseMatTransOp (argv[++i]);
      else if (StringMatches (argv[i], "--alpha", "-a", NULL)
	       && (i + 1) < argc)
	args->alpha = ParseValue (argv[++i]);
      else if (StringMatches (argv[i], "--omega", "-o", NULL)
	       && (i + 1) < argc)
	args->omega = ParseValue (argv[++i]);
      else if (StringMatches (argv[i], "--beta", "-b", NULL)
	       && (i + 1) < argc)
	args->beta = ParseValue (argv[++i]);
      else if (StringMatches (argv[i], "--zeta", "-e", NULL)
	       && (i + 1) < argc)
	args->zeta = ParseValue (argv[++i]);
      else if (StringMatches (argv[i], "--num-vecs", "-v", NULL)
	       && (i + 1) < argc)
	args->num_vecs = atoi (argv[++i]);
      else if (StringMatches (argv[i], "--num-calls", "-c", NULL)
	       && (i + 1) < argc)
	args->num_calls = atoi (argv[++i]);
      else if (StringMatches (argv[i], "--x-layout", "-x", NULL)
	       && (i + 1) < argc)
	args->x_layout = ParseDenseMatLayout (argv[++i]);
      else if (StringMatches (argv[i], "--w-layout", "-w", NULL)
	       && (i + 1) < argc)
	args->w_layout = ParseDenseMatLayout (argv[++i]);
      else if (StringMatches (argv[i], "--y-layout", "-y", NULL)
	       && (i + 1) < argc)
	args->y_layout = ParseDenseMatLayout (argv[++i]);
      else if (StringMatches (argv[i], "--z-layout", "-z", NULL)
	       && (i + 1) < argc)
	args->z_layout = ParseDenseMatLayout (argv[++i]);
      else
	break;
      i++;
    }
  return i;
}

static int
process_MatPowMult (int argc, char *argv[], int i_start,
		    kerargs_MatPowMult_t * args)
{
  int i;
  assert (args != NULL);
  if (i_start >= argc)
    return i_start - 1;

  /* Set default options */
  args->num_calls = 1;
  args->power = 2;
  args->op = OP_NORMAL;
  VAL_SET_ONE (args->alpha);
  VAL_SET_ZERO (args->beta);
  args->x_layout = LAYOUT_COLMAJ;
  args->y_layout = LAYOUT_COLMAJ;
  args->t_layout = LAYOUT_COLMAJ;

  /* Override */
  i = i_start;
  while (i < argc && argv[i][0] == '-')
    {
      if (StringMatches (argv[i], "--op", "-o", NULL) && (i + 1) < argc)
	args->op = ParseMatTransOp (argv[++i]);
      else if (StringMatches (argv[i], "--alpha", "-a", NULL)
	       && (i + 1) < argc)
	args->alpha = ParseValue (argv[++i]);
      else if (StringMatches (argv[i], "--beta", "-b", NULL)
	       && (i + 1) < argc)
	args->beta = ParseValue (argv[++i]);
      else if (StringMatches (argv[i], "--power", "-p", NULL)
	       && (i + 1) < argc)
	args->power = atoi (argv[++i]);
      else if (StringMatches (argv[i], "--num-calls", "-c", NULL)
	       && (i + 1) < argc)
	args->num_calls = atoi (argv[++i]);
      else if (StringMatches (argv[i], "--x-layout", "-x", NULL)
	       && (i + 1) < argc)
	args->x_layout = ParseDenseMatLayout (argv[++i]);
      else if (StringMatches (argv[i], "--y-layout", "-y", NULL)
	       && (i + 1) < argc)
	args->y_layout = ParseDenseMatLayout (argv[++i]);
      else if (StringMatches (argv[i], "--t-layout", "-t", NULL)
	       && (i + 1) < argc)
	args->t_layout = ParseDenseMatLayout (argv[++i]);
      else
	break;
      i++;
    }
  return i;
}

/**
 *  \param[in] argc Length of argument list.
 *  \param[in] argv Command-line argument list.
 *  \param[in] i_start Index of argv on which to start processing.
 *  \param[in,out] p_opt Address of a pointer in which to store
 *  the address of a kernel option, if processed. Must not be NULL.
 *
 *  \returns Index of the next unprocessed option in argv, and
 *  sets *p_opt to be equal to the address of a new kernel_t
 *  data structure. If a kernel option cannot be processed,
 *  returns i_start and sets *p_opt to NULL.
 */
int
keropts_Process (int argc, char *argv[], int i_start, kernel_t ** p_opt)
{
  int ker_num = 0;
  int i_next;
  kernel_t opt;

  if (p_opt == NULL || i_start >= argc)
    return i_start;

  oski_ZeroMem (&opt, sizeof (kernel_t));

  /* Check for valid kernel name */
  ker_num = StringMatches (argv[i_start],
			   "MatMult",
			   "MatTrisolve",
			   "MatTransMatMult",
			   "MatMultAndMatTransMult", "MatPowMult", NULL);
  switch (ker_num)
    {
    case 1:
      opt.kernel = KERNEL_MatMult;
      i_next = process_MatMult (argc, argv, i_start + 1, &(opt.args.MatMult));
      break;
    case 2:
      opt.kernel = KERNEL_MatTrisolve;
      i_next = process_MatTrisolve (argc, argv, i_start + 1,
				    &(opt.args.MatTrisolve));
      break;
    case 3:
      opt.kernel = KERNEL_MatTransMatMult;
      i_next = process_MatTransMatMult (argc, argv, i_start + 1,
					&(opt.args.MatTransMatMult));
      break;
    case 4:
      opt.kernel = KERNEL_MatMultAndMatTransMult;
      i_next = process_MatMultAndMatTransMult (argc, argv, i_start + 1,
					       &(opt.args.
						 MatMultAndMatTransMult));
      break;
    case 5:
      opt.kernel = KERNEL_MatPowMult;
      i_next = process_MatPowMult (argc, argv, i_start + 1,
				   &(opt.args.MatPowMult));
      break;
    default:			/* no match */
      opt.kernel = KERNEL_NULL;
      i_next = i_start;
      break;
    }
  if (i_next > i_start)
    {
      *p_opt = oski_Malloc (kernel_t, 1);
      if (*p_opt != NULL)
	oski_CopyMem (*p_opt, &opt, kernel_t, 1);
      else
	i_next = i_start;
    }
  return i_next;
}

/* eof */
