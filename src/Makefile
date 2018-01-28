TOP = $(realpath $(dir $(realpath $(firstword $(MAKEFILE_LIST))))..)
MT19937 = $(TOP)/deps/mt19937/install
TACO = $(TOP)/deps/taco/install
ARCH = $(shell uname)
define ENV_SH
endef
ifeq ($(wildcard $(TOP)/src/Makefile.$(ARCH)),)
	MYARCH = Default
else
	MYARCH = $(ARCH)
endif

include $(TOP)/src/Makefile.$(MYARCH)

CFLAGS += -std=c99 -I$(MT19937)/include -I$(TACO)/include
CXXFLAGS += -std=c++11 -I$(MT19937)/include -I$(TACO)/include -DDECIMAL_DIG=17
LDLIBS += -L$(TACO)/lib -ltaco -ldl

all: reference phil oski spmv spmv_record env.sh
clean:
	rm -rf reference phil oski spmv spmv_record env.sh *.o *.dSYM *.trace *.pyc

reference: run_fill.o test_fill.o reference.o util.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

phil: run_fill.o test_fill.o phil.o util.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

oski: run_fill.o test_fill.o oski.o util.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

spmv: run_spmv.o test_spmv.o util.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

spmv_record: run_spmv_record.o test_spmv.o util.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

export ENV_SH
env.sh:
	echo "$$ENV_SH" > $(TOP)/src/env.sh
