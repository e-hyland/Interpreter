#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "neillsdl2.h"
#include "extension.h"
#include "exthash.h"

/*Lookup values, ensuring that variable name is 
the same and that it belongs to the same function*/
float lookup(Prog *ptr, char *str)
{
   float val;
   unsigned int hash;
   Node *current=NULL;
   hash=hash_all(str)%ptr->size;
   current=ptr->vars[hash];

   if (current==NULL){ /*If variable not set, assume 0*/
      return 0;
   }
   if ((strcmp(current->str, str)==0)&&
       (strcmp(ptr->func, current->func)==0)){
      return current->val;
   }
   else {
      /*Walk through linked list*/
      while((strcmp(current->str, str)!=0)||
            (strcmp(ptr->func, current->func)!=0)){
         if (current->next==NULL){/*If variable not set, assume 0*/
            return 0;
         }
         current=current->next;
      }
      val=current->val;
      return val;
   }
}
/*Set variable to value */
int assign_var(char *str, Node **vars, float val, Prog *ptr)
{
   unsigned int hash=0;
   static int cnt=0;
   Node *p=NULL;
   Node *current=NULL;
   hash=hash_all(str)%ptr->size; 
   current=vars[hash];
   /*Variable not previously set, allocate new node and set value */
   if (vars[hash]==NULL){
      p=AllocateNode(p, str, val, ptr); 
      vars[hash]=p;
      return 1;
   }
   else {
      cnt=0;/*Measure list length*/
      cnt=search_list(str, current, val, cnt, ptr);
   }
   checklistlen(ptr, cnt);
   return 0;
}

int search_list(char *str, Node *current, float val, int cnt, Prog *ptr)
{
   Node *p=NULL;
  /*Walk through list until function and variable name match*/
   while (current->next!=NULL){
      cnt++;
      if((strcmp(current->str, str)==0)&&
         (strcmp(current->func, ptr->func)==0)){
         current->val=val;
         return cnt;
      }
      current=current->next;
   }
   if((strcmp(current->str, str)==0)&&
      (strcmp(current->func, ptr->func)==0)){
      current->val=val;
   }
   /*Variable not previously set, allocate new node and set value */
   else {
      cnt++;
      p=AllocateNode(p, str, val, ptr);
      current->next=p;
   }
   return cnt;
}

void checklistlen(Prog *ptr, int cnt)
{
   int oldsize;
   /*If there are more than three words in list, enlarge table*/
   if (cnt>LIST_LIMIT){
      cnt=0;
      oldsize=ptr->size;
      ptr->size=calcsize(ptr->size);/*Calculate new prime size array*/
      ptr->vars=resize_table(ptr, oldsize);
   } 
}
/*Enlarge variable table, freeing old table as we go.
Do not check list length at this point as this would make 
array unreasonably large and program slower.*/
Node **resize_table(Prog *ptr, int oldsize)
{
   Node **newtable;
   Node *list;
   int i;
   newtable=malloc(sizeof(Node *)*(ptr->size));
   N_ASSERT(newtable==NULL, "Error.\n");
   init_vartable(newtable, ptr->size);/*Initialise to NULL*/

   for (i=0;i<oldsize;i++){
      if (ptr->vars[i]!=NULL){
         assign_var(ptr->vars[i]->str, newtable, ptr->vars[i]->val, ptr);
         list=ptr->vars[i]->next;
         while (list!=NULL){
            assign_var(list->str, newtable, list->val, ptr);
            list=list->next;
         }
         freenodes(ptr, i);
      }
   }   
   free(ptr->vars);
   return newtable;
}

unsigned int hash_all(char *str)
{
   int i;
   int n;
   unsigned int hash=0;

   n = strlen(str);

   for (i=0;i<n;i++){
      hash = str[i]+PRIME*hash;
   }
   return hash;
}


Node *AllocateNode(Node *p, char *str, float val, Prog *ptr)
{
   p = (Node *)malloc(sizeof(Node));

   if (p==NULL){
      printf("Cannot allocate variable.\n");
      exit(1);
   }
   p->next=NULL;
   p->val=val;
   strcpy(p->func, ptr->func);
   strcpy(p->str, str);
   return p;
}

void freenodes(Prog *ptr, int i)
{
   Node *current=ptr->vars[i];

   while (current->next!=NULL){
      free(current);
      current=current->next;
   }
   free(current);
}

int calcsize(int x)
{
   int size=x+x;
   int i;

   for (i=0;;i++){
      size++; /* Add one to size until we find prime number */
      if (is_prime(size)){
         return size;
      }
   }
   return 0;
}

int is_prime(int v)
{
   int d;

   for (d=START;d<=(v-1);d++){
      if (v%d==0){ /* If modulo is 0, 'v' is not prime */
         return 0;
      }
   }
   return 1; 
}
