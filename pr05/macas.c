#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "asm.h"
#include "error.h"


const char sufix[] = ".maco";

//Recebe o nome do arquivo de entrada
//gera o nome do arquivo de saida

char* outputName(char *name){
	int s, size, i=0;
	char *out;
	size = strlen(name);
	out = malloc(sizeof(char)*(size+10));
	while(i < size && name[i] != '.') {
		out[i] = name[i];
		i++;
	}
	s = i;
	siz = strlen(sufix);
	for(i = s; i < (s + size); i++)
		out[i] = sufix[i - s];


	return out;
}


/* Interprets command line, opens input and output files and calls assemble() to assemble code.
Generates error messages where appropriate. */
int main(int argc, char **argv) {
    int i;
	FILE *in, *out;
	char *infile, *outfile;

	for(i = 1; i < argc; i++) {
		infile = argv[i];
		outfile = outputName(infile);
		in = fopen(infile, "r");
		out = fopen(outfile, "w");
		if(!assemble(infile, in, out)) 
			die("error");
	}	


    return 0;
}
