CFLAGS = -Wall -O3 -g
CXXFLAGS = -Wall -O3 -g
LDFLAGS = -L$(HOME)/.local/lib -lmpiP -lmpi
CC := mpicc
CXX := mpicxx

.PHONY: all
all: psrs test/40m.in

psrs: CC=$(CXX)
psrs: merge.o psrs.o main.o

merge.o: merge.cpp merge.h

psrs.o: psrs.c psrs.h

main.o: main.c

test:
	mkdir -p test

test/40m.in: | test
	seq 40000000 | shuf >$@

.PHONY: clean
clean:
	-$(RM) *.o psrs
