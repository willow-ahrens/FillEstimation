TOP = $(realpath $(dir $(realpath $(firstword $(MAKEFILE_LIST))))..)
DEPS = $(TOP)/deps

.PHONY: all taco mt19937 clean clean_taco

all: taco mt19937

clean: clean_taco clean_mt19937

mt19937:
	$(MAKE) -C $(DEPS)/mt19937 all

taco:
	$(MAKE) -C $(DEPS)/taco all

clean_mt19937:
	$(MAKE) -C $(DEPS)/mt19937 clean

clean_taco:
	$(MAKE) -C $(DEPS)/taco clean

