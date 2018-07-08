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
    To use with extern labels
*/
FILE *arq;

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
void print_error(const char *errptr, const char *filename, Buffer *b, int inputLine) {
    fprintf(stderr,"%s: line %d: %s\n", filename, inputLine, b->data);
    if (!errptr) {
        int tam = strlen(filename);
        for (int i = 0; i < tam; i++) fprintf(stderr," ");
        fprintf(stderr,"       ");
        int dig = 0, n = inputLine;
        while (n > 0) {
            dig++;
            n = n/10;
        }
        for (int i = 0; i < dig; i++) fprintf(stderr," ");
        fprintf(stderr," ");
        int spc = *errptr - b->p - 1;
        for (int i = 0; i <= spc; i++) fprintf(stderr," ");
        fprintf(stderr,"^"); 
    }
    fprintf(stderr,"\n");
    die(NULL);
}

/*
    visits extern table
*/
int extern_visit(const char *key, EntryData *data) {
    if(data->s == -1) {
        die("extern label '%s' never defined", key);
    }
    fprintf(arq, "E %s %lld\n", key, data->s);
    return 1;
}


/*
    Adds instruction to linked list
*/

Instruction *linkedList_insert(Instruction *list, Instruction *tmp)
{
    if(list == NULL) return tmp;
    Instruction *p;
    for(p = list; p->next; p = p->next);
    p->next = tmp;
    return list;
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
    Instruction* instr = emalloc(sizeof(Instruction));
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
    InsertionResult res;
    EntryData *data;
    Instruction *tmp = emalloc(sizeof(Instruction));
    set_prog_name("macas");
    int inputLine = -1, outputLine = 0;
    int hadPush = 0;
    // Reads text until EOF
    while (read_line(input, b)) {
        inputLine++;
        // if parse was successful, prints line and instr content
        if (parse(b->data, alias_table, &tmp, &errptr)) {
            // caso IS: armazena na ST
            if (tmp->op->opcode == -1) {
                if (stable_find(label_table, tmp->label)) {
                    fprintf(stderr, "line     = %s\n", b->data);
                    fprintf(stderr, "Invalid assignment: \"%s\" is already assigned as label.", tmp->label);
                }
                else {
                    res = stable_insert(alias_table, tmp->label);
                    if (res.new == 0) {
                        fprintf(stderr, "line     = %s\n", b->data);
                        fprintf(stderr, "Invalid assignment: \"%s\" is already assigned.\n", tmp->label);
                    }
                    else {
                        res.data->opd = emalloc(sizeof(Operand));
                        res.data->opd->type = REGISTER;
                        res.data->opd->value.reg = tmp->opds[0]->value.reg;
                    }
                }
            }
            else if (tmp->op->opcode == -2) {
                if (tmp->label) {
                    fprintf(stderr, "line     = %s\n", b->data);
                    fprintf(stderr, "Invalid assignment: EXTERN instructions cannot have labels.");

                    res = stable_insert(extern_table, tmp->opds[0][0].value.str);        
                    if(res.new == 0)
                        die("%s: line %d: two instructions EXTERN with the same label", filename, inputLine);
                    (*res.data).s = -1;
                    instr_destroy(tmp);
                }
                else {
                    InsertionResult res = stable_insert(extern_table, tmp->opds[0]->value.str);
                    if (res.new == 0) {
                        fprintf(stderr, "line     = %s\n", b->data);
                        fprintf(stderr, "Invalid assignment: \"%s\" is already assigned.", tmp->opds[0]->value.str);
                    }
                }
            }
            else {
                tmp->pos = outputLine;
                tmp->lineno = inputLine;
                if (tmp->label) {
                    if (stable_find(alias_table, tmp->label)) {
                        fprintf(stderr, "line     = %s\n", b->data);
                        fprintf(stderr, "Invalid assignment: \"%s\" is already assigned as alias.", tmp->label);
                    }
                    else {
                        InsertionResult res = stable_insert(label_table, tmp->label);
                        if (res.new == 0) {
                            fprintf(stderr, "line     = %s\n", b->data);
                            fprintf(stderr, "Invalid assignment: \"%s\" is already assigned.", tmp->label);
                        }
                        else {
                            (*res.data).s = outputLine;
                            data = stable_find(extern_table, tmp->label);
                            if (data != NULL)
                                data->s = outputLine;
                        }
                    }
                }
                if (!strcmp(tmp->op->name, "CALL")) outputLine += 4;
                else if (!strcmp(tmp->op->name, "PUSH")) { 
                    outputLine++;
                    hadPush = 1;
                }
                else if (!strcmp(tmp->op->name, "RET")) outputLine += 3;
                else if (!strcmp(tmp->op->name, "STR")) {
                    int tam = strlen((tmp->opds[0][0]).value.str);
                    tam = (tam+4)/4;
                    outputLine += tam;
                }
                else outputLine++;
                if (hadPush == 1 && strcmp(tmp->op->name, "PUSH")) {
                    outputLine++;
                    hadPush = 0;
                }
                instr = linkedList_insert(instr, tmp);
            }
        }
        // if an error occurred, prints error message to stderr with line
        else print_error(errptr, filename, b, inputLine);
        printf("\n");
        cur++;
    }
    fprintf(output,"%d\n", outputLine);
    arq = output;
    stable_visit(extern_table, extern_visit);
    fprintf(output,"B\n");

    Instruction *var = instr;
    hadPush = 0;

    // vamos imprimindo o código de máquina, exceto pelos rótulos não definidos
    while (var != NULL) {
        int rest = 0;
        int labelNotFound = 0;
        if(var->label != NULL) {
            data = stable_find(alias_table, var->label);
            if (data != NULL)
                die("%s: line %d: label cannot have same name as alias", filename, var->lineno);
        }
        for (int i = 0; i < 3; i++) {
            if (var->opds[i] == NULL) break;
            if ((var->opds[i][0]).type == LABEL) {
                data = stable_find(label_table, (var->opds[i][0]).value.label);
                if (stable_find(label_table, (var->opds[i][0]).value.label) == NULL) {
                    labelNotFound = 1;
                    break;
                }
                else {
                    octa dest = (*data).s;
                    octa source = var->pos;
                    if (dest < source) rest = 1;
                }
            }
        }
        if (hadPush > 0 && strcmp(var->op->name, "PUSH")) {
            fprintf(output, "31fdfd%02x\n", hadPush);
            hadPush = 0;
        }
        if (labelNotFound == 0) {
            if (!strcmp(var->op->name, "STR")) {
                char *str = (var->opds[0][0]).value.str;
                int tam = strlen(str);
                for (int i = 0; i < tam; i++) {
                    if ((i % 4 == 0) && i != 0) fprintf(output,"\n");
                    fprintf(output, "%02x", str[i]);
                }
                if (tam % 4 == 0) fprintf(output,"\n");
                fprintf(output, "00");
                tam++;
                if (tam % 4 != 0) {
                    while ((tam % 4) != 0) {
                        fprintf(output, "00");
                        tam++;
                    }
                }   
                fprintf(output, "\n");
            }
            else {
                if (!strcmp(var->op->name," NOP"))
                    fprintf(output,"ff000000");
                else if (!strcmp(var->op->name, "RET")) {
                    fprintf(output, "33fdfd%02x\n0ffafd%02x\n56fa0000", 
                        8*((var->opds[0][0]).value.reg + 1),
                        8*((var->opds[0][0]).value.reg));
                }
                else if (!strcmp(var->op->name, "PUSH")) {
                    fprintf(output,"1f%02xfd%02x", (var->opds[0][0]).value.reg , hadPush);
                    hadPush += 8;
                }
                else {
                    if (var->op->opd_types[2] == IMMEDIATE)
                        if ((var->opds[2][0]).type == NUMBER_TYPE) rest = 1;
                    if (strcmp(var->op->name, "TETRA") != 0)
                        fprintf(output,"%02x", var->op->opcode + rest);
                    for (int i = 0; i < 3; i++) {
                        if ((var->opds[i]) == NULL) break;
                        if ((var->opds[i][0]).type == REGISTER)
                            fprintf(output, "%02x", (var->opds[i][0]).value.reg);
                        else if ((var->opds[i][0]).type == NUMBER_TYPE) {
                            if (!strcmp(var->op->name,"NEG") || !strcmp(var->op->name,"NEGU")) 
                                fprintf(output, "%02llx",(var->opds[i][0]).value.num);    
                            else if (i == 0 && !strcmp(var->op->name, "TETRA")) 
                                fprintf(output, "%08llx", (var->opds[i][0]).value.num);
                            else if (i == 0) 
                                fprintf(output, "%06llx", (var->opds[i][0]).value.num);
                            else if (i == 1) 
                                fprintf(output, "%04llx", (var->opds[i][0]).value.num);
                            else if (i == 2) 
                                fprintf(output, "%02llx", (var->opds[i][0]).value.num);
                        }
                        else if ((var->opds[i][0]).type == LABEL) {
                            data = stable_find(label_table, (var->opds[i][0]).value.label);
                            octa dest = (*data).s;
                            octa source = var->pos;
                            int jmp = dest - source; 
                            if (jmp < 0) jmp *= -1;
                            if (i == 0) fprintf(output, "%06x", jmp);
                            else if (i == 1) fprintf(output, "%04x", jmp);
                            else if (i == 2) fprintf(output, "%02x", jmp);
                        }
                    }
                }
                fprintf(output,"\n");
            }
        }
        else {
            int opcode = var->op->opcode;
            if (!strcmp(var->op->name, "CALL")) {
                fprintf(output, "58fa0004\n1ffafd00\n31fdfd08\n");
                opcode = 72;
            }
            fprintf(output, "*");
            fprintf(output, " %d", opcode);
            for (int i = 0; i < 3; i++) {
                if ((var->opds[i]) == NULL) break;
                if ((var->opds[i][0]).type == REGISTER)
                    fprintf(output, " %d", (var->opds[i][0]).value.reg);
                else if ((var->opds[i][0]).type == NUMBER_TYPE)
                    fprintf(output, " %lld", (var->opds[i][0]).value.num);
                else if ((var->opds[i][0].type == LABEL))
                    fprintf(output, " %s", (var->opds[i][0]).value.label);
            }
            fprintf(output,"\n");
        }
        var = var->next;
    }

    buffer_destroy(b);
    stable_destroy(alias_table);
    stable_destroy(label_table);
    stable_destroy(extern_table);
    Instruction *tmpInstr;

    for (Instruction *c = instr; c; c = tmpInstr) {
        tmpInstr = c->next;
        instr_destroy(c);
    }

    free(tmp);
    return 1;
}
