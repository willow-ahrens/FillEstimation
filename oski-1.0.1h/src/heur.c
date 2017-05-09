/**
 *  \file heur.c
 *  \brief Heuristic management module.
 *
 *  The compile-time macro OSKI_BENCH_DIR_DEFAULT must be defined.
 */

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <oski/common.h>
#include <oski/heur.h>
#include <oski/heur_internal.h>
#include <oski/simplelist.h>

/* -------------------------------------------------------------------
 *  Private routines.
 */

/**
 *  \brief Global (but private to this module) list of available
 *  heuristics.
 */
static simplelist_t *g_avail_heurs = NULL;

/**
 *  \brief Frees the memory associated with a heuristic record.
 */
static void
FreeHeurRecord (oski_heurrec_t * rec)
{
  if (rec == NULL)
    return;

  if (rec->module != NULL)
    {
      /* Execute the module's CLOSE function, if available */
      oski_CloseModule (rec->module);
      oski_UnloadModule (rec->module);
    }

  if (rec->desc != NULL)
    oski_Free (rec->desc);
  if (rec->modname != NULL)
    oski_Free (rec->modname);
  if (rec->name)
    oski_Free (rec->name);

  rec->desc = NULL;
  rec->name = NULL;
  rec->id = INVALID_ID;
  rec->module = NULL;

  oski_Free (rec);
}

/**
 *  \brief Populate the heuristic table with the default available
 *  heuristics.
 */
static void
RegisterDefaultHeur (void)
{
  if (!oski_RegisterSiteModules ("site-modules-heur.txt", "heur",
				 oski_RegisterHeur))
    {
      /** Can't load heuristics from a site module file, so try to
	  load all possible types from a hard-coded list. */
      oski_id_t i;
      for (i = 1; i <= oski_GetNumScalarIndexTypes (); i++)
	{
	  oski_id_t j;
	  for (j = 1; j <= oski_GetNumScalarValueTypes (); j++)
	    oski_RegisterHeur ("symmrb", i, j);
	  for (j = 1; j <= oski_GetNumScalarValueTypes (); j++)
	    oski_RegisterHeur ("regprof", i, j);
	  for (j = 1; j <= oski_GetNumScalarValueTypes (); j++)
	    oski_RegisterHeur ("mregblock", i, j);
	}
    }
}

/* -------------------------------------------------------------------
 *  Public interface implementation.
 */

/**
 *  \brief
 *
 *  Initializes the list of available matrix types, #g_avail_heurs.
 */
void
oski_InitHeurManager (void)
{
  if (g_avail_heurs == NULL)
    {
      oski_PrintDebugMessage (2, "Initializing heuristic manager");
      g_avail_heurs = simplelist_Create ();
      RegisterDefaultHeur ();
    }
}

void
oski_CloseHeurManager (void)
{
  if (g_avail_heurs != NULL)
    {
      size_t i;
      size_t len;

      oski_PrintDebugMessage (2, "Shutting down heuristic manager");

      len = simplelist_GetLength (g_avail_heurs);

      /* Free individual heuristic records */
      for (i = 1; i <= len; i++)
	{
	  /* Get data element, but throw away 'const' qualifier. */
	  oski_heurrec_t *rec = (oski_heurrec_t *)
	    simplelist_GetElem (g_avail_heurs, i);

	  if (rec != NULL)
	    FreeHeurRecord (rec);
	}

      /* Destory the list. */
      simplelist_Destroy (g_avail_heurs);
      g_avail_heurs = NULL;
    }
}

