#include <stdio.h>
#include <string.h>
#include "error.h"
#include "stable.h"
#include "asmtypes.h"
#include "opcodes.h"
#include "optable.h"
#include "parser.h"
#include "buffer.h"

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
	char *words[100];
	breakWords(s, words);
	return 0;
}

static void breakWords(const char *s, char *words[]){
	char *token;
	int i = 0;
	char *copy = malloc((strlen(s) + 1) * sizeof(char));
	strcpy(copy,s);
	token = strtok(copy, " ,");
	while(token != NULL && token[0] != '*'){ 
		words[i++] = token;
		token = strtok (NULL, " ,");
	}	
}

static void checkWords(char *words[], Instruction **instr){
	
	Operator *op = optable_find(words[0]);
	if(op == NULL) {
		printf("words[0] is label; %s\n", words[0]);
		op = optable_find(words[1]);
		printf("%s is the Operator\n", words[1]);
	}
	printf("%s is the Operator\n", words[1]);

}