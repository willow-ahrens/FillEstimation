dnl
dnl ACX_GNU99_FLAGS
dnl
dnl Macro to check whether any special C compiler flag is required
dnl to activate GNU99. The user may force a particular flag by
dnl specifying the configure-time option, "--with-gnu99flag=<flag>".
dnl
dnl This macro has the following outputs/side-effects:
dnl
dnl   - Defines "HAVE_GNU99" in config.h to 1 if a GNU99 mode is supported
dnl   - Sets ${ac_compiler_gnu99} to "yes" if GNU99 mode is supported,
dnl     or to "no" otherwise
dnl   - Substitutes the variable GNU99FLAGS to the compiler flag needed
dnl     to activate GNU99 mode.
dnl
AC_DEFINE([HAVE_GNU99], [], [Define to non-zero if compiler has a GNU99 mode])

AC_DEFUN([ACX_GNU99_FLAGS],
[AC_PREREQ(2.59)
AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([AC_CANONICAL_HOST])

dnl Set this variable if there is a GNU99-mode compilation flag.
GNU99FLAGS=""
ac_compiler_gnu99=no

dnl Allow user to enable explicitly
AC_ARG_WITH(gnu99flag,
	AC_HELP_STRING([--with-gnu99flag=<flag>], [set compiler GNU99-mode flag]))

if test x"$with_gnu99flag" = x ; then
	dnl Try some common flags

	if test x"${ac_compiler_gnu}" = xno ; then
		dnl IBM AIX compiler flags
		dnl if test -z "${GNU99FLAGS}" ; then
		dnl	AX_CHECK_COMPILER_FLAGS([-qlanglvl=stdc99], [C99FLAGS="-qlanglvl=stdc99"], [])
		dnl fi

		dnl Sun C compiler flags
		dnl if test -z "${C99FLAGS}" ; then
		dnl 	AX_CHECK_COMPILER_FLAGS([-xc99], [C99FLAGS="-xc99"], [])
		dnl fi

		dnl HP-UX C compiler flag
		dnl if test -z "${C99FLAGS}" ; then
		dnl	AX_CHECK_COMPILER_FLAGS([-AC99], [C99FLAGS="-AC99"], [])
		dnl fi

		dnl Intel C flags
		if test -z "${GNU99FLAGS}" ; then
			AX_CHECK_COMPILER_FLAGS([-std=gnu99], [GNU99FLAGS="-std=gnu99"], [])
		fi
	else
		dnl Using GNU C compiler and GNU99FLAGS not set explicitly
		dnl gcc is a special case because it does not abort compilation
		dnl if the flag is not detected.

		if test -z "${GNU99FLAGS}" ; then
			AX_CHECK_COMPILER_FLAGS([-std=gnu99], [GNU99FLAGS="-std=gnu99"], [])
		fi
	fi

	dnl Problem with GNU99 flag on some architectures; disable GNU99FLAGS
	case "${host_vendor}" in
	ibm) GNU99FLAGS="" ;;
	*) ;;
	esac

	if test -n "${GNU99FLAGS}" ; then
		ac_compiler_gnu99=yes
		AC_DEFINE([HAVE_GNU99], [1])
	fi
elif test x"$with_gnu99flag" != xno ; then
	ac_compiler_gnu99=yes
	GNU99FLAGS="$with_gnu99flag"
	AC_DEFINE([HAVE_GNU99], [1])
fi

AC_SUBST([GNU99FLAGS])
])

dnl
dnl Series of macros to test for native GNU99-based IEEE floating point
dnl intrinsics.
dnl

dnl
dnl These macros define the following compile-time configuration variables:
dnl
AC_DEFINE([HAVE_GNU99_FPCLASSIFY], [], [Define to 1 if fpclassify() is available])
AC_DEFINE([HAVE_INFINITY], [], [Define to 1 if an infinity constant is available])
AC_DEFINE([HAVE_GNU99_INFINITY], [], [Define to 1 if an infinity constant is available in GNU99 mode])
AC_DEFINE([HAVE_NAN], [], [Define to 1 if a quiet NaN constant is available])
AC_DEFINE([HAVE_GNU99_NAN], [], [Define to 1 if a quiet NaN constant is available in GNU99 mode])
AC_DEFINE([HAVE_EPS_DOUBLE], [], [Define to 1 if a double-precision epsilon constant is available])
AC_DEFINE([HAVE_EPS_FLOAT], [], [Define to 1 if a single-precision epsilon constant is available])

