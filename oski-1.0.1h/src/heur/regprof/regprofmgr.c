/**
 *  \file src/heur/regprofmgr.c
 *  \brief Register profile manager.
 *
 *  This module implements minimal support for reading register
 *  profile files from disk.
 */

#include <stdio.h>

#include <oski/common.h>
#include <oski/mangle.h>
#include <oski/heur.h>
#include <oski/heur/regprofmgr.h>

/** Maximum string buffer size, used when reading the profile data. */
#define MAXBUF 1000

/* -----------------------------------------------------------------
 *  Profile management
 */

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
  oski_regprof_t P;
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
	REGPROF_SET (&P, r, c, (double) Mflops);
    }

  oski_PrintDebugRegProfile (3, "Register profile", &P);

  *p_max_r = P.max_r;
  *p_max_c = P.max_c;
  return P.perf;
}

/**
 *  \brief Returns 1 iff a valid profile could be loaded from the
 *  specified file.
 */
static int
LoadProfile (const char *filename, oski_regprof_t * profile)
{
  FILE *fp;
  int max_r, max_c;
  double *perf;

  if (profile == NULL)
    return 0;
  if ((fp = oski_OpenBenchmarkData (filename)) == NULL)
    return 0;
  perf = GetData (fp, &max_r, &max_c);
  oski_CloseBenchmarkData (fp);

  if (perf != NULL)
    {
      profile->max_r = max_r;
      profile->max_c = max_c;
      profile->perf = perf;
      return 1;
    }
  else
    return 0;
}

/**
 *  \brief
 *
 *  \returns A pointer to a newly allocated register profile
 *  data structure, initialized to an 'uninitialized' state.
 *  Returns NULL on error.
 */
oski_regprof_t *
oski_CreateRegProfile (oski_index_t max_r, oski_index_t max_c)
{
  oski_regprof_t *profile = oski_Malloc (oski_regprof_t, 1);
  oski_InitRegProfile (profile, max_r, max_c);
  return profile;
}

/**
 *  \brief
 *
 *  \param[in] filename File from which to read register profile
 *  data.
 *  \param[in,out] profile An uninitialized register profile buffer
 *  in which to store the profile data.
 *  \returns On success, returns profile data from the given file
 *  in the buffer 'profile', and returns 1. Otherwise, returns 0
 *  and leaves 'profile' in an uninitialized state.
 */
int
oski_LoadRegProfile (const char *filename, oski_regprof_t * profile)
{
  return profile != NULL && LoadProfile (filename, profile);
}

void
oski_ResetRegProfile (oski_regprof_t * profile)
{
  if (profile != NULL && profile->perf != NULL)
    {
      oski_Free (profile->perf);
      oski_ZeroMem (profile, sizeof (oski_regprof_t));
    }
}

void
oski_DestroyRegProfile (oski_regprof_t * profile)
{
  if (profile != NULL)
    {
      oski_ResetRegProfile (profile);
      oski_Free (profile);
    }
}

/**
 *  \brief
 *
 *  \returns 1 on success, 0 on an out of memory error.
 */
int
oski_InitRegProfile (oski_regprof_t * profile,
		     oski_index_t max_r, oski_index_t max_c)
{
  if (profile == NULL)
    return 0;
  oski_ZeroMem (profile, sizeof (oski_regprof_t));
  if ((max_r * max_c) <= 0)
    return 1;

  profile->perf = oski_Malloc (double, max_r * max_c);
  if (profile->perf != NULL)
    {
      profile->max_r = max_r;
      profile->max_c = max_c;
      oski_ZeroRegProfile (profile);
      return 1;
    }
  return 0;
}

void
oski_ZeroRegProfile (oski_regprof_t * profile)
{
  if (profile == NULL || profile->perf == NULL)
    return;
  oski_ZeroMem (profile->perf, profile->max_r * profile->max_c);
}

void
oski_ScaleRegProfile (oski_regprof_t * profile, double alpha)
{
  oski_index_t i;

  if (profile == NULL)
    return;

  for (i = 1; i <= profile->max_r; i++)
    {
      oski_index_t j;
      for (j = 1; j <= profile->max_c; j++)
	{
	  REGPROF_MULEQ (profile, i, j, alpha);
	}
    }
}

