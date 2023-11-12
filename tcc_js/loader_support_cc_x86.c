int stdout;
void puts_num(int x);

int call_wrap(FUNCTION t, int a, int b){
  int r;
  puts("call_wrap dummy stub");
  /* FIXME ljw need to properly marshal a and b */
  /* not args are back to front because of differing calling convention */
  r = t(b,a);
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

int generic6_tramp(int a, int b, int c, int d, int e, int f) {
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
      "lea_eax,[ebp+DWORD] %0x14"
      "mov_eax,[eax]"
      "push_eax"
      "lea_eax,[ebp+DWORD] %0x18"
      "mov_eax,[eax]"
      "push_eax"
      "lea_eax,[ebp+DWORD] %0x1C"
      "mov_eax,[eax]"
      "push_eax"
      "mov_ebp,edi"
      "push_ebx"
      "pop_eax"
      "call_eax"
      "pop_ebx"
      "pop_ebx"
      "pop_ebx"
      "pop_ebx"
      "pop_ebx"
      "pop_ebx"
      "pop_ebp"
      "pop_edi"
      "pop_ebp"
      "ret");
}

int malloc_wrap(int x){
  int r;
/*
  puts("malloc_wrap:");
  puts_num(x);
  puts("calling malloc");
*/
  r=malloc(x);
/*
  puts("malloc returned");
  puts_num(r);
*/
  return r;
}

int malloc_tramp(int x){
/*  puts("malloc_tramp called"); */
  asm("mov_ebx, &FUNCTION_malloc_wrap"
      "jmp %FUNCTION_generic1_tramp");
}

int strlen_tramp(int x){
/*  puts("strlen_tramp called"); */
  asm("mov_ebx, &FUNCTION_strlen"
      "jmp %FUNCTION_generic1_tramp");
}

int fopen_wrap(int x, int y){
  int r;
  puts("fopen_wrap:");
  puts(x);
  puts(y);
  r=fopen(x,y);
  puts("fopen_wrap fd:");
  puts_num(r);
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

int fwrite(int ptr,int size, int nitems, int stream) {
/*
  puts("fwrite:");
  puts_num(ptr);
  puts_num(size);
  puts_num(nitems);
  puts_num(stream);
  puts("fwrite not impl");
  exit(1);
*/
  int t=size*nitems;
  char *c=ptr;
  int c;
  while(t>0){
    fputc(c[0],stream);
    t=t-1;
    c=c+1;
  }
}

int fwrite_tramp(int a, int b, int c, int d){
/*  puts("fwrite_tramp called"); */
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
      "lea_eax,[ebp+DWORD] %0x14"
      "mov_eax,[eax]"
      "push_eax"
      "mov_ebp,edi"
      "call %FUNCTION_fwrite"
      "pop_ebx"
      "pop_ebx"
      "pop_ebx"
      "pop_ebx"
      "pop_ebp"
      "pop_edi"
      "pop_ebp"
      "ret");
}

int getc_unlocked_tramp(int x){
/*  puts("getc_unlocked_tramp called"); */
  asm("mov_ebx, &FUNCTION_fgetc"
      "jmp %FUNCTION_generic1_tramp");
}

int puts_tramp(int x){
/*  puts("puts_tramp called"); */
  asm("mov_ebx, &FUNCTION_puts"
      "jmp %FUNCTION_generic1_tramp");
}

int printf_wrap(int x){
/*
  puts("dummy printf:");
  puts(x);
*/
}

int printf_tramp(int x){
/*  puts("printf_tramp called"); */
  asm("mov_ebx, &FUNCTION_printf_wrap"
      "jmp %FUNCTION_generic1_tramp");
}

int exit_tramp(int x){
  puts("exit_tramp not impl");
  exit(1);
}

int memcpy_tramp(int a, int b, int c){
/*  puts("memcpy_tramp called"); */
  asm("mov_ebx, &FUNCTION_memcpy"
      "jmp %FUNCTION_generic3_tramp");
}

