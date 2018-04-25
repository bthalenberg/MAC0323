############################# Makefile ##########################
CFLAGS = -Wall -pedantic -O2 -Wextra

all: center

center: buffer.o center.o
	gcc ${CFLAGS} -o center buffer.o center.o

buffer.o: buffer.c
	gcc ${CFLAGS} -o buffer.o -c buffer.c

center.o: center.c buffer.h
	gcc ${CFLAGS} -o center.o -c center.c

clean:
	rm -rf *.o center
