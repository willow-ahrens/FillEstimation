/**
 *  \file src/heur/symmrb_eval.c
 *  \brief Implementation of a heuristic for choosing a block size.
 */

#include <stdio.h>

#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/tune.h>
#include <oski/xforms.h>
#include <oski/heur/symmrb.h>
#include <oski/heur/estfill.h>

#include <oski/CSR/format.h>
#include <oski/CSC/format.h>
#include <oski/MBCSR/format.h>

/** Maximum string buffer size, used when reading the profile data. */
#define MAXBUF 1000

/** Fraction of matrix to scan when estimating the block size. */
#define HEUR_SWEEP_FRAC .02

/* -----------------------------------------------------------------
 *  Profile management
 */

/**
 *  Cached register blocked SpMV profile for the currently
 *  instantiated type.
 */
static oski_profile_MBCSRSymmMatMult_t g_profile = { 0, 0, NULL };

/**
 *  Set to 1 iff g_profile contains valid data.
 */
static int g_is_valid_profile = 0;

/** Default name of the file containing the MBCSR SpMV profile data. */
#define PROFILE_FILE "profile_MBCSR_SymmMatMult_T" \
	MACRO_TO_STRING(IND_TAG) MACRO_TO_STRING(VAL_TAG) ".dat"

/**
 *  \brief Scans the given file to determine the largest block
 *  size specified.
 *
 *  \param[in] fp Input file, set to the logical beginning of
 *  the portion of the file to scan.
 *  \param[in,out] p_max_r Pointer to integer in which to
 *  return the maximum row block size found.
 *  \param[in,out] p_max_c Pointer to integer in which to
 *  return the maximum column block size found.
 *
 *  \returns 1 if a maximum block size could be determined, and
 *  sets (*p_max_r) and (*p_max_c) accordingly. Otherwise, returns
 *  0 and leaves (*p_max_r) and (*p_max_c) unchanged. The current
 *  file position is reset to the file position on entry in
 *  either case.
 */
static int
GetMaxBlockSize (FILE * fp, int *p_max_r, int *p_max_c)
{
  int max_r = 0, max_c = 0;
  long pos_begin = ftell (fp);

  while (!feof (fp))
    {
      char buffer[MAXBUF];
      int r, c;

      if (!oski_ReadBenchmarkLine (fp, buffer, MAXBUF))
	break;

      sscanf (buffer, "%d %d", &r, &c);
      if (r > max_r)
	max_r = r;
      if (c > max_c)
	max_c = c;
    }

  /* Rewind */
  fseek (fp, pos_begin, SEEK_SET);

  if (max_r == 0 && max_c == 0)
    return 0;
  else
    {
      *p_max_r = max_r;
      *p_max_c = max_c;
      return 1;
    }
}

/**
 *  \brief Display an array of profile data (FOR DEBUGGING).
 */
static void
DumpProfile (int debug_level, const char *title, const double *data,
	     size_t max_r, size_t max_c)
{
  oski_PrintDebugMessage (debug_level, "--- BEGIN %s", title);
  if (max_r * max_c >= 1)
    {
      int i;
      double max_val, min_val;
      const char *str_format;
      int r, c;

      for (max_val = data[0], min_val = data[0], i = 1;
	   i < max_r * max_c; i++)
	{
	  if (data[i] > max_val)
	    max_val = data[i];
	  if (data[i] < min_val)
	    min_val = data[i];
	}

      if (max_val >= 1000 || min_val <= -1000)
	str_format = "%6.2e";
      else if (max_val >= 100 || min_val <= -100)
	str_format = "%6.0f";
      else if (max_val >= 10 || min_val <= -10)
	str_format = "%6.1f";
      else if (max_val >= 1 || min_val <= -1)
	str_format = "%6.2f";
      else
	str_format = "%.2e";

      /* column header */
      oski_PrintDebugMessageShort (debug_level, "       ");
      for (c = 1; c <= max_c; c++)
	oski_PrintDebugMessageShort (debug_level, "%6d", c);
      oski_PrintDebugMessageShort (debug_level, "\n");

      /* performance data */
      for (r = 1; r <= max_r; r++)
	{
	  oski_PrintDebugMessageShort (debug_level, "[r=%2d] ", r);
	  for (c = 1; c <= max_c; c++)
	    oski_PrintDebugMessageShort (debug_level,
					 str_format,
					 data[(r - 1) * max_c + c - 1]);
	  oski_PrintDebugMessageShort (debug_level, "\n");
	}
    }
  oski_PrintDebugMessage (debug_level, "--- END %s", title);
}

