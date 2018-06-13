#include "error.h"
#include "stable.h"
#include "parser.h"
#include "asmtypes.h"
#include "buffer.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* --------------------------------- AUX -------------------------------------- */
/*
    Stores special registers
*/
void store_registers(SymbolTable alias_table) {
    // rA = return value register
    InsertionResult insertion = stable_insert(alias_table, "rA");
    Operand *opd1 = emalloc(sizeof(Operand));
    opd1->type = REGISTER;
    opd1->value.reg = 255;
    insertion.data->opd = opd1;
    // rR = remainder register
    insertion = stable_insert(alias_table, "rR");
    Operand *opd2 = emalloc(sizeof(Operand));
    opd2->type = REGISTER;
    opd2->value.reg = 254;
    insertion.data->opd = opd2;
    // rSP = stack pointer
    insertion = stable_insert(alias_table, "rSP");
    Operand *opd3 = emalloc(sizeof(Operand));
    opd3->type = REGISTER;
    opd3->value.reg = 253;
    insertion.data->opd = opd3;
    // rX = insterrupt operation register
    insertion = stable_insert(alias_table, "rX");
    Operand *opd4 = emalloc(sizeof(Operand));
    opd4->type = REGISTER;
    opd4->value.reg = 252;
    insertion.data->opd = opd4;
    // rY = interrupt operand register
    insertion = stable_insert(alias_table, "rY");
    Operand *opd5 = emalloc(sizeof(Operand));
    opd5->type = REGISTER;
    opd5->value.reg = 251;
    insertion.data->opd = opd5;
    // rZ = interrupt operand register
    insertion = stable_insert(alias_table, "rA");
    Operand *opd6 = emalloc(sizeof(Operand));
    opd6->type = REGISTER;
    opd6->value.reg = 250;
    insertion.data->opd = opd6;
}

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
    Checks if labels in EXTERN table were defined at some point.
    Returns 1 if table is consistent; else returns 0.
    TO DO!
*/
int check_extern_consistency(SymbolTable extern_table, SymbolTable label_table) {
    // itera pelos itens de extern_table (fazer isso com a extern_visit, precisa criar função
    // de visita especial)

    // para cada item, verifica se está em label table. if !stable_find(item_em_extern_table, label_table), return 0;
    return 1;
}


/* --------------------------------- MAIN ---------------------------------- */

/* Args: filename (to print error messages), input stream with code to assemble and output stream
to write object code. Prints appropriate error messages in stderr. If assemble is successful,
returns non-zero value. Else, returns zero. */

/* Errors captured: errors captured by parser only need to be reported. Other errors to be
detected include:
    > Um rótulo de uma instrução não pode ser um alias e vice-versa. (done)
    > Aliases não podem ser definidos mais de uma vez. (done)
    > Uma instrução EXTERN não pode ter um rótulo. (done) O rótulo exportado pela EXTERN deve ser definido. (IN PROGRESS)
    > Rótulos não podem ser definidos mais de uma vez. (done)
    > As instruções de desvio (JMP, JZ, etc.) suportam deslocamentos de tamanho máximo fixo (veja
    notas de aula). Você deve verificar se esses tamanhos foram respeitados.
*/
int assemble(const char *filename, FILE *input, FILE *output) {
    // init routine
    // config for error.c
    set_prog_name(filename);
    if (input == NULL) die("Invalid file.\n");
    Buffer *b = buffer_create(50);
    const char *errptr;
    Instruction* instr = malloc(sizeof(Instruction));
    int cur, len;
    cur = len = 1;
    // to store aliases defined through IS
    SymbolTable alias_table = stable_create();
    // to store labels and corresponding instruction number
    SymbolTable label_table = stable_create();
    // to store labels defined through EXTERN
    SymbolTable extern_table = stable_create();
    // storing standard aliases (registers)
    store_registers(alias_table);
    int instrCounter = 1;
    // Reads text until EOF
    while (len) {
        len = read_line(input, b);
        if (len == 0) break;
        // if parse was successful, prints line and instr content
        if (parse(b->data, alias_table, &instr, &errptr)) {
            // caso IS: armazena na ST
            if (instr->op->opcode == -1) {
                if (stable_find(label_table, instr->label)) {
                    fprintf(stderr, "line     = %s\n", b->data);
                    fprintf(stderr, "Invalid assignment: \"%s\" is already assigned as label.", instr->label);
                }
                else {
                    InsertionResult res = stable_insert(alias_table, instr->label);
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
            }
            else if (instr->label) {
                if (stable_find(alias_table, instr->label)) {
                    fprintf(stderr, "line     = %s\n", b->data);
                    fprintf(stderr, "Invalid assignment: \"%s\" is already assigned as alias.", instr->label);
                }
                else {
                    InsertionResult res = stable_insert(label_table, instr->label);
                    if (res.new == 0) {
                        fprintf(stderr, "line     = %s\n", b->data);
                        fprintf(stderr, "Invalid assignment: \"%s\" is already assigned.", instr->label);
                    }
                    else {
                        res.data->i = instrCounter;
                    }
                }
            }
            else if (instr->op->opcode == -2) {
                if (instr->label) {
                    fprintf(stderr, "line     = %s\n", b->data);
                    fprintf(stderr, "Invalid assignment: EXTERN instructions cannot have labels.");
                }
                else {
                    InsertionResult res = stable_insert(extern_table, instr->opds[0]->value.str);
                    if (res.new == 0) {
                        fprintf(stderr, "line     = %s\n", b->data);
                        fprintf(stderr, "Invalid assignment: \"%s\" is already assigned.", instr->opds[0]->value.str);
                    }
                }
            }
        }
        // if an error occurred, prints error message to stderr with line
        else print_error(errptr, b, cur);
        printf("\n");
        cur++;
    }
    if (!check_extern_consistency(extern_table, label_table)) {
        fprintf(stderr, "line     = %s\n", b->data);
        fprintf(stderr, "Invalid assignment: EXTERN instructions need to define a label.");
    }
    buffer_destroy(b);
    stable_destroy(alias_table);
    stable_destroy(label_table);
    stable_destroy(extern_table);
    return 1;
}
