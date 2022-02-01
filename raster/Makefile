CC = g++
CFLAGS = -Wall -O3
SDL = `sdl2-config --cflags --libs`
CLIBS = -lm -fopenmp

all: main

main: main.o
	$(CC) $(CFLAGS) -o $@ $^ $(CLIBS) $(SDL)

main.o: main.c raster.h Makefile
	$(CC) $(CFLAGS) -c $< $(CLIBS) $(SDL)


clean:
	rm -f *.o
	rm -f *.out
	rm -rf main