int free_tramp(int x){
/*  puts("free_tramp called"); */
  asm("mov_ebx, &FUNCTION_free"
      "jmp %FUNCTION_generic1_tramp");
}

int realloc_wrap(int ptr, int size) {
  int r;
/*
  puts("realloc_wrap called");
  puts_num(ptr);
  puts_num(size);
*/
  r=malloc(size);
  if(ptr!=0) {
    memcpy(r, ptr, size);
    free(ptr);
  }
  return r;
}

int realloc_tramp(int x, int y){
/*  puts("realloc_tramp called"); */
  asm("mov_ebx, &FUNCTION_realloc_wrap"
      "jmp %FUNCTION_generic2_tramp");
}

int atoi(int x){
  int p;
  puts("atoi");
  puts(x);
  p=strtoint(x);
  puts_num(p);
  return p;
}

int atoi_tramp(int x){
/*  puts("atoi_tramp called"); */
  asm("mov_ebx, &FUNCTION_atoi"
      "jmp %FUNCTION_generic1_tramp");
}

int fdopen_tramp(int x){
  puts("fdopen_tramp not impl");
  exit(1);
}

int fread_tramp(int x){
  puts("fread_tramp not impl");
  exit(1);
}

int fprintf(int stream, int format) {
  puts("fprintf");
  puts(format);
}

int fprintf_tramp(int x){
  puts("fprintf_tramp called");
  asm("mov_ebx, &FUNCTION_fprintf"
      "jmp %FUNCTION_generic2_tramp");
}

int sprintf(int a1, int a2, int a3, int a4, int a5, int a6){
  int format;
  format = a2;
  fputs("sprintf \"", stdout);
  fputs(format, stdout);
  fputs("\"\n", stdout);
  return 0;
}

int sprintf_tramp(int x){
  puts("sprintf_tramp called");
  asm("mov_ebx, &FUNCTION_sprintf"
      "jmp %FUNCTION_generic6_tramp");
}

int snprintf(int a1, int a2, int a3, int a4, int a5, int a6){
  int format;
  int size;
  size = a2;
  format = a3;
  fputs("snprintf size: ", stdout);
  fputs(int2str(size, 10, 0), stdout);
  fputs(" format: \"", stdout);
  fputs(format, stdout);
  fputs("\"\n", stdout);
  return 0;
}

int snprintf_tramp(int x){
  puts("snprintf_tramp called");
  asm("mov_ebx, &FUNCTION_snprintf"
      "jmp %FUNCTION_generic6_tramp");
}

int vfprintf(int stream, int format) {
  puts("vfprintf called");
  puts(format);
}

int vfprintf_tramp(int x){
  puts("vfprintf_tramp called");
  asm("mov_ebx, &FUNCTION_vfprintf"
      "jmp %FUNCTION_generic2_tramp");
}

int strcat(int de,int s) {
  int d;
  char *dest;
  char *src;
  dest=de;
  src=s;
  d=dest;
  int c;
  while(dest[0]){
    dest=dest+1;
  }
  while(c=src[0]){
    src=src+1;
    dest[0]=c;
    dest=dest+1;
  }
  dest[0]=0;
  return d;
}

int strcat_tramp(int x){
/*  puts("strcat_tramp called"); */
  asm("mov_ebx, &FUNCTION_strcat"
      "jmp %FUNCTION_generic2_tramp");
}

int strchr(int p, int c) {
  int c1;
  char *s;
  s=p;
  while(c1=s[0]){
    s=s+1;
    if(c1==c){
      return s-1;
    }
  }
  return 0;
}

int strchr_tramp(int x){
/*  puts("strchr_tramp called"); */
  asm("mov_ebx, &FUNCTION_strchr"
      "jmp %FUNCTION_generic2_tramp");
}

