#include <ctype.h>
#include <stdio.h>
#include "buffer.h"

int main(int argc, char *argv[])
{	
	char *input, *output;
	size_t member_size;
	Buffer *B;

	//nome do arquivo de entrada
	input = argv[1];

	//nome do arquivo de saida
	output = argv[2];

	//numero de colunas c
	member_size = atoi(argv[3]);

	printf("%s\n", input);
	printf("%s\n", output);
	printf("%ld\n", member_size);

	return 0;
}