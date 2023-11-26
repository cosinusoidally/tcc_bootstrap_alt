void puts_num(int x);

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

int call_wrap(FUNCTION t, int a, int b){
  int r;
  puts("call_wrap dummy stub");
  /* FIXME ljw need to properly marshal a and b */
  /* not args are back to front because of differing calling convention */
  r = t(b,a);
  return r;
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

int init_c(void) {
  p_size=4;
  stdin=0;
  stdout=1;
  stderr=3;
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

int generic1_tramp(int a) {
  asm("push_ebp"
      "mov_ebp,esp"
      "push_edi"
      "push_ebp"
      "mov_edi,esp"
      "lea_eax,[ebp+DWORD] %0x8"
      "mov_eax,[eax]"
      "push_eax"
      "mov_ebp,edi"
      "push_ebx"
      "pop_eax"
      "call_eax"
      "pop_ebx"
      "pop_ebp"
      "pop_edi"
      "pop_ebp"
      "ret");
}

int generic2_tramp(int a, int b) {
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
      "push_ebx"
      "pop_eax"
      "call_eax"
      "pop_ebx"
      "pop_ebx"
      "pop_ebp"
      "pop_edi"
      "pop_ebp"
      "ret");
}
int get_stdout(void){
  asm("mov_eax, &GLOBAL_stdout"
      "ret");
}

int get_stdin(void){
  asm("mov_eax, &GLOBAL_stdin"
      "ret");
}

int get_stderr(void){
  asm("mov_eax, &GLOBAL_stderr"
      "ret");
}

int fputs_tramp(int x){
  asm("mov_ebx, &FUNCTION_fputs"
      "jmp %FUNCTION_generic2_tramp");
}

int calloc_tramp(int x){
  puts("calloc not impl");
  exit(1);
}

int exit_tramp(int x){
  puts("exit not impl");
  exit(1);
}

int fputc_tramp(int x){
  puts("fputc not impl");
  exit(1);
}

int free_tramp(int x){
  puts("free not impl");
  exit(1);
}

int malloc_tramp(int x){
  asm("mov_ebx, &FUNCTION_malloc"
      "jmp %FUNCTION_generic1_tramp");
}

int realloc_tramp(int x){
  puts("realloc not impl");
  exit(1);
}

int open_tramp(int x){
  puts("open not impl");
  exit(1);
}

int close_tramp(int x){
  puts("close not impl");
  exit(1);
}

int read_tramp(int x){
  puts("read not impl");
  exit(1);
}

int fopen_tramp(int x){
  puts("fopen not impl");
  exit(1);
}

int fclose_tramp(int x){
  puts("fclose not impl");
  exit(1);
}
