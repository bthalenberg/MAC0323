#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "buffer.h"
#include "stable.h"
#include "error.h"
#include "asmtypes.h"

// finds error position
int get_error_position(const char *errptr, Buffer *b) {
    return 0;
}

// prints error message
void print_error(const char *errptr, Buffer *b, int line) {
    const char *error_msg;
    int pos;
    error_msg = get_error_msg();
    pos = error_position(errptr, b);
    // prints line with error
    printf ("line %d: %s\n", line, b->data);
    // blank spaces to align error msg
    for (int i = 0; i < pos + 6; i++)   printf(" ");
    // pointer and error msg
    printf("^\n%s\n", error_msg);
}

int main(int argc, char* argv) {
    if (argc != 2) die("Invalid argument.\n");

    set_prog_name("parse_test");

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
        else print_error(errptr, b, cur);
        printf("\n");
        cur++;
    }
    fclose(f);
    buffer_destroy(b);
    stable_destroy(st);
    return 0;
}
