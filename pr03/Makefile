############################# Makefile ##########################
CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -O2 -Wextra
DEPS = stable.h

%.o: %.c ${DEPS}
	${CC} -c -o $@ $< ${CFLAGS}

all: freq

freq: freq.o stable.o
	${CC} ${CFLAGS} -o freq freq.o stable.o

clean:
	rm -rf *.o freq
