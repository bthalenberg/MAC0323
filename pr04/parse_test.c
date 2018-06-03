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
    for(int i = 0; i < b->p && b->data[i] != '*'; i++)
                printf ("%c", b->data[i]);
   
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
    if (instr.label) printf ("label    = \"%s\"\n", instr.label);
    else printf ("label    = n/a\n");

    // Print operator
    printf ("operator = %s\n", instr.op->name);

    // Print operands
    // CASE NO OPERANDS
    if (instr.opds[0]->type == 0 && instr.opds[1]->type == 0 && instr.opds[2]->type == 0)
       printf ("operand  = n/a\n");

    // CASE ONE OPERAND of type string
    if (instr.opds[0]->type == STRING)
       printf ("operand  = %s(%s)\n", get_operand_string(instr.opds[0]->type), instr.opds[0]->value.str);
    // CASE ONE OPERAND of type label
    else if (instr.opds[1]->type == 0 && instr.opds[2]->type == 0 && instr.opds[0]->type == LABEL)
       printf ("operand  = %s(\"%s\")\n", get_operand_string(instr.opds[0]->type), instr.opds[0]->value.label);
    // CASE ONE OPERAND not a label
    else if (instr.opds[1]->type == 0 && instr.opds[2]->type == 0) {
        printf ("operand  = %s(%lld)\n", get_operand_string(instr.opds[0]->type), instr.opds[0]->value.num);
    }

    // CASE TWO OPERANDS, one being a label
    else if (instr.opds[2]->type == 0 && instr.opds[1]->type == LABEL) {
       printf ("operands = %s(%lld)", get_operand_string(instr.opds[0]->type), instr.opds[0]->value.num);
       printf (", %s(\"%s\")\n", get_operand_string(instr.opds[1]->type), instr.opds[1]->value.label);
    }
   // CASE TWO OPERANDS, none a label
    else if (instr.opds[2]->type == 0) {
       printf ("operands = %s(%lld)", get_operand_string(instr.opds[0]->type), instr.opds[0]->value.num);
       printf (", %s(%lld)\n", get_operand_string(instr.opds[1]->type), instr.opds[1]->value.num);
    }

    // CASE THREE OPERANDS
    else {
       printf ("operands = %s(%lld)", get_operand_string(instr.opds[0]->type), instr.opds[0]->value.num);
       printf (", %s(%lld)", get_operand_string(instr.opds[1]->type), instr.opds[1]->value.num);
       printf (", %s(%lld)\n", get_operand_string(instr.opds[2]->type), instr.opds[2]->value.num);
    }
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
                for(int i = 0; i < b->p && b->data[i] != '*'; i++)
                printf ("%c", b->data[i]);
                // caso IS: armazena na ST
                if (instr->op->opcode == -1) {
                    printf("case 2\n");
                    InsertionResult res = stable_insert(st, instr->opds[0]->value.label);
                    if (res.new == 0) {
                        fprintf(stderr, "line     = %s\n", b->data);
                        fprintf(stderr, "Invalid assignment: \"%s\" is already assigned.\n", instr->opds[0]->value.label);
                    }
                    else {
                        res.data->opd = emalloc(sizeof(Operand));
                        res.data->opd->type = REGISTER;
                        res.data->opd->value.reg = instr->opds[1]->value.reg;
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
