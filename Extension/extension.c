#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "neillsdl2.h"
#include "extension.h"
#include "exthash.h"

/*Parse and interpret turtle program according to 
Extended Turtle Grammar rules.
Output via SDL.*/

int main(int argc, char **argv)
{
   FILE *fp=NULL;
   Prog *ptr=NULL; 
   SDL_Simplewin sw;
   Neill_SDL_Init(&sw);
   Neill_SDL_SetDrawColour(&sw,WHITE, WHITE,WHITE);
   ptr=malloc(sizeof(Prog));
   N_ASSERT(ptr==NULL, "Error.\n");
 
   fp=openfile(fp, argc, argv);
   initialise(ptr);
   ptr->lw=readfile(fp, ptr); /*Set last word int to final 
                                word read in*/
   ptr->cw=0;/*Set current word to beginning of prog*/
   openbracket(ptr);/*Check file begins with open bracket*/
   instrclist(ptr, sw); 
   freewords(ptr);
   return 0;
}

FILE *openfile(FILE *fp, int argc, char **argv)
{
  if (argc==NUMFILES){
      if ((fp = fopen(argv[1], "r"))==NULL){
         fprintf(stderr, "Error, please enter valid file name.\n");
         exit(1);
      }
  }
  else {
     fprintf(stderr, "Error, please enter valid file name.\n");
     exit(1);
   }
   return fp;
}

int readfile(FILE *fp, Prog *ptr)
{
   char str[WRDLEN];
   int nsize=START_SIZE;

   while (fscanf(fp, "%s", str) != EOF){
      str[strlen(str)+1] = '\0';
      if (ptr->cw>(nsize*PERC)){/*If no. of words is > 80% of 
                                  array size, enlarge array*/
         nsize=nsize+START_SIZE;/*Increase size of array*/
         enlarge_array(ptr, nsize);
      }
      ptr->wds[ptr->cw]=malloc(sizeof(char)*WRDLEN);
      strcpy(ptr->wds[ptr->cw], str);
      ptr->cw++;
   }
   fclose(fp);
   return (ptr->cw-1);
}
/*Copies to temp array, freeing ptr->wds*/
void enlarge_array(Prog *ptr, int nsize)
{
   int i;
   char **temparray=malloc(sizeof(char *)*ptr->cw);
   N_ASSERT(temparray==NULL, "Error.\n");

   for (i=0;i<(ptr->cw);i++){
      temparray[i] = malloc(sizeof(char)*WRDLEN);
      strcpy(temparray[i], ptr->wds[i]);
      free(ptr->wds[i]);
   }
   free(ptr->wds);
   ptr->wds=NULL;
   copy_array(ptr, temparray, nsize);   
}
/*Copies back to ptr->wds after mallocing a 
larger space, frees temp array*/
void copy_array(Prog *ptr, char **temparray, int nsize)
{
   int i;

   ptr->wds=(char **)malloc(sizeof(char *)*nsize);
   for (i=0;i<ptr->cw;i++){
      ptr->wds[i]=malloc(sizeof(char)*WRDLEN);
      strcpy(ptr->wds[i], temparray[i]);
      free(temparray[i]);
   }
   free(temparray);
   temparray=NULL;
}

void openbracket(Prog *ptr)
{
   if (strcmp(ptr->wds[ptr->cw], "{")!=0){
      freewords(ptr);
      fprintf(stderr, "No opening bracket. Invalid program.\n");
      exit(1);
   }
   ptr->cw++;
}

void initialise(Prog *ptr)
{
   ptr->cw=0;
   ptr->wds=(char **)malloc(sizeof(char *)*START_SIZE);
   N_ASSERT(ptr->wds==NULL, "Error.\n");
   ptr->vars=(Node **)malloc(sizeof(Node *)*VARNUMS);
   N_ASSERT(ptr->wds==NULL, "Error.\n");
   
   /*Set function tag to main when program starts*/
   strcpy(ptr->func, "Main");
   ptr->size=VARNUMS;/*Set initial size of variable table*/
   ptr->x2=START_X;/*Set all coordinates to middle of window*/
   ptr->y2=START_Y;
   ptr->x=START_X;
   ptr->y=START_Y;
   init_vartable(ptr->vars, VARNUMS);
}

