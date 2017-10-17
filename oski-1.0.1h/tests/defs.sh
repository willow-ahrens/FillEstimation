#!/bin/sh

OSKI_LD_LIBRARY_PATH=`${top_builddir}/src/echo_oski_ldpath.sh`
export OSKI_LD_LIBRARY_PATH

OSKI_LUA_PATH=`${top_builddir}/src/echo_oski_lua_path.sh`
export OSKI_LUA_PATH

MAT_DIR=${top_srcdir}/tests
MAT_LIST="${MAT_DIR}/ibm32.pua"
export MAT_LIST

# eof
