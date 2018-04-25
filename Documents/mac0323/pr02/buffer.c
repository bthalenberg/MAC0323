#include "buffer.h"
#include <stdio.h>
#include <string.h>

/*
  Create and return a new buffer that holds member elements of size
  member_size.
*/
Buffer *buffer_create(size_t member_size){
	Buffer *B = malloc(sizeof(Buffer));
	B->data = malloc(member_size * sizeof(char));
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

	if (B->p < B->buffer_size) return (B->data + B->p);

	B->data = realloc(B->data, 2 * B->member_size * sizeof(char));
    B->member_size *= 2;
	return (B->data + B->p);
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
    int whiteCount = 1;
	do {
        char c = (char) fgetc(input);
        if (c == ' ' || c == '\t') {
            whiteCount++;
        }
        else whiteCount = 0;
        if (whiteCount <= 1) {
		buffer_push_char(B, c);
		B->p++;
        B->buffer_size++;
        }
	} while (B->data[B->p-1] != '\n' && B->data[B->p-1] != EOF);
    if (B->data[0] == EOF) return 0;
	return B->p;
}
