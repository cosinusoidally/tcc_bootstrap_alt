void *dlsym_wrap(void *handle, char *symbol)
{
//if (!strcmp(p->str, symbol))
  int p;
  printf("dlsym: %s\n",symbol);
  p=dlsym(handle,symbol);
  return p;
}

#define dlsym dlsym_wrap

#include "tcc.c"
