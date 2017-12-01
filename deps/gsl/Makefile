TOP = $(dir $(realpath $(firstword $(MAKEFILE_LIST))/../..))
GSL=$(TOP)deps/gsl/

.PHONY: all gsl clean clean_gsl

all: gsl

gsl:
	mkdir -p $(GSL)build
	(cd gsl-2.4; ./configure --prefix=$(GSL)build; make -j; make install)

clean: clean_gsl

clean:
	rm -rf $(GSL)build
