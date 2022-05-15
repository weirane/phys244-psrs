CFLAGS = -Wall -O3 -g
CXXFLAGS = -Wall -O3 -g
LDFLAGS = -L$(HOME)/.local/lib -lmpiP -lmpi

psrs: CC=$(CXX)
psrs: merge.o psrs.o main.o

merge.o: merge.cpp merge.h

psrs.o: psrs.c psrs.h

main.o: main.c

.PHONY: clean
clean:
	-$(RM) *.o psrs
