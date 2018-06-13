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
    return 1;
}
