TOP = $(realpath $(dir $(realpath $(firstword $(MAKEFILE_LIST))))../..)
MT19937=$(TOP)/deps/mt19937
ARCH = $(shell uname)
ifeq ($(wildcard $(MT19937)/Makefile.$(ARCH)),)
	MYARCH = Default
else
	MYARCH = $(ARCH)
endif

.PHONY: all clean

all: stamp

include $(MT19937)/Makefile.$(MYARCH)

clean:
	rm -rf $(MT19937)/stamp install
