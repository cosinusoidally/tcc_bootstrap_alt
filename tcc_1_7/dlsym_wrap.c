void *dlsym_wrap(void *handle, char *symbol)
{
//if (!strcmp(p->str, symbol))
  int p;
  printf("dlsym: %s\n",symbol);
  if(!strcmp("dlsym", symbol)) {
    return &dlsym_wrap;
  }
  p=dlsym(handle,symbol);
  return p;
}

#define dlsym dlsym_wrap

#include "tcc.c"
