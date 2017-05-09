/**
 *  \file bench/matopts.h
 *  \brief Matrix options processing and generation.
 *  \ingroup OFFBENCH
 *
 *  This module provides support for automatic matrix generation
 *  via specified constraints.
 *
 *  \todo Reimplement using Hoemmen's matrix generator.
 */

#if !defined(INC_BENCH_MATOPTS_H)
/** bench/matopts.h included. */
#define INC_BENCH_MATOPTS_H

#include <oski/common.h>
#include <oski/mangle.h>
#include <oski/matrix.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
#define matopts_t MANGLE_(matopts_t)
#define matopts_Init MANGLE_(matopts_Init)
#define matopts_Display MANGLE_(matopts_Display)
#define matopts_Usage MANGLE_(matopts_Usage)
#define matopts_Process MANGLE_(matopts_Process)
#define matopts_CreateMat MANGLE_(matopts_CreateMat)
#endif

/** \name Types of automatically generated test matrices. */
/*@{*/
/** Non-zero <==> x is a matrix of type mt */
#define IS_MATTYPE(x, mt) (strcmp(x, mt) == 0)

/** \brief Harwell-Boeing matrix pattern, read in from a file. */
#define MATTYPE_HARWELL_BOEING "hb"
/** \brief Dense matrix */
#define MATTYPE_DENSE "dense"
/** \brief Matrix with a dense blocked band */
#define MATTYPE_BANDED "band"
/** \brief Random, square matrix */
#define MATTYPE_RANDOM "random"
/** \brief Lower triangular matrix with a random non-zero pattern. */
#define MATTYPE_TRI_LOWER "lowtri"
/** \brief Upper triangular matrix with a random non-zero pattern. */
#define MATTYPE_TRI_UPPER "uptri"
/** \brief Dense, symmetric matrix. */
#define MATTYPE_SYMM "symm"
/** \brief Random rectangular matrix with more columns than rows. */
#define MATTYPE_RANDRECT "randrect"
/*@}*/

/** Stores valid matrix options */
typedef struct
{
  const char *mattype;
  const char *matfile;
  int expand_symm; /*!< Expand symmetric matrices to non-symm format. */
  oski_index_t min_rows;
  oski_index_t min_cols;
  oski_index_t min_nnz;
  oski_index_t min_row_nnz;
  oski_index_t row_multiple;
  oski_index_t col_multiple;
} matopts_t;

/** Initialize matrix options to default values. */
void matopts_Init (matopts_t * opts);

/** Display summary of matrix options */
void matopts_Display (const matopts_t * opts, FILE * fp);

/** Display summary of command-line matrix options */
void matopts_Usage (FILE * fp);

/** Process possible matrix-generation command-line option */
int matopts_Process (int argc, char *argv[], int i, matopts_t * opts);

/** Instantiate a matrix from the specified options */
oski_matrix_t matopts_CreateMat (const matopts_t * opts,
				 oski_index_t * p_num_rows,
				 oski_index_t * p_num_cols,
				 oski_index_t * p_nnz_stored,
				 oski_index_t * p_nnz);
#endif

/* eof */