void init_vartable(Node **vars, int size)
{
   int i;

   for (i=0;i<size;i++){ /*Set variables to null*/
      vars[i]=NULL;
   }
}

void draw_line(Prog *ptr, SDL_Simplewin sw)
{
   /*Draw line from (x2, y2) to (x, y)*/
   Neill_SDL_Events(&sw);
   if (!sw.finished){
      SDL_Delay(MILLISECONDDELAY);
      SDL_RenderDrawLine(sw.renderer, ptr->x2, 
                         ptr->y2, ptr->x, ptr->y);
      SDL_RenderPresent(sw.renderer);
      SDL_UpdateWindowSurface(sw.win); 
      ptr->x2=ptr->x;
      ptr->y2=ptr->y;
   }
   atexit(SDL_Quit);
}

int instrclist(Prog *ptr, SDL_Simplewin sw)
{  
   if (ptr->cw>ptr->lw){/*If currentword>lastword, 
    program does not have ending close bracket*/
      fprintf(stderr, "No closing bracket.\n");
      freewords(ptr);
      exit(1);
   }
   if (strcmp(ptr->wds[ptr->cw], "}")==0){
      return 1;/*Exit recursive loop on }*/
   }
   instruction(ptr, sw);
   ptr->cw++;
   instrclist(ptr, sw);
   return 1;
}
/*Check valid instruction*/
int instruction(Prog *ptr, SDL_Simplewin sw)
{
   if (strcmp(ptr->wds[ptr->cw], "DO")==0){
      ptr->cw++;
      doloop(ptr, sw);
      return 0;
   }
   if (strcmp(ptr->wds[ptr->cw], "FD")==0){
      ptr->cw++;
      move_forward(ptr, sw);
      return 0;
   }
   if ((strcmp(ptr->wds[ptr->cw], "LT")==0)||
      (strcmp(ptr->wds[ptr->cw], "RT")==0)){
      ptr->cw++;
      total_degrees(ptr);
      return 0;
   }
   if (strcmp(ptr->wds[ptr->cw], "SET")==0){
      ptr->cw++;
      set_var(ptr);
      return 0;
   }
   if (strcmp(ptr->wds[ptr->cw], "JUMP")==0){
      ptr->cw++;
      jump(ptr, sw);
      return 0;
   }
   else {
      fprintf(stderr, "Invalid command\n.");
      exit_prog(ptr);
   }
   return 0;
}

void jump(Prog *ptr, SDL_Simplewin sw)
{
   int a=ptr->cw;/*Save place in main to return to*/

   isalpha(ptr);/*Check valid variable*/
   /*Change ptr->func to function name so that all variables 
    will be tagged with this and hence only accessible locally*/
   strcpy(ptr->func, ptr->wds[ptr->cw]);
   ptr->cw++;

   while (strcmp(ptr->wds[ptr->cw], ptr->func)!=0){
      ptr->cw++;/*Walk through program until you reach function*/
   }
   ptr->cw++;
   openbracket(ptr);/*Check open bracket of function*/
   instrclist(ptr, sw);
   ptr->cw=a;/*Return to main and change func tag accordingly*/
   strcpy(ptr->func, "Main");
}

