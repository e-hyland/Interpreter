#define START 2
#define LIST_LIMIT 3
#define PRIME 31

void checklistlen(Prog *ptr, int cnt);
int calcsize(int x);
int is_prime(int v);
Node **resize_table(Prog *ptr, int oldsize);
unsigned int hash_all(char *str);
void freenodes(Prog *ptr, int i);
int search_list(char *str, Node *current, float val, int cnt, Prog *ptr);
Node *AllocateNode(Node *p, char *str, float val, Prog *ptr);
float lookup(Prog *ptr, char *str);
int assign_var(char *str, Node **vars, float val, Prog *ptr);

