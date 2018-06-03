#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "error.h"
#include "stable.h"
#include "asmtypes.h"
#include "opcodes.h"
#include "optable.h"
#include "parser.h"
#include "buffer.h"

/* -----------------------------------------------------------------------------
AUX FUNCTIONS
------------------------------------------------------------------------------*/

/*
    Reads one word in s starting from index i
    and returns last index read
*/
static int read_word(const char *s, Buffer *b, int i) {
    // frees buffer to receive new word
    buffer_reset(b);
    // skip spaces
    while (isspace(s[i])) i++;
    // skip comments
    if (s[i] == '*') return i;
    // reads until EOL or space
    while (s[i] != '\0' && s[i] != '\n' && !isspace(s[i])) {
        buffer_push_char(b, s[i++]);
        b->p++;
    }
    buffer_push_char(b,'\0');
    return i;
}

/*
    Validates supposed label
    If l is a label, saves it in the ST and returns 1;
    Else, sets error  message and returns 0;
*/
static int validate_label(Buffer *l, const char *s, const char **errptr, SymbolTable alias_table, int ind) {
    int error = -1;
    //Checks if size is valid
    if (l->buffer_size >= 16 || l->buffer_size == 0) {
        set_error_msg("invalid label size");
        if (errptr)  *errptr = &s[ind - (l->p - 1)];
        return 0;
    }
    //Check if label already exists
    if (stable_find(alias_table, l->data) != NULL) {
        set_error_msg("label already exists");
        if (errptr)  *errptr = &s[ind - (l->p - 1)];
        return 0;
    }
    // check if first char is valid
    if (!(isalpha(l->data[0])) || l->data[0] == ' ')
       error = 0;
    // goes through string trying to find error
    else {
        int i = 1;
        while (l->data[i] != '\0' && l->data[i] != '\n') {
            if (!(isalnum(l->data[i]) || l->data[i] == '_'))
               error = i;
            i++;
        }
    }
    // not a valid label, error contains index with invalid char
    if (error != -1) {
        set_error_msg("expected label or operator\n");
        if (errptr) *errptr = &s[ind - (l->p-1 ) + error];
        return 0;
    }
    return 1;
}

/*
    Finds out number of arguments operator is supposed to have
*/

static int number_of_operands (const Operator *opt) {
    int num = 0;
    for (int i = 0; i < 3; i++)
        if (opt->opd_types[i] != OP_NONE) num++;
    return num;
}

/*
    Reads Operands in the right format
*/
static int read_operand(const char *s, Buffer *b, int i) {

    buffer_reset(b);
    while (isspace (s[i])) i++;

    // Reads operand as string if it starts with '"'
    if (s[i] == '"') {
        buffer_push_char(b, s[i++]);
        b->p++;
        while (s[i] != '"' && s[i] != '\n' && s[i] != '\0') {
            buffer_push_char(b, s[i++]);
            b->p++;
            if (s[i] == '\\') {
                buffer_push_char(b, s[i++]);
                b->p++;
                buffer_push_char(b, s[i]);
                b->p++;
            }
        }
        if (s[i] == '"') {
            buffer_push_char(b, s[i++]);
            b->p++;
        }
        return i;
    }

    if (s[i] == ',' || isspace(s[i])) i++;
    while (s[i] != ',' && !isspace(s[i]) && s[i] != '\0' && s[i] != ';' && s[i] != '*') {
        buffer_push_char(b, s[i++]);
        b->p++;
    }
    return i;
}

octa to_decimal(Buffer *b){
    octa resp = 0;
    int digit;
    for(int i = 1; i < (int)b->buffer_size; i++){
        resp = resp * 16;
        if( b->data[i]>='0' && b->data[i]<='9')
            digit = b->data[i] - '0';
        else
            digit = b->data[i] + 10 - (b->data[i]>='A' && b->data[i]<='Z')?'a':'A';
        resp = resp + digit;
    }
    return resp;
}

