TOP = $(realpath $(dir $(realpath $(firstword $(MAKEFILE_LIST))))..)
GSL = $(TOP)/deps/gsl/build
TACO = $(TOP)/deps/taco/build
ARCH = $(shell uname)
ifeq ($(wildcard $(TOP)/src/Makefile.$(ARCH)),)
	MYARCH = Default
else
	MYARCH = $(ARCH)
endif

include $(TOP)/src/Makefile.$(MYARCH)

CFLAGS += -std=c99 `$(GSL)/bin/gsl-config --cflags` -I$(TACO)/include
CXXFLAGS += -std=c++11 `$(GSL)/bin/gsl-config --cflags` -I$(TACO)/include -DDECIMAL_DIG=17
LDLIBS += `$(GSL)/bin/gsl-config --libs` -L$(TACO)/lib -ltaco -ldl

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
