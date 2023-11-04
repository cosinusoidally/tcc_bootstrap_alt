#include "loader_support.c"

typedef int (*FUNCTION) ();

int call_wrap(FUNCTION t, int a, int b){
  return t(a,b);
}
