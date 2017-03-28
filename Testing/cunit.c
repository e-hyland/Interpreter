#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "funcs.c"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

void test_read(void);
void test_openbracket(void);
void test_instrclist(void);
void test_instruction(void);
void test_initialise(void);
void test_moveforward(void);
void test_variable(void);

int main(void)
{
   CU_pSuite pSuite1 = NULL;

   if (CUE_SUCCESS != CU_initialize_registry()){
      return CU_get_error();
   }
   pSuite1 = CU_add_suite("Suite_1", NULL, NULL);
   if (NULL==pSuite1) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   if (NULL==CU_add_test(pSuite1, "test initialise", test_initialise)){
      CU_cleanup_registry();
      return CU_get_error();
   }
   if (NULL==CU_add_test(pSuite1, "test read", test_read)){
      CU_cleanup_registry();
      return CU_get_error();
   }
   if (NULL==CU_add_test(pSuite1, "test openbracket", test_openbracket)){
      CU_cleanup_registry();
      return CU_get_error();
   }
   if (NULL==CU_add_test(pSuite1, "test instrclist", test_instrclist)){
      CU_cleanup_registry();
      return CU_get_error();
   }
   if (NULL==CU_add_test(pSuite1, "test instruction", test_instruction)){
      CU_cleanup_registry();
      return CU_get_error();
   }
   if (NULL==CU_add_test(pSuite1, "test moveforward", test_moveforward)){
      CU_cleanup_registry();
      return CU_get_error();
   }
   if (NULL==CU_add_test(pSuite1, "test variable", test_variable)){
      CU_cleanup_registry();
      return CU_get_error();
   }
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();

}

void test_initialise(void)
{
  Prog *ptr=NULL;
  ptr=malloc(sizeof(Prog));

  initialise(ptr);
  CU_ASSERT_EQUAL(ptr->vars[ALPHABET-1], 0);
  CU_ASSERT_EQUAL(ptr->x, START_X);
  CU_ASSERT_EQUAL(ptr->y, START_Y);
  CU_ASSERT_EQUAL(ptr->x2, START_X);
  CU_ASSERT_EQUAL(ptr->y2, START_Y);
  CU_ASSERT_EQUAL(ptr->cw, 0);
}

void test_read(void)
{
   Prog *ptr=NULL;
   ptr=malloc(sizeof(Prog));

   initialise(ptr);
   readfile("prog1.txt", ptr);

   CU_ASSERT_EQUAL(ptr->cw, 33);
   CU_ASSERT_STRING_EQUAL(ptr->wds[0], "{");
   CU_ASSERT_STRING_EQUAL(ptr->wds[2], "A");
   CU_ASSERT_STRING_EQUAL(ptr->wds[15], "1");
   CU_ASSERT_STRING_EQUAL(ptr->wds[20], "C");
}

void test_openbracket(void)
{
   Prog *ptr=NULL;
   ptr=malloc(sizeof(Prog));

   initialise(ptr);
   readfile("prog1.txt", ptr);
   ptr->cw=0;
   openbracket(ptr);

   CU_ASSERT_STRING_EQUAL(ptr->wds[0], "{");
   CU_ASSERT_EQUAL(ptr->cw, 1);
   CU_ASSERT_EQUAL(ptr->lw, 32);
}

void test_instrclist(void)
{
   Prog *ptr=NULL;
   SDL_Simplewin sw;
   ptr=malloc(sizeof(Prog));

   initialise(ptr);
   readfile("prog1.txt", ptr);
   ptr->cw=1;
   instrclist(ptr, sw);

   CU_ASSERT_EQUAL(ptr->lw, 32);
   CU_ASSERT_EQUAL(ptr->cw, 32);
   CU_ASSERT_STRING_EQUAL(ptr->wds[ptr->cw], "}");
   CU_ASSERT_EQUAL(ptr->vars[0], 51);
   CU_ASSERT_EQUAL(ptr->vars[1], 9);
   CU_ASSERT_EQUAL(ptr->vars[2], 10);
}

void test_instruction(void)
{
   Prog *ptr=NULL;
   SDL_Simplewin sw;
   ptr=malloc(sizeof(Prog));

   initialise(ptr);
   readfile("prog1.txt", ptr);
   ptr->cw=1;
   instruction(ptr, sw);
   CU_ASSERT_STRING_EQUAL(ptr->wds[ptr->cw], "}");
   CU_ASSERT_STRING_EQUAL(ptr->wds[ptr->cw-5], "FD");
   CU_ASSERT_STRING_EQUAL(ptr->wds[ptr->cw-1], "}");
   CU_ASSERT_STRING_EQUAL(ptr->wds[ptr->cw-2], "45");
   CU_ASSERT_EQUAL(ptr->cw, 31);
   
}

void test_moveforward(void)
{
   Prog *ptr=NULL;
   int radians=0, x, y;
   float val;
   ptr=malloc(sizeof(Prog));

   initialise(ptr);
   readfile("prog1.txt", ptr);
   ptr->cw=11;
   move_forward(ptr);
   CU_ASSERT_DOUBLE_EQUAL(radians, ptr->degrees*(M_PI/RADIANS), 0.05);
   CU_ASSERT_DOUBLE_EQUAL(x, (val)*cos(radians), 0.05);
   CU_ASSERT_DOUBLE_EQUAL(y, (val)*sin(radians), 0.05);
   CU_ASSERT_EQUAL(ptr->x, ptr->x+x);
   CU_ASSERT_EQUAL(ptr->y, ptr->y+y);
}

void test_variable(void)
{
   Prog *ptr=NULL;
   ptr=malloc(sizeof(Prog));

   initialise(ptr);
   readfile("prog1.txt", ptr);
   ptr->cw=4;
   variable(ptr);
   CU_ASSERT_STRING_EQUAL(ptr->wds[ptr->cw], "1");
   CU_ASSERT_STRING_EQUAL(ptr->wds[6], "50");
   CU_ASSERT_EQUAL(ptr->vars[0], 0);
}



