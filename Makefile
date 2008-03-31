CFLAGS = -g 
LDFLAGS = -lm

all: ganho
ganho: hash.o

clean:
	rm -f *.o ganho
