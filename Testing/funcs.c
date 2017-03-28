#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "neillsdl2.h"
#include "funcs.h"

void initialise(Prog *ptr)
{
   int i;
   ptr->cw=0;
   ptr->wds=(char **)malloc(sizeof(char *)*START_SIZE);
   N_ASSERT(ptr->wds==NULL, "Error.\n");

   for (i=0;i<ALPHABET;i++){
      ptr->vars[i]=0;
   }
   ptr->x2=START_X;
   ptr->y2=START_Y;
   ptr->x=START_X;
   ptr->y=START_Y;
}

int readfile(char *argv, Prog *ptr)
{
   char str[WRDLEN];
   int nsize=START_SIZE;
   FILE *fp = fopen(argv, "r");

   while (fscanf(fp, "%s", str)!=EOF){
      str[strlen(str)+1] = '\0';
      if (ptr->cw>(nsize*PERC)){
         nsize=nsize+START_SIZE;
         enlarge_array(ptr, nsize);
      }
      ptr->wds[ptr->cw]=malloc(sizeof(char)*WRDLEN);
      strcpy(ptr->wds[ptr->cw], str);
      ptr->cw++;
   }
   fclose(fp);
   ptr->lw=ptr->cw-1;
   return ptr->lw;
}

void enlarge_array(Prog *ptr, int nsize)
{
   int i;
   char **temparray=malloc(sizeof(char *)*ptr->cw);
   N_ASSERT(temparray==NULL, "Error.\n");

   for (i=0;i<ptr->cw;i++){
      newarray[i]=malloc(sizeof(char)*WRDLEN);
      strcpy(temparray[i], ptr->wds[i]);
      free(ptr->wds[i]);
   }
   free(ptr->wds);
   ptr->wds=NULL;
   copy_array(ptr, temparray, nsize);
}

void copy_array(Prog *ptr, char **temparray, int nsize)
{
   int i;
   ptr->wds=(char **)malloc(sizeof(char *)*nsize);
   N_ASSERT(ptr->wds==NULL, "Error.\n");

   for (i=0;i<ptr->cw;i++){
      ptr->wds[i]=malloc(sizeof(char)*WRDLEN);
      strcpy(ptr->wds[i], temparray[i]);
      free(temparray[i]);
   }
   free(newarray);
   newarray=NULL;
}

void openbracket(Prog *ptr)
{
   if (strcmp(ptr->wds[ptr->cw], "{")!=0){
      freewords(ptr);
      fprintf(stderr, "No opening bracket.\n");
      exit(1);
   }
   ptr->cw++;
}

int instrclist(Prog *ptr, SDL_Simplewin sw)
{
   if (ptr->cw>ptr->lw){
      fprintf(stderr, "No closing bracket.\n");
      freewords(ptr);
      exit(1);
   }
   if (strcmp(ptr->wds[ptr->cw], "}")==0){
      return 1;
   }
   instruction(ptr, sw);
   ptr->cw++;
   instrclist(ptr, sw);
   return 1;
}

int instruction(Prog *ptr, SDL_Simplewin sw)
{
   if (strcmp(ptr->wds[ptr->cw], "DO")==0){
      ptr->cw++;
      doloop(ptr, sw);
      return 1;
   }
   if (strcmp(ptr->wds[ptr->cw], "FD")==0){
      ptr->cw++;
      move_forward(ptr);
      return 1;
   }
   if ((strcmp(ptr->wds[ptr->cw], "LT")==0)||
      (strcmp(ptr->wds[ptr->cw], "RT")==0)){
      ptr->cw++;
      total_degrees(ptr);
      return 1;
   }
   if (strcmp(ptr->wds[ptr->cw], "SET")==0){
      ptr->cw++;
      set_var(ptr);
      return 1;
   }
   else {
      fprintf(stderr, "Invalid command\n.");
      exit_prog(ptr);
   }
   return 0;
}

void total_degrees(Prog *ptr)
{
   float val;
   val=variable(ptr);

   if (strcmp(ptr->wds[ptr->cw-1], "LT")==0){
      left_turn(ptr, val);
   }
   if (strcmp(ptr->wds[ptr->cw-1], "RT")==0){
      right_turn(ptr, val);
   }
}

void left_turn(Prog *ptr, float val)
{
   if (val > ptr->degrees){
      ptr->degrees = CIRCLE-(val-ptr->degrees);
   }
   else {      
      ptr->degrees = ptr->degrees-(val);
   }
}

