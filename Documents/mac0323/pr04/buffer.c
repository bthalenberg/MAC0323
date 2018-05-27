#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "buffer.h"
#include "error.h"

/*
  Create and return a new and empty buffer.
*/
Buffer *buffer_create() {
    
    Buffer *buf;
    buf = emalloc(sizeof(Buffer));
    
    buf->data = emalloc(1024);
    buf->n = 1024;
    buf->i = 0; 
    
    return(buf);
}

/*
  Destroy a buffer.
*/
void buffer_destroy(Buffer *B) {
    
    free(B->data);
    B->data = NULL;
    
    free(B);
    B = NULL;
}

/*
  Reset buffer, eliminating contents.
*/
void buffer_reset(Buffer *B) {

    while (B->i > 0) {
        
        B->data[B->i] = 0;
        B->i--;
    }

}

/*
  Add a character c to the end of the buffer.
*/
void buffer_push_back(Buffer *B, char c) {

    int j;
    Buffer *new_buf;
    
    //Realloc the buffer if necessary.
    if (B->i == B->n) {
        new_buf = emalloc(sizeof(Buffer));
        new_buf->data = emalloc(2*B->n);

        for (j = 0 ; j < B->i; j++)
            new_buf->data[j] = B->data[j];

        free(B->data);
        B->data = new_buf->data;
        B->i = B->n;
        B->n  *= 2;
        
        free(new_buf->data);
        new_buf->data = NULL;
        
        free(new_buf);
        new_buf = NULL;
    }

    B->data[B->i] = c;
    B->i++;
}

/*
  Read a line (i.e., reads up to a newline '\n' character or the
  end-of-file) from the input file and places it into the given
  buffer, including the newline character if it is present. The buffer
  is resetted before the line is read.

  Returns the number of characters read; in particular, returns ZERO
  if end-of-file is reached before any characters are read.
*/
int read_line(FILE *input, Buffer *B) {

    int c, word;

    word = 0;
    buffer_reset(B);

    while ((c = fgetc(input)) != EOF && c != 10) {
        
        if (isspace (c) && word) {
            buffer_push_back(B, ((char)c));
            word = 0;
        }

        else if (!isspace (c)) {
            word++;
            buffer_push_back(B, ((char)c));
        }
    }


    //Explit in cases '\n' or EOF.
    if (c == 10) {
        
        if (B->i && isspace (B->data[B->i - 1])) {
            B->i--;
        }
        
        buffer_push_back(B, ((char)c));
    }
    
    else {
        
        if (isspace (B->data[B->i - 1]))
            B->i--;
    }
    
    return B->i; 
}