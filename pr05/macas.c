#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "asm.h"
#include "parser.h"
#include "stable.h"
#include "buffer.h"

int main(int argc, char *argv[])
{
	if (argc == 1 || argc > 3) {
		return 0;
	}
	FILE *infile = fopen(argv[1],"r");
	if (infile == NULL)
		return 0;
	FILE *outfile;
	if (argc == 2) {
		char *s = estrdup(argv[1]);
		char *aux;
		int tam = strlen(s);
		for(int i = tam-1; i >= 0; i--) {
			if(s[i] == '.') {
				s[i] = '\0';
				break;
			}
		}
		aux = emalloc((tam+5)*sizeof(char));
		strcpy(aux,s);
		strcat(aux,".maco");
		outfile = fopen(aux,"w");
		free(s);
		free(aux);
	}
	else {
		outfile = fopen(argv[2],"w");
	}
	assemble(argv[1],infile,outfile);
	fclose(infile); fclose(outfile);
	return 0;
}