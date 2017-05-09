/**
 *  \file include/oski/oski_Tiz.h
 *  \brief Maps the default OSKI interface names to (int, complex_double).
 */

#if defined(OSKI_REBIND)
/** Remove all bindings */
#  define OSKI_UNBIND
#  include <oski/oski.h>
#  undef OSKI_UNBIND
#endif

/* Rebind */
#undef DEF_IND_TYPE
#undef DEF_VAL_TYPE
#define DEF_IND_TYPE 1 /**< Set default index type to 'int' */
#define DEF_VAL_TYPE 4 /**< Set default value type to 'complex_double' */
#include <oski/oski.h>

/* eof */