dnl
dnl ACX_GNU99_FPCLASSIFY
dnl
dnl Tests for GNU99 fpclassify() intrinsic and its possible return values.
dnl On exit, sets ${ac_gnu99_fpclassify} to "yes" if fpclassify() is
dnl detected, and "no" otherwise.
dnl
AC_DEFUN([ACX_GNU99_FPCLASSIFY],
[AC_PREREQ(2.59)
AC_REQUIRE([ACX_GNU99_FLAGS])
AC_LANG_PUSH([C])
ac_save_LDFLAGS="${LDFLAGS}"
LDFLAGS="${LDFLAGS} -lm"

if test x"${ac_compiler_gnu99}" = xyes ; then
	ac_save_CFLAGS="${CFLAGS}"
	CFLAGS="${CFLAGS} ${GNU99FLAGS}"
	AC_MSG_CHECKING([for fpclassify intrinsic])

	AC_LINK_IFELSE([AC_LANG_PROGRAM([[
		#include <math.h>
		#include <stdio.h>
		]]
		, [[
		printf("zero(%f): %d\n", 0.0, fpclassify(0.0) == FP_ZERO);
		printf("inf(%f): %d\n", 0.0, fpclassify(0.0) == FP_INFINITE);
		printf("nan(%f): %d\n", 0.0, fpclassify(0.0) == FP_NAN);
		printf("normal(%f): %d\n", 0.0, fpclassify(0.0) == FP_NORMAL);
		printf("subnormal(%f): %d\n", 0.0, fpclassify(0.0) == FP_SUBNORMAL);
		return 0;
		]])]
		, [ac_gnu99_fpclassify=yes],
		, [ac_gnu99_fpclassify=no]
	)

	AC_MSG_RESULT([${ac_gnu99_fpclassify-no}])
	CFLAGS="${ac_save_CFLAGS}"

	if test x"${ac_gnu99_fpclassify}" = xyes ; then
		AC_DEFINE([HAVE_GNU99_FPCLASSIFY], [1])
	fi
fi

LDFLAGS="${ac_save_LDFLAGS}"
AC_LANG_POP([C])
])

dnl
dnl ACX_GNU99_NAN
dnl
dnl Tests for GNU99 macro/constant "NAN" for a quiet floating point NaN.
dnl On exit, sets ${ac_gnu99_nan} to "yes" if it exists, or "no" otherwise.
dnl
AC_DEFUN([ACX_GNU99_NAN],
[AC_PREREQ(2.59)
AC_REQUIRE([ACX_GNU99_FLAGS])
AC_LANG_PUSH([C])
ac_save_LDFLAGS="${LDFLAGS}"
LDFLAGS="${LDFLAGS} -lm"

if test x"${ac_compiler_gnu99}" = xyes ; then
	ac_save_CFLAGS="${CFLAGS}"
	CFLAGS="${CFLAGS} ${GNU99FLAGS}"
	AC_MSG_CHECKING([for GNU99 quiet NaN constant/macro, NAN])

	AC_LINK_IFELSE([AC_LANG_PROGRAM([[
		#include <math.h>
		#include <stdio.h>
		]]
		, [[
		printf( "NAN == %f\n", NAN );
		return 0;
		]])]
		, [ac_gnu99_nan=yes],
		, [ac_gnu99_nan=no]
	)

	AC_MSG_RESULT([${ac_gnu99_nan-no}])
	CFLAGS="${ac_save_CFLAGS}"

	if test x"${ac_gnu99_nan}" = xyes ; then
		AC_DEFINE([HAVE_GNU99_NAN], [1])
	fi
fi

LDFLAGS="${ac_save_LDFLAGS}"
AC_LANG_POP([C])
])

dnl
dnl ACX_GNU99_INFINITY
dnl
dnl Tests for GNU99 macro/constant "INFINITY" for a quiet floating point NaN.
dnl On exit, sets ${ac_gnu99_infinity} to "yes" if it exists, or "no" otherwise.
dnl
AC_DEFUN([ACX_GNU99_INFINITY],
[AC_PREREQ(2.59)
AC_REQUIRE([ACX_GNU99_FLAGS])
AC_LANG_PUSH([C])
ac_save_LDFLAGS="${LDFLAGS}"
LDFLAGS="${LDFLAGS} -lm"

if test x"${ac_compiler_gnu99}" = xyes ; then
	ac_save_CFLAGS="${CFLAGS}"
	CFLAGS="${CFLAGS} ${GNU99FLAGS}"
	AC_MSG_CHECKING([for GNU99 constant/macro, INFINITY])

	AC_LINK_IFELSE([AC_LANG_PROGRAM([[
		#include <math.h>
		#include <stdio.h>
		]]
		, [[
		printf( "INFINITY == %f\n", INFINITY );
		return 0;
		]])]
		, [ac_gnu99_infinity=yes],
		, [ac_gnu99_infinity=no]
	)

	AC_MSG_RESULT([${ac_gnu99_infinity-no}])
	CFLAGS="${ac_save_CFLAGS}"

	if test x"${ac_gnu99_infinity}" = xyes ; then
		AC_DEFINE([HAVE_INFINITY], [1])
	fi
fi

LDFLAGS="${ac_save_LDFLAGS}"
AC_LANG_POP([C])
])


