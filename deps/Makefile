TOP = $(realpath $(dir $(realpath $(firstword $(MAKEFILE_LIST))))..)
DEPS = $(TOP)/deps

.PHONY: all taco clean clean_taco

all: taco

clean: clean_taco

taco:
	$(MAKE) -C $(DEPS)/taco all

clean_taco:
	$(MAKE) -C $(DEPS)/taco clean