int strcmp_tramp(int x){
/*  puts("strcmp_tramp called"); */
  asm("mov_ebx, &FUNCTION_strcmp"
      "jmp %FUNCTION_generic2_tramp");
}

int strrchr(int p, int c) {
  int c1;
  char *s;
  char *r;
  r=0;
  s=p;
  while(c1=s[0]){
    s=s+1;
    if(c1==c){
      r=s-1;
    }
  }
  return r;
}

int strrchr_tramp(int x){
/*  puts("strrchr_tramp called"); */
  asm("mov_ebx, &FUNCTION_strrchr"
      "jmp %FUNCTION_generic2_tramp");
}

int strcpy(int d, int s) {
  int c;
  char *dest;
  char *src;
  dest=d;
  src=s;
  while((c=src[0])!=0){
    src=src+1;
    dest[0]=c;
    dest=dest+1;
  }
  dest[0]=0;
}

int strcpy_tramp(int x){
/*  puts("strcpy_tramp called"); */
  asm("mov_ebx, &FUNCTION_strcpy"
      "jmp %FUNCTION_generic2_tramp");
}

int memcmp(int s1, int s2, int n) {
  int i;
  int r;
  char *p1;
  char *p2;
  r=0;
  p1=s1;
  p2=s2;
  for(i=0;i<n;i=i+1){
    if(p1[i]!=p2[i]){
/* FIXME ljw not quite right */
      r=1;
    }
  }
  return r;
}

int memcmp_tramp(int x){
/*  puts("memcmp_tramp called"); */
  asm("mov_ebx, &FUNCTION_memcmp"
      "jmp %FUNCTION_generic3_tramp");
}

int memset_tramp(int a, int b, int c){
/*  puts("memset_tramp called"); */
  asm("mov_ebx, &FUNCTION_memset"
      "jmp %FUNCTION_generic3_tramp");
}

int strdup(int s) {
  int i;
  int l;
  char *src;
  char *dest;
  src=s;
  l=strlen(src);
  dest=malloc(l+1);
  for(i=0;i<l;i=i+1){
    dest[i] = src[i];
  }
  dest[l] = 0;
  return dest;
}

int strdup_tramp(int x){
/*  puts("strdup_tramp called"); */
  asm("mov_ebx, &FUNCTION_strdup"
      "jmp %FUNCTION_generic1_tramp");
}

int mmap_wrap(int addr, int length, int prot, int flags, int fd, int offset) {
  puts("mmap_wrap:");
  puts_num(addr);
  puts_num(length);
  puts_num(prot);
  puts_num(flags);
  puts_num(fd);
  puts_num(offset);
  return malloc(length);
}

int mmap_tramp(int x){
/*  puts("mmap_tramp not impl"); */
  asm("mov_ebx, &FUNCTION_mmap_wrap"
      "jmp %FUNCTION_generic6_tramp");
}

int close_tramp(int x){
  puts("close_tramp called");
  asm("mov_ebx, &FUNCTION_fclose"
      "jmp %FUNCTION_generic1_tramp");
}

int fputc_tramp(int x){
  puts("fputc_tramp called");
  asm("mov_ebx, &FUNCTION_fputc"
      "jmp %FUNCTION_generic2_tramp");
}

int open_wrap(int pathname, int flags) {
  puts("open");
  puts(pathname);
  puts_num(flags);
  return open(pathname, 0, 0);
}

int open_tramp(int x){
  puts("open_tramp called");
  asm("mov_ebx, &FUNCTION_open_wrap"
      "jmp %FUNCTION_generic2_tramp");
}

int read_wrap(int fd, int buf, int count) {
  int r;
  puts("read");
  puts_num(count);
  r=fread(buf ,1, count, fd);
  puts_num(r);
  return r;
}

int read_tramp(int x){
  puts("read_tramp called");
  asm("mov_ebx, &FUNCTION_read_wrap"
      "jmp %FUNCTION_generic3_tramp");
}