oski_id_t
oski_RegisterHeur (const char *heur_name, oski_id_t id_ind, oski_id_t id_val)
{
  oski_heurrec_t *new_rec;
  oski_id_t id;

  /* @@ oski_InitHeurManager (); */
  if (g_avail_heurs == NULL)
    return INVALID_ID;

  /* Duplicate record? */
  id = oski_LookupHeurId (heur_name, id_ind, id_val);
  if (id != INVALID_ID)
    return id;

  /* Create new record and fill in basic data. */
  new_rec = oski_Malloc (oski_heurrec_t, 1);
  if (!new_rec)
    return INVALID_ID;

  new_rec->name = oski_DuplicateString (heur_name);
  new_rec->id_index_type = id_ind;
  new_rec->id_value_type = id_val;

  /* Cache module */
  new_rec->desc = oski_MakeModuleDesc (heur_name, id_ind, id_val);
  new_rec->modname = oski_MakeModuleName ("heur", heur_name, id_ind, id_val);
  new_rec->module = oski_LoadModuleOptional (new_rec->modname);

  if (new_rec->module == NULL)
    new_rec->id = INVALID_ID;
  else
    new_rec->id = (oski_id_t) simplelist_Append (g_avail_heurs, new_rec);

  if (new_rec->id == INVALID_ID)
    {
      FreeHeurRecord (new_rec);
      return INVALID_ID;
    }

  oski_PrintDebugMessage (2, "Registered new heuristic: %s (ID#%d)",
                          new_rec->desc, new_rec->id);
  return new_rec->id;
}

size_t
oski_GetNumHeur (void)
{
  if (g_avail_heurs == NULL)
    return 0;
  return simplelist_GetLength (g_avail_heurs);
}

const oski_heurrec_t *
oski_LookupHeur (const char *name, oski_id_t id_ind, oski_id_t id_val)
{
  size_t i;
  size_t len = oski_GetNumHeur ();

  for (i = 1; i <= len; i++)
    {
      const oski_heurrec_t *rec = (const oski_heurrec_t *)
	simplelist_GetElem (g_avail_heurs, i);

      if (rec == NULL)
	continue;

      if ((strcmp (name, rec->name) == 0)
	  && (id_ind == rec->id_index_type) && (id_val == rec->id_value_type))
	return rec;
    }
  return NULL;
}

const oski_heurrec_t *
oski_FindHeurRecord (oski_id_t id)
{
  size_t i;
  size_t len;

  if (id == INVALID_ID)
    return NULL;

  len = oski_GetNumHeur ();
  for (i = 1; i <= len; i++)
    {
      const oski_heurrec_t *rec = (const oski_heurrec_t *)
	simplelist_GetElem (g_avail_heurs, i);

      if (rec == NULL)
	continue;

      if (rec->id == id)
	return rec;
    }
  return NULL;
}

oski_id_t
oski_LookupHeurIdByNum (size_t i)
{
  oski_id_t id = INVALID_ID;
  if (g_avail_heurs != NULL)
    {
      const oski_heurrec_t *rec = (const oski_heurrec_t *)
	simplelist_GetElem (g_avail_heurs, i);
      if (rec != NULL)
	id = rec->id;
    }
  return id;
}

const char *
oski_LookupHeurDescById (oski_id_t id)
{
  const char *desc = "(Unnamed/unknown heuristic)";
  if (g_avail_heurs != NULL)
    {
      const oski_heurrec_t *rec = oski_FindHeurRecord (id);
      if ((rec != NULL) && (rec->desc != NULL))
	desc = rec->desc;
    }
  return desc;
}

oski_id_t
oski_LookupHeurId (const char *name, oski_id_t id_ind, oski_id_t id_val)
{
  const oski_heurrec_t *rec = oski_LookupHeur (name, id_ind, id_val);
  if (rec != NULL)
    return rec->id;
  else
    return INVALID_ID;
}

void *
oski_LookupHeurIdMethod (oski_id_t id, const char *method_name)
{
  const oski_heurrec_t *rec = oski_FindHeurRecord (id);
  if (rec == NULL)
    return NULL;
  return oski_LookupModuleMethod (rec->module, method_name);
}

