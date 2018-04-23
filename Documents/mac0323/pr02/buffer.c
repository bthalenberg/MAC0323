#include "buffer.h"
#include <stdio.h>
#include <string.h>

/*
  Create and return a new buffer that holds member elements of size
  member_size.
*/
Buffer *buffer_create(size_t member_size){
	Buffer *B = malloc(sizeof(Buffer));
	B->data = malloc(B->member_size * sizeof(char));
	B->member_size = member_size;
	B->buffer_size = 1;
	B->p = 0;
	return B;
}

/*
  Destroy a buffer.
*/
void buffer_destroy(Buffer *B){
	free (B->data);
	B->data = NULL;
	free (B);
	B = NULL;
}

/*
  Reset buffer, eliminating contents.
*/
void buffer_reset(Buffer *B){
	free(B->data);
	B->data = malloc(B->member_size * sizeof(char));
	B->buffer_size = 1;
	B->p = 0;
}

/*
  Return a valid pointer to the first free position of the
  buffer. This means that, if the space allocated is not enough, then
  the buffer size is increased and the contents are copied.
*/
void *buffer_push_back(Buffer *B){
	/*

	if p < buffer_size return p

	create new buffer
	allocate double the memory (buffer_size)
	copy the contents from b to it
	make pointer that points to b point to it
	return p
	*/
}

/*
  Read a line (i.e., reads up to a newline '\n' character or the
  end-of-file) from the input file and places it into the given
  buffer, including the newline character if it is present. The buffer
  is resetted before the line is read.
  Returns the number of characters read; in particular, returns ZERO
  if end-of-file is reached before any characters are read.
*/
int read_line(FILE *input, Buffer *B){
	buffer_reset(B);
	do{
		buffer_push_char(B,(char)fgetc(input));
		B->p++;
	}while(B->data[B->p-1] != '\n' && B->data[B->p-1] != EOF);

	if(B->data[B->p-1] == EOF) return 0;
	return 1;
}