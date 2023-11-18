#include "elf_loader_support.c"

typedef int (*FUNCTION) ();

int call_wrap(FUNCTION t, int a, int b){
  return t(a,b);
}

int init_c(void){
  p_size=1;
}

void puts_num(int x){
  printf("%u\n",x);
}
