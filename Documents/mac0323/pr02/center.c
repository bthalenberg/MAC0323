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

	//char buffer[1000];

	Buffer *B = buffer_create(member_size);

	int i = 1;
	do{
		i = read_line(input, B);
		int dif = B->member_size - B->buffer_size;
		for(int j = 1; j <= dif/2; j++) printf("_");
		for(int j = 0; j < B->buffer_size-1; j++){
			if(B->data[j] != EOF) printf("%c", B->data[j]);
		}	
		for(int j = 1; j <= dif/2; j++) printf("_");
		printf("\n");

	}while(i != 0);
	
	printf("\n\n");
	//while(fgets(buffer, 999, input) != NULL) 
    //{
    //    fprintf (output, "%s\n", buffer);
    //}

	buffer_destroy(B);

    fclose(input);
    fclose(output);

	return 0;
}