/**
 *  \brief Reads a performance profile from the given file.
 *
 *  \param[in] fp Input file, set to the logical beginning of
 *  the portion of the file to scan.
 *  \param[in,out] p_max_r Pointer to integer in which to
 *  return the maximum row block size found.
 *  \param[in,out] p_max_c Pointer to integer in which to
 *  return the maximum column block size found.
 *
 *  \returns On success, returns a pointer to a newly allocated
 *  buffer containing the row/column block size performance data
 *  in row-major format, and sets *p_max_r and *p_max_c to the
 *  maximum row and column block size, respectively. The file
 *  position in this case will be the end of the file.
 *  On error, returns NULL and leaves the file position at the
 *  position on entry.
 */
static double *
GetData (FILE * fp, int *p_max_r, int *p_max_c)
{
  oski_profile_MBCSRSymmMatMult_t P;
  int max_r = 0;
  int max_c = 0;

  if (!GetMaxBlockSize (fp, &max_r, &max_c))
    return NULL;
  P.max_r = (size_t) ((max_r > 0) ? max_r : 0);
  P.max_c = (size_t) ((max_c > 0) ? max_c : 0);

  P.perf = oski_Malloc (double, P.max_r * P.max_c);
  if (P.perf == NULL)
    return NULL;
  oski_ZeroMem (P.perf, sizeof (double) * P.max_r * P.max_c);

  while (!feof (fp))
    {
      char buffer[MAXBUF];
      float Mflops = 0.0;
      int r = 0, c = 0, v = 0;

      if (!oski_ReadBenchmarkLine (fp, buffer, MAXBUF))
	break;
      sscanf (buffer, "%d %d %d %f", &r, &c, &v, &Mflops);
      if (r != 0 && c != 0 && v == 1 && r <= P.max_r && c <= P.max_c)
	PROF_MBCSR_SET (&P, r, c, (double) Mflops);
    }

  DumpProfile (3, "Profile: MBCSR, MatMult", P.perf, P.max_r, P.max_c);

  *p_max_r = P.max_r;
  *p_max_c = P.max_c;
  return P.perf;
}

/**
 *  \brief Returns 1 iff a valid profile could be loaded from the
 *  specified file.
 *
 *  The profile is loaded into the private but global variable,
 *  \ref g_profile, overwriting the current contents on success.
 *  If an error occurs, the contents are left unmodified.
 *
 *  \param[in] filename Name of the file containing the profile
 *  data. This name is assumed to be a relative file subpath.
 */
static int
LoadProfile (const char *filename)
{
  FILE *fp;
  int max_r, max_c;
  double *perf;

  if ((fp = oski_OpenBenchmarkData (filename)) == NULL)
    return 0;
  perf = GetData (fp, &max_r, &max_c);
  oski_CloseBenchmarkData (fp);

  if (perf != NULL)
    {
      g_profile.max_r = max_r;
      g_profile.max_c = max_c;
      g_profile.perf = perf;
      return 1;
    }
  else
    return 0;
}

/**
 *  \brief Load a register blocking profile from the given file.
 *
 *  This implementation 
 */
const oski_profile_MBCSRSymmMatMult_t *
oski_LoadProfileMBCSRSymmMatMult (const char *filename)
{
  if (!g_is_valid_profile)	/* No profile loaded yet */
    g_is_valid_profile =
      LoadProfile (filename == NULL ? PROFILE_FILE : filename);

  if (g_is_valid_profile)	/* Valid profile exists? */
    return &g_profile;
  else
    return NULL;
}

void
oski_UnloadProfileMBCSRSymmMatMult (void)
{
  if (g_is_valid_profile && g_profile.perf != NULL)
    {
      oski_Free (g_profile.perf);
      g_profile.perf = NULL;
    }
}


