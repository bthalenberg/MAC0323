#include "stable.h"

//auxiliar structure for the linked list
typedef struct {
    //value
    EntryData *data;
    //key
    char *str;
    //pointer to next node in linked list
    void *nxt;
} Node;


struct stable_s {
    // array of linked lists
    Node **data;
    // number of keys in st
    int n;
    //previous M
    int prIndex;
};


typedef struct {
    char **str;
    int *val;
    int index;
    // length of longest key
    int maxLen;
} answer;
