CFLAGS = -g -std=c99
LDFLAGS = -lm

all: ganho
ganho: hash.o

clean:
	rm -f *.o ganho
