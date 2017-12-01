TOP = $(dir $(realpath $(firstword $(MAKEFILE_LIST))/../..))
TACO=$(TOP)deps/taco/

.PHONY: all taco clean clean_taco

all: taco

taco:
	mkdir -p $(TACO)build
	(cd taco-oopsla2017; cmake -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=$(TACO)build .;  make -j; make install)

clean: clean_taco

clean:
	rm -rf $(TACO)build
