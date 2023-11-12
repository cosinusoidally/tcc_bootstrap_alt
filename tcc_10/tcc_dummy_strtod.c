

double strtod(char *a, char **p){
  double v;
  int *x;
  x=&v;
  x[0]=0;
  x[1]=0;
  puts("tcc strtod");
  if(strcmp("4294967296.0",a) == 0){
    x[1]=0x41F00000;
  }
  return v;
}


#include "tcc.c"
