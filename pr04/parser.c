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
    while (s[i] != '\0' && s[i] != '\n' && !isspace(s[i]))
        buffer_push_char(b, s[i++]);

    return i;
}

/*
    Validates supposed label
    If l is a label, saves it in the ST and returns 1;
    Else, sets error  message and returns 0;
*/
static int validate_label(Buffer *l, const char *s, const char **errptr, SymbolTable alias_table, int ind) {
    int error = -1;
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

    //Checks if size is valid
    if (l->buffer_size >= 16 || l->buffer_size == 0) {
        set_error_msg("invalid label size");
        *errptr = l->data;
        return 0;
    }

    //Check if label already exists
    if (stable_find(alias_table, l->data) != NULL) {
        set_error_msg("label already exists");
        *errptr = l->data;
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
    EntryData *data;

    // reads s until EOL
    while (s[i] != '\0' && s[i] != '\n' ) {
        // reads word for word
        i = read_word(s, aux, i);
        // if there was no word, line is empty
        if (!aux->p) return 1;
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
        // finds how many operands operator is supposed to have (TO-DO)
        int opdNumber = number_of_operands(opt);
        // reads them one by one
        int k;
        for (k = 0; k < opdNumber; k++) {
            i = read_word(s, aux, i);
            // validates operand
            //Check if operand is on Alias Table
            data = stable_find(alias_table, aux->data);
            //if operand is alias
            if (data != NULL) {
                (*instr)->opds[k] = data->opd;
            }
            //if operand is label
            else if (((*instr)->op->opd_types[k]) == LABEL){
                (*instr)->opds[k] = operand_create_label(aux->data);
            }
            //if operand is register
            else if (((*instr)->op->opd_types[k]) == REGISTER){
                //ignorar o 1o char, que sera o $
                (*instr)->opds[k] = operand_create_register(aux->data[1]);
            }
            //if operand is string
            else if (((*instr)->op->opd_types[k]) == STRING) {

                (*instr)->opds[k] = operand_create_string(aux->data);
            }
            //if operand is number
            else {
                //needs to be properly done
                (*instr)->opds[opnum] = operand_create_number(aux->data);

            }

        }
        // checks if number of operands is correct
        for (k = 0; k < 3; k++) {
            if(k < opdNumber && (*instr)->opds[k] == NULL){
                set_error_msg("missing operand");
                *errptr = "NULL";
                return 0;
            }
            if(k >= opdNumber && (*instr)->opds[k] != NULL){
                set_error_msg("too many operands");
                *errptr = "NULL";
                return 0;
            }
        }

        // inserts OP_NONES, if any
        while (k < 3) {
            opd[k] = emalloc(sizeof(Operand));
            opd[k]->type = OP_NONE;
            k++;
        }
        // saves instruction in instruction list (TO FIX)
        //Instruction *new = instr_create(label, opt, opdNumber);
        // traverse linked list received (**instr) and adds it to end

        // goes to the start of next instruction, if any
        while (isspace(s[i])) i++;
        if (s[i] == ';') i++;
    }
    return 1;
}
