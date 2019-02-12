CC = g++
ARGS = -std=c++11 -Wall -m32

all: threads

threads: threads.cc
	$(CC) $(ARGS) -c -o threads.o threads.cc

clean:
	rm -rf *.o *~
