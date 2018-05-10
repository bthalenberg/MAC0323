
//auxiliar structure for the linked list
typedef struct {
  //occurences of the key
  int num;
  //key
  char *str;
  //pointer to next node in linked list
  void *nxt;
} Node;


typedef struct {
  Node *data;
} SymbolTable;

