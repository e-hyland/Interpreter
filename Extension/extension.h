#define WRDLEN 20
#define MILLISECONDDELAY 50
#define START_X 400
#define START_Y 300
#define ALPHABET 26
#define VARLEN 20
#define WHITE 250
#define CIRCLE 360
#define RADIANS 180
#define START_SIZE 10
#define VARNUMS 5
#define NUMFILES 2
#define PERC 0.8
#define N_ASSERT(STATEMENT, ERR_MSG){\
   if (STATEMENT){\
      fprintf(stderr, ERR_MSG);\
      exit(1);\
   }\
}

struct node{
   float val;
   char str[VARLEN];
   char func[VARLEN];
   struct node *next;
};
typedef struct node Node;

struct stackelem{
   float i;
   struct stackelem *prev;
};
typedef struct stackelem Elem;

struct thestack{
   Elem *tp;
};
typedef struct thestack Stack;

struct prog{
   char **wds;
   int size;
   int cw;
   int lw;
   float degrees;
   float x;
   float y;
   float x2;
   float y2;
   char func[VARLEN];
   Node **vars;
};
typedef struct prog Prog;

FILE *openfile(FILE *fp, int argc, char **argv);
void jump(Prog *ptr, SDL_Simplewin sw);
void init_vartable(Node **vars, int size);
int instrclist(Prog *ptr, SDL_Simplewin sw);
void check_stack(Prog *ptr, Stack *s);
int readfile(FILE *fp, Prog *ptr);
void copy_array(Prog *ptr, char **newarray, int nsize);
float is_float(Prog *ptr, char c);
float is_negative(Prog *ptr, char c);
float end_loop(Prog *ptr, float finish);
float start_loop(Prog *ptr, float val);
void exit_prog(Prog *ptr);
void enlarge_array(Prog *ptr, int nsize);
int left_turn(Prog *ptr, float val);
int right_turn(Prog *ptr, float val);
float is_number(Prog *ptr, char c);
int valid_number(Prog *ptr);
void freewords(Prog *ptr);
void freestack(Stack *s);
void loop_thru(char *str, float val, float finish, 
Prog *ptr, SDL_Simplewin sw, int s);
void initialise(Prog *ptr);
void openbracket(Prog *ptr);
void do_maths(Stack *s, char c, Prog *ptr);
int instruction(Prog *ptr, SDL_Simplewin sw);
void move_forward(Prog *ptr, SDL_Simplewin sw);
float variable(Prog *ptr);
int set_var(Prog *ptr);
float polish(Prog *ptr, Stack *s);
int operator_var(Prog *ptr, char c, Stack *s);
int doloop(Prog *ptr, SDL_Simplewin sw);
void is_alpha(Prog *ptr);
float Pop(Stack *s, Prog *ptr);
void buildstack(char *str, Prog *ptr, Stack *s);
void total_degrees(Prog *ptr);
void Push(Stack *s, float n);
void draw_line(Prog *ptr, SDL_Simplewin sw);
