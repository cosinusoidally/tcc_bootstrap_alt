void puts_num(int x);

int strcmp (int a1, int b1)
{
  char *a;
  char *b;
  a = a1;
  b = b1;
  while ((a[0] != 0) && (b[0] != 0) && (a[0] == b[0]))
    {
      a = a + 1;
      b = b + 1;
    }

  return a[0] - b[0];
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

int realloc(int ptr, int size) {
  int r;
  r=malloc(size);
  if(ptr!=0) {
    memcpy(r, ptr, size);
    free(ptr);
  }
  return r;
}

int open_wrap(int pathname, int flags) {
  puts("open");
  puts(pathname);
  puts_num(flags);
  return open(pathname, 0, 0);
}

int fread(int ptr,int size, int nitems, int stream) {
  int i;
  int t=size*nitems;
  if(size!=1) {
    puts("fread can only handle size 1");
  }
  char *c=ptr;
  int cr;
  for(i=0;i<t;i=i+1){
    cr=fgetc(stream);
    if(cr == -1) {
      puts("eof");
      return i;
    }
    c[i]=cr;
  }
  return nitems;
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

int generic3_tramp(int a, int b, int c) {
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
      "lea_eax,[ebp+DWORD] %0x10"
      "mov_eax,[eax]"
      "push_eax"
      "mov_ebp,edi"
      "push_ebx"
      "pop_eax"
      "call_eax"
      "pop_ebx"
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
  asm("mov_ebx, &FUNCTION_calloc"
      "jmp %FUNCTION_generic2_tramp");
}

int exit_tramp(int x){
  puts("exit not impl");
  exit(1);
}

int fputc_tramp(int x){
  asm("mov_ebx, &FUNCTION_fputc"
      "jmp %FUNCTION_generic2_tramp");
}

int free_tramp(int x){
/*  puts("free_tramp called"); */
  asm("mov_ebx, &FUNCTION_free"
      "jmp %FUNCTION_generic1_tramp");
}

int malloc_tramp(int x){
  asm("mov_ebx, &FUNCTION_malloc"
      "jmp %FUNCTION_generic1_tramp");
}

int realloc_tramp(int x){
  asm("mov_ebx, &FUNCTION_realloc"
      "jmp %FUNCTION_generic2_tramp");
}

int open_tramp(int x){
  puts("open_tramp called");
  asm("mov_ebx, &FUNCTION_open_wrap"
      "jmp %FUNCTION_generic2_tramp");
}

int close_tramp(int x){
  puts("close_tramp called");
  asm("mov_ebx, &FUNCTION_fclose"
      "jmp %FUNCTION_generic1_tramp");
}

int read_wrap(int fd, int buf, int count) {
  int r;
  puts("read");
  r=fread(buf ,1, count, fd);
  return r;
}

int read_tramp(int x){
  puts("read_tramp called");
  asm("mov_ebx, &FUNCTION_read_wrap"
      "jmp %FUNCTION_generic3_tramp");
}

int fopen_wrap(int x, int y){
  int r;
  puts("fopen_wrap:");
  puts(x);
  puts(y);
  r=fopen(x,y);
  return r;
}

int fopen_tramp(int x, int y){
/*  puts("fopen_tramp called"); */
  asm("mov_ebx, &FUNCTION_fopen_wrap"
      "jmp %FUNCTION_generic2_tramp");
}

int fclose_tramp(int x){
/*  puts("fclose_tramp called"); */
  asm("mov_ebx, &FUNCTION_fclose"
      "jmp %FUNCTION_generic1_tramp");
}