void total_degrees(Prog *ptr)
{
   float val;
   val = variable(ptr);/*Check variable is valid and retrieve value*/

   if (strcmp(ptr->wds[ptr->cw-1], "LT")==0){
      left_turn(ptr, val);
   }
   if (strcmp(ptr->wds[ptr->cw-1], "RT")==0){
      right_turn(ptr, val);
   }
}
/*Calculate angle of movement, ensuring degrees<360*/
int left_turn(Prog *ptr, float val)
{
   if (val > ptr->degrees){
      ptr->degrees = CIRCLE-(val-ptr->degrees);
      return 1;
   }
   else {      
      ptr->degrees = ptr->degrees-(val);
      return 1;
   }
}
/*Calculate angle of movement, ensuring degrees<360*/
int right_turn(Prog *ptr, float val)
{
   if ((val+ptr->degrees)>CIRCLE){
      ptr->degrees = (val+ptr->degrees)-CIRCLE;
      while (ptr->degrees>CIRCLE){
         ptr->degrees-=CIRCLE;
      }
      return 1;
   }
   else {
      ptr->degrees = ptr->degrees+(val);
      return 1;
   }
}
/*Convert degrees to radians, set new coordinates 
and draw line*/
void move_forward(Prog *ptr, SDL_Simplewin sw)
{
   float radians, val;
   int x, y;

   radians = ptr->degrees*(M_PI/RADIANS); 
   val = variable(ptr);
   x=(int)(val)*cos(radians);
   y=(int)(val)*sin(radians);
   ptr->x=ptr->x+x;/*Set new x and y values*/
   ptr->y=ptr->y+y;
   draw_line(ptr, sw);
}

int doloop(Prog *ptr, SDL_Simplewin sw)
{
   float val=0, finish=0;
   char str[VARLEN];
   strcpy(str,ptr->wds[ptr->cw]);

   is_alpha(ptr);/*Check valid variable*/
   ptr->cw++;
   val=start_loop(ptr, val);
   /*Assign variable starting number*/
   assign_var(str, ptr->vars, val, ptr);
   ptr->cw++;
   finish=end_loop(ptr, finish);
   if (val<=finish){/*If loop increasing*/
      loop_thru(str, val, finish, ptr, sw, 1);
   }
   else {/*If loop decreasing*/
      loop_thru(str, val, finish, ptr, sw, -1);
   }
   return 1;
}

float start_loop(Prog *ptr, float val)
{
   if (strcmp(ptr->wds[ptr->cw], "FROM")!=0){
      fprintf(stderr, "Require 'FROM'\n.");
      exit_prog(ptr);
   }
   ptr->cw++;
   val=variable(ptr);/*Get starting number for doloop*/
   return val;
}

float end_loop(Prog *ptr, float finish)
{
   if (strcmp(ptr->wds[ptr->cw], "TO")!=0){
      fprintf(stderr, "Require 'TO'\n.");
      exit_prog(ptr);
   }
   ptr->cw++;
   finish=variable(ptr);/*Get ending number for doloop*/
   ptr->cw++;
   openbracket(ptr); /*Check for open bracket before loop*/
   return finish;
}
/*If loop is decreasing (eg. from 8 to 1), 
while loop becomes: (-1*val)<finish+(-1).
Otherwise while loop is: (1*val)<finish+1. */
void loop_thru(char *str, float val, float finish, 
Prog *ptr, SDL_Simplewin sw, int s)
{
   int begin=ptr->cw, fin=0;
   /*Set starting coordinates before entering doloop*/
   ptr->x2=ptr->x;
   ptr->y2=ptr->y;
/*While variable is less than endpoint of doloop*/
   while ((s*val)<finish+s){
      instrclist(ptr, sw);
      Neill_SDL_Events(&sw);
      if (sw.finished){
         atexit(SDL_Quit);
         freewords(ptr);
         exit(1);
      }
      fin=ptr->cw;/*Track end of doloop*/
      val=lookup(ptr, str);
      val+=s;
      /*Update variable*/
      assign_var(str, ptr->vars, val, ptr);
      ptr->cw=begin;/*Jump to top of doloop*/
   }
   ptr->cw=fin;
}

