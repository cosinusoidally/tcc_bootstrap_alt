typedef int (*FUNCTION) ();

int call_wrap(FUNCTION t, int a, int b){
  return t(a,b);
}

int init_c(void){
}

#include "loader_boot.c"
