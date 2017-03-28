#define WRDLEN 20
#define START_SIZE 5
#define PERC 0.8
#define NUMFILES 2
#define N_ASSERT(STATEMENT, ERR_MSG){\
   if (STATEMENT){\
      fprintf(stderr, ERR_MSG);\
      exit(1);\
   }\
}

struct prog{
   char **wds;
   int cw;
   int lw;
};
typedef struct prog Prog;

FILE *openfile(FILE *fp, int argc, char **argv);
void is_float(Prog *ptr, char c);
void is_negative(Prog *ptr, char c);
int readfile(FILE *fp, Prog *ptr);
void initialise(Prog *ptr);
void end_loop(Prog *ptr);
void start_loop(Prog *ptr);
void exit_prog(Prog *ptr);
char **resize_array(Prog *ptr, int nsize);
void freewords(Prog *ptr);
void openbracket(Prog *ptr);
int instrclist(Prog *ptr);
int instruction(Prog *ptr);
int variable(Prog *ptr);
void set_var(Prog *ptr);
int polish(Prog *ptr);
int operator_var(Prog *ptr, char c);
int doloop(Prog *ptr);
int is_number(Prog *ptr, char c);
void is_alpha(Prog *ptr);
void copy_array(Prog *ptr, char **newarray, int nsize);
void enlarge_array(Prog *ptr, int fll);
int valid_number(Prog *ptr);