void right_turn(Prog *ptr, float val)
{
   if ((val+ptr->degrees)>CIRCLE){
      ptr->degrees = (val+ptr->degrees)-CIRCLE;
   }
   else {
      ptr->degrees = ptr->degrees+(val);
   }
}

void move_forward(Prog *ptr)
{
   float radians, val;
   int x, y;

   radians=ptr->degrees*(M_PI/RADIANS); 
   val=variable(ptr);
   x=(int)(val)*cos(radians);
   y=(int)(val)*sin(radians);
   ptr->x=ptr->x+x;
   ptr->y=ptr->y+y;
}

int doloop(Prog *ptr, SDL_Simplewin sw)
{
   float start=0, finish=0;
   char c=ptr->wds[ptr->cw][0];

   is_alpha(ptr);
   ptr->cw++;
   start=start_loop(ptr, start);
   ptr->vars[(c-'A')] = start;
   ptr->cw++;
   finish=end_loop(ptr, finish);
   if (start<=finish){
      loop_thru(c, finish, ptr, sw, 1);
   }
   else {
      loop_thru(c, finish, ptr, sw, -1);
   }
   return 1;
}

float start_loop(Prog *ptr, float start)
{
   if (strcmp(ptr->wds[ptr->cw], "FROM")!=0){
      fprintf(stderr, "Require 'FROM'\n.");
      exit_prog(ptr);
   }
   ptr->cw++;
   start=variable(ptr);
   return start;
}

float end_loop(Prog *ptr, float finish)
{
   if (strcmp(ptr->wds[ptr->cw], "TO")!=0){
      fprintf(stderr, "Require 'TO'\n.");
      exit_prog(ptr);
   }
   ptr->cw++;
   finish=variable(ptr);
   ptr->cw++;
   if (strcmp(ptr->wds[ptr->cw], "{")!=0){
      fprintf(stderr, "Require '{' before loop\n.");
      exit_prog(ptr);
   }
   ptr->cw++;
   return finish;
}

void loop_thru(char c, int finish, Prog *ptr, SDL_Simplewin sw, int s)
{
   int begin=ptr->cw, end=0;
   ptr->x2=ptr->x;
   ptr->y2=ptr->y;

   while (s*ptr->vars[(c-'A')]<finish+s){
      instrclist(ptr, sw);
      end=ptr->cw;
      ptr->vars[(c-'A')]+=s;
      ptr->cw=begin;
   }
   ptr->cw=end;
}

int set_var(Prog *ptr)
{
   Stack s;
   char c=ptr->wds[ptr->cw][0];

   is_alpha(ptr);
   ptr->cw++;
   if (strcmp(ptr->wds[ptr->cw], ":=")!=0){
      fprintf(stderr, "Require: :=\n");
      exit_prog(ptr);
   }
   ptr->cw++;
   buildstack(c, ptr, &s);
   return 1;
}

void buildstack(char c, Prog *ptr, Stack *s)
{
   float val;

   s->tp = (Elem *)calloc(1, sizeof(Elem));
   N_ASSERT(s->tp==NULL, "Error\n");
   s->tp->prev = NULL;

   polish(ptr, s);
   check_stack(ptr, s);
   val=Pop(s, ptr);
   ptr->vars[(c-'A')]=val;
   freestack(s);
}

void check_stack(Prog *ptr, Stack *s)
{
   int cnt=0;
   Elem *c=s->tp;

   while (c->prev!=NULL){
      cnt++;
      c=c->prev;
   }
   if (cnt!=1){
      fprintf(stderr, "Incorrect polish.\n");
      freestack(s);
      exit_prog(ptr);
   }
}

void Push(Stack *s, float n)
{
   Elem *e;
   e = (Elem *)calloc(1, sizeof(Elem));
   N_ASSERT(e==NULL, "Error\n");

   e->prev = s->tp;
   s->tp->i = n;
   s->tp = e;   
}

float Pop(Stack *s, Prog *ptr)
{
   if (s->tp->prev!=NULL){
      s->tp = s->tp->prev;
      return s->tp->i;
   }
   else {
      fprintf(stderr, "Insufficient operands.\n");
      freestack(s); 
      exit_prog(ptr);
      return 0;
   } 
}

