#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "error.h"
#include "parser.h"
#include "asmtypes.h"
#include "buffer.h"
#include "stable.h"
#include "asm.h"

/* Reads many files with object code and links final program in assembly language.

Errors checked:
    > "main" label undefined
    > same label defined more than once
    > undefined label (checks if unresolved labels in each file were exported by any other file)
*/

int main(int argc, char **argv) {

	if(argc <= 2) return 1;

	Buffer *b = buffer_create();
	SymbolTable extern_table = stable_create(); //SymbolTable que conterá os rótulos extern 
	int n = argc - 2; //numero de códigos-objetos
	int *acc = malloc((n+1) * sizeof(int)); //guardará a soma acumulada das linhas até o arquivo i
	int main_pos = -1;  //guardará a posição do rótulo main 
	FILE *out = fopen(argv[1], "w"); //arquivo de saida
	FILE **in = emalloc((n+1) * sizeof(FILE*)); //n entradas
	acc[0] = 0;

	for(int i = 1; i <= n; i++) {
		//Abre os arquivos de entrada
		if((in[i] = fopen(argv[i+1],"r")) == NULL) {
			die("ERRO file '%s' not found", argv[i+1]);
		}
		// Lê número de linhas
		read_line(in[i], b);
		// Calculamos soma acumulada
		if(b->data[b->i-2] == '\n') b->data[b->i-2] = '\0';
		int tam = atoi(b->data);
		acc[i] = acc[i-1] + tam;
	}

	//Recebe os rótulos extern e põe na st com localização
	// verifica erros de extern
	for (int i = 1; i <= n; i++) {
		while (read_line(in[i], b) && b->data[0] != 'B') {
			if (b->data[b->i-2] == '\n') b->data[b->i-2] = '\0';
			char *token = strtok(b->data," ");
			char *label; 
			int rel_pos;
			token = strtok(NULL, " ");
			label = estrdup(token);
			token = strtok(NULL, " ");
			rel_pos = atoi(token);
			if (!strcmp(label,"main")) {
				if (main_pos >= 0) {
					die("EXTERN label main defined twice");
				}
				main_pos = acc[i-1] + rel_pos + 1;
			}
			InsertionResult res = stable_insert(extern_table, label);
			if (res.new == 0) {
				die("EXTERN label defined twice");
			}
			(*res.data).s = acc[i-1] + rel_pos + 1;
			free(label);
		}
	}

	//Se main_pos nao mudou de valor, main nao foi definida
	if (main_pos == -1) die("EXTERN label 'main' never defined");
	fprintf(out,"48%06x\n", main_pos);
	int line = 0;

	// Fazemos leitura linha a linha, calculando desvios necessários
	for (int i = 1; i <= n; i++) {
		while (read_line(in[i], b)) {
			line++;
			if (b->data[b->i-2] == '\n') b->data[b->i-2] = '\0';
			if (b->data[0] == '*') {

				char *token = strtok(b->data," *");
				int opcode = atoi(token);
				token = strtok(NULL, " *");

				if (isdigit(token[0])) {
					int reg = atoi(token);
					token = strtok(NULL, " *");
					EntryData *data;
					data = stable_find(extern_table, token);
					if (data == NULL) die("Unresolved symbol %s", token);
					int dest = data->s, source = line;
					if (dest >= source)
						fprintf(out,"%02x%02x%04x\n", opcode, reg, dest - source);
					else
						fprintf(out,"%02x%02x%04x\n", opcode + 1, reg, source - dest);
				}

				else {
					EntryData *data;
					data = stable_find(extern_table, token);
					if (data == NULL) die("Unresolved symbol '%s'", token);
					int dest = data->s, source = line;
					if (dest >= source)
						fprintf(out, "%02x%06x\n", opcode, dest - source);
					else
						fprintf(out, "%02x%06x\n", opcode + 1, source - dest);
				}
			}

			else fprintf(out,"%s\n",b->data);
		}
	}

	free(acc);
	fclose(out);
	for (int i = 1; i <= n; i++) fclose(in[i]);
	free(in);
	stable_destroy(extern_table);
	buffer_destroy(b);
	return 0;
}


