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
    int i, j;
    // goes through buffer char by char
    for (i = 0; i < b->i; i++) {
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
    pos = error_position(errptr, b);
    // prints line with error
    printf ("line %d: %s\n", line, b->data);
    // blank spaces to align error msg
    for (int i = 0; i < pos + 6; i++)   printf(" ");
    // pointer and error msg
    printf("^\n%s\n", error_msg);
}

/*
   Returns string containing type of operand for output formatting
*/
char *get_operand_string (OperandType type) {
    char *operand = " ";
    if (type == LABEL) return operand = "Label";
    if (type == REGISTER) return operand = "Register";
    if (type == NUMBER_TYPE) return operand = "Number";
    if (type == STRING) return operand = "String";
    return operand;

}

/*
    Prints instructions to StdOut
*/
void print_instruction(Instruction instr) {
    // Print label
    if (instr->label) printf ("label    = \"%s\"\n", instr->label);
    else printf ("label    = n/a\n");

    // Print operator
    printf ("operator = %s\n", instr->op->name);

    // Print operands
    // CASE NO OPERANDS
    if (instr->opds[0]->type == 0 && instr->opds[1]->type == 0 && instr->opds[2]->type == 0)
       printf ("operand  = n/a\n");

    // CASE ONE OPERAND of type string
    if (instr->opds[0]->type == STRING)
       printf ("operand  = %s(%s)\n", get_operand_string(instr->opds[0]->type), instr->opds[0]->value.str);
    // CASE ONE OPERAND of type label
    else if (instr->opds[1]->type == 0 && instr->opds[2]->type == 0 && instr->opds[0]->type == LABEL)
       printf ("operand  = %s(\"%s\")\n", get_operand_string(instr->opds[0]->type), instr->opds[0]->value.label);
    // CASE ONE OPERAND not a label
    else if (instr->opds[1]->type == 0 && instr->opds[2]->type == 0) {
        printf ("operand  = %s(%d)\n", get_operand_string(instr->opds[0]->type), instr->opds[0]->value);
    }

    // CASE TWO OPERANDS, one being a label
    else if (instr->opds[2]->type == 0 && instr->opds[1]->type == LABEL) {
       printf ("operands = %s(%d)", get_operand_string(instr->opds[0]->type), instr->opds[0]->value);
       printf (", %s(\"%s\")\n", get_operand_string(instr->opds[1]->type), instr->opds[1]->value.label);
    }
   // CASE TWO OPERANDS, none a label
    else if (instr->opds[2]->type == 0) {
       printf ("operands = %s(%d)", get_operand_string(instr->opds[0]->type), instr->opds[0]->value);
       printf (", %s(%d)\n", get_operand_string(instr->opds[1]->type), instr->opds[1]->value);
    }

    // CASE THREE OPERANDS
    else {
       printf ("operands = %s(%d)", get_operand_string(instr->opds[0]->type), instr->opds[0]->value);
       printf (", %s(%d)", get_operand_string(instr->opds[1]->type), instr->opds[1]->value);
       printf (", %s(%d)\n", get_operand_string(instr->opds[2]->type), instr->opds[2]->value);
    }
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
        if (parse (b->data, st, &instr, &errptr)) {
            if (b->data[0] != '*') printf ("line     = %s\n", b->data);
            // caso IS: armazena na ST
            else if (instr->op.opcode == -1) {
                InsertionResult res = stable_insert(st, instr->opds[0]);
                if (res.new == 0) {
                    fprintf(stderr, "line     = %s\n", b->data);
                    fprintf(stderr, "Invalid assignment: \"%s\" is already assigned.\n", label);
                }
                else res->data = instr->opds[1];
            }
            // printa as instruções
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
