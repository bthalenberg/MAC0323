#include <stdio.h>
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
        buffer_push_back(b, s[i++]);
    return i;
}

/*
    Validates supposed label
    If l is a label, saves it in the ST and returns 1;
    Else, sets error  message and returns 0;
*/
int validate_label(Buffer *l, const char *s, const char **errptr) {
    int error = -1;
    // check if first char is valid
    if (!(isalpha(l->data[0])) || l->data[0] == ' ')
       error = 0;
    // goes through string trying to find error
    else {
        int i = 1;
        while (l->data[i] != '\0' && l->data[i] != '\n') {
            if (!(isalnum(l->data[i]) || l->data[i] == '_' ||))
               error = i;
            i++;
        }
    }
    //Checks if size is valid
    if (l->buffer_size >= 16) {
        set_error_msg("invalid label");
        *errptr = l->data;
        return 0;
    }
    // not a valid label, error contains index with invalid char
    if (error != -1) {
        set_error_msg("expected label or operator\n");
        if (errptr) *errptr = &s[ind - (l->p- 1 ) + aux];
        return 0;
    }
    return 1;
}

/*
    Finds out number of arguments operator is supposed to have
*/

int number_of_operands (const Operator *opt) {
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
	Buffer *aux = buffer_create();
    int i = 0;
    const Operador *opt;
    char *label;
    Operand *opd[3];
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
            if (!validate_label(aux, s, errptr)) return 0;
            // if it is a label, saves it
            label = emalloc(sizeof(aux->data));
            strcpy(label, aux->data);
            // see if next word is operator
            i = read_word(s, aux, i);
    		opt = optable_find(aux->data);
            // check if valid
            if (!opt) {
                set_error_msg("expected operator\n");
                if (errptr) *errpt = &s[i - (aux->p - 1)];
                return 0;
            }
        }
        // first word is an operator
        else label = NULL;
        // finds how many operands operator is supposed to have (TO-DO)
        int opds = number_of_operands(opt);
        // reads them one by one
        int k;
        for (k = 0; k < opds; k++) {
            i = read_word(s, aux, i);
            // validates operand

            // adds it to opd array

        }
        // checks if number of operands is correct (TO DO)

        // inserts OP_NONES, if any
        while (k < 3) {
            opd[k] = emalloc(sizeof(Operand));
            opd[k]->type = OP_NONE;
            k++
        }
        // saves instruction in instruction list (TO FIX)
        Instruction *new = instr_create(label, opt, opds);
        // traverse linked list received (**instr) and adds it to end

        // goes to the start of next instruction, if any
        while (isspace(s[i])) i++;
        if (s[i] == ';') i++;
    }
	return 1;
}
