TOP = $(realpath $(dir $(realpath $(firstword $(MAKEFILE_LIST))))../..)
TACO=$(TOP)/deps/taco
ARCH = $(shell uname)
ifeq ($(wildcard $(TACO)/Makefile.$(ARCH)),)
    ARCH = default
endif

.PHONY: all clean

all: stamp

include $(TACO)/Makefile.$(ARCH)

clean:
	rm -f $(TACO)/stamp
	rm -rf $(TACO)/build
