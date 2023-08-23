#define main main_orig

#include "tcc.c"

#undef main

int main(void){
  printf("// use gen_metadata.c to create\n");
  printf("var TOK_IDENT=0x%x;\n",TOK_IDENT);
  printf("var SYM_FIRST_ANOM=0x%x;\n",SYM_FIRST_ANOM);
  printf("var TokenSym_size=0x%x;\n",sizeof(TokenSym));
  return 0;
}
