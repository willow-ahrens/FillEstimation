TOP = $(realpath $(dir $(realpath $(firstword $(MAKEFILE_LIST))))../..)
GSL=$(TOP)/deps/gsl
ARCH = $(shell uname)
ifeq ($(wildcard $(GSL)/Makefile.$(ARCH)),)
	MYARCH = default
else
	MYARCH = ARCH
endif

.PHONY: all clean

all: stamp

include $(GSL)/Makefile.$(MYARCH)

clean:
	rm -f $(GSL)/stamp
	rm -rf $(GSL)/build
