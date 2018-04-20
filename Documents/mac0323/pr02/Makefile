############################# Makefile ##########################
all: center

center: buffer.o center.o
	gcc -o center buffer.o center.o

buffer.o: buffer.c
	gcc -o buffer.o -c buffer.c 

center.o: center.c buffer.h
	gcc -o center.o -c center.c 

clean:
	rm -rf *.o center