void *
oski_LookupHeurMethod (const char *heur_name,
		       oski_id_t id_ind, oski_id_t id_val,
		       const char *method_name)
{
  oski_id_t id = oski_LookupHeurId (heur_name, id_ind, id_val);
  if (id == INVALID_ID)		/* if not found, try to register */
    id = oski_RegisterHeur (heur_name, id_ind, id_val);
  if (id == INVALID_ID)		/* still not found */
    return NULL;
  else
    return oski_LookupHeurIdMethod (id, method_name);
}

/* ------------------------------------------------------------ */

/**
 *  \brief Returns a pointer to an open file, given separately
 *  specified directory and file names.
 *
 *  \param[in] dir Name of a directory, or NULL for the current
 *  directory.
 *  \param[in] base_filename Name of a file, or NULL for none.
 *  \param[in] mode File opening mode (see fopen()), or NULL for
 *  the default of "rt" (text file for reading).
 */
static FILE *
OpenFilePath (const char *dir, const char *base_filename, const char *mode)
{
  FILE *fp = NULL;
  size_t len;
  char *full_filename;

  len = 0;
  if (dir != NULL)
    len += strlen (dir);
  if (base_filename != NULL)
    len += strlen (base_filename);
  len += 2;

  full_filename = oski_Malloc (char, len);
  if (full_filename != NULL)
    {
      full_filename[0] = (char) NULL;
      if (dir != NULL)
	{
	  strcat (full_filename, dir);
	  strcat (full_filename, "/");
	}
      if (base_filename != NULL)
	strcat (full_filename, base_filename);

      oski_PrintDebugMessage (2, "Trying to open '%s'...", full_filename);

      fp = fopen (full_filename, (mode != NULL) ? mode : "rt");
      oski_Free (full_filename);
    }

  return fp;
}

/**
 *  \brief
 *
 *  \param[in] filename Name of the benchmark data file. This name
 *  should only contain a relative path.
 *
 *  If an environment variable named 'OSKI_BENCH_DIR' is defined,
 *  then this routine searches for 'filename' in that path first,
 *  before checking the default directory,
 *  OSKI_BENCH_DIR_DEFAULT (compile-time macro).
 *
 *  \returns Returns NULL on error, but does not call the error
 *  handler so this routine can be used to check quietly for file
 *  existence. Otherwise, returns a valid pointer to an open file
 *  for reading on success.
 */
FILE *
oski_OpenBenchmarkData (const char *filename)
{
  FILE *fp = OpenFilePath (getenv ("OSKI_BENCH_DIR"), filename, "rt");
  if (fp == NULL)
    fp = OpenFilePath (OSKI_BENCH_DIR_DEFAULT, filename, "rt");
  return fp;
}

void
oski_CloseBenchmarkData (FILE * fp)
{
  if (fp != NULL)
    fclose (fp);
}

/**
 *  \brief Returns 1 iff the given character denotes a 'comment'
 *  in the benchmark data input file.
 */
static int
IsCommentChar (char x)
{
  return x == '%' || x == '#';
}

/**
 *  \brief
 *
 *  \param[in] fp Pointer to a benchmark data file.
 *  \param[in,out] buffer Buffer in which to store the next line.
 *  \param[in] max_len Maximum length of 'buffer'.
 *
 *  \returns Returns 0 when the end-of-file is reached, or 1
 *  if a non-comment line was read from the file. 'buffer' always
 *  contains the last line read from the file regardless of the
 *  return value.
 */
int
oski_ReadBenchmarkLine (FILE * fp, char *buffer, size_t max_len)
{
  int i;

  if (!fgets (buffer, max_len, fp))
    return 0;

  /* Skip leading blanks */
  for (i = 0; buffer[i] && isspace (buffer[i]); i++)
    ;

  /* If first character is a comment character, skip this line */
  if (IsCommentChar (buffer[i]))
    return oski_ReadBenchmarkLine (fp, buffer, max_len);

  return 1;
}

/* eof */
