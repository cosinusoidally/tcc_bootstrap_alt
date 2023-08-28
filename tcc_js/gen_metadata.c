#define main main_orig

#include "tcc.c"

#undef main

int main(void){
  printf("// use gen_metadata.c to create\n");
  printf("var TOK_IDENT=0x%x;\n",TOK_IDENT);
  printf("var SYM_FIRST_ANOM=0x%x;\n",SYM_FIRST_ANOM);
  printf("var TokenSym_size=0x%x;\n",sizeof(TokenSym));
  printf("var CValue_size=0x%x;\n",sizeof(CValue));
  printf("var TOK_NUM=0x%x;\n",TOK_NUM);
  printf("var TOK_CCHAR=0x%x;\n",TOK_CCHAR);
  printf("var TOK_LCHAR=0x%x;\n",TOK_LCHAR);
  printf("var TOK_STR=0x%x;\n",TOK_STR);
  printf("var TOK_LSTR=0x%x;\n",TOK_LSTR);
  printf("var TOK_CFLOAT=0x%x;\n",TOK_CFLOAT);
  printf("var TOK_CDOUBLE=0x%x;\n",TOK_CDOUBLE);
  printf("var TOK_CHAR=0x%x;\n",TOK_CHAR);
  printf("var TOK_VOID=0x%x;\n",TOK_VOID);
  printf("var TOK_SHORT=0x%x;\n",TOK_SHORT);
  printf("var TOK_INT=0x%x;\n",TOK_INT);
  printf("var TOK_ENUM=0x%x;\n",TOK_ENUM);
  printf("var TOK_STRUCT=0x%x;\n",TOK_STRUCT);
  printf("var TOK_UNION=0x%x;\n",TOK_UNION);
  printf("var TOK_CONST=0x%x;\n",TOK_CONST);
  printf("var SymStack_size=0x%x;\n",sizeof(SymStack));
  printf("var Sym_size=0x%x;\n",sizeof(Sym));
  printf("var IncludeFile_size=0x%x;\n",sizeof(IncludeFile));
  printf("var SValue_size=0x%x;\n",sizeof(SValue));
  return 0;
}
