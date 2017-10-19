CC = gcc
CXX = g++
CFLAGS = -O3 -std=c99 -march=native `../gsl/bin/gsl-config --cflags` `pkg-config --cflags glib-2.0` -I../taco/include
CXXFLAGS = -O3 -std=c++11 `../gsl/bin/gsl-config --cflags` `pkg-config --cflags glib-2.0` -I../taco/include -DDECIMAL_DIG=17
LDLIBS = `../gsl/bin/gsl-config --libs` `pkg-config --libs glib-2.0` -L../taco/lib -ltaco -ldl

all: reference asx oski spmv
clean:
	rm -rf reference asx oski *.o *.dSYM *.trace *.pyc

reference: run_fill.o test_fill.o reference.o util.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

asx: run_fill.o test_fill.o asx.o util.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

oski: run_fill.o test_fill.o oski.o util.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

spmv: run_spmv.o test_spmv.o util.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)
