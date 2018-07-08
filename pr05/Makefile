############################# Makefile ##########################
TARGET = prog
CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -O2 -Wextra

.PHONY: clean

all: macas maclk

macas: parser.o \
		stable.o \
		error.o \
		asmtypes.o \
		optable.o \
		buffer.o \
		asm.o \
		macas.o

maclk: parser.o \
		stable.o \
		error.o \
		asmtypes.o \
		optable.o \
		buffer.o \
		asm.o \
		maclk.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *~ macas maclk