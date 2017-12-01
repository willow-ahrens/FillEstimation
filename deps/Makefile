TOP = $(dir $(realpath $(firstword $(MAKEFILE_LIST))/..))

.PHONY: all gsl taco clean clean_gsl clean_taco

all: gsl taco

clean: clean_gsl clean_taco

gsl:
	$(MAKE) -C $(TOP)gsl all

taco:
	$(MAKE) -C $(TOP)taco all

clean_gsl:
	$(MAKE) -C $(TOP)gsl clean

clean_taco:
	$(MAKE) -C $(TOP)taco clean

