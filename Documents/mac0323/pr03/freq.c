#include "stable.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;

    // cria a hashtable
    SymbolTable *st = stable_create();

    // começa leitura do arquivo
	FILE *input = fopen(argv[1], "r");
    char word[100];
    while ((fscanf(file, "%s", word)) != EOF) {
        // vê se palavra já está na hashtable
        InsertionResult *r = stable_insert(st, word);
        // se não está, insere
        if (r->new != 0)
            r->data.i = 1;
        // se está, atualiza valor
        else r->data.i++;
    }

    // percorre as entradas da HT e printa em ordem
    // lexicográfica cada palavra e a contagem

    stable_destroy();
    return 0;
}
