#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "buffer.h"
#include "stable.h"
#include "error.h"
#include "asmtypes.h"

int main(int argc, char* argv) {
    if (argc != 2) die("Invalid argument.\n");

    SymbolTable *st = stable_create();
    Buffer *b = buffer_create();
    const char *errptr;
    Instruction instr;

    FILE *f = fopen(argv[1], "r");
    if (f == null) die("Invalid file.\n")
    int cur, len;
    cur = len = 1;
    // Reads text until EOF
    while (len) {
        len = read_line(f, b);
        if (len == 0) break;
        // quando a instrução é o pseudo-operador IS, associar o valor do operando ao rótulo passado
        // inserindo o par correspondente na alias_table. Se já foi associado, produzir mensagem de erro.

        // if parse was successful, prints line and instr content
        if (parse (b->data, st, &instr, &errptr)) {
            if (b->data[0] != '*') printf ("line     = %s\n", b->data);
            // aqui vem código para printar instruções
        }
        // if an error occurred, prints error message to stderr with line
        else print_error(errptr, bu, cur);
        printf("\n");
        cur++;
    }
    fclose(f);
    buffer_destroy(b);
    stable_destroy(st);
    return 0;
}
