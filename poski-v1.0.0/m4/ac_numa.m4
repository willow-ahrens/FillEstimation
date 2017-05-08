dnl ---------------------------------------------------------------------------
dnl Macro: AC_NUMA
dnl Defines HAVE_NUMA_H and NUMA_LIBS if --with-numa is used and 
dnl the target platform has libnuma installed
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_NUMA], [{
  AC_MSG_CHECKING([whether we should try to build with NUMA optimizations (libnuma) ])
  AC_ARG_WITH(numa, AC_HELP_STRING([--with-numa], [Compile with NUMA optimizations (default is NO)]), [], [with_numa=no])
  AC_MSG_RESULT([$with_numa])

  NUMA_LIBS=""
  case $with_numa in
    yes)
      	AC_CHECK_HEADER(numa.h, numa_inc_found=yes, [])
      	AC_CACHE_VAL(ac_cv_numa_lib_found, [AC_CHECK_LIB(numa, numa_available, ac_cv_numa_lib_found=yes)])
      	if test "$numa_inc_found" = "yes" -a "$ac_cv_numa_lib_found" = "yes" ; then
		NUMA_LIBS="-lnuma"
          	AC_DEFINE([HAVE_NUMA_H], [1], [Define to enable NUMA optimizations])
     	else
          	AC_MSG_RESULT([libnuma not found (install libnuma package if you want to use libnuma)])
     	fi 
	;;
   "" | no) 
	;;
  esac

  AC_SUBST(NUMA_LIBS)
}])

dnl AC_NUMA


