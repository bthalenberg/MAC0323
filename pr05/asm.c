#include "error.h"
#include "stable.h"
#include "parser.h"
#include "asmtypes.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* --------------------------------- AUX -------------------------------------- */
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

/* --------------------------------- MAIN ---------------------------------- */

/* Args: filename (to print error messages), input stream with code to assemble and output stream
to write object code. Prints appropriate error messages in stderr. If assemble is successful,
returns non-zero value. Else, returns zero. */

/* Errors captured: errors captured by parser only need to be reported. Other errors to be
detected include:
    > Um rótulo de uma instrução não pode ser um alias e vice-versa.
    > Aliases não podem ser definidos mais de uma vez.
    > Uma instrução EXTERN não pode ter um rótulo. O rótulo exportado pela EXTERN deve ser definido.
    > Rótulos não podem ser definidos mais de uma vez.
    > As instruções de desvio (JMP, JZ, etc.) suportam deslocamentos de tamanho máximo fixo (veja
    notas de aula). Você deve verificar se esses tamanhos foram respeitados.
*/
int assemble(const char *filename, FILE *input, FILE *output) {
    // to store aliases defined through IS
    SymbolTable alias_table = stable_create();
    // to store labels and corresponding instruction number
    SymbolTable label_table = stable_create();
    // to store labels defined through EXTERN
    SymbolTable extern_table = stable_create();

    // storing standard aliases (registers)
    store_registers(alias_table);

    return 1;
}
