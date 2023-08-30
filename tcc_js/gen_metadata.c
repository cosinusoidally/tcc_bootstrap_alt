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
  printf("var TOK_VOLATILE=0x%x;\n",TOK_VOLATILE);
  printf("var TOK_REGISTER=0x%x;\n",TOK_REGISTER);
  printf("var TOK_SIGNED=0x%x;\n",TOK_SIGNED);
  printf("var TOK_AUTO=0x%x;\n",TOK_AUTO);
  printf("var TOK_INLINE=0x%x;\n",TOK_INLINE);
  printf("var TOK_RESTRICT=0x%x;\n",TOK_RESTRICT);
  printf("var TOK_UNSIGNED=0x%x;\n",TOK_UNSIGNED);
  printf("var TOK_EXTERN=0x%x;\n",TOK_EXTERN);
  printf("var TOK_STATIC=0x%x;\n",TOK_STATIC);
  printf("var TOK_TYPEDEF=0x%x;\n",TOK_TYPEDEF);
  printf("var SymStack_size=0x%x;\n",sizeof(SymStack));
  printf("var Sym_size=0x%x;\n",sizeof(Sym));
  printf("var IncludeFile_size=0x%x;\n",sizeof(IncludeFile));
  printf("var SValue_size=0x%x;\n",sizeof(SValue));
  printf("var VT_BTYPE=0x%x;\n",VT_BTYPE);
  printf("var VT_ENUM=0x%x;\n",VT_ENUM);
  printf("var VT_STRUCT=0x%x;\n",VT_STRUCT);
  printf("var VT_VOID=0x%x;\n",VT_VOID);
  printf("var VT_TYPEDEF=0x%x;\n",VT_TYPEDEF);
  printf("var VT_STATIC=0x%x;\n",VT_STATIC);
  printf("var VT_EXTERN=0x%x;\n",VT_EXTERN);
  printf("var VT_FUNC=0x%x;\n",VT_FUNC);
  printf("var VT_STRUCT_SHIFT=0x%x;\n",VT_STRUCT_SHIFT);
  printf("var VT_LOCAL=0x%x;\n",VT_LOCAL);
  printf("var TOK_IF=0x%x;\n",TOK_IF);
  printf("var TOK_WHILE=0x%x;\n",TOK_WHILE);
  printf("var TOK_RETURN=0x%x;\n",TOK_RETURN);
  printf("var TOK_SHL=0x%x;\n",TOK_SHL);
  printf("var TOK_SAR=0x%x;\n",TOK_SAR);
  printf("var TOK_ULE=0x%x;\n",TOK_ULE);
  printf("var TOK_GT=0x%x;\n",TOK_GT);
  printf("var TOK_ULT=0x%x;\n",TOK_ULT);
  printf("var TOK_UGE=0x%x;\n",TOK_UGE);
  printf("var TOK_EQ=0x%x;\n",TOK_EQ);
  printf("var TOK_NE=0x%x;\n",TOK_NE);
  printf("var TOK_LAND=0x%x;\n",TOK_LAND);
  printf("var TOK_LOR=0x%x;\n",TOK_LOR);
  return 0;
}
