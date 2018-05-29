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

void print_instruction(Instruction instr) {
    // Print label
    if (instr->label) printf ("label    = \"%s\"\n", instr->label);
    else printf ("label    = n/a\n");

    // Print operator
    printf ("operator = %s\n", instr->op->name);

    // Prints operands
    // FAZER ESSE CÓDIGO
}

/*
  Reads a line in search for assignment operador IS
  If not found, returns 0;
  If found, tries to register it in the symbol table.
  If value was already assigned, prints error message and returns 0.
  Else, registers it and returns 1;
*/
int assignment_function(char *s, int i) {
    int i = 0;
    Buffer *b = buffer_create();
    // Ignores spaces
    while (isspace(s[i])) i++;
    // Stops if first word is comment
    if (s[i] == '*') return 0;
    // Scans in search for IS
    while (s[i] != '\0' && s[i] != '\n' && s[i] == 'I')
        if (!isspace(s[i]) buffer_push_back (b, s[i++]);
    // acabou a linha sem achar
    if (s[i] == '\0' || s[i] == '\n') return 0;
    // achou o IS, a partir de agora vai ler espaços e o valor
    else if (s[i++] == 'S') {
        InsertionResult res = stable_insert(st, b);
        if (res.new == 0) {
            fprintf(stderr, "line     = %s\n", b->data);
            fprintf(stderr, "Invalid assignment: %s is already assigned.\n", label);
            return 0;
        }
        else {
            buffer_reset(b);
            while (s[i] != '\0' && s[i] != '\n' && !isspace(s[i]))
                buffer_push_back (b, s[i++]);
            // AQUI FALTA TRANSFORMAR PARA TIPO DO OPERANDO
            OperandValue val = b;
            res->data = val;
        }
    }
    return 1;
}

int main(int argc, char* argv) {
    if (argc != 2) die("Invalid argument.\n");
    // config for error.c
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
        if (assignment_function()) continue;
        // if parse was successful, prints line and instr content
        else if (parse (b->data, st, &instr, &errptr)) {
            if (b->data[0] != '*') printf ("line     = %s\n", b->data);
            while (instr) {
                print_instruction(instr);
                instr = instr->next;
            }
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
