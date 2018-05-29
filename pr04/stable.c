#include "aux.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Precisa ser algum primo dahora
const int primes[] = {97, 197, 397, 797, 1597, 3203, 6421, 12853, 25717,
    51437, 102877, 205759, 411527, 823117, 1646237, 3292489, 6584983,
    13169977, 26339969, 52679969, 105359939, 210719881, 421439783,
    842879579, 1685759167};

/*
  Return a new symbol table.
*/
SymbolTable stable_create() {
    SymbolTable ht = malloc(sizeof (SymbolTable));
    // checks if malloc was successfull
    if (ht == NULL) {
        free (ht);
        return NULL;
    }
    ht->data = malloc((primes[0]) * sizeof(Node));
    if (ht->data == NULL) {
        free (ht->data);
        return NULL;
    }
    for (int h = 0; h < primes[0]; h++) ht->data[h] = NULL;
    ht->n = 0;
    ht->prIndex = 0;
    return ht;
}

/*
  Destroy a given symbol table.
*/
void stable_destroy(SymbolTable table) {
    // free em todos os nodes
    for (int h = 0; h < primes[table->prIndex]; h++) {
        Node *head = table->data[h];
        Node *t;
        while (head != NULL) {
            // free em todos os entrydata
            t = head;
            head = head->nxt;
            free(t->data);
            t->data = NULL;
        }
    }
    free(table);
    table = NULL;
}

/*
    Hashing modular function
*/

static int hash(const char *key, int index) {
    //regular rolling hash function
    unsigned int h = key[0] % primes[index];

    for (int i = 1; key[i] != '\0'; i++)
        h = (h * 251 + key[i]) % primes[index];
    return h;
}

/*
    (Optional) Rehashing to keep load factor under 10 (that we couldn't make work)
*/
// static void rehash(SymbolTable table) {
//     // realloc
//
//     SymbolTable newTable = malloc((primes[table->prIndex]) * sizeof(Node));
//     newTable->n = table->n;
//     newTable->prIndex = table->prIndex+1;
//     for (int i = 0; i < table->prIndex ; i++) {
//         while (table->data[i] != NULL) {
//             stable_insert(newTable, table->data[i]->str);
//             table->data[i] = table->data[i]->nxt;
//         }
//     }
//     stable_destroy(table);
//     table = newTable;
// }

Node *createNode() {
    EntryData *dat = malloc(sizeof(EntryData));
    Node *n = malloc(sizeof(Node));
    n->data = dat;
    n->nxt = NULL;
    return n;
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
        //if (table->n/primes[table->prIndex] > 10) {rehash(table);
        //printf("\n");}
        res->new = 1;
        int h = hash(key, table->prIndex);
        Node *n = createNode();
        // if list is empty, key is the new head
        if (table->data[h] == NULL) table->data[h] = n;
        // else we go to the end of the list to add the new data
        else {
            Node *last = table->data[h];
            while (last->nxt != NULL) last = last->nxt;
            // add link to new node
            last->nxt = n;
        }
        n->str = malloc(strlen(key) + 1);
        strcpy(n->str, key);
        res->data = n->data;
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
    int h = hash(key, table->prIndex);
    Node *this = table->data[h];
    // if list is empty, key isn't there
    if (this == NULL) return NULL;
    // if list isn't empty, we traverse the list trying to find the key
    else {
        while (this != NULL && strcmp(this->str, key) != 0) {
            this = this->nxt;

        }
    }
    // if we got to the end of the list without finding it, the key isn't there
    if (this == NULL) return NULL;
    // else we found it
    else return this->data;
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
    //for each possible node
    int r;
    for (int h = 0; h < primes[table->prIndex]; h++){
        //goes through the respective linked list
        if(table->data[h] != NULL){
            Node *this = table->data[h];
            r = visit(this->str, this->data);
            if (r == 0) {
                fprintf(stderr, "Erro ao ler as entradas.\n");
                return r;
            }
            this = this->nxt;
            while (this != NULL){
                visit(this->str, this->data);
                this = this->nxt;
            }
        }
    }
    return 1;
}
