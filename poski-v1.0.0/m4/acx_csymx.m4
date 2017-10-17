dnl
dnl ACX_CSYMX(output-file)
dnl
dnl Creates a shell script that can be used to extract a list of
dnl C symbols from an object or library file. The script can be
dnl used to initialize an 'lt_preloaded_symbols' array for
dnl users who wish to link against ltdl-based libraries but
dnl who do not wish to depend on or use libtool.
dnl
dnl This macro requires 'libtool.m4' from libtool 1.5 or higher.
dnl
dnl Author: Rich Vuduc <richie@vuduc.com>
dnl

AC_DEFUN([ACX_CSYMX], [
AC_REQUIRE([AC_LIBTOOL_SYS_GLOBAL_SYMBOL_PIPE])

ac_csymx_output="$1"
ac_csymx_nm="${NM}"
ac_csymx_pipe="${lt_cv_sys_global_symbol_pipe}"
ac_csymx_sym2decl="${lt_cv_sys_global_symbol_to_cdecl}"
ac_csymx_scriptargs="\$""@"

if test -n "${ac_csymx_output}" \
    && test -n "${ac_csymx_pipe}${acx_csymx_sym2decl}" \
; then
  # Output the script that will extract C symbols.
  rm -f "${ac_csymx_output}"
  echo \
"#!/bin/sh
#
# C symbol extractor.
#

#-----------------------------------------------------------------------
# System-dependent variables.

# Command to list symbols in an object archive
if test -z \"\${NM}\" ; then
  NM=\"${ac_csymx_nm}\"
fi

# Command to convert process \${NM} output
if test -z \"\${SYMPIPE}\" ; then
  SYMPIPE=\"${ac_csymx_pipe}\"
fi

# Command to convert \${SYMPIPE} output
# to C external symbol declarations.
if test -z \"\${SYM2DECL}\" ; then
  SYM2DECL=\"${ac_csymx_sym2decl}\"
fi

#-----------------------------------------------------------------------
# Process command-line options.

# Default value for options.
DO_PIPE=no
DO_DECL=no
DO_LT_PAIRS=no

is_option=yes
while test x\"\${is_option}\" = xyes ; do
    ARG=\"\$""1\"
    do_shift=yes
    case \"\${ARG}\" in
        -h | -H | --help)
            echo \\
\"
usage: \$""0 [options] <obj1> [<obj2> ...]

Extract global C symbols contained in the given list of objects.

Valid [options]:
  -p | --pipe
    Print a list of global C symbols.

  -d | --decl
    List C style declarations of the form
      extern int <var>;
    for each symbol <sym>.

  -l | --lt-pairs
    Dump pairs in a form suitable for initializing
    an lt_preloaded_symbols table:
      \\\"<sym>\\\", &<sym>,

  -h | -H | --help
    Displays this message.
\"
            exit 0
            ;;
        -p | --pipe) DO_PIPE=yes ;;
        -d | --decl) DO_DECL=yes ;;
        -l | --lt-pairs) DO_LT_PAIRS=yes ;;
        -*)
            echo \"*** Unrecognized option, '\${ARG}' ***\"
            echo \"Rerun with option '-h' (or '--help') for usage.\"
            exit 1
            ;;
        *) is_option=no ; do_shift=no ;;
    esac

    if test x\"\${do_shift}\" = xyes ; then
        shift
    fi
done

if test x\"\${DO_PIPE}\" = xyes \\
    || test x\"\${DO_DECL}\" = xyes \\
    || test x\"\${DO_LT_PAIRS}\" = xyes \\
; then
    DO_ANY=yes
else
    DO_ANY=no
fi

for obj in \"\$""@\" ; do
    if test x\"\${DO_ANY}\" = xno ; then
        eval \"\${NM} \${obj}\"
    fi
    if test x\"\${DO_PIPE}\" = xyes ; then
        eval \"\${NM} \${obj} | \${SYMPIPE}\"
    fi
    if test x\"\${DO_LT_PAIRS}\" = xyes ; then
        echo \"\\\"\`basename \${obj}\`\\\", 0x0,\"
        echo '{print \"\\\"\"\$''3\"\\\", &\"\$''3\",\";}' \\
             > _t_csymx_awkscript.awk
        eval \"\${NM} \${obj} | \${SYMPIPE} | awk -f _t_csymx_awkscript.awk\"
        rm -f _t_csymx_awkscript.awk
    fi
    if test x\"\${DO_DECL}\" = xyes ; then
        eval \"\${NM} \${obj} | \${SYMPIPE} | \${SYM2DECL}\"
    fi
done

# eof" > "${ac_csymx_output}"
  chmod +x "${ac_csymx_output}"
fi
])

dnl eof

