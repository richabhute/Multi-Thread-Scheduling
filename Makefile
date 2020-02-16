.phony all:
all: P2

P2: mts.c
	gcc -pthread -o mts mts.c


.PHONY clean:
clean:
	-rm -rf *.o *.exe

