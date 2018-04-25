#include <ctype.h>
#include <stdio.h>
#include "buffer.h"

int main(int argc, char *argv[])
{
    if (argc != 4) return 1;
	//nome do arquivo de entrada
	FILE *input = fopen(argv[1], "r");

	//nome do arquivo de saida
	FILE *output = fopen(argv[2], "w");

	//numero de colunas c
	size_t member_size = atoi(argv[3]);

	Buffer *B = buffer_create(member_size);

	int i = 1;
	int line = 0;
	int errorLines[100];
	int errors = 0;
	do {
		line++;
		i = read_line(input, B);
        if (i == 0) break;
		int dif = atoi(argv[3]) - i;
		if (dif < 0) {
			errorLines[errors++] = line;
		}
		for (int j = 0; j <= dif/2; j++) fprintf(output, "+");
		for (unsigned j = 0; j < B->buffer_size-1; j++)
			if(B->data[j] != EOF) fprintf(output, "%c", B->data[j]);
		for (int j = 1; j <= dif/2; j++) fprintf(output, "+");
		fprintf(output, "\n");

	} while (i != 0);

	for (i = 0; i < errors; i++)
		fprintf(stderr, "center: %s: line %d: line too long.\n", argv[1], errorLines[i]);

	buffer_destroy(B);

    fclose(input);
    fclose(output);

	return 0;
}