int set_var(Prog *ptr)
{
   Stack s;
   char str[VARLEN];
   strcpy(str, ptr->wds[ptr->cw]);

   is_alpha(ptr);/*Check valid variable*/
   ptr->cw++;
   if (strcmp(ptr->wds[ptr->cw], ":=")!=0){
      fprintf(stderr, "Require: :=\n");
      exit_prog(ptr);
   }
   ptr->cw++;
   buildstack(str, ptr, &s);
   return 1;
}
/*Initialise stack to perform reverse polish, pop final no.*/
void buildstack(char *str, Prog *ptr, Stack *s)
{
   float val;

   s->tp = (Elem *)calloc(1, sizeof(Elem));
   N_ASSERT(s->tp==NULL, "Error\n");
   s->tp->prev = NULL;
   polish(ptr, s);
   check_stack(ptr, s);
   val=Pop(s, ptr);
   /*Assign value to variable, str*/
   assign_var(str, ptr->vars, val, ptr);
   freestack(s);
   s=NULL;
}
/*Check that there is only one variable on stack 
for final pop*/
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
   if (s->tp->prev != NULL){
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
      return 1;/*Base case ";"*/
   }
   /*Check if valid operator or variable*/
   if (operator_var(ptr, c, s)){
      ptr->cw = ptr->cw+1;
      polish(ptr, s);
   }
   else { 
      fprintf(stderr, "Invalid symbol in polish.\n");
      freestack(s);
      exit_prog(ptr);
   }
   return 0;
}

int operator_var(Prog *ptr, char c, Stack *s)
{
   float val; 

   /*Ensure *5 not accepted*/
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
/*Perform operator on previous two operands. 
If there is insufficient operands, pop will error.*/
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

   if (isalpha(c)){
      is_alpha(ptr);
      /*Assign value to variable*/
      val=lookup(ptr, ptr->wds[ptr->cw]);
      return val;
   }
   if (isdigit(c)||(c=='.')||(c=='-')){
      val=is_number(ptr, c);
      return val;
   }
   else {
      fprintf(stderr, "Invalid variable/No semi-colon\n"); 
      exit_prog(ptr);
   }
   return 0;
}

float is_number(Prog *ptr, char c)
{
   float val=0;

   /*Eliminate strings which don't have digits*/
   valid_number(ptr);
   if ((c=='-')||(isdigit(c))){
      val=is_negative(ptr, c);
      return val;
   }
   if (c=='.'){
      val=is_float(ptr,c);
      return val;
   }
   else {
      fprintf(stderr, "Invalid variable/No semi-colon\n"); 
      exit_prog(ptr); 
   }  
   return val;
}

int valid_number(Prog *ptr)
{
   int i=0;

   while (ptr->wds[ptr->cw][i]!='\0'){
      if (isdigit(ptr->wds[ptr->cw][i])){
         return 1;
      }
      i++;
   }
   fprintf(stderr, "Invalid variable/No semi-colon\n"); 
   exit_prog(ptr);
   return 0;

}

float is_negative(Prog *ptr, char c)
{
  int i=1, cnt=0;
  float val;

  while (ptr->wds[ptr->cw][i]!='\0'){
     c = ptr->wds[ptr->cw][i];
     if (c=='.'){
     /*If there is more than one '.', invalid variable*/
        if (++cnt>1){ 
           exit_prog(ptr);
        }
     }
     if ((!isdigit(c))&&(c!='.')){
        fprintf(stderr, "Invalid variable/No semi-colon\n"); 
         exit_prog(ptr);
     }
     i++;
  }
  val=atof(ptr->wds[ptr->cw]);/*Convert string to float*/
  return val;

}

float is_float(Prog *ptr, char c)
{
  int i=1;
  float val;

  while (ptr->wds[ptr->cw][i]!='\0'){
     c = ptr->wds[ptr->cw][i];
     if (!isdigit(c)){
        fprintf(stderr, "Invalid variable/No semi-colon\n"); 
        exit_prog(ptr);
     }
     i++;
  }
  val=atof(ptr->wds[ptr->cw]);
  return val;

}

void is_alpha(Prog *ptr)
{
   int i, n=strlen(ptr->wds[ptr->cw]);

   if (!isalpha(ptr->wds[ptr->cw][0])){
      fprintf(stderr, "Invalid variable/No semi-colon\n");
      exit_prog(ptr);
   }
   for (i=0;i<n;i++){
      if (!isalnum(ptr->wds[ptr->cw][i])){
         fprintf(stderr, "Invalid variable name.\n");
         exit_prog(ptr);
      }
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

