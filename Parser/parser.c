#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "parser.h"

/*Check grammar of input program is valid 
with Turtle Grammar rules.
Does not run program. */

int main(int argc, char **argv)
{
   FILE *fp=NULL;
   Prog *ptr=NULL; 
   ptr = malloc(sizeof(Prog));
   N_ASSERT(ptr==NULL, "Error\n");

   fp=openfile(fp, argc, argv);
   initialise(ptr);
   ptr->lw=readfile(fp, ptr);/*Set last word int 
                           to final word read in*/
   ptr->cw=0; /*Set current word to beginning of prog*/
   openbracket(ptr);/*Check file begins with open bracket*/
   instrclist(ptr); 
   freewords(ptr);
   return 0;
}

FILE *openfile(FILE *fp, int argc, char **argv)
{
  if (argc==NUMFILES){
      if ((fp = fopen(argv[1], "r")) == NULL){
         fprintf(stderr, "Error, invaild file.\n");
         exit(1);
      }
  }
  else {
     fprintf(stderr, "Error, invalid file.\n");
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
      if (ptr->cw > (nsize*PERC)){ /*If no. of words is > 
                          80% of array size, enlarge array*/
         nsize=nsize+START_SIZE; /*Increase size of array*/
         enlarge_array(ptr, nsize);
      }
      ptr->wds[ptr->cw]=malloc(sizeof(char)*WRDLEN);
      strcpy(ptr->wds[ptr->cw], str);
      ptr->cw++;
   }
   fclose(fp);
   return (ptr->cw-1);
}

void initialise(Prog *ptr)
{
   ptr->cw = 0;
   ptr->wds= (char **)malloc(sizeof(char *)*START_SIZE);
   N_ASSERT(ptr->wds==NULL, "Error.\n");
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
      fprintf(stderr, "No opening bracket.\n");
      exit(1);
   }
   ptr->cw++;
}


int instrclist(Prog *ptr)
{  
/*If currentword>lastword, program does 
not have ending close bracket*/
   if (ptr->cw>ptr->lw){ 
      fprintf(stderr, "No closing bracket.\n");
      freewords(ptr);
      exit(1);
   }
   if (strcmp(ptr->wds[ptr->cw], "}")==0){
      return 1; /*Exit recursive loop on }*/
   }
   instruction(ptr);
   ptr->cw++;
   instrclist(ptr); 
   return 1;
}
/*Check valid instruction*/
int instruction(Prog *ptr)
{
   if (strcmp(ptr->wds[ptr->cw], "FD")==0){
      ptr->cw++;
      variable(ptr);
      return 1;
   }
   if ((strcmp(ptr->wds[ptr->cw], "LT")==0)||
      (strcmp(ptr->wds[ptr->cw], "RT")==0)){
      ptr->cw++;
      variable(ptr);
      return 1;
   }
   if (strcmp(ptr->wds[ptr->cw], "DO")==0){
      ptr->cw++;
      doloop(ptr);
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
      return 0;
   }
}

int doloop(Prog *ptr)
{
   is_alpha(ptr);/*Check variable is valid*/
   ptr->cw++;
   start_loop(ptr);
   ptr->cw++;
   end_loop(ptr);
   instrclist(ptr);
   return 1;
}

void start_loop(Prog *ptr)
{
   if (strcmp(ptr->wds[ptr->cw], "FROM")!=0){
      fprintf(stderr, "Require 'FROM'\n.");
      exit_prog(ptr);
   }
   ptr->cw++;
   variable(ptr);
}

void end_loop(Prog *ptr)
{
   if (strcmp(ptr->wds[ptr->cw], "TO")!=0){
      fprintf(stderr, "Require 'TO'\n.");
      exit_prog(ptr);
   }
   ptr->cw++;
   variable(ptr);/*Check valid variable*/
   ptr->cw++;
   openbracket(ptr); /*Check for open bracket before loop*/
}

void set_var(Prog *ptr)
{
   is_alpha(ptr);
   ptr->cw++;
   if (strcmp(ptr->wds[ptr->cw], ":=")!=0){
      fprintf(stderr, "Require: :=\n");
      exit_prog(ptr);
   }
   ptr->cw++;
   polish(ptr);
}

int polish(Prog *ptr)
{
   char c=ptr->wds[ptr->cw][0];

   if (strcmp(ptr->wds[ptr->cw], ";")==0){
      return 1;
   }
   /*Check that string is operator or variable*/
   if (operator_var(ptr, c)){
      ptr->cw = ptr->cw+1;
      polish(ptr);
   }
   else { 
      fprintf(stderr, "Invalid symbol in polish.\n");
      exit_prog(ptr);
   }
   return 0;
}

int operator_var(Prog *ptr, char c)
{
   if ((strlen(ptr->wds[ptr->cw])==1)&& 
      ((c=='+')||(c=='-')||(c=='/')||(c=='*'))){
      return 1;
   }
   else {
      variable(ptr);/*Check valid variable*/
      return 1;
   }
   return 0;
}

/*Check that variable is valid letter or number*/
int variable(Prog *ptr)
{
   char c=ptr->wds[ptr->cw][0];

   if (isupper(c)){
      is_alpha(ptr);
      return 1;
   }
   if (isdigit(c)||(c=='.')||(c=='-')){
      is_number(ptr, c);
      return 1;
   }
   else {
      fprintf(stderr, "Invalid variable/No semi-colon\n"); 
      exit_prog(ptr);
   }
   return 0;
}

int is_number(Prog *ptr, char c)
{
   valid_number(ptr);/*Eliminate strings without digits*/
   if ((c=='-')||(isdigit(c))){
      is_negative(ptr, c);
      return 1;
   }
   if (c=='.'){
      is_float(ptr,c);
      return 1;
   }
   else {
      fprintf(stderr, "Invalid variable/No semi-colon\n"); 
      exit_prog(ptr); 
   }  
   return 0;
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

void is_float(Prog *ptr, char c)
{
   int i=1;
   
   while (ptr->wds[ptr->cw][i]!='\0'){
      c = ptr->wds[ptr->cw][i];
      if (!isdigit(c)){
         fprintf(stderr, "Invalid variable/No semi-colon\n"); 
         exit_prog(ptr);
      }
      i++;
   }
}

void is_negative(Prog *ptr, char c)
{
   int i=1, cnt=0;

   while (ptr->wds[ptr->cw][i]!='\0'){
      c = ptr->wds[ptr->cw][i];
      /*If there is more than one decimal point, 
        invalid number*/
      if (c=='.'){
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

}

void is_alpha(Prog *ptr)
{
   char c=ptr->wds[ptr->cw][0];

   if (!isupper(c)||(strlen(ptr->wds[ptr->cw])!=1)){
      fprintf(stderr, "Invalid variable/No semi-colon\n"); 
      exit_prog(ptr);
   }
}

void freewords(Prog *ptr)
{
   int i;

   for (i=0;i<ptr->lw+1;i++){
      free(ptr->wds[i]);
   }
   free(ptr->wds);
   free(ptr);
}

void exit_prog(Prog *ptr)
{
   fprintf(stderr, "'%s %s %s': INVALID. Invalid program.\n", 
   ptr->wds[ptr->cw-1], ptr->wds[ptr->cw], ptr->wds[ptr->cw+1]);
   freewords(ptr);
   exit(1);
}
