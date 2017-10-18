#!/bin/sh
#
# This script initializes and exports an environment variable,
# ENV_VARS, which contains all environment initialization needed
# to run the benchmark program 'oskibench_Txy'. ${ENV_VARS} can
# be passed directly to 'env'.
#
# In addition, the script will export other variables needed
# to execute the benchmark (e.g., setting up the PATH variable
# under cygwin so DLLs can be loaded).
#
# A caller will normally source this script. The script accepts
# the following environment variables:
#
#   DISABLE_BENCH
#     If set to 'yes', this script will 'touch' the output file
#     if given and execute 'exit 0'.
#

# -- Set environment variables
ENV_VARS="" ; export ENV_VARS

if test x"${OSKI_LD_LIBRARY_PATH}" "!=" x ; then
    ENV_VARS="${ENV_VARS} OSKI_LD_LIBRARY_PATH=${OSKI_LD_LIBRARY_PATH}"
else
    ENV_VARS="${ENV_VARS} OSKI_LD_LIBRARY_PATH=`${top_builddir}/src/echo_oski_ldpath.sh`"
fi

if test x"${OSKI_DEBUG_LEVEL}" "!=" x ; then
    ENV_VARS="${ENV_VARS} OSKI_DEBUG_LEVEL=${OSKI_DEBUG_LEVEL}"
fi

if test x"${OSKI_LUA_PATH}" "!=" x ; then
    ENV_VARS="${ENV_VARS} OSKI_LUA_PATH=${OSKI_LUA_PATH}"
else
    ENV_VARS="${ENV_VARS} OSKI_LUA_PATH=`${top_builddir}/src/echo_oski_lua_path.sh`"
fi

if test x"`uname`" = xAIX ; then
    if test x"${LIBPATH}" = x ; then
	ENV_VARS="${ENV_VARS} LIBPATH=`${top_builddir}/src/echo_oski_ldpath.sh`"
    else
	ENV_VARS="${ENV_VARS} LIBPATH=`${top_builddir}/src/echo_oski_ldpath.sh`:${LIBPATH}"
    fi
fi

if test x"`uname | grep -i CYGWIN`" != x ; then
    oski_path=`${top_builddir}/src/echo_oski_ldpath.sh`
    if test x"${PATH}" != x ; then
	new_path="${oski_path}:${PATH}"
    else
	new_path="${oski_path}"
    fi
    PATH=`echo "${PATH}" | sed 's, ,\\ ,g'`
    export PATH
fi

# eof
