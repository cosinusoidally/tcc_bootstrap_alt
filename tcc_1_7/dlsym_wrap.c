void *dlsym_wrap2(void *handle, char *symbol)
{
  return dlsym(handle,symbol);
}

#define dlsym dlsym_wrap

#include "tcc.c"

typedef struct TCCSyms {
    char *str;
    void *ptr;
} TCCSyms;

static TCCSyms tcc_syms[] = {
    { NULL, NULL },
};

void *dlsym_wrap(void *handle, char *symbol)
{
//if (!strcmp(p->str, symbol))
  TCCSyms *s;
  s=tcc_syms;

  int p;
  int i=0;
  printf("dlsym: %s\n",symbol);
  if(!strcmp("dlsym", symbol)) {
    return &dlsym_wrap;
  }

  while(s->str != NULL) {
    if (!strcmp(s->str, symbol))
      return s->ptr;
    s++;
  }

  p=dlsym_wrap2(handle,symbol);
  return p;
}
