#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stable.h"
#include "error.h"
#include "types.h"


unsigned long init_size = 1024;


/*
  Hash function used to define key locations in the symbol table.
*/
unsigned long hash (const char* str) {

    unsigned long hash = 5381;
    int c;

    while ((c = *str++) && *str != 0) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}

/*
  Return a new symbol table.
*/
SymbolTable stable_create() {

    unsigned long j;
    
    SymbolTable stable = emalloc (sizeof(*stable)); 
    stable->data = emalloc (init_size * sizeof(*stable->data));
    
    stable->size = init_size;
    stable->count = 0;
    
    //Make sure that the frequencies are zero.
    for (j = 0; j < init_size; j++) {
        
        stable->data[j].input = emalloc (sizeof(EntryData));

        stable->data[j].key = emalloc (1024);
        
        stable->data[j].input->i = 0;
        stable->data[j].key = NULL;
    }
    
    return stable;
}

/*
  Local function that reallocs the symbol table,
  in case the original size was not big enough.
*/
void reallocSTable(SymbolTable table) {
    
    short int cmp;
    unsigned long i, ind;
    SymbolTable new;
    
    init_size *= 2;
    
    new = stable_create();
    new->count = table->count;
    
    for (i = 0; i < table->size; i++) {
        
        if (table->data[i].key != NULL) {
            
            ind = hash(table->data[i].key)%new->size;

            while(new->data[ind].key != NULL && 
                (cmp = strcmp(new->data[ind].key, table->data[i].key))) { 
                    ind++;
                    ind = ind%new->size;
            }

            new->data[ind].key = estrdup(table->data[i].key);
            new->data[ind].input->i = table->data[i].input->i;
        }
    }
    
    for (i = 0; i < table->size; i++)
        free(table->data[i].key);
    
    free(table->data);
    
    table->size *= 2;
    table->data = new->data;
}

/*
  Destroy a given symbol table.
*/
void stable_destroy(SymbolTable table) {

    unsigned long i;
    
    for (i = 0; i < table->size; i++) {
        if (table->data[i].key)
            free(table->data[i].key);
        /*Victor esteve aqui*/
        if (table->data[i].input->opd != NULL)
            operand_destroy(table->data[i].input->opd);
    }

    free(table->data);
    table->data = NULL;

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
   
    short int cmp;
    unsigned long ind, end;
    InsertionResult IR;
    
    end = ind = hash(key)%table->size;
    
    while(table->data[ind].key != NULL && 
            (cmp = strcmp(key, table->data[ind].key))) {
        
        ind++;
        ind = ind%table->size;
        
        //Check if all the positions were tested, if positive, realloc.
        if (ind == end) {
            reallocSTable(table);
            ind = hash(key)%table->size;
        }
    }

    if (table->data[ind].key != NULL)
        IR.new = 0;
    
    else {
        
        IR.new = 1;
        table->data[ind].key = estrdup(key);
        table->count++;
    }

    IR.data = table->data[ind].input;
 
    return(IR);    
}

/*
  Find the data associated with a given key.

  Given a key, returns a pointer to the data associated with it, or a
  NULL pointer if the key is not found.
*/
EntryData *stable_find(SymbolTable table, const char *key) {
    
    short int cmp;
    unsigned long ind;
    EntryData* ED;

    ind = hash(key)%table->size;
    
    ED = emalloc(sizeof(EntryData));

    while(table->data[ind].key && 
            (cmp = strcmp(key, table->data[ind].key))) {
        
        ind++;
        ind = ind%table->size;
    }

    if (table->data[ind].key == NULL)
        ED = NULL;
    
    else 
        ED->i = table->data[ind].input->i;

    return ED;
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
        int(*visit)(const char *key, EntryData *data)) {
    
    unsigned long ind, res;
    ind = 0;

    //Print the ordered data of the SymbolTable.
    while ((res = visit(table->data[ind].key, table->data[ind].input)))
        ind++;
    
    if (ind == table->count)
        return 1;

    return 0;
}