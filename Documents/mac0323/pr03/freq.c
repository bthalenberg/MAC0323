#include "stable.h"
#include "aux.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

answer *final;

//initializes the answer
static void init(SymbolTable table){
    // creating new answer
    // final = malloc(sizeof(answer));
    final->val = malloc(table->n * sizeof(int));
    final->str = malloc(table->n * sizeof(100 * sizeof(char)));
    final->index=0;
}

// auxiliary function in stable_visit
int visit(const char *key, EntryData *data){
    if(data == NULL) return 0;
    //if data is there, updates the answer array
    final->str[final->index] = key;
    final->val[final->index] = data->i;
    final->index++;
    int len = strlen(key);
    if (len > final->maxLen) final->maxLen = len;
    return 1;
}

//sorts the array and prints the result
static void sortAndPrint(SymbolTable table){
    int num = table->n;
    //bubble sorting the keys
    for (int i = 0; i < num; i++){
        for (int j = 0; j < num-1; j++){
            if (strcmp(final->str[j], final->str[j+1]) > 0){
                //some swaps
                char *t1 = final->str[j];
                final->str[j] = final->str[j+1];
                final->str[j+1] = t1;
                int t2 = final->val[j];
                final->val[j] = final->val[j+1];
                final->val[j+1] = t2;
            }
        }
    }
    //print the answer
    for (int i = 0; i < num; i++){
        printf("%s:%.*%d\n", table->str[i], final->maxLen, table->val[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;

    // cria a hashtable
    SymbolTable st = stable_create();

    // começa leitura do arquivo
	FILE *input = fopen(argv[1], "r");
    char word[100];
    while ((fscanf(input, "%s", word)) != EOF) {
        // vê se palavra já está na hashtable
        InsertionResult r = stable_insert(st, word);
        // se não está, insere
        if (r.new != 0)
            r.data->i = 1;
        // se está, atualiza valor
        else r.data->i++;
    }

    // percorre as entradas da HT e printa em ordem
    // lexicográfica cada palavra e a contagem

    // cria a estrutura da resposta
    init(st);
    // faz o processo de iteração
    int res = stable_visit(st, visit);
    if (res == 0) fprintf(stderr, "Erro ao percorrer as entradas!\n");
    // ordena e imprime as entradas
    else sortAndPrint(st);

    stable_destroy(st);
    return 0;
}
