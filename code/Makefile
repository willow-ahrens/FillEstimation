CC = gcc-7
CFLAGS = -std=c99 -lm -march=native -O3 -g3 `pkg-config --cflags glib-2.0`
LDFLAGS = -lgsl `pkg-config --libs glib-2.0`

all: reference asx oski
clean:
	rm -rf reference asx oski *.o *.dSYM

reference: run.c test.o reference.o

asx: run.c test.o asx.o util.o

oski: run.c test.o oski.o