void
oski_CopyRegProfile (const oski_regprof_t * src, oski_regprof_t * dest)
{
  oski_index_t i;
  oski_index_t min_r, min_c;
  if (src == NULL || dest == NULL)
    return;

  min_r = src->max_r < dest->max_r ? src->max_r : dest->max_r;
  min_c = src->max_c < dest->max_c ? src->max_c : dest->max_c;

  for (i = 1; i <= min_r; i++)
    {
      oski_index_t j;
      for (j = 1; j <= min_c; j++)
	{
	  REGPROF_SET (dest, i, j, REGPROF_GET (src, i, j));
	}
      while (j <= dest->max_c)
	{
	  REGPROF_SET (dest, i, j, 0.0);
	  j++;
	}
    }
  while (i <= dest->max_r)
    {
      oski_index_t j;
      for (j = 1; j <= min_c; j++)
	{
	  REGPROF_SET (dest, i, j, REGPROF_GET (src, i, j));
	}
      while (j <= dest->max_c)
	{
	  REGPROF_SET (dest, i, j, 0.0);
	  j++;
	}
      i++;
    }
}

void
oski_MulEqRegProfile (oski_regprof_t * dest, const oski_regprof_t * src)
{
  oski_index_t i;
  oski_index_t min_r, min_c;
  if (src == NULL || dest == NULL)
    return;

  min_r = src->max_r < dest->max_r ? src->max_r : dest->max_r;
  min_c = src->max_c < dest->max_c ? src->max_c : dest->max_c;

  for (i = 1; i <= min_r; i++)
    {
      oski_index_t j;
      for (j = 1; j <= min_c; j++)
	{
	  REGPROF_MULEQ (dest, i, j, REGPROF_GET (src, i, j));
	}
      while (j <= dest->max_c)
	{
	  REGPROF_SET (dest, i, j, 0.0);
	  j++;
	}
    }
  while (i <= dest->max_r)
    {
      oski_index_t j;
      for (j = 1; j <= min_c; j++)
	{
	  REGPROF_MULEQ (dest, i, j, REGPROF_GET (src, i, j));
	}
      while (j <= dest->max_c)
	{
	  REGPROF_SET (dest, i, j, 0.0);
	  j++;
	}
      i++;
    }
}

void
oski_AddEqRegProfile (oski_regprof_t * dest, const oski_regprof_t * src)
{
  oski_index_t i;
  oski_index_t min_r, min_c;
  if (src == NULL || dest == NULL)
    return;

  min_r = src->max_r < dest->max_r ? src->max_r : dest->max_r;
  min_c = src->max_c < dest->max_c ? src->max_c : dest->max_c;

  for (i = 1; i <= min_r; i++)
    {
      oski_index_t j;
      for (j = 1; j <= min_c; j++)
	{
	  REGPROF_ADDEQ (dest, i, j, REGPROF_GET (src, i, j));
	}
    }
}

void
oski_InvRegProfile (oski_regprof_t * profile)
{
  oski_index_t i;
  if (profile == NULL)
    return;
  for (i = 0; i < profile->max_r * profile->max_c; i++)
    profile->perf[i] = 1.0 / profile->perf[i];
}

/**
 *  \brief
 *
 *  \param[in] profile Profile \f$P\f$.
 *  \param[in,out] p_r  Pointer to an index variable into which to
 *  store the row index of the min entry. May be NULL if this
 *  return value is not needed.
 *  \param[in,out] p_c  Pointer to an index variable into which to
 *  store the column index of the min entry. May be NULL if this
 *  return value is not needed.
 *  \returns The value of the min entry, and the row and column
 *  index in *p_r, *p_c (if p_r, p_c are not NULL), respectively.
 */
double
oski_MinRegProfile (const oski_regprof_t * profile,
		    oski_index_t * p_r, oski_index_t * p_c)
{
  oski_index_t i;
  oski_index_t r_opt = 0, c_opt = 0;
  double p_opt = 0.0;
  if (profile != NULL && profile->max_r >= 1 && profile->max_c >= 1)
    {
      p_opt = REGPROF_GET (profile, 1, 1);
      r_opt = 1;
      c_opt = 1;
      for (i = 1; i <= profile->max_r; i++)
	{
	  oski_index_t j;
	  for (j = 1; j <= profile->max_c; j++)
	    {
	      double p_ij = REGPROF_GET (profile, i, j);
	      if (p_ij < p_opt)
		{
		  p_opt = p_ij;
		  r_opt = i;
		  c_opt = j;
		}
	    }
	}
    }
  if (p_r != NULL)
    *p_r = r_opt;
  if (p_c != NULL)
    *p_c = c_opt;
  return p_opt;
}