/* Create operand */
static Operand *create_operand(Buffer *b, SymbolTable alias_table, const char **errptr, int i) {
    octa aux = 0;
    char *s = emalloc ((b->p + 1) * sizeof(char));
    strcpy (s, b->data);
    // if is register
    if (s[0] == '$') {
        for (int j = 1; s[j] != '\0'; j++) {
            if (s[j] < '0' || s[j] > '9') return NULL;
            aux *= 10;
            aux += (s[j] - '0');
        }
        if (aux > 255) return NULL;
        return operand_create_register(aux);
    }

    // if is string
    if (s[0] == '"') {
        if (s[b->p - 1] == '"') return operand_create_string(s);
        else {
            set_error_msg("mismatched quotes");
            if (errptr) *errptr = &s[i - (b->p - 1)];
            return NULL;
        }
    }

    // if hex number
    if (s[0] == '#') {
        for (int j = 1; s[j] != '\0'; j++) {
            if (!(s[j] >= '0' && s[j] <= '9') || (s[j] >= 'A' && s[j] <= 'F') || (s[j] >= 'a' && s[j] <= 'f'))
               return NULL;
        }
        aux = to_decimal(b);
        return operand_create_number(aux);
    }

    // if base 10
    if ((s[0] >= '0' && s[0] <= '9') || s[0] == '-') {
        for (int j = 1; s[j] != '\0'; j++)
            if (s[j] < '0' || s[j] > '9') return NULL;
        aux = atoi (s);
        return operand_create_number (aux);
    }

    // if label
    if (validate_label(b, s, errptr, alias_table, i)) {
        EntryData *alias = stable_find(alias_table, s);
        if (alias) return operand_create_register(alias->i);
        return operand_create_label(s);
    }
    return NULL;
}

/*
    Process operands
*/
static Operand *process_operand(Buffer *b, SymbolTable alias_table, int k, const char **errptr,
                            const Operator *opt, const char *s) {
    Operand *opd = create_operand(b, alias_table, errptr, k);
    if (opd && opd->type & NUMBER_TYPE) {
        int num  = opd->value.num;
        if (opt->name[0] == 'J') {
            if (num > 16777215 || num < -16777215)
                opd = NULL;
        }
        else if (opt->opcode & INT) {
            if (b->data[0] != 'D' && b->data[1] != 'B')
               if (num > 255)
                   opd = NULL;
        }
        else
            if (num > 255)
                opd = NULL;
    }
    // Checks if the type of operand_read and the expected_type coincides
    if (opd && opd->type & opt->opd_types[k]) {
        set_error_msg ("invalid operand");
        if (errptr) *errptr = &s[k - (b->p - 1)];
    }
    return opd;
}

/*
    Inserts instruction in linked list and returns it
*/
Instruction *insert_instruction(Instruction *head, char *label, const Operator *opt, Operand *opds[3]) {

    Instruction *new = head;
    while (new != NULL) new = new->next;
    new = instr_create(label, opt, opds);
    return new;
}

/* ------------------------------- PARSER -------------------------------------- */
/*
  Return instruction corresponding to assembly language line.
  INPUT:
  - s -- line of assembly code.
  - alias_table -- table of aliases used so far.
  Returns nonzero on success, zero to signal an error. On success,
  *instr contains the parsed instruction, unless the line was
  empty. On error, *errptr, if non-NULL, points to the character in s
  where the error was found.
*/
int parse(const char *s, SymbolTable alias_table, Instruction **instr, const char **errptr) {
    Buffer *aux = buffer_create(100);
    int i = 0;
    const Operator *opt;
    char *label;
    Operand *opd[3];

    // reads s until EOL
    while (s[i] != '\0' && s[i] != '\n' ) {
        // reads word for word
        i = read_word(s, aux, i);
        // if there was no word, line is empty
        if (aux->p == 0) return 1;
        // is the word an operator?
        opt = optable_find(aux->data);
        // not an operator. first word should be label or operator.
        if (!opt) {
            // validates label
            if (!validate_label(aux, s, errptr, alias_table, i)) return 0;
            // if it is a label, saves it
            label = emalloc(sizeof(aux->data));
            strcpy(label, aux->data);
            // see if next word is operator
            i = read_word(s, aux, i);
            // ERRO DE ADD
            printf("%d %s\n", i, aux->data);
            opt = optable_find(aux->data);
            // check if valid
            if (!opt) {
                set_error_msg("expected operator\n");
                if (errptr) *errptr = &s[i - (aux->p - 1)];
                return 0;
            }
        }
        // first word is an operator
        else label = NULL;
        // finds how many operands operator is supposed to have
        int opdNumber = number_of_operands(opt);
        // reads them one by one
        int k;
        for (k = 0; k < opdNumber; k++) {
            i = read_operand(s, aux, i);
            if (aux->p == 0) {
                set_error_msg ("missing operand");
                if (errptr)  *errptr = &s[i - (aux->p - 1)];
                return 0;
            }
            // stores and checks operands
            opd[k] = process_operand(aux, alias_table, k, errptr, opt, s);
        }
        // inserts OP_NONES, if any
        while (k < 3) {
            opd[k] = emalloc(sizeof(Operand));
            opd[k]->type = OP_NONE;
            k++;
        }
        // saves instruction in instruction list
        *instr = insert_instruction(*instr, label, opt, opd);
        // goes to the start of next instruction, if any
        while (isspace(s[i])) i++;
        if (s[i] == ';') i++;
    }
    return 1;
}