float polish(Prog *ptr, Stack *s)
{
   char c=ptr->wds[ptr->cw][0];

   if (strcmp(ptr->wds[ptr->cw], ";")==0){
      return 1;
   }
   if (operator_var(ptr, c, s)){
      ptr->cw = ptr->cw+1;
      polish(ptr, s);
   }
   else { 
      fprintf(stderr, "Invalid symbol in polish equation.\n");
      freestack(s);
      exit_prog(ptr);
   }
   return 0;
}

int operator_var(Prog *ptr, char c, Stack *s)
{
   float val; 

   if ((strlen(ptr->wds[ptr->cw])==1)&&
      ((c=='+')||(c=='-')||(c=='/')||(c=='*'))){
      do_maths(s, c, ptr);    
      return 1;
   }
   else {
      val=variable(ptr);
      Push(s, val);
      return 1;
   }
   return 0;
}

void do_maths(Stack *s, char c, Prog *ptr)
{
   float op1, op2;

   switch (c){
      case '+':
         Push(s, (Pop(s, ptr)+Pop(s, ptr)));
         break;
      case '-':
         Push(s, (Pop(s, ptr)-Pop(s, ptr)));
         break;
      case '*':
         Push(s, (Pop(s, ptr)*Pop(s, ptr)));
         break;
      case '/':
         op1 = Pop(s, ptr);
         op2 = Pop(s, ptr);
         Push(s, (op2/op1));
         break;
      default:
         fprintf(stderr, "Invalid operator.\n");
         freestack(s);
         exit_prog(ptr);
   }
}
float variable(Prog *ptr)
{
   char c=ptr->wds[ptr->cw][0];
   float val;

   if (isupper(c)){
      is_alpha(ptr);
      val=ptr->vars[(c-'A')];
      return val;
   }
   if (isdigit(c)||(c=='.')||(c=='-')){
      val=is_number(ptr, c);
      return val;
   }
   else {
      fprintf(stderr, "Invalid variable/No semi-colon after equation\n"); 
      exit_prog(ptr);
   }
   return 0;
}

float is_number(Prog *ptr, char c)
{
   float val=0;

   if ((c=='-')||(isdigit(c))){
      val=is_negative(ptr, c);
      return val;
   }
   if (c=='.'){
      val=is_float(ptr,c);
      return val;
   }
   else {
      fprintf(stderr, "Invalid variable/No semi-colon after equation\n"); 
      exit_prog(ptr); 
   }  
   return val;
}

float is_negative(Prog *ptr, char c)
{
   int i=1, cnt=0;
   float val;

   while (ptr->wds[ptr->cw][i]!='\0'){
      c = ptr->wds[ptr->cw][i];
      if (c=='.'){
         cnt++;
      }
      if ((!isdigit(c))&&(c!='.')){
         fprintf(stderr, "Invalid variable/No semi-colon after equation\n"); 
          exit_prog(ptr);
      }
      i++;
   }
   if (cnt>1){
      exit_prog(ptr);
   }
   val=atof(ptr->wds[ptr->cw]);
   return val;
}

float is_float(Prog *ptr, char c)
{
  int i=1;
  float val;
   
   while (ptr->wds[ptr->cw][i]!='\0'){
      c = ptr->wds[ptr->cw][i];
      if (!isdigit(c)){
         fprintf(stderr, "Invalid variable/No semi-colon after equation\n"); 
         exit_prog(ptr);
      }
      i++;
   }
   val=atof(ptr->wds[ptr->cw]);
   return val;
}

void is_alpha(Prog *ptr)
{
   char c=ptr->wds[ptr->cw][0];

   if (!isupper(c)||(strlen(ptr->wds[ptr->cw])!=1)){
      fprintf(stderr, "Invalid variable/No semi-colon after equation\n");
      exit_prog(ptr);
   }
}

void freewords(Prog *ptr)
{
   int i=0;
   while (i<ptr->lw+1){
      free(ptr->wds[i]);
      i++;
   }
   free(ptr->wds);
   free(ptr);
}

void exit_prog(Prog *ptr)
{
   atexit(SDL_Quit);
   fprintf(stderr, "'%s %s %s': INVALID. Invalid program.\n", 
   ptr->wds[ptr->cw-1], ptr->wds[ptr->cw], ptr->wds[ptr->cw+1]);
   freewords(ptr);
   exit(1);
}

void freestack(Stack *s)
{
   while (s->tp!= NULL){
      free(s->tp);
      s->tp = s->tp->prev;
   }
}

