
//auxiliar structure for the linked list
typedef struct {
  //value
  EntryData *data;
  //key
  char *str;
  //pointer to next node in linked list
  void *nxt;
} Node;


typedef struct {
  Node *data;
} SymbolTable;

