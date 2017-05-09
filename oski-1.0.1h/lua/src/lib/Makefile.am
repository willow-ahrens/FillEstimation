#
# Lua standard libraries
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.luaoski

noinst_LTLIBRARIES = liblualib.la
liblualib_la_SOURCES = lauxlib.c lbaselib.c ldblib.c liolib.c \
	lmathlib.c ltablib.c lstrlib.c loadlib.c
liblualib_la_CFLAGS = $(lua_cflags)
#liblualib_la_LDFLAGS = $(lua_ldflags)

# eof
