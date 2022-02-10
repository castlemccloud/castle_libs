CC = g++
CFLAGS = -Wall -O3 -g
SDL = `sdl2-config --cflags --libs`
CLIBS = -lm -fopenmp

all: main.out

main.out: main.o raster.o
	$(CC) $(CFLAGS) -o $@ $^ $(CLIBS) $(SDL)

main.o: main.c raster.h Makefile
	$(CC) $(CFLAGS) -c $< $(CLIBS) $(SDL)

raster.o: raster.c raster.h Makefile
	$(CC) $(CFLAGS) -c $< $(CLIBS)

test: main.out
	valgrind --tool=callgrind --collect-jumps=yes --dump-instr=yes ./main.out
	
clean:
	rm -f *.o
	rm -f *.out


