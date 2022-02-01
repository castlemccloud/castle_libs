CC = g++
CFLAGS = -Wall -O3

all: main.out

main.out: main.o btree.o
	$(CC) $(CFLAGS) -o $@ $^ -fopenmp -lm `sdl2-config --cflags --libs`

main.o: main.cpp btree.h tiny_obj_loader.h Makefile
	$(CC) $(CFLAGS) -c $< -lm -fopenmp `sdl2-config --cflags --libs`

btree.o: btree.c btree.h Makefile
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o
	rm -f *.out
