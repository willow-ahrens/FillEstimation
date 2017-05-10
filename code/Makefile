CC = gcc-7
CFLAGS = -std=c99 -march=native -lm -g
LDFLAGS = -lgsl

all: reference asx oski
clean:
	rm -rf reference asx oski *.o *.dSYM

reference: run.c test.o reference.o

asx: run.c test.o asx.o

oski: run.c test.o oski.o
