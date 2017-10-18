dnl
dnl configure.ac -- Process this file with autoconf to produce configure
dnl
AC_INIT(OSKI, 1.0.1h, oski-users@sf.net, oski)
AC_COPYRIGHT([Copyright (c) 2005-2007, Regents of the University of California])

dnl ------------------------------------------------------------------
dnl Boiler-plate stuff
dnl
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
AM_CONFIG_HEADER(config/config.h:config/config-h.in)

dnl Override some default macro names...

dnl ------------------------------------------------------------------
dnl New command-line options
dnl

dnl Enable slow timer (taken from FFTW-3.0 configure.in file).
AC_ARG_WITH(slow-timer, [AC_HELP_STRING([--with-slow-timer],[use low-precision timers (SLOW)])], with_slow_timer=$withval, with_slow_timer=no)
if test "$with_slow_timer" = "yes"; then
	AC_DEFINE(WITH_SLOW_TIMER,1,[Use low-precision timers])
fi

dnl Let user decide what types to build
OSKI_ENABLE_TYPES

dnl Let user disable MBCSR kernel submodules
OSKI_ENABLE_MBCSR

dnl Let the user disable off-line benchmarking
OSKI_ENABLE_BENCH

dnl ------------------------------------------------------------------
dnl Programs needed.
dnl

dnl C compiler: order favors vendor compilers over gcc
dnl
dnl NOTE: Subdirectories with independent 'configure'
dnl scripts should also include this statement so that
dnl all scripts select the same compiler.
dnl
AC_PROG_CC(xlc icc ecc cc gcc)

dnl F77 compiler (for linking against BLAS libraries)
dnl Check for the '--disable-fortran' flag.
OSKI_ENABLE_FORTRAN
dnl Select a Fortran compiler, if needed.
AS_IF([test x"$enable_fortran" = x"yes"], [AC_PROG_F77(xlf xlf90 ifort ifc efc f90 f77 gfortran g77)], [])
dnl Setup Fortran-from-C calling/linking conventions, if needed.
AS_IF([test x"$enable_fortran" = x"yes"], [OSKI_FORTRAN_SETUP], [])

dnl Determine platform
ACX_GETCPU

dnl Other programs.
AC_PROG_INSTALL
AC_PROG_RANLIB

dnl ------------------------------------------------------------------
dnl Library support.
dnl

dnl Platform-dependent flag fix-up
OSKI_FIXUP_FLAGS

dnl MacOS X-specific hacks
AM_CONDITIONAL(BUILD_darwin, [test x`echo "${host_os}" | grep "darwin"` != x])

dnl Shared library support.
AC_CONFIG_SUBDIRS(libltdl)
AC_LIBTOOL_DLOPEN
AC_LIBLTDL_CONVENIENCE
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)

dnl Generate a C symbol extractor, for use with LTDL
ACX_CSYMX([csymx.sh])

dnl Scientific computing library support.
dnl ACX_PROG_CC_MAXOPT(oski)
ACX_GET_COPTFLAGS_CPU
ACX_PTHREAD
ACX_BLAS
ACX_PAPI

AM_CONDITIONAL(HAVE_BLAS_LIBS, test x"$acx_blas_ok" = xyes)
AM_CONDITIONAL(BUILD_NOLAZY,
	test x"${enabled_shared}" = xno ||
	test x"${host_os}" = xcygwin)

dnl Add debug flag?
AC_ARG_WITH(debug,
	[AC_HELP_STRING([--with-debug=<opt>],
		[add debug flags to CFLAGS; <opt> in {yes,no,<string>}])])
ac_append_debug=""
case $with_debug in
	"" | no) ;;
	yes) ac_append_debug="-g" ;;
	*) ac_append_debug="${with_debug}" ;;
esac
if test -n "${ac_append_debug}" ; then
	AC_MSG_NOTICE([appending debugging flags to CFLAGS: ${ac_append_debug}])
	CFLAGS="${CFLAGS} ${ac_append_debug}"
fi

dnl ------------------------------------------------------------------
dnl Required headers and language features.
dnl

dnl Headers.
AC_HEADER_STDC
AC_HEADER_DIRENT
AC_CHECK_HEADERS([argz.h float.h limits.h locale.h malloc.h memory.h])
AC_CHECK_HEADERS([stddef.h stdlib.h string.h strings.h sys/time.h unistd.h])

# AIX-specific check
AC_CHECK_HEADERS([sys/systemcfg.h])

dnl Language features, structures and typedefs.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
# FIXME: AC_C_RESTRICT supercedes ACX_C_RESTRICT
AC_C_RESTRICT
AC_HEADER_TIME
AC_STRUCT_TM
AC_HEADER_STDBOOL
AC_C_VOLATILE
AC_CHECK_TYPES([ptrdiff_t])