/* -----------------------------------------------------------------
 *  Heuristic evaluation.
 */

/**
 *  \brief Evaluates the register blocked SpMV heuristic on the
 *  given matrix, and returns the results in an object of type
 *  \ref oski_symmrb_t.
 *
 *  \returns Returns a \ref oski_symmrb_t structure if the
 *  heuristic could be evaluated and returned a non-trivial
 *  block size (i.e., block size not equal to 1x1).
 *
 *  \note This heuristic uses the same approximate fill ratio
 *  used in the BCSR-heuristic case.
 */
void *
oski_HeurEvaluate (const oski_matrix_t A)
{
  oski_symmrb_t *results;
  oski_fillprofile_BCSR_t *fill;

  oski_PrintDebugMessage (3, "Evaluating register blocked SpMV heuristic...");

  if (!g_is_valid_profile)
    {
      oski_PrintDebugMessage (4, "No profile is available.");
      return NULL;
    }

  /* Placeholder for results */
  results = oski_Malloc (oski_symmrb_t, 1);
  if (results == NULL)
    {
      OSKI_ERR (oski_HeurEvaluate, ERR_OUT_OF_MEMORY);
      return NULL;
    }
  results->r = 1;
  results->c = 1;

  /* Evaluate heuristic */
  fill = oski_EstimateFillBCSR (&(A->input_mat), &(A->props),
				g_profile.max_r, g_profile.max_c,
				HEUR_SWEEP_FRAC);
  if (fill != NULL)
    {
      int r_heur = 0, c_heur = 0;
      double perf_est_max = 0.0;
      int r;

      DumpProfile (3, "Fill ratio: MBCSR", fill->ratio,
		   fill->max_r, fill->max_c);

      for (r = 1; r <= g_profile.max_r; r++)
	{
	  int c;
	  for (c = 1; c <= g_profile.max_c; c++)
	    {
	      double perf_est = PROF_MBCSR_GET (&g_profile, r, c)
		/ PROF_FILLBCSR_GET (fill, r, c);
	      if (perf_est > perf_est_max)
		{
		  perf_est_max = perf_est;
		  r_heur = r;
		  c_heur = c;
		}
	    }
	}

      if (r_heur > 0 && c_heur > 0)
	{
	  oski_PrintDebugMessage (4,
				  "Selected %d x %d (~%.1g Mflop/s)",
				  (int) r_heur, (int) c_heur, perf_est_max);
	  results->r = r_heur;
	  results->c = c_heur;
	}
    }

  if (results->r == 1 && results->c == 1)
    {
      oski_Free (results);
      return NULL;
    }
  return results;
}

/* -----------------------------------------------------------------
 *  Apply heuristic results.
 */

/**
 *  Returns a newly allocated string containing a BeBOP-Lua program
 *  to convert an input matrix to r x c MBCSR format.
 */
static char *
MakeTransformString (const char *input_mat, int r, int c)
{
  return oski_StringPrintf ("return MBCSR(%s, %d, %d)", input_mat, r, c);
}

/**
 *  This routine converts the given matrix to MBCSR format according
 *  to the specified heuristic evaluation results.
 */
int
oski_HeurApplyResults (const void *results, oski_matrix_t A_tunable)
{
  const oski_symmrb_t *block_size = (const oski_symmrb_t *) results;
  char *xform;
  int err;

  if (results == NULL)
    return ERR_BAD_ARG;
  xform = MakeTransformString (OLUA_INMAT, block_size->r, block_size->c);
  if (xform == NULL)
    return ERR_OUT_OF_MEMORY;

  err = oski_ApplyMatTransforms (A_tunable, xform);
  oski_Free (xform);

  /* Disable various kernels */
  if (!err && A_tunable->tuned_mat.repr != NULL)
    {
      oski_matMBCSR_t *mat = (oski_matMBCSR_t *) A_tunable->tuned_mat.repr;
      mat->enabled.MatTransMatMult = 0;
      mat->enabled.MatMultAndMatMult = 0;
      mat->enabled.MatMultAndMatTransMult = 0;
      mat->enabled.MatPowMult = 0;
      mat->enabled.MatTransPowMult = 0;
    }
  return err;
}

/* eof */
