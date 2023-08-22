#define main main_orig

#include "tcc.c"

#undef main

int main(void){
  printf("// use gen_metadata.c to create\n");
  printf("var TOK_IDENT=0x%x\n",TOK_IDENT);
  return 0;
}
