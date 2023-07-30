void *dlsym_wrap2(void *handle, char *symbol)
{
  return dlsym(handle,symbol);
}

#define dlsym dlsym_wrap

#include "tcc.c"

void *dlsym_wrap(void *handle, char *symbol)
{
//if (!strcmp(p->str, symbol))
  int p;
  printf("dlsym: %s\n",symbol);
  if(!strcmp("dlsym", symbol)) {
    return &dlsym_wrap;
  }
  p=dlsym_wrap2(handle,symbol);
  return p;
}
