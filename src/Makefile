TOP = $(realpath $(dir $(realpath $(firstword $(MAKEFILE_LIST))))..)
TACO = $(TOP)/deps/taco/install
ARCH = $(shell uname)
LDXX = $(CXX)
define ENV_SH
endef
ifeq ($(wildcard $(TOP)/src/Makefile.$(ARCH)),)
	MYARCH = Default
else
	MYARCH = $(ARCH)
endif

include $(TOP)/src/Makefile.$(MYARCH)

#CFLAGS += -std=c99 -I$(TACO)/include
CXXFLAGS += -std=c++11 -fopenmp -I$(TACO)/include #-DDECIMAL_DIG=17
LDLIBS += -L$(TACO)/lib -ltaco -ldl

all: reference oski phil pphil spmv spmv_record env.sh
clean:
	rm -rf reference phil oski spmv spmv_record env.sh *.o *.dSYM *.trace *.pyc

reference: run_fill.o test_fill.o reference.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

oski: run_fill.o test_fill.o oski.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

phil: run_fill.o test_fill.o phil.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

pphil: run_fill.o test_fill.o pphil.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

spmv: run_spmv.o test_spmv.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

spmv_record: run_spmv_record.o test_spmv.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

export ENV_SH
env.sh:
	echo "$$ENV_SH" > $(TOP)/src/env.sh
