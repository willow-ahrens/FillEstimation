CFLAGS +=
CXXFLAGS += -DDECIMAL_DIG=17
LDLIBS +=

TOP = $(realpath $(dir $(realpath $(firstword $(MAKEFILE_LIST))))..)
ARCH = $(shell uname)
ifeq ($(wildcard $(TOP)/src/Makefile.$(ARCH)),)
	MYARCH = default
else
	MYARCH = ARCH
endif
include $(TOP)/src/Makefile.$(MYARCH)

all: reference asx oski spmv
clean:
	rm -rf reference asx oski *.o *.dSYM *.trace *.pyc

reference: run_fill.o test_fill.o reference.o util.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

asx: run_fill.o test_fill.o asx.o util.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

oski: run_fill.o test_fill.o oski.o util.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

spmv: run_spmv.o test_spmv.o util.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)
