CC = cc
CFLAGS = -std=c99 -march=native -O3 `~/gsl-install/bin/gsl-config --cflags` `pkg-config --cflags glib-2.0`
LDLIBS = `~/gsl-install/bin/gsl-config --libs` `pkg-config --libs glib-2.0`

all: reference asx oski
clean:
	rm -rf reference asx oski *.o *.dSYM *.trace *.pyc

run.o: run.c

reference: run.o test.o reference.o util.o

asx: run.o test.o asx.o util.o

oski: run.o test.o oski.o util.o
