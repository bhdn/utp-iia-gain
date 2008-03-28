CFLAGS = -g

all: ganho
ganho: hash.o

clean:
	rm -f *.o ganho
