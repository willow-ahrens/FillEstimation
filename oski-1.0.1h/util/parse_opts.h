/**
 *  \file util/parse_opts.h
 *  \brief Parse command-line options.
 */

#if !defined(INC_UTIL_PARSE_OPTS_H)
/** util/parse_opts.h included. */
#define INC_UTIL_PARSE_OPTS_H

#include <stdio.h>
#include <oski/common.h>

#if defined(IND_TAG_CHAR) && defined(VAL_TAG_CHAR)

#include <oski/mangle.h>
#include <oski/vecview.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define ParseValue  MANGLE_(ParseValue)
#define PrintValue  MANGLE_(PrintValue)
#define PrintDebugValue  MANGLE_(PrintDebugValue)
#define PrintVecViewInfo  MANGLE_(PrintVecViewInfo)
#define PrintDebugVecViewInfo  MANGLE_(PrintDebugVecViewInfo)
/*@}*/
#endif /* DO_NAME_MANGLING */

/**
 *  \brief Parses a string containing a real or complex value.
 */
oski_value_t ParseValue (const char *s);

/**
 *  \brief Print a value to a file.
 */
void PrintValue (FILE * fp, const char *name, oski_value_t x);

/**
 *  \brief Print a value to a file.
 */
void PrintDebugValue (int level, const char *name, oski_value_t x);

/**
 *  \brief Print a vector view summary.
 */
void PrintVecViewInfo (FILE * fp, const char *name, oski_vecview_t x);

/**
 *  \brief Print a vector view summary.
 */
void PrintDebugVecViewInfo (int level, const char *name, oski_vecview_t x);
#endif /* IND_TAG_CHAR && VAL_TAG_CHAR */

/**
 *  \brief Returns 0 <==> the given source string does not match
 *  any of a list of possible strings.
 */
int StringMatches (const char *source, ...);

/**
 *  \brief Parses a string specifying a size in bytes.
 */
size_t ParseByteString (const char *s);

/**
 *  \brief Parse a vector layout string.
 */
oski_storage_t ParseDenseMatLayout (const char *s);

/**
 *  \brief Parse a matrix-transpose option.
 */
oski_matop_t ParseMatTransOp (const char *s);

/**
 *  \brief Print matrix transpose option.
 */
void PrintMatTransOp (FILE * fp, const char *matname, oski_matop_t op);

/**
 *  \brief Parse a \f$A^TA\cdot x\f$ matrix-transpose option.
 */
oski_ataop_t ParseMatATAOp (const char *s);

/**
 *  \brief Print \f$A^TA\cdot x\f$ matrix-transpose option.
 */
void PrintMatATAOp (FILE * fp, const char *matname, oski_ataop_t op);

/**
 *  \brief Print matrix transpose option.
 */
void PrintDebugMatTransOp (int level, const char *matname, oski_matop_t op);

/**
 *  \brief Print \f$A^TA\cdot x\f$ matrix-transpose option.
 */
void PrintDebugMatATAOp (int level, const char *matname, oski_ataop_t op);

#endif

/* eof */