/**
 *  \brief
 *
 *  \param[in] profile Profile \f$P\f$.
 *  \param[in,out] p_r  Pointer to an index variable into which to
 *  store the row index of the max entry. May be NULL if this
 *  return value is not needed.
 *  \param[in,out] p_c  Pointer to an index variable into which to
 *  store the column index of the max entry. May be NULL if this
 *  return value is not needed.
 *  \returns The value of the max entry, and the row and column
 *  index in *p_r, *p_c (if p_r, p_c are not NULL), respectively.
 */
double
oski_MaxRegProfile (const oski_regprof_t * profile,
		    oski_index_t * p_r, oski_index_t * p_c)
{
  oski_index_t i;
  oski_index_t r_opt = 0, c_opt = 0;
  double p_opt = 0.0;
  if (profile != NULL && profile->max_r >= 1 && profile->max_c >= 1)
    {
      p_opt = REGPROF_GET (profile, 1, 1);
      r_opt = 1;
      c_opt = 1;
      for (i = 1; i <= profile->max_r; i++)
	{
	  oski_index_t j;
	  for (j = 1; j <= profile->max_c; j++)
	    {
	      double p_ij = REGPROF_GET (profile, i, j);
	      if (p_ij > p_opt)
		{
		  p_opt = p_ij;
		  r_opt = i;
		  c_opt = j;
		}
	    }
	}
    }
  if (p_r != NULL)
    *p_r = r_opt;
  if (p_c != NULL)
    *p_c = c_opt;
  return p_opt;
}

void
oski_CalcHarmonicMeanRegProfile (oski_regprof_t * dest, double w1,
				 const oski_regprof_t * src, double w2)
{
  oski_index_t i;
  oski_index_t min_r, min_c;
  if (src == NULL || dest == NULL)
    return;

  min_r = src->max_r < dest->max_r ? src->max_r : dest->max_r;
  min_c = src->max_c < dest->max_c ? src->max_c : dest->max_c;

  for (i = 1; i <= min_r; i++)
    {
      oski_index_t j;
      for (j = 1; j <= min_c; j++)
	{
	  double a = REGPROF_GET (dest, i, j);
	  double b = REGPROF_GET (dest, i, j);
	  REGPROF_SET (dest, i, j, 1.0 / ((w1 / a) + (w2 / b)));
	}
      while (j <= dest->max_c)
	{
	  REGPROF_SET (dest, i, j, 0.0);
	  j++;
	}
    }
  while (i <= dest->max_r)
    {
      oski_index_t j;
      for (j = 1; j <= min_c; j++)
	{
	  REGPROF_SET (dest, i, j, 0.0);
	}
      while (j <= dest->max_c)
	{
	  REGPROF_SET (dest, i, j, 0.0);
	  j++;
	}
      i++;
    }
}

void
oski_PrintDebugRegProfile (int debug_level, const char *title,
			   const oski_regprof_t * profile)
{
  oski_PrintDebugMessage (debug_level, "--- BEGIN %s",
			  title != NULL ? title : "");
  if (profile != NULL && (profile->max_r * profile->max_c) >= 1)
    {
      double max_val = oski_MaxRegProfile (profile, NULL, NULL);
      double min_val = oski_MinRegProfile (profile, NULL, NULL);
      const char *str_format;
      oski_index_t r, c;

      if (max_val <= 1 && min_val >= -1)
	str_format = "%8.2e";
      else if (max_val <= 10 && min_val >= -10)
	str_format = "%8.3f";
      else if (max_val <= 100 && min_val >= -100)
	str_format = "%8.2f";
      else if (max_val <= 1000 && min_val >= -1000)
	str_format = "%8.1f";
      else if (max_val <= 10000 && min_val >= -10000)
	str_format = "%8.0f";
      else
	str_format = "%.2e";

      /* column header */
      oski_PrintDebugMessageShort (debug_level, "     ");
      for (c = 1; c <= profile->max_c; c++)
	oski_PrintDebugMessageShort (debug_level, "%8d", c);
      oski_PrintDebugMessageShort (debug_level, "\n");

      /* performance data */
      for (r = 1; r <= profile->max_r; r++)
	{
	  oski_PrintDebugMessageShort (debug_level, "[r=%2d] ", r);
	  for (c = 1; c <= profile->max_c; c++)
	    oski_PrintDebugMessageShort (debug_level,
					 str_format, REGPROF_GET (profile, r,
								  c));
	  oski_PrintDebugMessageShort (debug_level, "\n");
	}
    }
  oski_PrintDebugMessage (debug_level, "--- END %s", title);
}

/* eof */
