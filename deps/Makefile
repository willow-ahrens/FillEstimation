TOP = $(dir $(realpath $(firstword $(MAKEFILE_LIST))/..))
DEPS = $(TOP)deps/

.PHONY: all gsl taco clean clean_gsl clean_taco

all: gsl taco

clean: clean_gsl clean_taco

gsl:
	$(MAKE) -C $(DEPS)gsl all

taco:
	$(MAKE) -C $(DEPS)taco all

clean_gsl:
	$(MAKE) -C $(DEPS)gsl clean

clean_taco:
	$(MAKE) -C $(DEPS)taco clean

