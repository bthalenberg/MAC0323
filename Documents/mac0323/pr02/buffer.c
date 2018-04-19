#include "buffer.h"

/*
  Create and return a new buffer that holds member elements of size
  member_size.
*/
Buffer *buffer_create(size_t member_size){
	Buffer *B = malloc(sizeof(Buffer));
	B->member_size = member_size;
	B->buffer_size = 1;
	B->p = 0;
	return B;
}

/*
  Destroy a buffer.
*/
void buffer_destroy(Buffer *B){

}

/*
  Reset buffer, eliminating contents.
*/
void buffer_reset(Buffer *B){
}
