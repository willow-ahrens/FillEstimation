#
# Main BeBOP Library (matrix-type independent)
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.luaoski

noinst_LTLIBRARIES = liblua.la
liblua_la_SOURCES = lapi.c lcode.c ldebug.c ldo.c ldump.c lfunc.c lgc.c \
	llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c \
	ltable.c ltests.c ltm.c lundump.c lvm.c lzio.c \
	\
	lapi.h ldo.h llex.h lobject.h lstate.h ltm.h \
	lzio.h lcode.h lfunc.h llimits.h lopcodes.h lstring.h lundump.h \
	ldebug.h lgc.h lmem.h lparser.h ltable.h lvm.h
liblua_la_CFLAGS = $(lua_cflags)
#liblua_la_LDFLAGS = $(lua_ldflags)

# eof
