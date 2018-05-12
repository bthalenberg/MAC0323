#include "stable.h"
#include "aux.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Precisa ser algum primo dahora
static int primes[] = {97, 197, 397, 797, 1597, 3203, 6421, 12853, 25717,
    51437, 102877, 205759, 411527, 823117, 1646237, 3292489, 6584983,
    13169977, 26339969, 52679969, 105359939, 210719881, 421439783,
    842879579, 1685759167};

static int index = 0;

static int M = primes[0];


/*
  Return a new symbol table.
*/
SymbolTable stable_create() {
    SymbolTable *ht = malloc(M * sizeof (Node));
    for (int h = 0; h < M; h++) ht[h] = NULL;
    ht->n = 0;
    return ht;
}

/*
  Destroy a given symbol table.
*/
void stable_destroy(SymbolTable table) {
    for (int h = 0; h < M; h++) {
        free(table.data[h]);
        table.data[h] = NULL;
    }
    free(table);
    table = NULL;
}

/*
    Hashing modular function
*/

static int hash(const char *key) {
    int i;
    unsigned int h = key[0];
    for (i = 1; key[i] != '\0'; i++) 
        h = (h * 251 + key[i]) % M;
    return h;  
}

/*
    Rehashing to keep load factor under 10
*/
static void rehash(SymbolTable table) {
    // finds new prime and sets M to it (TO DO)
    M = primes[++index];
    // realloc
    SymbolTable newTable = malloc(table, M * sizeof(Node));
    for (int i = 0; i < M; i++) {
        while (table.data[i] != NULL) {
            InsertionResult res = stable_insert(newTable, table.data[i]->str);
            res->data = table.data[i]->data;
            // precisa dar free em table.data[h]->data ou destroy já faz isso?
            table.data[i] = table.data[i]->nxt;
        }
    }
    stable_destroy(table);
    table = newTable;
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
    // if we did not find the key, we need to insert it
    if (dat == NULL) {
        table->n++;
        if (table->n/M > 10) rehash(table);
        res->new = 1;
        int h = hash(key, M);
        dat = malloc(sizeof(EntryData));
        res->data = dat;
        // create new node
        Node *n = malloc(sizeof(Node));
        n->data = dat;
        n->str = key;
        n->nxt = NULL;
        // if list is empty, key is the new head
        if (table.data[h] == NULL) table.data[h] = n;
        // else we go to the end of the list to add the new data
        else {
            Node *last = table.data[h];
            while (last->nxt != NULL) last = last->nxt;
            // add link to new node
            last->nxt = n;
        }
    }
    // if we found the key, we need to return it
    else {
        res->new = 0;
        res->data = dat;
    }
    return *res;
}

/*
  Find the data associated with a given key.
  Given a key, returns a pointer to the data associated with it, or a
  NULL pointer if the key is not found.
*/
EntryData *stable_find(SymbolTable table, const char *key) {
    // finds in which linked list the key is supposed to be
    int h = hash(key);
    Node *this = table.data[h];
    // if list is empty, key isn't there
    if (this == NULL) return NULL;
    // if list isn't empty, we traverse the list trying to find the key
    else {
        while (this != NULL || strcmp(this->str, key) != 0)
            this = this->nxt;
    }
    // if we got to the end of the list without finding it, the key isn't there
    if (this == NULL) return NULL;
    // else we found it
    else return this->data;
}


/*
    Function that does something to each data point visited during
    stable_visit
*/

static int visit(char *key, EntryData *data){
    if(data == NULL) return EXIT_FAILURE;
    printf("Chave: %s e Valor %d\n", key, data);
    return EXIT_SUCCESS;
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
    for (int h = 0; h < M; h++){
        if(table.data[h] != NULL){
            Node *this = table.data[h];
            visit(this[h].str, this[h].data);
            this = this->nxt;
            while (this != NULL){
                visit(this[h].str, this[h].data);
                this = this->nxt;
            }
        }
    }

    return EXIT_SUCCESS;
}

