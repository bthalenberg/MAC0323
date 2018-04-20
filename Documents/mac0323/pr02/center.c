#include <ctype.h>
#include <stdio.h>
#include "buffer.h"

int main(int argc, char *argv[])
{	
	//nome do arquivo de entrada
	FILE *input = fopen(argv[1], "r");

	//nome do arquivo de saida
	FILE *output = fopen(argv[2], "w");

	//numero de colunas c
	size_t member_size = atoi(argv[3]);

	char buffer[1000];

	Buffer *B = buffer_create(member_size);

	printf("%ld\n", B->member_size);

	while(fgets(buffer, 999, input) != NULL) 
    {
        fprintf (output, "%s\n", buffer);
    }

    fclose(input);
    fclose(output);

	return 0;
}