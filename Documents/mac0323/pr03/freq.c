#include "aux.h"
#include "stable.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

answer *final;

//initializes the answer
static void init(SymbolTable table){
    // creating new answer
    final = malloc(sizeof(answer));
    final->val = malloc((table->n + 1 )* sizeof(int));
    final->str = malloc((table->n + 1) * sizeof(100 * sizeof(char)));
    final->index=0;
}

// auxiliary function in stable_visit
int visit(const char *key, EntryData *data){
    if(data == NULL) return 0;
    //if data is there, updates the answer array
    final->str[final->index] = (char*) key;
    final->val[final->index] = data->i;
    final->index++;
    int len = strlen(key);
    if (len > final->maxLen) final->maxLen = len;
    return 1;
}

//sorts the array and prints the result
static void sortAndPrint(SymbolTable table){
    int num = table->n;
    int j, v;
    char *k;
    //insertion sort in the keys
    for (int i = 1; i < num; i++){
        k = final->str[i];
        v = final->val[i];
        j = i - 1;
        while (j >= 0 && strcmp(final->str[j], k) > 0) {
            final->str[j+1] = final->str[j];
            final->val[j+1] = final->val[j];
            j--;
        }
        final->str[j+1] = k;
        final->val[j+1] = v;
    }

    //print the answer
    for (int i = 0; i < num; i++){
        printf("%s:%.*d\n", final->str[i], final->maxLen, final->val[i]);
    }
    free(final->val);
    final->val = NULL;
    free(final->str);
    final->str = NULL;
    free(final);
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
        // SEGFAULT AQUI
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
