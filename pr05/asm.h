#include <stdio.h>

/* Args: filename (to print error messages), input stream with code to assemble and output stream
to write object code. Prints appropriate error messages in stderr. If assemble is successful,
returns non-zero value. Else, returns zero. */
int assemble(const char *filename, FILE *input, FILE *output);
