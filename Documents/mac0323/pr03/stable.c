#include "stable.h"
#include "aux.h"
#include <stdio.h>
#include <string.h>

// depois a gente arruma pra fazer rehashing
//Precisa ser algum primo dahora
long M 97

/*
  Return a new symbol table.
*/
SymbolTable stable_create() {
    SymbolTable *ht = malloc(M * sizeof (Node));
    for (int h = 0; h < M; h++)
        ht->data[h] = NULL;
    return ht;
}

/*
  Destroy a given symbol table.
*/
void stable_destroy(SymbolTable table) {
    for (int h = 0; h < M; h++) {
        free(table->data[h]);
        table->data[h] = NULL;
    }
    free(table);
    table = NULL;
}

/*
  Insert a new entry on the symbol table given its key.

  If there is already an entry with the given key, then a struct
  InsertionResult is returned with new == 0 and data pointing to the
  data associated with the entry. Otherwise, a struct is returned with
  new != 0 and data pointing to the data field of the new entry.

  If there is not enough space on the table, or if there is a memory
  allocation error, then crashes with an error message.
*/
InsertionResult stable_insert(SymbolTable table, const char *key) {
    EntryData *dat = stable_find(table, key);
    InsertionResult *res = malloc(sizeof(InsertionResult));
    // se não está, insere
    if (dat == NULL) {
        res->new = 1;
        int h = hash(key, M);
        dat = malloc(sizeof(EntryData));
        dat->i = 1;
        dat->str = key;

        res->new = 1;
        res->data = dat;

        if(table[h] = NULL){
          table[h] = dat;
        }
        else{
          EntryData *last = table[h];
          while(last->p != NULL){
            last = last->p;
          }
          last->p = dat;          
        }
    }
    else {
        dat->i = (dat->i) + 1;
        res->new = 0;
        res->data = dat;
    }
    return res;
}

/*
  Find the data associated with a given key.

  Given a key, returns a pointer to the data associated with it, or a
  NULL pointer if the key is not found.
*/
EntryData *stable_find(SymbolTable table, const char *key) {
    int h = hash(key, M);
    if(table[h] == NULL) return NULL;
    return table[h];
}

/*
  Visit each entry on the table.

  The visit function is called on each entry, with pointers to its key
  and data. If the visit function returns zero, then the iteration
  stops.

  Returns zero if the iteration was stopped by the visit function,
  nonzero otherwise.
*/
int stable_visit(SymbolTable table,
                 int (*visit)(const char *key, EntryData *data)) {
  

}

/*
    Hashing modular function
*/

int hash(const char *key) {
    int i;
    unsigned int h = key[0];
     for (i = 1; key[i] != '\0'; i++) 
        h = (h * 251 + key[i]) % M;
     return h;  
}
