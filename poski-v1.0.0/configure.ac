dnl --------------------------------------------------------------------------
dnl $Id: configure.ac for pOSKI-v1.0.0
dnl Process this file with "autoconf" to produce a configure script.
dnl Copyright (C) 2011, Regents of the University of California, Berkeley
AC_PREREQ([2.65])
AC_INIT([pOSKI],[v1.0.0],[bebop-poski-admin@lists.eecs.berkeley.edu],poski)
AC_COPYRIGHT([Copyright (C) 2011, University of California, Berkeley])

dnl --------------------------------------------------------------------------
dnl Configure inputs
dnl
AC_CONFIG_SRCDIR([config])
AC_CONFIG_HEADERS([config/config.h:config/config-h.in])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl --------------------------------------------------------------------------
dnl Check existing OSKI 
dnl
AC_ARG_WITH(oski, [AC_HELP_STRING([--with-oski=<opt>], [build oski; <opt> in {yes,no}])], [], [with_oski=no])
case $with_oski in
	"" | no) ;;
	yes) OSKIDIR="${prefix}/build_oski" ; OSKIBUILD="${ac_pwd}/build_oski";;
esac

if test x"${OSKIDIR}" = x""; then
	echo ""
	echo "	!* Error: can't find existing OSKI."
	echo "	!*        make sure that you have an existing OSKI,"
	echo "	!*        set OSKI environment correctly: % export OSKIDIR={path to your existing oski install directory} if you have existing OSKI, or"
	echo "  !*        run ./configure --with-oski=yes if you don't have existing OSKI."
	echo ""
	exit 
fi
if test x"${OSKIBUILD}" = x""; then
	echo ""
	echo "	!* Error: can't find existing OSKI."
	echo "	!*        make sure that you have an existing OSKI,"
	echo "	!*        set OSKI environment correctly: % export OSKIBUILD={path to your existing oski build directory} if you have existing OSKI, or"
	echo "  !*        run ./configure --with-oski=yes if you don't have existing OSKI."
	echo ""
	exit 
fi



dnl --------------------------------------------------------------------------
dnl Checks for platform
dnl
AC_CANONICAL_HOST
case "${host_cpu}" in
	x86*) have_tested=yes;;
	*) have_tested=no;;
esac

dnl --------------------------------------------------------------------------
dnl Checks for programs
dnl
AC_PROG_CC([icc gcc cc])
AC_PROG_INSTALL

AC_PROG_RANLIB

dnl --------------------------------------------------------------------------
dnl Add libltdl support
dnl
AC_CONFIG_SUBDIRS([libltdl])
AC_LIBTOOL_DLOPEN
AC_LIBLTDL_CONVENIENCE
AC_PROG_LIBTOOL
AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)

dnl --------------------------------------------------------------------------
dnl Generate a C symbol extractor, for use with LTDL
dnl
ACX_CSYMX([csymx.sh])

dnl --------------------------------------------------------------------------
dnl Checks for header files
dnl
AC_HEADER_STDC
AC_CHECK_HEADERS([stdio.h stdlib.h stdbool.h stdarg.h string.h strings.h])
AC_CHECK_HEADERS([malloc.h assert.h time.h sys/time.h sys/types.h sys/sysinfo.h])
AC_CHECK_HEADERS([stddef.h unistd.h argz.h float.h limits.h locale.h memory.h])
AC_CHECK_HEADERS([sched.h])
# Solaris-specific check
AC_CHECK_HEADERS([sys/processor.h sys/procset.h])
# AIX-specific check
AC_CHECK_HEADERS([sys/systemcfg.h])


ACX_GNU99_FLAGS
dnl ACX_C99_FPCLASSIFY


dnl --------------------------------------------------------------------------
dnl Checks for threads library
dnl
ACX_PTHREAD
AX_OPENMP
AC_NUMA

dnl --------------------------------------------------------------------------
dnl Checks for SSE supports
dnl
AX_EXT

dnl --------------------------------------------------------------------------
dnl Checks for types & structures
dnl
AC_C_CONST
AC_C_INLINE
AC_C_RESTRICT
AC_C_VOLATILE
AC_TYPE_SIZE_T

dnl --------------------------------------------------------------------------
dnl Checks for library functions
dnl
AC_FUNC_VPRINTF
AC_FUNC_MALLOC
AC_CHECK_FUNCS(bzero memset, break)
AC_CHECK_FUNCS(strcpy bcopy)
AC_CHECK_FUNCS(memcmp bcmp strncmp, break)
AC_CHECK_FUNCS(drand48 random rand)

dnl --------------------------------------------------------------------------
dnl Checks for system services
dnl

dnl --------------------------------------------------------------------------
dnl Checks for compiler characteristics
dnl
dnl --------------------------------------------------------------------------
dnl Override CFLAGS selection for compiler dependent flag
dnl
CFLAGS="-I${OSKIDIR}/include ${CFLAGS}"
CFLAGS="${CFLAGS} ${GNU99FLAGS} ${PTHREAD_CFLAGS} ${OPENMP_CFLAGS} ${SIMD_FLAGS} ${NUMA_LIBS}"
LDFLAGS="${LDFLAGS} -lm"

dnl --------------------------------------------------------------------------
dnl AC_CONFIG_FILES
dnl
AC_CONFIG_FILES([Makefile	\
		 include/poski/Makefile \
                 src/Makefile	\
		 m4/Makefile	\
		 tests/Makefile])
AC_OUTPUT

dnl --------------------------------------------------------------------------
dnl Report configuration
dnl
echo "---------------------------------------------------------------------
Configuration
	Source code location:
		${srcdir}
	Host:
		${host}
	Compiler:
		${CC}
	Compiler flags:
		${CFLAGS}
	Linker flags:
		${LDFLAGS}
	Install path:
		${prefix}
---------------------------------------------------------------------
NOTE: 
	!* OSKI is in ${OSKIDIR}.
	!* pOSKI is in ${prefix}/."
if test x"$have_tested" = x"yes"; then
	echo "	!* pOSKI has been tested on your host cpu (${host_cpu})."
else
	echo "	!* WARNING: pOSKI has not been tested on your host cpu (${hot_cpu})."
fi
if test x"$acx_pthread_ok" = x"no"; then
	echo "	!* WARNING: Your system not support pthread."
fi
if test x"$ax_openmp_ok" = x"no"; then
	echo "	!* WARNING: Your system not support openmp."
fi

## Create an environment variable file
varfile=poski-var.sh
touch $varfile > /dev/null 2>$varfile && echo "	!* File $varfile created." || echo "	!* Error: Failed to create $varfile file."
echo "#!/bin/bash" >> $varfile
echo "
export OSKIDIR=${OSKIDIR}
export OSKIBUILD=${OSKIBUILD}
export POSKIDIR=${prefix}
export LD_LIBRARY_PATH=${prefix}:${OSKIDIR}:${LD_LIBRARY_PATH}
export POSKISRC=${srcdir}
export POSKICC=${CC}
export POSKICFLAGS="\""${CFLAGS}\"""
" >> $varfile
chmod +x $varfile
echo "	!* You can run \"% source $varfile\" to set the poski environment."
echo "---------------------------------------------------------------------"

