#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "buffer.h"
#include "stable.h"
#include "error.h"
#include "asmtypes.h"

/*
    Finds error position in a string
*/
int get_error_position(const char *errptr, Buffer *b) {
    unsigned int i, j;
    // goes through buffer char by char
    for (i = 0; i < b->p; i++) {
        // compares in search for error
        for (j = 0; errptr[j] == b->data[i+j] && errptr[j] != '\0' && errptr[j] != '\n'; j++);
        if (errptr[j] == '\0' || errptr[j] == '\n') break;
    }
    return i;
}

/*
    Prints error message
*/
void print_error(const char *errptr, Buffer *b, int line) {
    const char *error_msg;
    int pos;
    error_msg = get_error_msg();
    pos = get_error_position(errptr, b);
    // prints line with error
    printf ("line %d: ", line);
    for(unsigned int i = 0; i < b->p && b->data[i] != '*'; i++)
                printf ("%c", b->data[i]);

    // blank spaces to align error msg
    for (int i = 0; i < pos + 6; i++)   printf(" ");
    // pointer and error msg
    printf("^\n%s\n", error_msg);
}

/*
    Prints instructions to StdOut
*/
void print_instruction(Instruction instr) {
    // Print label

    if (instr.label) printf ("label    = \"%s\"\n", instr.label);
    else printf ("label    = n/a\n");

    // Print operator
    printf ("operator = %s\n", instr.op->name);

    // Print operands
    printf ("operands =");
    // CASE NO OPERANDS
    if (instr.opds[0]->type == 0) {
       printf (" n/a\n");
       return;
   }

    int i = 0;
    while (i < 3 && instr.opds[i]->type != 0) i++;

    for (int j = 0; j < i; ++j) {
        if (instr.opds[j]->type == STRING){
            printf(" String(%s)", instr.opds[j]->value.str);
            if (j < i - 1) printf(",");
        }
        if (instr.opds[j]->type == LABEL){
            printf(" Label(\"%s\")", instr.opds[j]->value.label);
            if (j < i - 1) printf(",");
        }
        if (instr.opds[j]->type == REGISTER) {
            printf(" Register(%d)", instr.opds[j]->value.reg);
            if (j < i - 1) printf(",");
        }
        else {
            printf (" Number(%lld)", instr.opds[j]->value.num);
            if (j < i - 1) printf(",");
        }
    }
    printf("\n");
}

int main(int argc, char** argv) {
    if (argc != 2) die("Invalid argument.\n");
    // config for error.c
    set_prog_name("parse_test");

    SymbolTable st = stable_create();
    Buffer *b = buffer_create(100);
    const char *errptr;
    Instruction* instr = malloc(sizeof(Instruction));

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) die("Invalid file.\n");
    int cur, len;
    cur = len = 1;
    // Reads text until EOF
    while (len) {
        len = read_line(f, b);
        if (len == 0) break;
        // if parse was successful, prints line and instr content
        if (parse (b->data, st, &instr, &errptr)) {
            if (b->data[0] != '*'){
                printf ("line     = ");
                unsigned int i;
                for (i = 0; i < b->p && b->data[i] != '*'; i++)
                printf ("%c", b->data[i]);
                if (b->data[i] == '*') printf("\n");
                // caso IS: armazena na ST
                if (instr->op->opcode == -1) {
                    InsertionResult res = stable_insert(st, instr->label);
                    if (res.new == 0) {
                        fprintf(stderr, "line     = %s\n", b->data);
                        fprintf(stderr, "Invalid assignment: \"%s\" is already assigned.\n", instr->label);
                    }
                    else {
                        res.data->opd = emalloc(sizeof(Operand));
                        res.data->opd->type = REGISTER;
                        res.data->opd->value.reg = instr->opds[0]->value.reg;
                    }
                }
                // printa as instruções
                while (instr != NULL ) {
                    print_instruction(*instr);
                    instr = instr->next;
                }
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
