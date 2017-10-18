CC = cc
CFLAGS = -std=c99 -march=native -O3 `~/gsl-install/bin/gsl-config --cflags` `pkg-config --cflags glib-2.0` -I../taco/include
CPPFLAGS = -O3 `~/gsl-install/bin/gsl-config --cflags` `pkg-config --cflags glib-2.0` -I../taco/include
LDLIBS = `~/gsl-install/bin/gsl-config --libs` `pkg-config --libs glib-2.0` -L../taco/lib/ -ltaco

all: reference asx oski spmv
clean:
	rm -rf reference asx oski *.o *.dSYM *.trace *.pyc

reference: run_fill.o test_fill.o reference.o util.o

asx: run_fill.o test_fill.o asx.o util.o

oski: run_fill.o test_fill.o oski.o util.o

spmv: run_spmv.o test_spmv.o util.o