ACX_C99_FLAGS
ACX_C99_FPCLASSIFY

dnl Functions.
AC_FUNC_VPRINTF
dnl AC_FUNC_REALLOC
AC_CHECK_FUNCS(bzero memset, break)
AC_CHECK_FUNCS(strcpy bcopy)
AC_CHECK_FUNCS(memcmp bcmp strncmp, break)
AC_CHECK_FUNCS(drand48 random rand)

dnl Timer-related.
ACX_BEBOP_TEMPO

dnl ------------------------------------------------------------------
dnl Specific configuration for OSKI-Lua
if test "${exec_prefix}" = NONE ; then
	DEF_OSKI_LUA_PATH="${prefix}/lib/${PACKAGE}/?.lua;${prefix}/lib/${PACKAGE}/?"
else
	DEF_OSKI_LUA_PATH="${exec_prefix}/lib/${PACKAGE}/?.lua;${exec_prefix}/lib/${PACKAGE}/?"
fi
AC_SUBST([DEF_OSKI_LUA_PATH])

dnl ------------------------------------------------------------------
dnl Combine C and, if available, C99-mode flags
dnl
CFLAGS="${CFLAGS} ${C99FLAGS}"

dnl ------------------------------------------------------------------
dnl List of Makefiles to generate.
dnl
dnl		gen/Makefile gen/driver/Makefile
dnl		gen/asg/Makefile gen/asg_trans/Makefile gen/dense/Makefile
dnl			gen/gen/Makefile gen/matrix/Makefile gen/oop/Makefile
dnl			gen/prog/Makefile gen/sparse/Makefile gen/util/Makefile
dnl			gen/sparse/BCSR/Makefile gen/sparse/MBCSR/Makefile
dnl		include/oski/BDIAG1/Makefile src/BDIAG1/Makefile
dnl
AC_CONFIG_FILES([
		Makefile
		include/oski/Makefile
		include/oski/oski.h
		lua/include/Makefile lua/src/Makefile lua/src/lib/Makefile
		src/Makefile src/oski.lua
		src/timer/Makefile
		src/corelib/Makefile
		include/oski/CSR/Makefile src/CSR/Makefile
			src/CSR/MatMult/Makefile
			src/CSR/SymmMatMult/Makefile
			src/CSR/MatTrisolve/Makefile
		include/oski/CSC/Makefile src/CSC/Makefile
		include/oski/BCSR/Makefile src/BCSR/Makefile
			src/BCSR/MatMult/Makefile
			src/BCSR/MatTransMatMult/Makefile
		include/oski/MBCSR/Makefile src/MBCSR/Makefile
			src/MBCSR/MatMult/Makefile
			src/MBCSR/MatTrisolve/Makefile
			src/MBCSR/MatTransMatMult/Makefile
			src/MBCSR/SymmMatMult/Makefile
			src/MBCSR/MatMultAndMatTransMult/Makefile
		include/oski/GCSR/Makefile src/GCSR/Makefile
			src/GCSR/MatMult/Makefile
		include/oski/CB/Makefile src/CB/Makefile
		include/oski/DENSE/Makefile src/DENSE/Makefile
		include/oski/VBR/Makefile src/VBR/Makefile
		include/oski/heur/Makefile src/heur/Makefile
			src/heur/mregblock/Makefile
			src/heur/regprof/Makefile
			src/heur/symmrb/Makefile
                src/lt/Makefile
		util/Makefile
		bench/Makefile
		tests/Makefile
		doc/Makefile
	])

AC_OUTPUT

echo "------------------------------------------------------------------------
Configuration

  Source code location:
    ${srcdir}
  Host (CPU architecture):
    ${host} (${CPU})
  Compiler:
    ${CC}
  Compiler flags:
    ${CFLAGS}
  Linker flags:
    ${LDFLAGS}
  Fortran linker flags:
    ${FLIBS}
  Install path:
    ${prefix}
"

if test x"$ac_oski_build_int_double" = x"no" ; then
  if test x"$with_index_binding" = x"" \
     || test x"$with_value_binding" = x"" \
  ; then
    echo "
  *** WARNING ***
    You have disabled the int-double scalar types, but did not explicitly
    override the default bindings for 'oski_index_t' and 'oski_value_t',
    which are 'int' and 'double', respectively. This means that your
    OSKI programs will need to include the type-specific oski/oski_Txy.h
    header-file, rather than the usual oski/oski.h.
"
  fi
fi

echo "------------------------------------------------------------------------"

dnl OSKI_ESTBUILDTIME

dnl eof
