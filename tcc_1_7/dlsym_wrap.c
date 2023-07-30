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

int blah_wrap(){
  printf("blah unimplemented\n");
  exit(1);
}

int dlerror_wrap(){
  printf("dlerror unimplemented\n");
  exit(1);
}

int dlopen_wrap(){
  printf("dlopen unimplemented\n");
  exit(1);
}

int close();

void *dlsym_wrap(void *handle, char *symbol)
{
  // TCCSyms must be defined here, I think there is a bug around global data
  // relocations
  TCCSyms tcc_syms[] = {
      { "atoi", &atoi, },
      { "close", &close, },
      { "dlerror", &dlerror_wrap, },
      { "dlopen", &dlopen_wrap, },
// don't override here
//      { "dlsym", &dlsym_wrap, },
      { "exit", &exit, },
      { "fclose", &fclose, },
      { "fdopen", &fdopen, },
      { "blah", &blah_wrap, },
      { NULL, NULL },
  };

  TCCSyms *s;
  s=tcc_syms;
  int p;

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
