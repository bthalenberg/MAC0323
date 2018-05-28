#include "parser.h"
#include "stable.h"
#include <stdio.h>

int main(int argc, char* argv) {
    if (argc < 2) {
        fprintf(stderr, "O nome do arquivo deve ser fornecido como argumento.\n");
        return 1;
    }
    FILE *f = fopen(argv[1]);
    SymbolTable *st = stable_create();
    // ler cada linha do arquivo e fornecê-la à função parse
    // se parse retorna com sucesso, imprime a linha de código e o conteúdo da estrutura instr devolvido
    // se ocorreu um erro, mostrar onde e imprimir mensagem de erro apropriada
    // quando a instrução é o pseudo-operador IS, associar o valor do operando ao rótulo passado
    // inserindo o par correspondente na alias_table. Se já foi associado, produzir mensagem de erro.
    return 0;
}