int strtod(int nptr, int endptr){
  /* FIXME real impl */
  puts("strtod");
  puts(nptr);
  puts_num(endptr);
  asm("DEFINE fldl DD05"
      "fldl &GLOBAL_double_buf"
      "ret"
      ":GLOBAL_double_buf"
      "00000000 00000000");
}

int strtof(int nptr, int endptr){
  /* FIXME real impl */
  puts("strtof");
  puts(nptr);
  puts_num(endptr);
  asm("DEFINE flds D905"
      "flds &GLOBAL_float_buf"
      "ret"
      ":GLOBAL_float_buf"
      "00000000");
}

int strtod_tramp(int x){
  puts("strtod_tramp called");
  asm("mov_ebx, &FUNCTION_strtod"
      "jmp %FUNCTION_generic2_tramp");
}

int strtof_tramp(int x){
  puts("strtof_tramp called");
  asm("mov_ebx, &FUNCTION_strtof"
      "jmp %FUNCTION_generic2_tramp");
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
  if(strcmp(sym, "memcpy")==0) {
    return memcpy_tramp;
  }
  if(strcmp(sym, "free")==0) {
    return free_tramp;
  }
  if(strcmp(sym, "realloc")==0) {
    return realloc_tramp;
  }
  if(strcmp(sym, "atoi")==0) {
    return atoi_tramp;
  }
  if(strcmp(sym, "stdout")==0) {
    return &stdout;
  }
  if(strcmp(sym, "stderr")==0) {
    return &stderr;
  }
  if(strcmp(sym, "fdopen")==0) {
    return fdopen_tramp;
  }
  if(strcmp(sym, "fread")==0) {
    return fread_tramp;
  }
  if(strcmp(sym, "fprintf")==0) {
    return fprintf_tramp;
  }
  if(strcmp(sym, "sprintf")==0) {
    return sprintf_tramp;
  }
  if(strcmp(sym, "snprintf")==0) {
    return snprintf_tramp;
  }
  if(strcmp(sym, "vfprintf")==0) {
    return vfprintf_tramp;
  }
  if(strcmp(sym, "strcat")==0) {
    return strcat_tramp;
  }
  if(strcmp(sym, "strchr")==0) {
    return strchr_tramp;
  }
  if(strcmp(sym, "strcmp")==0) {
    return strcmp_tramp;
  }
  if(strcmp(sym, "strrchr")==0) {
    return strrchr_tramp;
  }
  if(strcmp(sym, "strcpy")==0) {
    return strcpy_tramp;
  }
  if(strcmp(sym, "memcmp")==0) {
    return memcmp_tramp;
  }
  if(strcmp(sym, "memset")==0) {
    return memset_tramp;
  }
  if(strcmp(sym, "strdup")==0) {
    return strdup_tramp;
  }
  if(strcmp(sym, "mmap")==0) {
    return mmap_tramp;
  }
  if(strcmp(sym, "close")==0) {
    return close_tramp;
  }
  if(strcmp(sym, "fputc")==0) {
    return fputc_tramp;
  }
  if(strcmp(sym, "open")==0) {
    return open_tramp;
  }
  if(strcmp(sym, "read")==0) {
    return read_tramp;
  }
  if(strcmp(sym, "strtod")==0) {
    return strtod_tramp;
  }
  if(strcmp(sym, "strtof")==0) {
    return strtof_tramp;
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
  int i;
  int t=size*nitems;
  if(size!=1) {
    puts("fread can only handle size 1");
  }
  char *c=ptr;
  int cr;
  for(i=0;i<t;i=i+1){
    cr=fgetc(stream);
    if(cr==-1) {
      puts("eof");
      return i;
    }
    c[i]=cr;
  }
  return nitems;
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

int init_c(void) {
  p_size=4;
  stdin=0;
  stdout=1;
  stderr=3;
}
