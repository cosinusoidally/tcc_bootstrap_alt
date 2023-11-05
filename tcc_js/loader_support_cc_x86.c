int stdout;
void puts_num(int x);

int call_wrap(FUNCTION t, int a, int b){
  int r;
  puts("call_wrap dummy stub");
  /* FIXME ljw need to properly marshal a and b */
  r = t(a,b);
  return r;
}

int strcmp(int a, int b){
  char *s1;
  char *s2;
  int i;
  int r;
  i=0;
  r=1;
  s1=a;
  s2=b;
  while(s1[i]!=0){
    r=0;
    if(s1[i]!=s2[i]){
      r=1;
      break;
    }
    i=i+1;
  }
  return r;
}

int malloc_tramp(int x){
  puts("malloc_tramp called");
  asm("lea_eax,[esp+DWORD] %4"
      "mov_eax,[eax]"
      "push_edi"
      "push_ebp"
      "mov_edi,esp"
      "push_eax"
      "mov_ebp,edi"
      "call %FUNCTION_malloc"
      "pop_ebx"
      "pop_ebp"
      "pop_edi"
      "ret");
}

int strlen_tramp(int x){
  puts("strlen_tramp called");
  asm("push_ebp"
      "mov_ebp,esp"
      "push_edi"
      "push_ebp"
      "mov_edi,esp"
      "lea_eax,[ebp+DWORD] %0x8"
      "mov_eax,[eax]"
      "push_eax"
      "mov_ebp,edi"
      "call %FUNCTION_strlen"
      "pop_ebx"
      "pop_ebp"
      "pop_edi"
      "pop_ebp"
      "ret");
}

int fopen_wrap(int x, int y){
  puts("fopen_wrap:");
  puts(x);
  puts(y);
  return fopen(x,y);
}

int fopen_tramp(int x, int y){
  puts("fopen_tramp called");
  asm("push_ebp"
      "mov_ebp,esp"
      "push_edi"
      "push_ebp"
      "mov_edi,esp"
      "lea_eax,[ebp+DWORD] %0x8"
      "mov_eax,[eax]"
      "push_eax"
      "lea_eax,[ebp+DWORD] %0xC"
      "mov_eax,[eax]"
      "push_eax"
      "mov_ebp,edi"
      "call %FUNCTION_fopen_wrap"
      "pop_ebx"
      "pop_ebx"
      "pop_ebp"
      "pop_edi"
      "pop_ebp"
      "ret");
}

int fclose_tramp(int x){
  puts("fclose_tramp called");
  asm("push_ebp"
      "mov_ebp,esp"
      "push_edi"
      "push_ebp"
      "mov_edi,esp"
      "lea_eax,[ebp+DWORD] %0x8"
      "mov_eax,[eax]"
      "push_eax"
      "mov_ebp,edi"
      "call %FUNCTION_fclose"
      "pop_ebx"
      "pop_ebp"
      "pop_edi"
      "pop_ebp"
      "ret");
}

int fwrite_tramp(int x){
  puts("fwrite_tramp not impl");
  exit(1);
}

int getc_unlocked_tramp(int x){
/*  puts("getc_unlocked_tramp called"); */
  asm("push_ebp"
      "mov_ebp,esp"
      "push_edi"
      "push_ebp"
      "mov_edi,esp"
      "lea_eax,[ebp+DWORD] %0x8"
      "mov_eax,[eax]"
      "push_eax"
      "mov_ebp,edi"
      "call %FUNCTION_fgetc"
      "pop_ebx"
      "pop_ebp"
      "pop_edi"
      "pop_ebp"
      "ret");
}

int puts_tramp(int x){
  puts("puts_tramp called");
  asm("lea_eax,[esp+DWORD] %4"
      "mov_eax,[eax]"
      "push_edi"
      "push_ebp"
      "mov_edi,esp"
      "push_eax"
      "mov_ebp,edi"
      "call %FUNCTION_puts"
      "pop_ebx"
      "pop_ebp"
      "pop_edi"
      "ret");
}

int printf_tramp(int x){
  puts("printf_tramp not impl");
  exit(1);
}

int exit_tramp(int x){
  puts("exit_tramp not impl");
  exit(1);
}

int dlsym(int h,int sym){
  if(strcmp(sym, "malloc")==0) {
    return malloc_tramp;
  }
  if(strcmp(sym, "strlen")==0) {
    return strlen_tramp;
  }
  if(strcmp(sym, "fopen")==0) {
    return fopen_tramp;
  }
  if(strcmp(sym, "fclose")==0) {
    return fclose_tramp;
  }
  if(strcmp(sym, "fwrite")==0) {
    return fwrite_tramp;
  }
  if(strcmp(sym, "getc_unlocked")==0) {
    return getc_unlocked_tramp;
  }
  if(strcmp(sym, "puts")==0) {
    return puts_tramp;
  }
  if(strcmp(sym, "printf")==0) {
    return printf_tramp;
  }
  if(strcmp(sym, "exit")==0) {
    return exit;
  }
  puts("dlsym missing sym");
  exit(1);
}

int puts(char *a) {
  fputs(a,stdout);
  fputs("\n",stdout);
}

void puts_num(int x){
  char *s;
  s=int2str(x,10,0);
  puts(s);
  free(s);
  return;
}

int fread(int ptr,int size, int nitems, int stream) {
/*
  puts("fread not impl");
  exit(1);
*/
  int t=size*nitems;
  char *c=ptr;
  while(t>0){
    c[0]=fgetc(stream);
    t=t-1;
    c=c+1;
  }
}

int memcpy(int a, int b, int c) {
  char *dest;
  char *src;
  int i;
  dest=a;
  src=b;
  for(i=0;i<c;i=i+1){
    dest[i]=src[i];
  }
}
